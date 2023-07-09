#ifndef MARKUP_WINDOW_H
#define MARKUP_WINDOW_H

#include <QWidget>
#include <QStringListModel>
#include <QVector>
#include "common/markup.h"
#include "common/audio_file.h"
#include "../qcustomplot/qcustomplot.h"
#include "../widgets/audio_view_mode_selector.h"
#include "../widgets/graph_controls.h"
#include "../interfaces/i_samples_provider.h"
#include "../interfaces/i_markup_provider.h"
#include "../processing/markup_list_model.h"
#include "../helpers/markup_draw_helper.h"
#include "../helpers/audio_view_cache.h"

namespace Ui {
    class MarkupWindow;
}

class MarkupWindow : public QWidget
{
    Q_OBJECT

    struct NewMarkupData {
        double start_x;
        QCPItemStraightLine *start = nullptr;
        QCPItemStraightLine *end = nullptr;
    };

    struct EditMarkupData {
        int markup_key;
        QCPItemRect *rect;
        bool is_left_edit;
        double before_edit_left;
        double before_edit_right;
    };

public:
    explicit MarkupWindow(
        QWidget *parent,
        std::shared_ptr<ISamplesProvider> _samples_provider,
        std::shared_ptr<IMarkupProvider> _markup_provider);

    ~MarkupWindow();

    void set_mode(GraphControls::Mode mode);

private slots:
    void audio_file_selection_changed();
    void markups_changed(SampleKey sample_key);

    void on_listView_markups_clicked(const QModelIndex &index);
    void markup_item_selected(QCPAbstractItem *item, QMouseEvent *event);

    void plot_add_pressed(QMouseEvent *event);
    void plot_add_moved(QMouseEvent *event);
    void plot_add_released(QMouseEvent *event);

    void plot_edit_pressed(QMouseEvent *event);
    void plot_edit_moved(QMouseEvent *event);
    void plot_edit_released(QMouseEvent *event);

    void on_pushButton_save_clicked();
    void keyPressEvent(QKeyEvent *event) override;
    void update_audio_view_mode(AudioViewMode mode, std::optional<int> mean_window);

private:
    bool is_intersected_except(const Markup::SampleDetails &sample_details, double left, double right, MarkupKey markup_key) const noexcept;
    bool is_intersected_any(const Markup::SampleDetails &sample_details, double left, double right) const noexcept;

    std::optional<std::tuple<MarkupKey, QCPItemRect*>>
        get_selected_markup();

    void load_markups();
    void draw_audio();
    int get_max_key(const Markup::SampleDetails &sample_details);

private:
    Ui::MarkupWindow *ui;
    MarkupListModel *markups_model;

    std::shared_ptr<ISamplesProvider> samples_provider;
    std::shared_ptr<IMarkupProvider> markup_provider;

    std::unique_ptr<AudioViewCache> audio_view_cache;

    std::optional<NewMarkupData> new_markup_data;
    std::optional<EditMarkupData> edit_markup_data;

    std::unique_ptr<MarkupDrawHelper> markup_draw_helper;
};

#endif // MARKUP_WINDOW_H
