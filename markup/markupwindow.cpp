#include "markupwindow.h"
#include "ui_markupwindow.h"
#include <QMessageBox>

typedef int Key;
typedef int ModelIdx;

MarkupWindow::MarkupWindow(QWidget *parent, Markup::MarkupData *_markup_data) :
    QWidget(parent),
    ui(new Ui::MarkupWindow),
    markup_data(_markup_data)
{
    ui->setupUi(this);
    files_model = new QStringListModel(this);
    markups_model = new QStringListModel(this);

    ui->listView_audio_files->setModel(files_model);
    ui->listView_markups->setModel(markups_model);

    connect(ui->listView_audio_files, &QListView::clicked, this, &MarkupWindow::file_selection_changed);
    connect(ui->controls, &GraphControls::mode_updated, this, &MarkupWindow::set_mode);
    connect(ui->plot, &QCustomPlot::itemClick, this, &MarkupWindow::markup_item_selected);
}

MarkupWindow::~MarkupWindow()
{
    Markup::json::save_markup_file(Markup::json::filename, *markup_data);
}

void MarkupWindow::sample_details_updated()
{
    file_selected_idx.reset();
    files_model->removeRows(0, files_model->rowCount());
    audio_file = AudioFile<double>();
    QStringList list;
    for (auto const &elem : markup_data->sample_details){
        list << elem.filename;
    }
    files_model->setStringList(list);
}

