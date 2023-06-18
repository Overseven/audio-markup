#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "i_js_case_provider.h"
#include "js_functions_provider.h"
#include "common.h"

class Executor
{
public:
    Executor();
    ExecutionResult execute(IJsCaseProvider* case_provider, JsFunctionsProvider* func_provider);
};

#endif // EXECUTOR_H
