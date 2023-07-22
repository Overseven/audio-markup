#include "markup_window.h"
#include "ui_markup_window.h"
#include <QMessageBox>

MarkupWindow::MarkupWindow(
        QWidget *parent,
        std::shared_ptr<ISamplesProvider> _samples_provider,
        std::shared_ptr<IMarkupProvider> _markup_provider
) :
    QWidget(parent),
    ui(new Ui::MarkupWindow),
    samples_provider(_samples_provider),
    markup_provider(_markup_provider)
{
    ui->setupUi(this);
    audio_view_cache = std::make_unique<AudioViewCache>();

    markups_model = new MarkupListModel(this);

    ui->listView_markups->setModel(markups_model);
    ui->audio_file_selector->set_sample_provider(samples_provider);

    samples_provider->connect_to_signal_selection_changed(this, SLOT(audio_file_selection_changed()), true);
    markup_provider->connect_to_signal_markups_changed(this, SLOT(markups_changed(SampleKey)), true);

    connect(ui->controls, &GraphControls::mode_updated, this, &MarkupWindow::set_mode);
    connect(ui->plot, &QCustomPlot::itemClick, this, &MarkupWindow::markup_item_selected);
    connect(ui->audio_view_mode_selector, &AudioViewModeSelector::view_mode_changed, this, &MarkupWindow::update_audio_view_mode);
}

MarkupWindow::~MarkupWindow()
{
    markup_provider->save();
}

void MarkupWindow::set_mode(GraphControls::Mode mode)
{
    ui->plot->setInteraction(QCP::iRangeDrag, false);
    disconnect(ui->plot, &QCustomPlot::mousePress, this, &MarkupWindow::plot_add_pressed);
    disconnect(ui->plot, &QCustomPlot::mouseMove, this, &MarkupWindow::plot_add_moved);
    disconnect(ui->plot, &QCustomPlot::mouseRelease, this, &MarkupWindow::plot_add_released);

    disconnect(ui->plot, &QCustomPlot::mousePress, this, &MarkupWindow::plot_edit_pressed);
    disconnect(ui->plot, &QCustomPlot::mouseMove, this, &MarkupWindow::plot_edit_moved);
    disconnect(ui->plot, &QCustomPlot::mouseRelease, this, &MarkupWindow::plot_edit_released);

    if (mode == GraphControls::Mode::Move) {
        ui->plot->setInteraction(QCP::iRangeDrag, true);

    } else if (mode == GraphControls::Mode::Add) {
        connect(ui->plot, &QCustomPlot::mousePress, this, &MarkupWindow::plot_add_pressed);
        connect(ui->plot, &QCustomPlot::mouseRelease, this, &MarkupWindow::plot_add_released);

    } else if (mode == GraphControls::Mode::Edit) {
        connect(ui->plot, &QCustomPlot::mousePress, this, &MarkupWindow::plot_edit_pressed);
        connect(ui->plot, &QCustomPlot::mouseRelease, this, &MarkupWindow::plot_edit_released);
    }
}

void MarkupWindow::audio_file_selection_changed()
{
    draw_audio();
    set_mode(ui->controls->get_current_mode());
    load_markups();
}

void MarkupWindow::markups_changed(SampleKey sample_key)
{
    qDebug() << Q_FUNC_INFO;
    Q_UNUSED(sample_key)
    load_markups();
}

bool MarkupWindow::is_intersected_except(const Markup::SampleDetails &sample_details, double left, double right, MarkupKey except_markup_key)  const noexcept
{
    for (const auto &m : sample_details.markups) {
        if (m.key == except_markup_key){
            continue;
        }
        if (
                ((left >= m.left && left  <= m.right)
             || (right >= m.left && right <= m.right)
             || (left  <= m.left && right >= m.right))
            ){
                return true;
        }
    }
    return false;
}

bool MarkupWindow::is_intersected_any(const Markup::SampleDetails &sample_details, double left, double right) const noexcept
{
    for (const auto &m : sample_details.markups) {
        if (
                ((left >= m.left && left  <= m.right)
             || (right >= m.left && right <= m.right)
             || (left  <= m.left && right >= m.right))
            ){
                return true;
        }
    }
    return false;
}

