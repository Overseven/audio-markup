#ifndef TYPES_H
#define TYPES_H

#include <QMap>
#include <QString>
#include <QHash>

typedef int MarkupKey;
typedef QString SampleKey;
typedef QString JsScriptKey;
typedef QString HashKey;

struct Range {
    int left;
    int right;
};

struct Markup {
    MarkupKey key;
    Range range;
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

struct ProcessingResult {
    QVector<Range> ranges;
};

struct ProcessingResultFile {
    QString dir;
    QMap<SampleKey, QMap<JsScriptKey, QMap<HashKey, ProcessingResult>>> results;
};

#endif // TYPES_H
