#include "main_window.h"
#include "ui_main_window.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include "../../common/settings.h"
#include "../../common/types.h"
#include "../../processing/executor.h"
#include "../../providers/samples_provider.h"
#include "../../providers/markup_provider.h"
#include "../../providers/js_script_provider.h"

#define SETTINGS_FILE_NAME "settings.ini"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      markup_data(std::make_shared<MarkupData>()),
      dir_provider(std::make_shared<DirProvider>()),
      samples_provider(std::make_shared<SamplesProvider>(dir_provider)),
      markup_provider(std::make_shared<MarkupProvider>(dir_provider)),
      js_script_provider(std::make_shared<JsScriptProvider>()),
      js_function_provider(std::make_shared<JsFunctionsProvider>()),
      processing_result_provider(std::make_shared<ProcessingResultProvider>(dir_provider)),
      settings(new QSettings(SETTINGS_FILE_NAME, QSettings::Format::IniFormat)),
      markup_window(new MarkupWindow(this, samples_provider, markup_provider)),
      script_window(new JsScriptWindow(this, js_script_provider)),
      result_window(new ResultWindow(this, samples_provider, markup_provider, js_script_provider, js_function_provider)),
      stats_window(new StatsWindow(this, samples_provider, markup_provider, js_script_provider, js_function_provider, processing_result_provider))
{
    ui->setupUi(this);
    ui->tab_markup->layout()->addWidget(markup_window);
    ui->tab_scripts->layout()->addWidget(script_window);
    ui->tab_result->layout()->addWidget(result_window);
    ui->tab_stats->layout()->addWidget(stats_window);

    QAction* action_open = ui->menuFile->addAction("Open dir..");
    action_open->setShortcuts(QKeySequence::Open);
    action_open->setStatusTip(tr("Open an existing dir "));
    connect(action_open, &QAction::triggered, this, &MainWindow::open_dir);
    load_samples_info();
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
