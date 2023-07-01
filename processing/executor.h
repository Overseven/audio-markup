#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "interfaces/i_js_case_provider.h"
#include "js_functions_provider.h"
#include "common.h"

class Executor
{
public:
    Executor();
    ExecutionResult execute(QVector<double> &input_samples, IJsCaseProvider* case_provider, JsFunctionsProvider* func_provider);
};

#endif // EXECUTOR_H
