#ifndef PROCESSINGRESULTPROVIDER_H
#define PROCESSINGRESULTPROVIDER_H

#include <QObject>
#include "../interfaces/i_processing_result_provider.h"
#include "dir_provider.h"

class ProcessingResultProvider : public QObject, public IProcessingResultProvider
{
    Q_OBJECT
public:
    explicit ProcessingResultProvider(std::shared_ptr<DirProvider> _dir_provider, QObject *parent = nullptr);

    virtual void load() override;
    virtual void save() override;

    virtual std::optional<ProcessingResultFile> get_processing_result_file() const override;
    virtual std::optional<ProcessingResult> get_processing_result(const SampleKey &sample_key, const JsScriptKey &js_script_key, const HashKey &hash_key) const override;

    virtual void set_processing_result_file(const ProcessingResultFile &result_file) override;
    virtual void set_processing_result(const SampleKey &sample_key, const JsScriptKey &js_script_key, const HashKey &hash_key, const ProcessingResult &result) override;

    virtual void remove_result(const SampleKey &sample_key, const JsScriptKey &js_script_key, const HashKey &hash_key) override;

    virtual bool connect_to_signal_result_changed(QObject* pReceiver, const char* pszSlot, bool bConnect) const override;

private slots:
    void dir_changed();

private:
    void save_processing_result_file(const QString &directory, const QString &filename, const ProcessingResultFile &processing_result_file) const;
    std::optional<ProcessingResultFile> load_processing_result_file(const QString &directory, const QString &filename) const;

signals:
    void results_file_changed();

private:
    std::shared_ptr<DirProvider> dir_provider;
    std::optional<ProcessingResultFile> processing_result_file;
};

#endif // PROCESSINGRESULTPROVIDER_H
