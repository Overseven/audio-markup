#ifndef I_MARKUP_PROVIDER_H
#define I_MARKUP_PROVIDER_H

#include <QVector>
#include <QModelIndex>
#include "../common/types.h"

struct IMarkupProvider {
    virtual ~IMarkupProvider() {};

    virtual void load() = 0;
    virtual void save() = 0;
    virtual std::optional<SampleDetails> get_sample_details(const SampleKey &sample_key) const = 0;
    virtual void set_sample_details(const SampleDetails &sample_details) = 0;
    virtual QVector<SampleDetails> get_all_samples_details() const = 0;

    virtual QVector<Markup> get_sorted_markups(const SampleKey &sample_key) const = 0;
    virtual std::optional<Markup> get_markup(const SampleKey &sample_key, const MarkupKey &markup_key) const = 0;
    virtual void set_markup(const SampleKey &sample_key, const Markup &markup) = 0;
    virtual void remove_markup(const SampleKey &sample_key, const MarkupKey &markup_key) = 0;
    virtual bool connect_to_signal_markups_changed(QObject* pReceiver, const char* pszSlot, bool bConnect) const = 0;
};


#endif // I_MARKUP_PROVIDER_H
