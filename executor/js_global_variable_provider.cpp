#include "js_global_variable_provider.h"

JsGlobalVariableProvider::JsGlobalVariableProvider()
{

}

void JsGlobalVariableProvider::set_sample_data(const QVector<double> &data)
{
    // TODO
}

std::tuple<QString, QJSValue> JsGlobalVariableProvider::get_sys_variables_definition() const noexcept
{
    QString series = "var " + output_series + " = [];";
    QString ranges = "var " + output_ranges + " = [];";
    QString samples = "var " + input_samples + " = [];";
    return series + "\n" + ranges + "\n" + samples + "\n";
}

QVector<OutputSeries> JsGlobalVariableProvider::get_output_series(QJSEngine &engine) const noexcept
{
    auto js_value = engine.evaluate(output_series);
    if (!js_value.isArray()) {
        return {};
    }
    QVector<OutputSeries> result;
    auto length = js_value.property(QStringLiteral("length")).toInt();
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
        result.append({label, data});
    }
    return result;
}

QVector<OutputRanges> JsGlobalVariableProvider::get_output_ranges(QJSEngine &engine) const noexcept
{
    auto js_value = engine.evaluate(output_ranges);
    if (!js_value.isArray()) {
        return {};
    }
    QVector<OutputRanges> result;
    auto length = js_value.property(QStringLiteral("length")).toInt();
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
        result.append({label, data});
    }
    return result;
}
