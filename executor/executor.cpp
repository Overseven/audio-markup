#include "executor.h"
#include <QDebug>
#include <QDir>
#include "i_js_case_provider.h"
#include "js_global_variable_provider.h"

Executor::Executor(){}

ExecutionResult Executor::execute(IJsCaseProvider* case_provider, JsFunctionsProvider* functions_provider)
{
    QJSEngine engine;
    JsGlobalVariableProvider global_var_provider;
    QString sys_variables = global_var_provider.get_sys_variables_definition();
    QString functions = functions_provider->get_all_functions();
    QString case_code = case_provider->get_case_code();

    if (case_code.isEmpty()) {
        qDebug() << "case code is empty, nothing to execute";
        return {};
    }
    QString final_code = sys_variables + functions + "\n" + case_code;
    auto output = engine.evaluate(final_code);
    auto series = global_var_provider.get_output_series(engine);
    auto ranges = global_var_provider.get_output_ranges(engine);

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
    return { output, series, ranges };
}


