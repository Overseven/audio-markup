#ifndef SAMPLESPROVIDER_H
#define SAMPLESPROVIDER_H

#include <QObject>
#include "../interfaces/i_samples_provider.h"
#include "common/audio_file.h"
#include "dir_provider.h"

class SamplesProvider : public QObject, public ISamplesProvider
{
    Q_OBJECT

public:
    explicit SamplesProvider(std::shared_ptr<DirProvider> _dir_provider, QObject *parent = nullptr);

    virtual void reset_selection() override;

    virtual void select_sample(const SampleKey &sample_key) override;
    virtual std::optional<SampleKey> get_selected_file_key() override;

    virtual QVector<double> get_selected_file_samples() override;
    virtual QList<std::tuple<SampleKey, QVector<double>>> get_all_files_samples() override;
    virtual QVector<SampleKey> get_files_list() override;

    virtual bool connect_to_signal_samples_list_changed(
            QObject* pReceiver,
            const char* pszSlot,
            bool bConnect
    ) const override;

    virtual bool connect_to_signal_selection_changed(
            QObject* pReceiver,
            const char* pszSlot,
            bool bConnect
    ) const override;

signals:
    void samples_list_changed();
    void selection_changed();

private:
    void dir_changed();
    bool load_audio_file(const QString &directory, const QString &filename);

private:
    std::shared_ptr<DirProvider> dir_provider;
    std::optional<SampleKey> selected_file;
    AudioFile<double> audio_file;
};

#endif // SAMPLESPROVIDER_H
