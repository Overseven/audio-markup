#ifndef MARKUPDRAWHELPER_H
#define MARKUPDRAWHELPER_H

#include "../qcustomplot/qcustomplot.h"

class MarkupDrawHelper
{
public:
    MarkupDrawHelper(QCustomPlot *_plot);

    void plot_add_pressed();
    void plot_add_moved();
    void plot_add_released();

private:
    QCustomPlot *plot;
};

#endif // MARKUPDRAWHELPER_H
