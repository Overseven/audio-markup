#include "result_window.h"
#include "ui_result_window.h"

ResultWindow::ResultWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ResultWindow)
{
    ui->setupUi(this);
    ui->plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
}

ResultWindow::~ResultWindow()
{
    delete ui;
}

void ResultWindow::draw_results(QVector<double> &samples, Markup::SampleDetails &sample_details, ExecutionResult &processing_result)
{
    ui->plot->clearGraphs();
    auto graph = ui->plot->addGraph();
    QVector<double> x(samples.count());
    for (int i = 0; i < x.length(); i++) {
        x[i] = i;
    }
    graph->setData(x, samples, true);

    for (auto &markup : sample_details.markups){
        draw_markup(markup.left, markup.right, Qt::gray);
    }

    for (auto &ranges : processing_result.ranges){
        for (auto &range : ranges.ranges){
            draw_markup(range.start, range.end, Qt::blue);
        }
    }
    ui->plot->rescaleAxes();
    ui->plot->replot(QCustomPlot::RefreshPriority::rpQueuedReplot);
}

void ResultWindow::draw_markup(double left, double right, QColor color)
{
    auto rect = new QCPItemRect(ui->plot);
    rect->setPen(QPen(color));
    rect->topLeft->setCoords(left, 1);
    rect->bottomRight->setCoords(right, -1);
    rect->setBrush(QBrush(color, Qt::BrushStyle::Dense5Pattern));

    ui->plot->replot(QCustomPlot::RefreshPriority::rpQueuedReplot);
}
