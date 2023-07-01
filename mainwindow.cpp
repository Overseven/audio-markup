#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include "common/settings.h"
#include "common/markup.h"
#include "processing/executor.h"

#define SETTINGS_FILE_NAME "settings.ini"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      markup_data(new Markup::MarkupData()),
      settings(new QSettings(SETTINGS_FILE_NAME, QSettings::Format::IniFormat)),
      functions_provider(std::make_shared<JsFunctionsProvider>()),
      markup_window(new MarkupWindow(this, markup_data)),
      case_window(new JsCaseWindow(this)),
      result_window(new ResultWindow(this))
{
    ui->setupUi(this);
    ui->tab_markup->layout()->addWidget(markup_window);
    ui->tab_processing->layout()->addWidget(case_window);
    ui->tab_result->layout()->addWidget(result_window);

    QAction* action_open = ui->menuFile->addAction("Open dir..");
    action_open->setShortcuts(QKeySequence::Open);
    action_open->setStatusTip(tr("Open an existing dir "));
    connect(action_open, &QAction::triggered, this, &MainWindow::open_dir);
    load_samples_info();

    connect(case_window, &JsCaseWindow::process_samples_requested, this, &MainWindow::process_samples);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::open_dir()
{
    settings->beginGroup(Settings::MainWindow::group_name);
    QString dir_name =
        QFileDialog::getExistingDirectory(this,
        tr("Open Directory"),
        settings->value(Settings::MainWindow::open_dir_default).toString(),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
    );
    settings->endGroup();
    if (dir_name.isEmpty()) {
        QMessageBox::warning(this,
            "Empty directory name",
            QString("The dir \"") + dir_name + "\" is empty",
            QMessageBox::Ok
        );
        return;
    }

    settings->beginGroup(Settings::MainWindow::group_name);
    settings->setValue(Settings::MainWindow::open_dir_default, dir_name);
    settings->endGroup();

    load_samples_info();
}

void MainWindow::load_samples_info()
{
    settings->beginGroup(Settings::MainWindow::group_name);
    QString dir_name = settings->value(Settings::MainWindow::open_dir_default).toString();
    settings->endGroup();

    QDir dir = QDir(dir_name);
    dir.setNameFilters({"*.wav"});
    dir.setFilter(QDir::Files | QDir::NoDotAndDotDot);

    QStringList fileList = dir.entryList();
    if (fileList.isEmpty()) {
        QMessageBox::warning(this,
            "Empty directory",
            QString("The dir \"") + dir_name + "\" has no audio files",
            QMessageBox::Ok
        );
        return;
    }

    dir.setNameFilters({Markup::json::filename});
    QStringList markupFileList = dir.entryList();
    if (markupFileList.isEmpty()){
        auto empty_markup_data = Markup::MarkupData{dir_name, {}};
        Markup::json::save_markup_file(Markup::json::filename, empty_markup_data);
    }
    markup_data->sample_details.clear();
    dir.setNameFilters({Markup::json::filename});
    markupFileList = dir.entryList();

    auto new_markup_data = Markup::json::load_markup_file(dir_name, markupFileList.first());
    markup_data->dir = new_markup_data.dir;
    for (const auto &audio_file : fileList) {
        auto markup = std::find_if(
            std::begin(new_markup_data.sample_details),
            std::end(new_markup_data.sample_details),
            [&](Markup::SampleDetails &gg){ return gg.filename.toLower() == audio_file.toLower(); }
        );
        if (markup != std::end(new_markup_data.sample_details)) {
            markup_data->sample_details.push_back(*markup);
        } else {
            markup_data->sample_details.push_back({audio_file, {}});
        }
    }

    markup_window->sample_details_updated();
}

void MainWindow::process_samples()
{
    qDebug() << Q_FUNC_INFO;
    auto func_provider = JsFunctionsProvider();
    auto executor = Executor();

    auto file_idx = markup_window->get_selected_file_index();
    if (!file_idx.has_value()) {
        qDebug() << "File not selected";
        return;
    }
    auto samples = markup_window->get_selected_file_samples();
    auto &sample_details = markup_data->sample_details[file_idx.value()];
    auto result = executor.execute(samples, case_window, &func_provider);
    result_window->draw_results(samples, sample_details, result);
}
