#ifndef MARKUPWINDOW_H
#define MARKUPWINDOW_H

#include <QWidget>
#include <QStringListModel>
#include <QVector>
#include "common/markup.h"
#include "common/AudioFile.h"
#include "qcustomplot/qcustomplot.h"
#include "graphcontrols.h"

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
    explicit MarkupWindow(QWidget *parent, Markup::MarkupData *_markup_data);
    ~MarkupWindow();

    void sample_details_updated();
    void load_audio_file(QString &directory, QString &filename);
    void set_mode(GraphControls::Mode mode);

private slots:
    void on_listView_markups_clicked(const QModelIndex &index);
    void selection_changed_by_user();
    void markup_item_selected(QCPAbstractItem *item, QMouseEvent *event);

    void plot_double_clicked(QMouseEvent *event);

    void plot_add_pressed(QMouseEvent *event);
    void plot_add_moved(QMouseEvent *event);
    void plot_add_released(QMouseEvent *event);

    void plot_edit_pressed(QMouseEvent *event);
    void plot_edit_moved(QMouseEvent *event);
    void plot_edit_released(QMouseEvent *event);

    void on_pushButton_save_clicked();
    void keyPressEvent(QKeyEvent *event) override;
    void on_pushButton_view_mode_set_clicked();

private:
    void file_selection_changed(const QModelIndex &index);

    bool is_intersected(double left, double right, int markup_id = -1);

    std::optional<std::tuple<int, int, QCPItemRect*>>
        get_selected_markup();

    void load_markups();
    void draw_audio();
    int get_max_key();
    Markup::Markup* get_markup_with_key(int key);

    void update_samples_abs();
    void update_samples_mean(int window_len);

private:
    Ui::MarkupWindow *ui;
    std::optional<int> file_selected_idx;
    QStringListModel *files_model;
    QStringListModel *markups_model;
    Markup::MarkupData *markup_data;
    AudioFile<double> audio_file;
    // key -> [ model_idx, rect ]
    QMap<int, std::tuple<int, QCPItemRect*>> markups_map;
    std::optional<QVector<double>> samples_abs;
    std::optional<std::tuple<QVector<double>, int>> samples_mean;
    std::optional<NewMarkupData> new_markup_data;
    std::optional<EditMarkupData> edit_markup_data;
};

#endif // MARKUPWINDOW_H
