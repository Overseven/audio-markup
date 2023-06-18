#ifndef EXECUTOR_COMMON_H
#define EXECUTOR_COMMON_H

#include <QString>
#include <QVector>
#include <QJSValue>

struct JsFunction {
    QString name;
    QString filename;
    QString code;
};

struct OutputSeries {
    QString name;
    QVector<double> data;
};

struct Range {
    int start;
    int end;
};

struct OutputRanges {
    QString name;
    QVector<Range> ranges;
};

struct ExecutionResult {
    QJSValue output;
    QVector<OutputSeries> series;
    QVector<OutputRanges> ranges;
};

#endif // EXECUTOR_COMMON_H