std::optional<std::tuple<MarkupKey, QCPItemRect*>> MarkupWindow::get_selected_markup()
{
    auto selected_items = ui->plot->selectedItems();
    if (selected_items.length() != 1) {
        return {};
    }
    auto item = selected_items[0];
    auto markup_key_optional = markups_model->get_markup_key(item);
    if (!markup_key_optional.has_value()) {
        return {};
    }
    auto markup_key = markup_key_optional.value();
    return {{markup_key, static_cast<QCPItemRect*>(item)}};
}

void MarkupWindow::load_markups()
{
    markups_model->clear_markup_data();
    ui->plot->clearItems();

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

    for (const auto &markup : qAsConst(sample_details.markups)) {
        auto rect = new QCPItemRect(ui->plot);
        rect->setSelectedPen(QPen(Qt::red));
        rect->topLeft->setCoords(markup.left, 1);
        rect->bottomRight->setCoords(markup.right, -1);
        rect->setBrush(QBrush(Qt::gray, Qt::BrushStyle::Dense5Pattern));
        rect->setSelectedBrush(QBrush(Qt::red, Qt::BrushStyle::Dense5Pattern));

        markups_model->insert_markup(markup.key, markup.comment, static_cast<QCPAbstractItem*>(rect));
    }
    ui->plot->replot(QCustomPlot::RefreshPriority::rpQueuedReplot);
}

void MarkupWindow::draw_audio()
{
    auto file_selected_key = samples_provider->get_selected_file_key();
    if (!file_selected_key.has_value()){
        return;
    }
    auto samples = samples_provider->get_selected_file_samples();
    if (samples.isEmpty()) {
        return;
    }
    ui->plot->clearGraphs();
    ui->plot->clearItems();

    auto graph = ui->plot->addGraph();
    audio_view_cache->set_audio_raw(samples);

    auto [mode, mean_window] = ui->audio_view_mode_selector->get_mode();
    update_audio_view_mode(mode, mean_window);

    ui->plot->xAxis->setLabel("Sample index");
    ui->plot->yAxis->setLabel("y");
    ui->plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectItems);

    graph->rescaleAxes();
}

int MarkupWindow::get_max_key(const Markup::SampleDetails &sample_details)
{
    int max_key = -1;
    for (const auto &m : sample_details.markups) {
        if (m.key > max_key) {
            max_key = m.key;
        }
    }
    return max_key;
}

void MarkupWindow::markup_item_selected(QCPAbstractItem *item, QMouseEvent *event)
{
    Q_UNUSED(item)
    Q_UNUSED(event)
    auto file_selected_key = samples_provider->get_selected_file_key();
    if (!file_selected_key.has_value()){
        return;
    }
    auto selected = get_selected_markup();
    if (!selected.has_value()) {
        return;
    }

    auto [markup_key, rect] = selected.value();
    auto model_index_optional = markups_model->index_from_markup_key(markup_key);
    if (!model_index_optional.has_value()) {
        return;
    }
    auto model_index = model_index_optional.value();
    ui->listView_markups->setCurrentIndex(model_index);
}

void MarkupWindow::plot_add_pressed(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton) {
        return;
    }
    auto selected_file_key_option = samples_provider->get_selected_file_key();
    if (!selected_file_key_option.has_value()){
        return;
    }

    auto x = ui->plot->xAxis->pixelToCoord(event->position().x());

    auto selected_file_key = selected_file_key_option.value();
    auto sample_details_option = markup_provider->get_sample_details(selected_file_key);
    if (!sample_details_option.has_value()) {
        markup_provider->set_sample_details({selected_file_key, {}});
        sample_details_option = markup_provider->get_sample_details(selected_file_key);
    }
    auto sample_details = sample_details_option.value();

    if (!sample_details.markups.isEmpty()) {
        bool intersected = std::any_of(
            sample_details.markups.begin(),
            sample_details.markups.end(),
            [&](const Markup::Markup &m) { return x >= m.left && x <= m.right; }
        );
        if (intersected) {
            return;
        }
    }

    QCPItemStraightLine *start = new QCPItemStraightLine(ui->plot);
    start->point1->setCoords(x, 1);
    start->point2->setCoords(x, -1);
    start->setPen(QPen(Qt::red));

    QCPItemStraightLine *end = new QCPItemStraightLine(ui->plot);
    end->point1->setCoords(x, 1);
    end->point2->setCoords(x, -1);
    end->setPen(QPen(Qt::red));

    new_markup_data = NewMarkupData{x, start, end};
    connect(ui->plot, &QCustomPlot::mouseMove, this, &MarkupWindow::plot_add_moved);
}

