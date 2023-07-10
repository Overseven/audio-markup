#ifndef PROCESSINGRESULTCACHE_H
#define PROCESSINGRESULTCACHE_H

#include <QString>
#include <QVector>
#include <QMap>
#include "../interfaces/i_js_script_provider.h"
#include "../processing/common.h"

class ProcessingResultCache
{
    struct ResultData {
        QString script_hash;
        QVector<Range> ranges;
    };

public:
    ProcessingResultCache(std::shared_ptr<IJsScriptProvider> _js_script_provider);

    QVector<QString> get_script_names() const noexcept;
    QVector<Range> get_ranges(const QString &script_name) const noexcept;
    void set_ranges(const QString &script_name, QVector<Range> ranges);

    bool contains(const QString &script_name);
    void reset_cache();

private:
    std::shared_ptr<IJsScriptProvider> js_script_provider;
    QMap<QString, ResultData> cache;
};

#endif // PROCESSINGRESULTCACHE_H
