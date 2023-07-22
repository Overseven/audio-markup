#ifndef JS_GLOBAL_VARIABLE_PROVIDER_H
#define JS_GLOBAL_VARIABLE_PROVIDER_H

#include <QString>
#include <QVector>
#include <QJSEngine>
#include "../processing/common.h"

class JsGlobalVariableProvider
{
public:
    JsGlobalVariableProvider();

    void init_variables(QJSEngine &engine, QVector<double> &input_samples) const noexcept;

    OutputRanges get_output_ranges(QJSEngine &engine) const noexcept;

    QVector<OutputSeries> get_debug_series(QJSEngine &engine) const noexcept;
    QVector<OutputRanges> get_debug_ranges(QJSEngine &engine) const noexcept;
    QVector<OutputValue>  get_debug_values(QJSEngine &engine) const noexcept;

private:
    const QString system_var_obj_label = "system_var";
    const QString input_samples_label = "input_samples";
    const QString output_ranges_label = "output_ranges";
    const QString debug_series_label = "debug_series";
    const QString debug_ranges_label = "debug_ranges";
    const QString debug_values_label = "debug_values";
};

#endif // JS_GLOBAL_VARIABLE_PROVIDER_H
