#ifndef STATS_WINDOW_H
#define STATS_WINDOW_H

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
#include "../widgets/audio_file_multi_selector.h"

namespace Ui {
class StatsWindow;
}

class StatsWindow : public QWidget
{
    Q_OBJECT

public:
    explicit StatsWindow(QWidget *parent,
        std::shared_ptr<ISamplesProvider> _samples_provider,
        std::shared_ptr<IMarkupProvider> _markup_provider,
        std::shared_ptr<IJsScriptProvider> _js_script_provider,
        std::shared_ptr<JsFunctionsProvider> _js_function_provider);

    ~StatsWindow();

private slots:
    void on_pushButton_execute_clicked();

private:
    Ui::StatsWindow *ui;

    std::shared_ptr<ISamplesProvider> samples_provider;
    std::shared_ptr<IMarkupProvider> markup_provider;
    std::shared_ptr<IJsScriptProvider> js_script_provider;
    std::shared_ptr<JsFunctionsProvider> js_function_provider;
};

#endif // STATS_WINDOW_H