void MarkupWindow::load_audio_file(QString &directory, QString &filename)
{
    QString file = directory + "/" + filename;
    bool is_loaded = audio_file.load(file.toStdString());
    if (!is_loaded) {
        QMessageBox::warning(this,
            "Can't open audio file",
            QString("Can't open audio file: ") + file,
            QMessageBox::Ok
        );
    }
    draw_audio();
    set_mode(ui->controls->get_current_mode());
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

void MarkupWindow::file_selection_changed(const QModelIndex &index)
{
    if (file_selected_idx == index.row()) {
        return;
    }
    file_selected_idx = index.row();
    load_audio_file(markup_data->dir, markup_data->sample_details[file_selected_idx.value()].filename);
    load_markups();
}

bool MarkupWindow::is_intersected(double left, double right, int markup_key)
{
    const auto &markups = markup_data->sample_details[file_selected_idx.value()].markups;
    if (!markups.isEmpty()) {
        for (int i = 0; i < markups.length(); i++) {
            const auto &m = markups[i];
            if (
                m.key != markup_key &&
                ((left >= m.left && left <= m.right)
                || (right >= m.left && right <= m.right)
                || (left <= m.left && right >= m.right))
            ){
                return true;
            }
        }
    }
    return false;
}

std::optional<std::tuple<Key, ModelIdx, QCPItemRect *>> MarkupWindow::get_selected_markup()
{
    auto selected_items = ui->plot->selectedItems();
    if (selected_items.length() != 1) {
        return {};
    }

    for (int markup_key : markups_map.keys()) {
        auto [model_idx, markup] = markups_map[markup_key];
        if (markup == selected_items.first()) {
            return {{ markup_key,  model_idx, (QCPItemRect*)selected_items.first() }};
        }
    }
    return {};
}

void MarkupWindow::load_markups()
{
    if (!file_selected_idx.has_value()) {
        return;
    }
    markups_map.clear();

    ui->plot->clearItems();
    auto &markups = markup_data->sample_details[file_selected_idx.value()].markups;
    for (int i = 0; i < markups.length(); i++) {
        const auto &markup = markups[i];
        auto rect = new QCPItemRect(ui->plot);
        rect->setSelectedPen(QPen(Qt::red));
        rect->topLeft->setCoords(markup.left, 1);
        rect->bottomRight->setCoords(markup.right, -1);
        rect->setBrush(QBrush(Qt::gray, Qt::BrushStyle::Dense5Pattern));
        rect->setSelectedBrush(QBrush(Qt::red, Qt::BrushStyle::Dense5Pattern));
        markups_map.insert(markup.key, {i, rect});
    }

    QStringList list;
    for (const auto &markup : markups) {
        list << markup.comment;
    }
    markups_model->setStringList(list);
    ui->plot->replot(QCustomPlot::RefreshPriority::rpQueuedReplot);
}

void MarkupWindow::draw_audio()
{
    if (!file_selected_idx.has_value()) {
        return;
    }
    if (audio_file.getNumChannels() == 0) {
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

int MarkupWindow::get_max_key()
{
    int max_key = -1;
    for (const auto &m : markup_data->sample_details[file_selected_idx.value()].markups) {
        if (m.key > max_key) {
            max_key = m.key;
        }
    }
    return max_key;
}

Markup::Markup* MarkupWindow::get_markup_with_key(int key)
{
    auto &markups = markup_data->sample_details[file_selected_idx.value()].markups;
    for (int i = 0; i < markups.length(); i++) {
        if (markups[i].key == key) {
            return &markups[i];
        }
    }
    return nullptr;
}

void MarkupWindow::update_samples_abs()
{
    auto vec = audio_file.samples[0];
    for (auto i = 0; i < vec.length(); i++) {
        vec[i] = abs(vec[i]);
    }
    samples_abs = vec;
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
    if (!file_selected_idx.has_value()){
        return;
    }
    auto selected = get_selected_markup();
    if (!selected.has_value()) {
        return;
    }

    auto [key, model_idx, rect] = selected.value();
    auto idx = markups_model->index(model_idx);
    ui->listView_markups->setCurrentIndex(idx);
}

void MarkupWindow::plot_add_pressed(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton) {
        return;
    }
    if (!file_selected_idx.has_value()) {
        return;
    }

    auto x = ui->plot->xAxis->pixelToCoord(event->position().x());

    const auto &markups = markup_data->sample_details[file_selected_idx.value()].markups;

    if (!markups.isEmpty()) {
        bool intersected = std::any_of(
            markups.begin(),
            markups.end(),
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

    if (new_markup_data.has_value() && file_selected_idx.has_value()) {
        auto x = ui->plot->xAxis->pixelToCoord(event->position().x());
        auto &data = new_markup_data.value();
        double start = x > data.start_x ? data.start_x : x;
        double end = x > data.start_x ? x : data.start_x;

        if (is_intersected(start, end)){
            ui->plot->removeItem(data.start);
            ui->plot->removeItem(data.end);
            new_markup_data.reset();
            ui->plot->replot(QCustomPlot::RefreshPriority::rpQueuedReplot);
            return;
        }

        int max_key = get_max_key();
        markup_data->sample_details[file_selected_idx.value()].markups
            .append(Markup::Markup{
                max_key+1,
                static_cast<int>(start),
                static_cast<int>(end),
                QString("markup_") + QString::number(max_key+1)
            }
        );
        load_markups();

        ui->plot->removeItem(data.start);
        ui->plot->removeItem(data.end);
        ui->plot->replot(QCustomPlot::RefreshPriority::rpQueuedReplot);

        new_markup_data.reset();
    }
}

void MarkupWindow::plot_edit_pressed(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton) {
        return;
    }
    if (!file_selected_idx.has_value()) {
        return;
    }

    auto selected = get_selected_markup();
    if (!selected.has_value()) {
        return;
    }

    auto [key, model_idx, rect] = selected.value();

    double before_edit_left = rect->topLeft->key();
    double before_edit_right = rect->bottomRight->key();

    auto x = ui->plot->xAxis->pixelToCoord(event->position().x());
    auto markup = get_markup_with_key(key);

    double range = markup->right - markup->left;
    double active_range_coeff = 0.1;

    double left_start_range = markup->left - range * active_range_coeff;
    double left_end_range = markup->left + range * active_range_coeff;

    double right_start_range = markup->right - range * active_range_coeff;
    double right_end_range = markup->right + range * active_range_coeff;

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
        markup->left = x;
    } else {
        rect->bottomRight->setCoords(x, -1.0);
        markup->right = x;
    }

    edit_markup_data = EditMarkupData{
        key,
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
    if (!file_selected_idx.has_value()) {
        return;
    }

    if (!edit_markup_data.has_value()) {
        return;
    }

    auto x = ui->plot->xAxis->pixelToCoord(event->position().x());
    auto &edit_markup = edit_markup_data.value();
    auto markup = get_markup_with_key(edit_markup.markup_key); // markup_data->sample_details[file_selected_idx.value()].markups[edit_markup.markup_id];

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

    markup->left = edit_markup.rect->topLeft->key();
    markup->right = edit_markup.rect->bottomRight->key();

    ui->plot->replot(QCustomPlot::RefreshPriority::rpQueuedReplot);
}

void MarkupWindow::plot_edit_released(QMouseEvent *event)
{
    Q_UNUSED(event)
    disconnect(ui->plot, &QCustomPlot::mouseMove, this, &MarkupWindow::plot_edit_moved);

    if (!file_selected_idx.has_value()) {
        return;
    }

    if (!edit_markup_data.has_value()) {
        return;
    }

    auto &edit_markup = edit_markup_data.value();

    if (is_intersected(
            edit_markup.rect->topLeft->key(),
            edit_markup.rect->bottomRight->key(),
            edit_markup.markup_key)
    ){
        edit_markup.rect->topLeft->setCoords(edit_markup.before_edit_left, 1);
        edit_markup.rect->bottomRight->setCoords(edit_markup.before_edit_right, -1);
        auto markup = get_markup_with_key(edit_markup.markup_key);
        markup->left = edit_markup.before_edit_left;
        markup->right = edit_markup.before_edit_right;
        edit_markup_data.reset();
        ui->plot->replot(QCustomPlot::RefreshPriority::rpQueuedReplot);
        return;
    }

    edit_markup_data.reset();
    ui->plot->replot(QCustomPlot::RefreshPriority::rpQueuedReplot);
}

void MarkupWindow::on_listView_markups_clicked(const QModelIndex &index)
{
    int markup_idx = index.row();

    int markup_key = markup_data->sample_details[file_selected_idx.value()].markups[markup_idx].key;

    if (!markups_map.contains(markup_key)) {
        QMessageBox::warning(this,
            "Markup can't be selected",
            "Markup can't be selected",
            QMessageBox::Ok
        );
        return;
    }
    auto [model_id, rect] = markups_map[markup_key];
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
    if (!file_selected_idx.has_value()) {
        return;
    }
    Markup::json::save_markup_file(Markup::json::filename, *markup_data);
}

void MarkupWindow::keyPressEvent(QKeyEvent *event)
{
    if (!file_selected_idx.has_value()) {
        return;
    }
    if (event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace) {
        auto selected = get_selected_markup();
        if (!selected.has_value()) {
            return;
        }

        auto [markup_key, model_idx, markup] = selected.value();
        markup_data->sample_details[file_selected_idx.value()].markups.removeAt(model_idx);
        load_markups();
    }
}


void MarkupWindow::on_pushButton_view_mode_set_clicked()
{
    if (!file_selected_idx.has_value()) {
        return;
    }

    QVector<double> x(audio_file.samples[0].count());
    for (int i = 0; i < x.length(); i++) {
        x[i] = i;
    }

    if (ui->radioButton_view_mode_raw->isChecked()) {
        ui->plot->graph()->setData(x, audio_file.samples[0]);

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

