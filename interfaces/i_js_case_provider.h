#ifndef I_JS_CASE_PROVIDER_H
#define I_JS_CASE_PROVIDER_H

#include <QString>

class IJsCaseProvider {
public:
    virtual ~IJsCaseProvider() {}
    virtual QString get_case_code() = 0;
};

#endif // I_JS_CASE_PROVIDER_H