void MarkupWindow::plot_add_moved(QMouseEvent *event)
{
    if (new_markup_data.has_value()) {
        auto x = ui->plot->xAxis->pixelToCoord(event->position().x());
        auto &data = new_markup_data.value();
        data.end->point1->setCoords(x, 1);
        data.end->point2->setCoords(x, -1);
        ui->plot->replot(QCustomPlot::RefreshPriority::rpQueuedReplot);
    }
}

void MarkupWindow::plot_add_released(QMouseEvent *event)
{
    disconnect(ui->plot, &QCustomPlot::mouseMove, this, &MarkupWindow::plot_add_moved);

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
    if (new_markup_data.has_value()) {
        auto x = ui->plot->xAxis->pixelToCoord(event->position().x());
        auto &data = new_markup_data.value();
        double start = x > data.start_x ? data.start_x : x;
        double end = x > data.start_x ? x : data.start_x;

        if (is_intersected_any(sample_details, start, end)){
            ui->plot->removeItem(data.start);
            ui->plot->removeItem(data.end);
            new_markup_data.reset();
            ui->plot->replot(QCustomPlot::RefreshPriority::rpQueuedReplot);
            return;
        }

        int max_key = get_max_key(sample_details);
        auto new_markup = Markup::Markup{
            max_key+1,
            static_cast<int>(start),
            static_cast<int>(end),
            QString("markup_") + QString::number(max_key+1)
        };
        markup_provider->set_markup(sample_details.filename, new_markup);
        new_markup_data.reset();
        load_markups();
    }
}

void MarkupWindow::plot_edit_pressed(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton) {
        return;
    }
    auto selected_file_key_option = samples_provider->get_selected_file_key();
    if (!selected_file_key_option.has_value()){
        return;
    }
    auto selected_file_key = selected_file_key_option.value();

    auto selected = get_selected_markup();
    if (!selected.has_value()) {
        return;
    }

    auto [markup_key, rect] = selected.value();

    auto sample_details_option = markup_provider->get_sample_details(selected_file_key);
    if (!sample_details_option.has_value()) {
        return;
    }
    auto sample_details = sample_details_option.value();

    double before_edit_left = rect->topLeft->key();
    double before_edit_right = rect->bottomRight->key();

    auto x = ui->plot->xAxis->pixelToCoord(event->position().x());
    auto markup = sample_details.markups[markup_key];

    double range = markup.right - markup.left;
    double active_range_coeff = 0.1;

    double left_start_range = markup.left - range * active_range_coeff;
    double left_end_range = markup.left + range * active_range_coeff;

    double right_start_range = markup.right - range * active_range_coeff;
    double right_end_range = markup.right + range * active_range_coeff;

    bool is_left_edit = false;
    if (x >= left_start_range && x <= left_end_range) {
        is_left_edit = true;
    } else if (x >= right_start_range && x <= right_end_range) {
        is_left_edit = false;
    } else {
        return;
    }

    if (is_left_edit){
        rect->topLeft->setCoords(x, 1.0);
    } else {
        rect->bottomRight->setCoords(x, -1.0);
    }

    edit_markup_data = EditMarkupData{
        markup_key,
        rect,
        is_left_edit,
        before_edit_left,
        before_edit_right
    };

    ui->plot->replot(QCustomPlot::RefreshPriority::rpQueuedReplot);
    connect(ui->plot, &QCustomPlot::mouseMove, this, &MarkupWindow::plot_edit_moved);
}

