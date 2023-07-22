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

ExecutionResult Executor::execute_script(const QString &script_name, const QString &audio_file_name)
{
    // TODO: using cache

    QJSEngine engine;
    JsGlobalVariableProvider global_var_provider;
    QVector<double> samples;
    if (audio_file_name.isEmpty()) {
        samples = samples_provider->get_selected_file_samples();
    } else {
        samples = samples_provider->get_file_samples(audio_file_name);
    }

    global_var_provider.init_variables(engine, samples);
    QString functions = js_function_provider->get_all_functions();
    QString script_code = js_script_provider->get_script_code(script_name);

    if (script_code.isEmpty()) {
        qDebug() << "script code is empty, nothing to execute";
        return {};
    }
    QString final_code = functions + "\n" + script_code;
    auto output = engine.evaluate(final_code);
    auto ranges = global_var_provider.get_output_ranges(engine);

    auto debug_series = global_var_provider.get_debug_series(engine);
    auto debug_ranges = global_var_provider.get_debug_ranges(engine);
    auto debug_values = global_var_provider.get_debug_values(engine);

    qDebug() << "Output: " << output.toString();

    qDebug() << "Output ranges:" << ranges.name;
    for (const auto& r : ranges.ranges) {
        qDebug() << "  start:" << r.start << "end:" << r.end;
        qDebug() << "";
    }

    qDebug() << "Debug series (" << debug_series.length() << "):";
    for (const auto& s : debug_series) {
        qDebug() << "  Series name:" << s.name;
        qDebug() << " " << s.data << "\n";
    }
    qDebug() << "Debug ranges (" << debug_ranges.length() << "):";
    for (const auto& r : debug_ranges) {
        qDebug() << "  Ranges name:" << r.name;
        for (const auto& rr : r.ranges) {
            qDebug() << "  start:" << rr.start << "end:" << rr.end;
        }
        qDebug() << "";
    }

    qDebug() << "Debug values (" << debug_values.length() << "):";
    for (const auto& v : debug_values) {
        qDebug() << "  Name:" << v.name << " value: " << v.value;
    }
    return { output, ranges, {debug_series, debug_ranges, debug_values} };
}


