#ifndef I_JS_SAMPLES_PROVIDER_H
#define I_JS_SAMPLES_PROVIDER_H

#include <QVector>

class IJsSamplesProvider {
public:
    virtual ~IJsSamplesProvider() {}
    virtual QVector<double> get_selected_file_samples() = 0;
    virtual QVector<QVector<double>> get_all_files_samples() = 0;
};

#endif // I_JS_SAMPLES_PROVIDER_H
