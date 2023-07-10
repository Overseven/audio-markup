#ifndef RESULT_WINDOW_H
#define RESULT_WINDOW_H

#include <QWidget>
#include "../../processing/executor.h"
#include "../../processing/common.h"
#include "../../common/markup.h"
#include "../../helpers/audio_view_cache.h"
#include "../../helpers/processing_result_cache.h"
#include "../../interfaces/i_samples_provider.h"
#include "../../interfaces/i_markup_provider.h"
#include "../../interfaces/i_js_script_provider.h"
#include "../../providers/js_functions_provider.h"
#include "../widgets/audio_view_mode_selector.h"
#include "../qcustomplot/qcustomplot.h"

namespace Ui {
class ResultWindow;
}

class ResultWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ResultWindow(
            QWidget *parent,
            std::shared_ptr<ISamplesProvider> _samples_provider,
            std::shared_ptr<IMarkupProvider> _markup_provider,
            std::shared_ptr<IJsScriptProvider> _js_script_provider,
            std::shared_ptr<JsFunctionsProvider> _js_function_provider);

    ~ResultWindow();

    void draw_results(QVector<double> &samples, Markup::SampleDetails &sample_details, ExecutionResult &processing_result);

private slots:
    void audio_file_selection_changed();
    void markups_changed(SampleKey sample_key);

    void update_audio_view_mode(AudioViewMode mode, std::optional<int> mean_window);

    void on_pushButton_execute_clicked();

private:
    void draw_audio();
    void draw_markup(double left, double right, QColor color);
    void load_markups();

private:
    Ui::ResultWindow *ui;
//    MarkupListModel *markups_model;

    std::shared_ptr<ISamplesProvider> samples_provider;
    std::shared_ptr<IMarkupProvider> markup_provider;
    std::shared_ptr<IJsScriptProvider> js_script_provider;
    std::shared_ptr<JsFunctionsProvider> js_function_provider;
    std::unique_ptr<AudioViewCache> audio_view_cache;
    std::unique_ptr<ProcessingResultCache> processing_result_cache;
    std::unique_ptr<Executor> executor;
};

#endif // RESULT_WINDOW_H
