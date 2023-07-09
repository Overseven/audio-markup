#include "js_global_variable_provider.h"

JsGlobalVariableProvider::JsGlobalVariableProvider()
{

}

void JsGlobalVariableProvider::init_variables(QJSEngine &engine, QVector<double> &input_samples) const noexcept
{
    QJSValue input_samples_js_array = engine.newArray(input_samples.length());
    for (int i = 0; i < input_samples.length(); i++) {
        input_samples_js_array.setProperty(i, input_samples[i]);
    }

    QJSValue sys_obj = engine.newObject();
    sys_obj.setProperty(input_samples_label, input_samples_js_array);
    sys_obj.setProperty(output_series_label, engine.newArray());
    sys_obj.setProperty(output_ranges_label, engine.newArray());
    sys_obj.setProperty(output_values_label, engine.newArray());
    engine.globalObject().setProperty(system_var_obj_label, sys_obj);
}

QVector<OutputSeries> JsGlobalVariableProvider::get_output_series(QJSEngine &engine) const noexcept
{
    auto js_value = engine.evaluate(system_var_obj_label + "." + output_series_label);
    if (!js_value.isArray()) {
        return {};
    }
    auto length = js_value.property(QStringLiteral("length")).toInt();
    QVector<OutputSeries> result(length);
    for (int i = 0; i < length; i++) {
        auto elem = js_value.property(i);
        if (!elem.hasProperty("label")) {
            qDebug() << "get_output_series: series has no \"label\" field";
            return {};
        }
        auto label = elem.property("label").toString();
        auto js_series = elem.property("data");
        auto data_len = js_series.property(QStringLiteral("length")).toInt();
        QVector<double> data(data_len);
        for (int j = 0; j < data_len; j++) {
            auto x = js_series.property(j).toNumber();
            data[j] = x;
        }
        result[i] = {label, data};
    }
    return result;
}

QVector<OutputRanges> JsGlobalVariableProvider::get_output_ranges(QJSEngine &engine) const noexcept
{
    auto js_value = engine.evaluate(system_var_obj_label + "." + output_ranges_label);
    if (!js_value.isArray()) {
        return {};
    }
    auto length = js_value.property(QStringLiteral("length")).toInt();
    QVector<OutputRanges> result(length);
    for (int i = 0; i < length; i++) {
        auto elem = js_value.property(i);
        if (!elem.hasProperty("label")) {
            qDebug() << "get_output_series: series has no \"label\" field";
            return {};
        }
        auto label = elem.property("label").toString();
        auto js_series = elem.property("data");
        auto data_len = js_series.property(QStringLiteral("length")).toInt();
        QVector<Range> data(data_len);
        for (int j = 0; j < data_len; j++) {
            auto x = js_series.property(j);
            auto start_val = x.property("start").toInt();
            auto end_val = x.property("end").toInt();
            data[j] = {start_val, end_val};
        }
        result[i] = {label, data};
    }
    return result;
}

QVector<OutputValue> JsGlobalVariableProvider::get_output_values(QJSEngine &engine) const noexcept
{
    auto js_value = engine.evaluate(system_var_obj_label + "." + output_values_label);
    if (!js_value.isArray()) {
        return {};
    }
    auto length = js_value.property(QStringLiteral("length")).toInt();
    QVector<OutputValue> result(length);
    for (int i = 0; i < length; i++) {
        auto elem = js_value.property(i);
        if (!elem.hasProperty("label")) {
            qDebug() << "get_output_values: value has no \"label\" field";
            return {};
        }
        auto label = elem.property("label").toString();
        auto val = elem.property("value").toVariant();
        result[i] = {label, val};
    }
    return result;
}
