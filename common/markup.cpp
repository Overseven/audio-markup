#include "markup.h"

Markup::MarkupData Markup::json::load_markup_file(QString directory, QString filename) {
    auto result = QMap<QString, SampleDetails>();
    QFile file(directory + "/" + filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        throw std::invalid_argument("Can't open markup file");
    }
    QJsonDocument loadDoc(QJsonDocument::fromJson(file.readAll()));

    QJsonObject json = loadDoc.object();
    if (!json.contains(label_markups) || !json[label_markups].isArray()) {
        throw std::invalid_argument("Can't parse markup file (markup label)");
    }
    QJsonArray markups = json[label_markups].toArray();
    for (auto const &markup_val : markups) {
        auto sample_detail = SampleDetails();
        auto markup = markup_val.toObject();
        if (!markup.contains(label_filename) || !markup[label_filename].isString()) {
            throw std::invalid_argument("Can't parse markup file (filename label)");
        }
        auto audio_file_name = markup.value(label_filename).toString();
        if (!markup.contains(label_sections) || !markup[label_sections].isArray()) {
            throw std::invalid_argument("Can't parse markup file (sections label)");
        }
        sample_detail.filename = audio_file_name;

        QJsonArray sections = markup[label_sections].toArray();
        for (auto const &section_val : sections) {
            auto section = section_val.toObject();
            if (!section.contains(label_range) || !section[label_range].isArray()) {
                throw std::invalid_argument("Can't parse markup file (range label)");
            }
            if (!section.contains(label_key) || !section[label_key].isDouble()) {
                throw std::invalid_argument("Can't parse markup file (key label)");
            }
            QJsonArray range = section[label_range].toArray();
            if (range.count() != 2) {
                throw std::invalid_argument("Section.range has incorrect len!");
            }

            if (!section.contains(label_comment) || !section[label_comment].isString()) {
                throw std::invalid_argument("Can't parse markup file (comment label)");
            }
            auto key = section[label_key].toInt();
            sample_detail.markups[key] = {
                    key,
                    range[0].toInt(),
                    range[1].toInt(),
                    section[label_comment].toString()
                };
        }

        result[sample_detail.filename] = sample_detail;
    }
    return {directory, result};
}

void Markup::json::save_markup_file(QString filename, MarkupData &markup_data) {
    auto markups_vec = QJsonArray();

    for (const auto &sample_detail : markup_data.sample_details.values()) {
        auto sections = QJsonArray();

        for (const auto &markup : sample_detail.markups) {
            auto m = QJsonObject();
            auto range = QJsonArray();
            range.append(markup.left);
            range.append(markup.right);

            m.insert("key", markup.key);
            m.insert("range", range);
            m.insert("comment", markup.comment);

            sections.append(m);
        }

        auto m = QJsonObject();
        m.insert("filename", sample_detail.filename);
        m.insert("sections", sections);
        markups_vec.append(m);
    }

    auto obj = QJsonObject();
    obj.insert("markups", markups_vec);

    qInfo() << markup_data.dir + "/" + filename;
    QFile json_file(markup_data.dir + "/" + filename);
    json_file.open(QFile::WriteOnly);

    auto document = QJsonDocument(obj);
    json_file.write(document.toJson());
}