void MarkupWindow::plot_edit_moved(QMouseEvent *event)
{
    auto file_selected_key = samples_provider->get_selected_file_key();
    if (!file_selected_key.has_value()){
        return;
    }

    if (!edit_markup_data.has_value()) {
        return;
    }

    auto selected_file_key_option = samples_provider->get_selected_file_key();
    if (!selected_file_key_option.has_value()){
        return;
    }
    auto selected_file_key = selected_file_key_option.value();

    auto sample_details_option = markup_provider->get_sample_details(selected_file_key);
    if (!sample_details_option.has_value()) {
        return;
    }
    auto sample_details = sample_details_option.value();

    auto x = ui->plot->xAxis->pixelToCoord(event->position().x());
    auto &edit_markup = edit_markup_data.value();
    auto markup = sample_details.markups[edit_markup.markup_key];

    if (edit_markup.is_left_edit) {
        if (x > edit_markup.rect->bottomRight->key()) {
            edit_markup.is_left_edit = false;
            edit_markup.rect->bottomRight->setCoords(x, -1.0);
        } else {
            edit_markup.rect->topLeft->setCoords(x, 1.0);
        }
    } else {
        if (x < edit_markup.rect->topLeft->key()) {
            edit_markup.is_left_edit = true;
            edit_markup.rect->topLeft->setCoords(x, 1.0);
        } else {
            edit_markup.rect->bottomRight->setCoords(x, -1.0);
        }
    }
    ui->plot->replot(QCustomPlot::RefreshPriority::rpQueuedReplot);
}

void MarkupWindow::plot_edit_released(QMouseEvent *event)
{
    Q_UNUSED(event)
    disconnect(ui->plot, &QCustomPlot::mouseMove, this, &MarkupWindow::plot_edit_moved);

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

    if (!edit_markup_data.has_value()) {
        return;
    }

    auto &edit_markup = edit_markup_data.value();

    if (!is_intersected_except(
            sample_details,
            edit_markup.rect->topLeft->key(),
            edit_markup.rect->bottomRight->key(),
            edit_markup.markup_key)
    ){
        auto markup = sample_details.markups[edit_markup.markup_key];
        markup.left = edit_markup.rect->topLeft->key();
        markup.right = edit_markup.rect->bottomRight->key();
        edit_markup_data.reset();
        markup_provider->set_markup(selected_file_key, markup);
        return;
    }

    edit_markup_data.reset();
    load_markups();
}

void MarkupWindow::on_listView_markups_clicked(const QModelIndex &index)
{
    auto markup_key_option = markups_model->get_markup_key(index);
    if (!markup_key_option.has_value()) {
        return;
    }
    auto markup_key = markup_key_option.value();

    auto rect_optional = markups_model->get_item(markup_key);
    if (!rect_optional.has_value()) {
        QMessageBox::warning(this,
            "Markup can't be selected",
            "Markup can't be selected",
            QMessageBox::Ok
        );
        return;
    }
    auto rect = static_cast<QCPItemRect*>(rect_optional.value());
    ui->plot->deselectAll();
    rect->setSelected(true);

    double leftZoom = rect->topLeft->key();
    double rightZoom = rect->bottomRight->key();
    double paddingCoef = 0.1;
    double range = rightZoom - leftZoom;

    ui->plot->graph()->keyAxis()->setRange(leftZoom - range * paddingCoef, rightZoom + range * paddingCoef);
    ui->plot->graph()->rescaleValueAxis();

    ui->plot->replot(QCustomPlot::RefreshPriority::rpQueuedReplot);
}

void MarkupWindow::on_pushButton_save_clicked()
{
    auto file_selected_key = samples_provider->get_selected_file_key();
    if (!file_selected_key.has_value()){
        return;
    }
    markup_provider->save();
}

void MarkupWindow::keyPressEvent(QKeyEvent *event)
{
    auto file_selected_key_option = samples_provider->get_selected_file_key();
    if (!file_selected_key_option.has_value()){
        return;
    }
    auto file_selected_key = file_selected_key_option.value();
    if (event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace) {
        auto selected = get_selected_markup();
        if (!selected.has_value()) {
            return;
        }

        auto [markup_key, _] = selected.value();
        markup_provider->remove_markup(file_selected_key, markup_key);
    }
}


void MarkupWindow::update_audio_view_mode(AudioViewMode mode, std::optional<int> mean_window)
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

