#include "result_window.h"
#include "ui_result_window.h"
#include "../../common/utils.h"

ResultWindow::ResultWindow(
        QWidget *parent,
        std::shared_ptr<ISamplesProvider> _samples_provider,
        std::shared_ptr<IMarkupProvider> _markup_provider,
        std::shared_ptr<IJsScriptProvider> _js_script_provider,
        std::shared_ptr<JsFunctionsProvider> _js_function_provider
) :
    QWidget(parent),
    ui(new Ui::ResultWindow),
    samples_provider(_samples_provider),
    markup_provider(_markup_provider),
    js_script_provider(_js_script_provider),
    js_function_provider(_js_function_provider)
{
    ui->setupUi(this);
    ui->plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectItems);
    ui->plot->xAxis->setLabel("Sample index");
    ui->plot->yAxis->setLabel("y");
    ui->plot->legend->setVisible(true);

    audio_view_cache = std::make_unique<AudioViewCache>();
    processing_result_cache = std::make_unique<ProcessingResultCache>(js_script_provider);
    executor = std::make_unique<Executor>(samples_provider, js_script_provider, js_function_provider);

    samples_provider->connect_to_signal_selection_changed(this, SLOT(audio_file_selection_changed()), true);
    markup_provider->connect_to_signal_markups_changed(this, SLOT(markups_changed(SampleKey)), true);
    ui->audio_file_selector->set_sample_provider(samples_provider);
    ui->js_script_multi_selector->set_js_script_provider(js_script_provider);

    connect(ui->audio_view_mode_selector, &AudioViewModeSelector::view_mode_changed, this, &ResultWindow::update_audio_view_mode);
    connect(ui->plot, &QCustomPlot::legendClick, this, &ResultWindow::legend_item_clicked);
}

ResultWindow::~ResultWindow()
{
    delete ui;
}

void ResultWindow::draw_audio()
{
    auto file_selected_key = samples_provider->get_selected_file_key();
    if (!file_selected_key.has_value()){
        return;
    }
    auto samples = samples_provider->get_selected_file_samples();
    if (samples.isEmpty()) {
        return;
    }
    ui->plot->legend->clearItems();
    ui->plot->clearGraphs();
    ui->plot->clearItems();

    auto graph = ui->plot->addGraph();
    graph->setName("Audio");
    audio_view_cache->set_audio_raw(samples);

    auto [mode, mean_window] = ui->audio_view_mode_selector->get_mode();
    update_audio_view_mode(mode, mean_window);
    graph->rescaleAxes();
}

void ResultWindow::audio_file_selection_changed()
{
    qDebug() << Q_FUNC_INFO;
    draw_audio();
    load_markups();
}

void ResultWindow::markups_changed(SampleKey sample_key)
{
    Q_UNUSED(sample_key)
    audio_file_selection_changed();
}

void ResultWindow::legend_item_clicked(QCPLegend *legend, QCPAbstractLegendItem *item, QMouseEvent *event)
{
    Q_UNUSED(legend)
    Q_UNUSED(event)

    if (item == nullptr) {
        return;
    }
    auto is_disable = !item->disabled();
    item->setDisabled(is_disable);

    auto rect_legend = static_cast<QCPRectListLegendItem*>(item);
    const auto rect_items = rect_legend->rect_items();
    for (const auto &rect : rect_items) {
        rect->setVisible(!is_disable);
    }

    ui->plot->replot(QCustomPlot::RefreshPriority::rpQueuedReplot);
}

