#include "markup_provider.h"
#include <QDir>

MarkupProvider::MarkupProvider(std::shared_ptr<DirProvider> _dir_provider, QObject *parent)
    : QObject{parent},
      dir_provider(_dir_provider)
{
    connect(dir_provider.get(), &DirProvider::dir_changed, this, &MarkupProvider::dir_changed);
}

void MarkupProvider::load()
{
    auto dir_option = dir_provider->get_dir();
    if (!dir_option.has_value()) {
        qDebug() << Q_FUNC_INFO << "dir not selected";
        return;
    }
    QString dir_name = dir_option.value();
    if (dir_name.isEmpty()) {
        qDebug() << Q_FUNC_INFO << "dir empty";
        return;
    }
    QDir dir = QDir(dir_name);

    dir.setNameFilters({Markup::json::filename});
    QStringList markupFileList = dir.entryList();
    if (markupFileList.isEmpty()){
        auto empty_markup_data = Markup::MarkupData{dir_name, {}};
        Markup::json::save_markup_file(Markup::json::filename, empty_markup_data);
    }
    markup_data.sample_details.clear();
    dir.setNameFilters({Markup::json::filename});
    markupFileList = dir.entryList();

    markup_data = Markup::json::load_markup_file(dir_name, markupFileList.first());
    emit markups_changed(markupFileList.first());
}

void MarkupProvider::save()
{
    Markup::json::save_markup_file(Markup::json::filename, markup_data);
}

std::optional<Markup::SampleDetails> MarkupProvider::get_sample_details(const SampleKey &sample_key) const
{
    auto it = markup_data.sample_details.find(sample_key);
    if (it != std::end(markup_data.sample_details)) {
        return *it;
    }
    return {};
}

void MarkupProvider::set_sample_details(const Markup::SampleDetails &sample_details)
{
    markup_data.sample_details[sample_details.filename] = sample_details;
    emit markups_changed(sample_details.filename);
}

std::optional<Markup::Markup> MarkupProvider::get_markup(const SampleKey &sample_key, const MarkupKey &markup_key) const
{
    auto it_sample = markup_data.sample_details.find(sample_key);
    if (it_sample != std::end(markup_data.sample_details)) {
        auto it_markup = (*it_sample).markups.find(markup_key);
        if (it_markup != std::end((*it_sample).markups)) {
            return *it_markup;
        }
    }
    return {};
}

void MarkupProvider::set_markup(const SampleKey &sample_key, const Markup::Markup &markup)
{
    markup_data.sample_details[sample_key].markups[markup.key] = markup;
    emit markups_changed(sample_key);
}

void MarkupProvider::remove_markup(const SampleKey &sample_key, const MarkupKey &markup_key)
{
    markup_data.sample_details[sample_key].markups.remove(markup_key);
    emit markups_changed(sample_key);
}

QVector<Markup::SampleDetails> MarkupProvider::get_all_samples_details() const
{
    return markup_data.sample_details.values();
}

bool MarkupProvider::connect_to_signal_markups_changed(QObject *pReceiver, const char *pszSlot, bool bConnect) const
{
    if(bConnect) {
       return connect(this, SIGNAL(markups_changed(SampleKey)), pReceiver, pszSlot);
    }
    return disconnect(this, SIGNAL(markups_changed(SampleKey)), pReceiver, pszSlot);
}

void MarkupProvider::dir_changed()
{
    load();
}
