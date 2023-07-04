#ifndef MARKUP_H
#define MARKUP_H

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QException>
#include "types.h"

namespace Markup {

struct Markup {
    MarkupKey key;
    int left;
    int right;
    QString comment;
};

struct SampleDetails {
    QString filename;
    QMap<MarkupKey, Markup> markups;
};

struct MarkupData {
    QString dir;
    QMap<QString, SampleDetails> sample_details;
};



namespace json {

static const QString filename = "audio_markup.json";
static const QString label_markups = "markups";
static const QString label_filename = "filename";
static const QString label_sections = "sections";
static const QString label_key = "key";
static const QString label_range = "range";
static const QString label_comment = "comment";

MarkupData  load_markup_file(QString directory, QString filename);
void        save_markup_file(QString filename, MarkupData &markup_data);


}
}

#endif // MARKUP_H
