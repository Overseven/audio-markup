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

    void init_variables(QJSEngine &engine, QVector<double> &input_samples) const noexcept;
    QVector<OutputSeries> get_output_series(QJSEngine &engine) const noexcept;
    QVector<OutputRanges> get_output_ranges(QJSEngine &engine) const noexcept;
    QVector<OutputValue> get_output_values(QJSEngine &engine) const noexcept;

private:
    const QString system_var_obj_label = "system_var";
    const QString input_samples_label = "input_samples";
    const QString output_series_label = "output_series";
    const QString output_ranges_label = "output_ranges";
    const QString output_values_label = "output_values";
};

#endif // JS_GLOBAL_VARIABLE_PROVIDER_H
