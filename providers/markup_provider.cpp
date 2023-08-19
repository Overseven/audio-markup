#include "markup_provider.h"
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

namespace json::markup {
static const QString filename = "audio_markup.json";
static const QString label_markups = "markups";
static const QString label_filename = "filename";
static const QString label_sections = "sections";
static const QString label_key = "key";
static const QString label_range = "range";
static const QString label_comment = "comment";
}


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

    dir.setNameFilters({json::markup::filename});
    QStringList markupFileList = dir.entryList();
    if (markupFileList.isEmpty()){
        auto empty_markup_data = MarkupData{dir_name, {}};
        save_markup_file(json::markup::filename, empty_markup_data);
    }
    markup_data.sample_details.clear();
    dir.setNameFilters({json::markup::filename});
    markupFileList = dir.entryList();

    markup_data = load_markup_file(dir_name, markupFileList.first());
    emit markups_changed(markupFileList.first());
}

void MarkupProvider::save()
{
    save_markup_file(json::markup::filename, markup_data);
}

std::optional<SampleDetails> MarkupProvider::get_sample_details(const SampleKey &sample_key) const
{
    auto it = markup_data.sample_details.find(sample_key);
    if (it != std::end(markup_data.sample_details)) {
        return *it;
    }
    return {};
}

void MarkupProvider::set_sample_details(const SampleDetails &sample_details)
{
    markup_data.sample_details[sample_details.filename] = sample_details;
    emit markups_changed(sample_details.filename);
}

std::optional<Markup> MarkupProvider::get_markup(const SampleKey &sample_key, const MarkupKey &markup_key) const
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

QVector<Markup> MarkupProvider::get_sorted_markups(const SampleKey &sample_key) const
{
    auto it = markup_data.sample_details.find(sample_key);
    if (it == std::end(markup_data.sample_details)) {
        return {};
    }
    auto markups = (*it).markups.values();
    std::sort(std::begin(markups), std::end(markups),
              [](const Markup &v1, const Markup &v2) {
        return v1.range.left < v2.range.left;
    });
    return markups;
}

void MarkupProvider::set_markup(const SampleKey &sample_key, const Markup &markup)
{
    markup_data.sample_details[sample_key].markups[markup.key] = markup;
    emit markups_changed(sample_key);
}

void MarkupProvider::remove_markup(const SampleKey &sample_key, const MarkupKey &markup_key)
{
    markup_data.sample_details[sample_key].markups.remove(markup_key);
    emit markups_changed(sample_key);
}

QVector<SampleDetails> MarkupProvider::get_all_samples_details() const
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

MarkupData MarkupProvider::load_markup_file(QString directory, QString filename)
{
    auto result = QMap<QString, SampleDetails>();
    QFile file(directory + "/" + filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        throw std::invalid_argument("Can't open markup file");
    }
    QJsonDocument loadDoc(QJsonDocument::fromJson(file.readAll()));

    QJsonObject json = loadDoc.object();
    if (!json.contains(json::markup::label_markups) || !json[json::markup::label_markups].isArray()) {
        throw std::invalid_argument("Can't parse markup file (markup label)");
    }
    QJsonArray markups = json[json::markup::label_markups].toArray();
    for (auto const &markup_val : markups) {
        auto sample_detail = SampleDetails();
        auto markup = markup_val.toObject();
        if (!markup.contains(json::markup::label_filename) || !markup[json::markup::label_filename].isString()) {
            throw std::invalid_argument("Can't parse markup file (filename label)");
        }
        auto audio_file_name = markup.value(json::markup::label_filename).toString();
        if (!markup.contains(json::markup::label_sections) || !markup[json::markup::label_sections].isArray()) {
            throw std::invalid_argument("Can't parse markup file (sections label)");
        }
        sample_detail.filename = audio_file_name;

        QJsonArray sections = markup[json::markup::label_sections].toArray();
        for (auto const &section_val : sections) {
            auto section = section_val.toObject();
            if (!section.contains(json::markup::label_range) || !section[json::markup::label_range].isArray()) {
                throw std::invalid_argument("Can't parse markup file (range label)");
            }
            if (!section.contains(json::markup::label_key) || !section[json::markup::label_key].isDouble()) {
                throw std::invalid_argument("Can't parse markup file (key label)");
            }
            QJsonArray range = section[json::markup::label_range].toArray();
            if (range.count() != 2) {
                throw std::invalid_argument("Section.range has incorrect len!");
            }

            if (!section.contains(json::markup::label_comment) || !section[json::markup::label_comment].isString()) {
                throw std::invalid_argument("Can't parse markup file (comment label)");
            }
            auto key = section[json::markup::label_key].toInt();
            sample_detail.markups[key] = {
                key,
                {
                    range[0].toInt(),
                    range[1].toInt()
                },
                section[json::markup::label_comment].toString()
            };
        }

        result[sample_detail.filename] = sample_detail;
    }
    return {directory, result};
}

void MarkupProvider::save_markup_file(QString filename, MarkupData &markup_data)
{
    auto markups_vec = QJsonArray();

    for (const auto &sample_detail : markup_data.sample_details.values()) {
        auto sections = QJsonArray();

        for (const auto &markup : sample_detail.markups) {
            auto m = QJsonObject();
            auto range = QJsonArray();
            range.append(markup.range.left);
            range.append(markup.range.right);

            m.insert(json::markup::label_key, markup.key);
            m.insert(json::markup::label_range, range);
            m.insert(json::markup::label_comment, markup.comment);

            sections.append(m);
        }

        auto m = QJsonObject();
        m.insert(json::markup::label_filename, sample_detail.filename);
        m.insert(json::markup::label_sections, sections);
        markups_vec.append(m);
    }

    auto obj = QJsonObject();
    obj.insert(json::markup::label_markups, markups_vec);

    qInfo() << markup_data.dir + "/" + filename;
    QFile json_file(markup_data.dir + "/" + filename);
    json_file.open(QFile::WriteOnly);

    auto document = QJsonDocument(obj);
    json_file.write(document.toJson());
}
