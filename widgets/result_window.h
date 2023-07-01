#ifndef RESULT_WINDOW_H
#define RESULT_WINDOW_H

#include <QWidget>
#include "../processing/common.h"
#include "../common/markup.h"

namespace Ui {
class ResultWindow;
}

class ResultWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ResultWindow(QWidget *parent = nullptr);
    ~ResultWindow();

    void draw_results(QVector<double> &samples, Markup::SampleDetails &sample_details, ExecutionResult &processing_result);

private:
    void draw_markup(double left, double right, QColor color);
private:
    Ui::ResultWindow *ui;
};

#endif // RESULT_WINDOW_H
