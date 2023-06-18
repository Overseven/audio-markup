#ifndef JS_GLOBAL_VARIABLE_PROVIDER_H
#define JS_GLOBAL_VARIABLE_PROVIDER_H

#include <QString>
#include <QVector>
#include <QJSEngine>
#include "common.h"

class JsGlobalVariableProvider
{
public:
    JsGlobalVariableProvider();

    void set_sample_data(const QVector<double> &data);
    std::tuple<QString, QJSValue> get_sys_variables_definition() const noexcept;
    QVector<OutputSeries> get_output_series(QJSEngine &engine) const noexcept;
    QVector<OutputRanges> get_output_ranges(QJSEngine &engine) const noexcept;

private:
    const QString input_samples = "input_samples";
    const QString output_series = "sys_output_series";
    const QString output_ranges = "sys_output_ranges";
};

#endif // JS_GLOBAL_VARIABLE_PROVIDER_H
