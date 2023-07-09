#ifndef JS_FUNCTIONS_PROVIDER_H
#define JS_FUNCTIONS_PROVIDER_H

#include <QMap>
#include "../processing/common.h"
#include "../interfaces/i_samples_provider.h"

class JsFunctionsProvider
{
public:
    JsFunctionsProvider();

    int load_from_files();
    QString get_all_functions() const noexcept;

private:
    void load_dir(QString dir_name);
    void load_file(QString file);
    QString parse_function_name(QString filename, QString code);

    QMap<QString, JsFunction> function_map;

    ISamplesProvider *samples_provider;
};

#endif // JS_FUNCTIONS_PROVIDER_H
