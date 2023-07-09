#ifndef MARKUP_PROVIDER_H
#define MARKUP_PROVIDER_H

#include <QObject>
#include "../interfaces/i_markup_provider.h"
#include "dir_provider.h"

class MarkupProvider : public QObject, public IMarkupProvider
{
    Q_OBJECT
public:
    explicit MarkupProvider(std::shared_ptr<DirProvider> _dir_provider, QObject *parent = nullptr);

    virtual void load() override;
    virtual void save() override;

    virtual std::optional<Markup::SampleDetails> get_sample_details(const SampleKey &sample_key) const override;
    virtual QVector<Markup::SampleDetails> get_all_samples_details() const override;
    virtual void set_sample_details(const Markup::SampleDetails &sample_details) override;


    virtual std::optional<Markup::Markup> get_markup(const SampleKey &sample_key, const MarkupKey &markup_key) const override;
    virtual void set_markup(const SampleKey &sample_key, const Markup::Markup &markup) override;
    virtual void remove_markup(const SampleKey &sample_key, const MarkupKey &markup_key) override;
    virtual bool connect_to_signal_markups_changed(QObject* pReceiver, const char* pszSlot, bool bConnect) const override;

signals:
    void markups_changed(SampleKey sample_key);

private slots:
    void dir_changed();

private:
    std::shared_ptr<DirProvider> dir_provider;
    Markup::MarkupData markup_data;
};

#endif // MARKUP_PROVIDER_H