void ResultWindow::update_audio_view_mode(AudioViewMode mode, std::optional<int> mean_window)
{
    auto file_selected_key = samples_provider->get_selected_file_key();
    if (!file_selected_key.has_value()){
        return;
    }

    auto audio_raw = audio_view_cache->get_raw();
    if (audio_raw.isEmpty()) {
        return;
    }
    QVector<double> x(audio_raw.count());
    for (int i = 0; i < x.length(); i++) {
        x[i] = i;
    }

    if (mode == AudioViewMode::Raw) {
        ui->plot->graph()->setData(x, audio_raw);

    } else if (mode == AudioViewMode::Abs) {
        auto audio_abs = audio_view_cache->get_abs();
        ui->plot->graph()->setData(x, audio_abs);

    } else if (mode == AudioViewMode::Mean && mean_window.has_value()) {
        int window_len = mean_window.value();
        auto audio_mean = audio_view_cache->get_mean(window_len);
        ui->plot->graph()->setData(x, audio_mean);
    }

    ui->plot->yAxis->rescale();
    ui->plot->replot(QCustomPlot::RefreshPriority::rpQueuedReplot);
}

void ResultWindow::load_markups()
{
    auto selected_file_key_option = samples_provider->get_selected_file_key();
    if (!selected_file_key_option.has_value()) {
        return;
    }
    auto selected_file_key = selected_file_key_option.value();
    auto sample_details_option = markup_provider->get_sample_details(selected_file_key);
    if (!sample_details_option.has_value()) {
        return;
    }
    auto sample_details = sample_details_option.value();
    ui->plot->legend->clearItems();
    ui->plot->clearItems();

    {
        auto rect_list = QList<QCPItemRect*>();
        for (const auto &markup : qAsConst(sample_details.markups)) {
            auto rect = new QCPItemRect(ui->plot);
            rect->setSelectedPen(QPen(Qt::red));
            rect->topLeft->setCoords(markup.left, 1);
            rect->bottomRight->setCoords(markup.right, -1);
            rect->setBrush(QBrush(Qt::gray, Qt::BrushStyle::Dense5Pattern));
            rect->setSelectedBrush(QBrush(Qt::red, Qt::BrushStyle::Dense5Pattern));
        }
        auto rect_legend_item = new QCPRectListLegendItem(ui->plot->legend, rect_list, "Markups");
        ui->plot->legend->addItem(rect_legend_item);
    }
    auto processed_scripts = processing_result_cache->get_script_names();
    auto selected = ui->js_script_multi_selector->get_selected_scripts_filenames();

    int i = 0;
    for (const auto &script_name : qAsConst(processed_scripts)) {
        auto it = std::find_if(std::begin(selected), std::end(selected), [&script_name](SelectedScript script){ return script.filename == script_name; });
        if (it == std::end(selected)) {
            continue;
        }
        auto ranges = processing_result_cache->get_ranges(script_name);
        auto rect_list = QList<QCPItemRect*>();
        for (const auto &range : qAsConst(ranges)) {
            auto rect = new QCPItemRect(ui->plot);
            rect->setSelectedPen(QPen(Qt::red));
            rect->topLeft->setCoords(range.start, 1);
            rect->bottomRight->setCoords(range.end, -1);
            rect->setBrush(QBrush(Utils::get_color(i), Qt::BrushStyle::Dense5Pattern));
            rect->setSelectedBrush(QBrush(Qt::red, Qt::BrushStyle::Dense5Pattern));
            rect_list.push_back(rect);
        }
        i++;
        auto rect_legend_item = new QCPRectListLegendItem(ui->plot->legend, rect_list, script_name);
        rect_legend_item->setDisabledTextColor(Qt::gray);
//        connect(rect_legend_item, &QCPAbstractLegendItem::selectionChanged, this, &ResultWindow::legend_item_clicked);
        ui->plot->legend->addItem(rect_legend_item);
    }

    ui->plot->replot(QCustomPlot::RefreshPriority::rpQueuedReplot);
}

void ResultWindow::on_pushButton_execute_clicked()
{
    auto selected = ui->js_script_multi_selector->get_selected_scripts_filenames();
    for (const auto &script : selected) {
        qDebug() << "Selected:" << script.filename;;
        if (!processing_result_cache->contains(script.filename)){
            auto result = executor->execute_script(script.filename);
            if (result.ranges.isEmpty()) {
                return;
            }
            processing_result_cache->set_ranges(script.filename, result.ranges.last().ranges);
        }
    }
    load_markups();
}

