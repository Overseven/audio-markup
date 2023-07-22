#include "stats_window.h"
#include "ui_stats_window.h"

StatsWindow::StatsWindow(
        QWidget *parent,
        std::shared_ptr<ISamplesProvider> _samples_provider,
        std::shared_ptr<IMarkupProvider> _markup_provider,
        std::shared_ptr<IJsScriptProvider> _js_script_provider,
        std::shared_ptr<JsFunctionsProvider> _js_function_provider
) :
    QWidget(parent),
    ui(new Ui::StatsWindow),
    samples_provider(_samples_provider),
    markup_provider(_markup_provider),
    js_script_provider(_js_script_provider),
    js_function_provider(_js_function_provider),
    executor(std::make_unique<Executor>(_samples_provider, _js_script_provider, _js_function_provider))
{
    ui->setupUi(this);

    ui->js_script_multi_selector->set_js_script_provider(js_script_provider);
    ui->audio_file_multi_selector->set_sample_provider(samples_provider);
}

StatsWindow::~StatsWindow()
{
    delete ui;
}

struct AudioResult {
    QVector<Markup::Markup> markups;
    // script -> range errors
    QMap<QString, QVector<double>> range_errors;
};

struct ScriptStats {
    double mean_error = -1;
    double median_error = -1;
    double max_error = -1;
    QString max_error_file;
    QStringList invalid_files;
};

void StatsWindow::on_pushButton_execute_clicked()
{
    auto audio_filenames = ui->audio_file_multi_selector->get_selected_audio_filenames();
    qDebug() << Q_FUNC_INFO << audio_filenames;
    auto scripts = ui->js_script_multi_selector->get_selected_scripts_filenames();
    qDebug() << Q_FUNC_INFO << scripts;

    ui->tableWidget->clearContents();

    // TODO: using cache

    // script -> audio -> result[]
    // auto results = QMap<QString, QMap<QString, AudioResult>>();
    auto scripts_stats = QMap<QString, ScriptStats>();

    for (const auto &script : scripts) {
        auto script_stats = ScriptStats{};
        auto errors = QVector<double>();

        for (const auto &file : audio_filenames) {
            auto audio_result = AudioResult();

            auto markups = markup_provider->get_sorted_markups(file);
            if (markups.isEmpty()) {
                continue;
            }
            audio_result.markups = markups;

            auto exec_result = executor->execute_script(script, file);
            if (markups.length() != exec_result.ranges.ranges.length()) {
                script_stats.invalid_files.append(file);
                continue;
            }

            const size_t markups_len = markups.length();
            for (size_t i = 0; i < markups_len; i++) {
                auto left_error = qAbs(markups[i].left - exec_result.ranges.ranges[i].start);
                auto right_error = qAbs(markups[i].right - exec_result.ranges.ranges[i].end);
                auto error = (left_error + right_error) / 2.0;

                if (script_stats.max_error_file.isEmpty()) {
                    script_stats.max_error = error;
                    script_stats.max_error_file = file;

                } else if (error > script_stats.max_error) {
                    script_stats.max_error = error;
                    script_stats.max_error_file = file;
                }
                errors.push_back(error);
            }
        }
        if (errors.isEmpty()) {
            scripts_stats[script] = script_stats;
            continue;
        }
        double sum = std::accumulate(std::begin(errors), std::end(errors), 0);
        script_stats.mean_error = sum / errors.length();
        std::sort(std::begin(errors), std::end(errors));
        auto i = errors.length() / 2;

        if (errors.length() % 2 != 0) {
            script_stats.median_error = errors[i];
        } else {
            script_stats.median_error = (errors[i] + errors[i-1]) / 2.0;
        }
        scripts_stats[script] = script_stats;
    }

    for (int i = 0; i < scripts.length(); i++) {
        auto script = scripts[i];
        auto stats = scripts_stats[script];
        ui->tableWidget->insertRow(i);

        auto *item_invalid_files = new QTableWidgetItem(stats.invalid_files.join(", "));

        if (stats.max_error == -1) {
            auto *item_name = new QTableWidgetItem(script);
            auto *item_mean = new QTableWidgetItem("-");
            auto *item_median = new QTableWidgetItem("-");
            auto *item_max = new QTableWidgetItem("-");
            auto *item_max_file = new QTableWidgetItem("-");

            ui->tableWidget->setItem(i, 0, item_name);
            ui->tableWidget->setItem(i, 1, item_mean);
            ui->tableWidget->setItem(i, 2, item_median);
            ui->tableWidget->setItem(i, 3, item_max);
            ui->tableWidget->setItem(i, 4, item_max_file);

        } else {
            auto *item_name = new QTableWidgetItem(script);
            auto *item_mean = new QTableWidgetItem(QString::number(stats.mean_error));
            auto *item_median = new QTableWidgetItem(QString::number(stats.median_error));
            auto *item_max = new QTableWidgetItem(QString::number(stats.max_error));
            auto *item_max_file = new QTableWidgetItem(stats.max_error_file);


            ui->tableWidget->setItem(i, 0, item_name);
            ui->tableWidget->setItem(i, 1, item_mean);
            ui->tableWidget->setItem(i, 2, item_median);
            ui->tableWidget->setItem(i, 3, item_max);
            ui->tableWidget->setItem(i, 4, item_max_file);
        }

        ui->tableWidget->setItem(i, 5, item_invalid_files);
    }
//    auto script_stats = QMap<QString, ScriptStats>();
//    for (const auto &script : scripts) {
//        result[script]. [audio_filenames.first()]
//        auto max_mean_error_file = std::optional<QString>{};
//        auto max_median_error_file = std::optional<QString>{};

//        auto errors = QVector<double>();
//        for (const auto &file : audio_filenames) {

//        }
//    }
}

