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
    samples_model = new QStringListModel(this);
    markups_model = new MarkupListModel(this);

    ui->listView_audio_files->setModel(samples_model);
    ui->listView_markups->setModel(markups_model);

    samples_provider->connect_to_signal_samples_list_changed(this, SLOT(samples_list_changed()), true);
    samples_provider->connect_to_signal_selection_changed(this, SLOT(file_selection_changed()), true);
    markup_provider->connect_to_signal_markups_changed(this, SLOT(markups_changed(SampleKey)), true);
    connect(ui->controls, &GraphControls::mode_updated, this, &MarkupWindow::set_mode);
    connect(ui->plot, &QCustomPlot::itemClick, this, &MarkupWindow::markup_item_selected);
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

void MarkupWindow::samples_list_changed()
{
    samples_model->removeRows(0, samples_model->rowCount());
    auto sample_files = samples_provider->get_files_list();
    samples_model->setStringList(sample_files);
}

void MarkupWindow::file_selection_changed()
{
    draw_audio();
    set_mode(ui->controls->get_current_mode());
    load_markups();
}

void MarkupWindow::markups_changed(SampleKey sample_key)
{
    Q_UNUSED(sample_key)
    load_markups();
}

bool MarkupWindow::is_intersected(const Markup::SampleDetails &sample_details, double left, double right, int markup_key)  const noexcept
{
    if (!sample_details.markups.contains(markup_key)) {
        return false;
    }
    auto m = sample_details.markups[markup_key];
    return ((left >= m.left && left <= m.right)
        || (right >= m.left && right <= m.right)
        || (left <= m.left && right >= m.right));
}

bool MarkupWindow::is_intersected_any(const Markup::SampleDetails &sample_details, double left, double right) const noexcept
{
    for (const auto &m : sample_details.markups) {
        if (is_intersected(sample_details, left, right, m.key)){
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
    markups_model->clear_markup_data();
    ui->plot->clearItems();

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
    if (samples_provider->get_selected_file_samples().isEmpty()) {
        return;
    }
    ui->plot->clearGraphs();
    ui->plot->clearItems();

    auto graph = ui->plot->addGraph();
    samples_abs.reset();
    samples_mean.reset();
    on_pushButton_view_mode_set_clicked();

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

void MarkupWindow::update_samples_abs()
{
    auto samples = samples_provider->get_selected_file_samples();
    for (auto i = 0; i < samples.length(); i++) {
        samples[i] = abs(samples[i]);
    }
    samples_abs = samples;
}

void MarkupWindow::update_samples_mean(int window_len)
{
    auto vec = samples_abs.value();
    for (auto i = 0; i < window_len/2 - 1; i++) {
        vec[i] = 0;
        vec[vec.length() - i - 1] = 0;
    }
    for (auto i = window_len/2; i < vec.length() - window_len/2; i++) {
        double sum = 0;
        for (auto j = i - window_len/2; j < i + window_len/2; j++) {
            sum += vec[j];
        }
        vec[i] = sum / window_len;
    }
    samples_mean = std::tuple<QVector<double>, int>(vec, window_len);
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
        return;
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
        markup.left = x;
    } else {
        rect->bottomRight->setCoords(x, -1.0);
        markup.right = x;
    }

    markup_provider->set_markup(selected_file_key, markup);

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

    markup.left = edit_markup.rect->topLeft->key();
    markup.right = edit_markup.rect->bottomRight->key();
    markup_provider->set_markup(selected_file_key, markup);
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

    if (is_intersected(
            sample_details,
            edit_markup.rect->topLeft->key(),
            edit_markup.rect->bottomRight->key(),
            edit_markup.markup_key)
    ){
        edit_markup.rect->topLeft->setCoords(edit_markup.before_edit_left, 1);
        edit_markup.rect->bottomRight->setCoords(edit_markup.before_edit_right, -1);
        auto markup = sample_details.markups[edit_markup.markup_key];
        markup.left = edit_markup.before_edit_left;
        markup.right = edit_markup.before_edit_right;
        edit_markup_data.reset();
        ui->plot->replot(QCustomPlot::RefreshPriority::rpQueuedReplot);
        return;
    }

    edit_markup_data.reset();
    ui->plot->replot(QCustomPlot::RefreshPriority::rpQueuedReplot);
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


void MarkupWindow::on_pushButton_view_mode_set_clicked()
{
    auto file_selected_key = samples_provider->get_selected_file_key();
    if (!file_selected_key.has_value()){
        return;
    }

    auto samples = samples_provider->get_selected_file_samples();
    QVector<double> x(samples.count());
    for (int i = 0; i < x.length(); i++) {
        x[i] = i;
    }

    if (ui->radioButton_view_mode_raw->isChecked()) {
        ui->plot->graph()->setData(x, samples);

    } else if (ui->radioButton_view_mode_abs->isChecked()) {
        if (!samples_abs.has_value()) {
            update_samples_abs();
        }
        ui->plot->graph()->setData(x, samples_abs.value());

    } else if (ui->radioButton_view_mode_mean->isChecked()) {
        bool ok = true;
        int window_len = ui->lineEdit_mean_window->text().toInt(&ok);
        if (!ok) {
            QMessageBox::warning(this,
                "Window len error",
                "Window len can't parsed",
                QMessageBox::Ok
            );
            return;
        }
        if (window_len < 3) {
            QMessageBox::warning(this,
                "Window len error",
                "Window len must be >= 3",
                QMessageBox::Ok
            );
            return;
        }
        if (window_len % 2 == 0) {
            QMessageBox::warning(this,
                "Window len error",
                "Window len must be odd",
                QMessageBox::Ok
            );
            return;
        }

        if (!samples_abs.has_value()) {
            update_samples_abs();
        }
        if (!samples_mean.has_value()) {
            update_samples_mean(window_len);
        } else {
            auto [vec, window] = samples_mean.value();
            if (window != window_len) {
                update_samples_mean(window_len);
            }
        }
        auto [meaned, _] = samples_mean.value();
        ui->plot->graph()->setData(x, meaned);
    }

    ui->plot->replot(QCustomPlot::RefreshPriority::rpQueuedReplot);
}

void MarkupWindow::on_listView_audio_files_clicked(const QModelIndex &index)
{
    auto sample_key = samples_model->data(index).toString();
    qDebug() << Q_FUNC_INFO << "index:" << index.row() << "sample_key:" << sample_key;
    samples_provider->select_sample(sample_key);
}

