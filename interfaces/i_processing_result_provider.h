#ifndef I_PROCESSING_RESULT_PROVIDER_H
#define I_PROCESSING_RESULT_PROVIDER_H

#include <QVector>
#include <QModelIndex>
#include "../common/types.h"

struct IProcessingResultProvider {
    virtual ~IProcessingResultProvider() {};

    virtual void load() = 0;
    virtual void save() = 0;

    virtual std::optional<ProcessingResultFile> get_processing_result_file() const = 0;
    virtual std::optional<ProcessingResult> get_processing_result(const SampleKey &sample_key, const JsScriptKey &js_script_key, const HashKey &hash_key) const = 0;

    virtual void set_processing_result_file(const ProcessingResultFile &result_file) = 0;
    virtual void set_processing_result(const SampleKey &sample_key, const JsScriptKey &js_script_key, const HashKey &hash_key, const ProcessingResult &result) = 0;

    virtual void remove_result(const SampleKey &sample_key, const JsScriptKey &js_script_key, const HashKey &hash_key) = 0;

    virtual bool connect_to_signal_result_changed(QObject* pReceiver, const char* pszSlot, bool bConnect) const = 0;
};

#endif // I_PROCESSING_RESULT_PROVIDER_H
