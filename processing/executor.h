#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "interfaces/i_samples_provider.h"
#include "interfaces/i_js_script_provider.h"
#include "../providers/js_functions_provider.h"
#include "common.h"

class Executor
{
public:
    Executor(
            std::shared_ptr<ISamplesProvider> _samples_provider,
            std::shared_ptr<IJsScriptProvider> _js_script_provider,
            std::shared_ptr<JsFunctionsProvider> _js_function_provider);

    ExecutionResult execute_script(const QString &script_name, const QString &audio_file_name = "");

private:
    std::shared_ptr<ISamplesProvider> samples_provider;
    std::shared_ptr<IJsScriptProvider> js_script_provider;
    std::shared_ptr<JsFunctionsProvider> js_function_provider;
};

#endif // EXECUTOR_H
