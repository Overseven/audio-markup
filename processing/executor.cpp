#include "executor.h"
#include <QDebug>
#include <QDir>
#include "../interfaces/i_js_script_provider.h"
#include "../providers/js_global_variable_provider.h"

Executor::Executor(){}

ExecutionResult Executor::execute(QVector<double> &input_samples, std::shared_ptr<IJsScriptProvider> &script_provider, JsFunctionsProvider* functions_provider)
{
    QJSEngine engine;
    JsGlobalVariableProvider global_var_provider;
    global_var_provider.init_variables(engine, input_samples);
    QString functions = functions_provider->get_all_functions();
//    QString script_code = js_script_provider->get_script_code();

//    if (script_code.isEmpty()) {
//        qDebug() << "script code is empty, nothing to execute";
//        return {};
//    }
//    QString final_code = functions + "\n" + script_code;
//    auto output = engine.evaluate(final_code);
//    auto series = global_var_provider.get_output_series(engine);
//    auto ranges = global_var_provider.get_output_ranges(engine);
//    auto values = global_var_provider.get_output_values(engine);

//    qDebug() << "Output: " << output.toString();

//    qDebug() << "Series (" << series.length() << "):";
//    for (const auto& s : series) {
//        qDebug() << "  Series name:" << s.name;
//        qDebug() << " " << s.data << "\n";
//    }
//    qDebug() << "Ranges (" << ranges.length() << "):";
//    for (const auto& r : ranges) {
//        qDebug() << "  Ranges name:" << r.name;
//        for (const auto& rr : r.ranges) {
//            qDebug() << "  start:" << rr.start << "end:" << rr.end;
//        }
//        qDebug() << "";
//    }

//    qDebug() << "Values (" << values.length() << "):";
//    for (const auto& v : values) {
//        qDebug() << "  Name:" << v.name << " value: " << v.value;
//    }
//    return { output, series, ranges, values };
    return {};
}


