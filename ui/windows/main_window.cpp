#include "main_window.h"
#include "ui_main_window.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include "../../common/settings.h"
#include "../../common/markup.h"
#include "../../processing/executor.h"
#include "../../providers/samples_provider.h"
#include "../../providers/markup_provider.h"
#include "../../providers/js_script_provider.h"

#define SETTINGS_FILE_NAME "settings.ini"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      markup_data(std::make_shared<Markup::MarkupData>()),
      dir_provider(std::make_shared<DirProvider>()),
      samples_provider(std::make_shared<SamplesProvider>(dir_provider)),
      markup_provider(std::make_shared<MarkupProvider>(dir_provider)),
      js_script_provider(std::make_shared<JsScriptProvider>()),
      settings(new QSettings(SETTINGS_FILE_NAME, QSettings::Format::IniFormat)),
      functions_provider(std::make_shared<JsFunctionsProvider>()),
      markup_window(new MarkupWindow(this, samples_provider, markup_provider)),
      script_window(new JsScriptWindow(this, js_script_provider)),
      result_window(new ResultWindow(this, samples_provider, markup_provider, js_script_provider))
{
    ui->setupUi(this);
    ui->tab_markup->layout()->addWidget(markup_window);
    ui->tab_scripts->layout()->addWidget(script_window);
    ui->tab_result->layout()->addWidget(result_window);

    QAction* action_open = ui->menuFile->addAction("Open dir..");
    action_open->setShortcuts(QKeySequence::Open);
    action_open->setStatusTip(tr("Open an existing dir "));
    connect(action_open, &QAction::triggered, this, &MainWindow::open_dir);
    load_samples_info();
    connect(script_window, &JsScriptWindow::process_samples_requested, this, &MainWindow::process_samples);
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

    dir_provider->set_dir(dir_name);
}

void MainWindow::process_samples()
{
    qDebug() << Q_FUNC_INFO;
    auto func_provider = JsFunctionsProvider();
    auto executor = Executor();

    auto selected_file_key_option = samples_provider->get_selected_file_key();
    if (!selected_file_key_option.has_value()) {
        qDebug() << "File not selected";
        return;
    }
    auto selected_file_key = selected_file_key_option.value();
    auto samples = samples_provider->get_selected_file_samples();
    auto sample_details_option = markup_provider->get_sample_details(selected_file_key);
    if (!sample_details_option.has_value()) {
        qDebug() << "Markups not found";
        return;
    }
    auto sample_details = sample_details_option.value();
    auto result = executor.execute(samples, js_script_provider, &func_provider);
    result_window->draw_results(samples, sample_details, result);
}
