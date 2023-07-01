#ifndef GRAPH_CONTROLS_H
#define GRAPH_CONTROLS_H

#include <QWidget>

namespace Ui {
class GraphControls;
}

class GraphControls : public QWidget
{
    Q_OBJECT
public:
    enum Mode {
        Move,
        Add,
        Edit
    };

    explicit GraphControls(QWidget *parent = nullptr);
    ~GraphControls();

    Mode get_current_mode();

signals:
    void mode_updated(GraphControls::Mode mode);

private slots:
    void on_pushButton_add_clicked();

    void on_pushButton_move_clicked();

    void on_pushButton_edit_clicked();

private:
    void set_mode(Mode mode);

private:
    Mode mode = Mode::Move;
    Ui::GraphControls *ui;
};

#endif // GRAPH_CONTROLS_H
