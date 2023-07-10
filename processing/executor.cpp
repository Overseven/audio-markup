#include "executor.h"
#include <QDebug>
#include <QDir>
#include "../interfaces/i_js_script_provider.h"
#include "../providers/js_global_variable_provider.h"

Executor::Executor(
    std::shared_ptr<ISamplesProvider> _samples_provider,
    std::shared_ptr<IJsScriptProvider> _js_script_provider,
    std::shared_ptr<JsFunctionsProvider> _js_function_provider
) : samples_provider(_samples_provider),
    js_script_provider(_js_script_provider),
    js_function_provider(_js_function_provider)
{

}

ExecutionResult Executor::execute_script(const QString &script_name)
{
    QJSEngine engine;
    JsGlobalVariableProvider global_var_provider;
    auto samples = samples_provider->get_selected_file_samples();
    global_var_provider.init_variables(engine, samples);
    QString functions = js_function_provider->get_all_functions();
    QString script_code = js_script_provider->get_script_code(script_name);

    if (script_code.isEmpty()) {
        qDebug() << "script code is empty, nothing to execute";
        return {};
    }
    QString final_code = functions + "\n" + script_code;
    auto output = engine.evaluate(final_code);
    auto series = global_var_provider.get_output_series(engine);
    auto ranges = global_var_provider.get_output_ranges(engine);
    auto values = global_var_provider.get_output_values(engine);

    qDebug() << "Output: " << output.toString();

    qDebug() << "Series (" << series.length() << "):";
    for (const auto& s : series) {
        qDebug() << "  Series name:" << s.name;
        qDebug() << " " << s.data << "\n";
    }
    qDebug() << "Ranges (" << ranges.length() << "):";
    for (const auto& r : ranges) {
        qDebug() << "  Ranges name:" << r.name;
        for (const auto& rr : r.ranges) {
            qDebug() << "  start:" << rr.start << "end:" << rr.end;
        }
        qDebug() << "";
    }

    qDebug() << "Values (" << values.length() << "):";
    for (const auto& v : values) {
        qDebug() << "  Name:" << v.name << " value: " << v.value;
    }
    return { output, series, ranges, values };
}


