#include "stats_window.h"
#include "ui_stats_window.h"

StatsWindow::StatsWindow(
        QWidget *parent,
        std::shared_ptr<ISamplesProvider> _samples_provider,
        std::shared_ptr<IMarkupProvider> _markup_provider,
        std::shared_ptr<IJsScriptProvider> _js_script_provider,
        std::shared_ptr<JsFunctionsProvider> _js_function_provider) :
    QWidget(parent),
    ui(new Ui::StatsWindow),
    samples_provider(_samples_provider),
    markup_provider(_markup_provider),
    js_script_provider(_js_script_provider),
    js_function_provider(_js_function_provider)
{
    ui->setupUi(this);
    ui->js_script_multi_selector->set_js_script_provider(js_script_provider);
    ui->audio_file_multi_selector->set_sample_provider(samples_provider);
}

StatsWindow::~StatsWindow()
{
    delete ui;
}

void StatsWindow::on_pushButton_execute_clicked()
{
    auto audio_filenames = ui->audio_file_multi_selector->get_selected_audio_filenames();
    qDebug() << audio_filenames;
    // TODO: finish
}

