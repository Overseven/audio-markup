#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "interfaces/i_js_script_provider.h"
#include "../providers/js_functions_provider.h"
#include "common.h"

class Executor
{
public:
    Executor();
    ExecutionResult execute(QVector<double> &input_samples, std::shared_ptr<IJsScriptProvider> &script_provider, JsFunctionsProvider* func_provider);
};

#endif // EXECUTOR_H
