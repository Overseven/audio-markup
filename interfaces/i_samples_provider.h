#ifndef I_SAMPLES_PROVIDER_H
#define I_SAMPLES_PROVIDER_H

#include <QVector>
#include <QModelIndex>
#include "../common/types.h"

struct ISamplesProvider{
    virtual ~ISamplesProvider() {};

    virtual void reset_selection() = 0;
    virtual void select_sample(const SampleKey &sample_key) = 0;
    virtual std::optional<QString> get_selected_file_key() = 0;
    virtual QVector<double> get_selected_file_samples() = 0;
    virtual QVector<double> get_file_samples(const QString &filename) = 0;
    virtual QList<std::tuple<SampleKey, QVector<double>>> get_all_files_samples() = 0;
    virtual QVector<QString> get_files_list() = 0;

    virtual bool connect_to_signal_samples_list_changed(QObject* pReceiver, const char* pszSlot, bool bConnect) const = 0;
    virtual bool connect_to_signal_selection_changed(QObject* pReceiver, const char* pszSlot, bool bConnect) const = 0;
};

#endif // I_SAMPLES_PROVIDER_H
