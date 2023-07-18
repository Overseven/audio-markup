#include "processing_result_cache.h"
#include <QCryptographicHash>
#include "../common/utils.h"

ProcessingResultCache::ProcessingResultCache(
    std::shared_ptr<IJsScriptProvider> _js_script_provider
) : js_script_provider(_js_script_provider) {

}

QVector<QString> ProcessingResultCache::get_script_names() const noexcept
{
    return cache.keys();
}

QVector<Range> ProcessingResultCache::get_ranges(const QString &script_name) const noexcept
{
    if (!cache.contains(script_name)) {
        return {};
    }
    auto elem = cache[script_name];
    return elem.ranges;
}

void ProcessingResultCache::set_ranges(const QString &script_name, QVector<Range> ranges)
{
    qDebug() << Q_FUNC_INFO << "set range for script:" << script_name;
    auto script_code = js_script_provider->get_script_code(script_name);
    auto hash = Utils::hash(script_code);
    cache[script_name] = { hash, ranges };
}

bool ProcessingResultCache::contains(const QString &script_name)
{
    if (!cache.contains(script_name)) {
        return false;
    }
    auto script_code = js_script_provider->get_script_code(script_name);
    auto hash = Utils::hash(script_code);
    auto elem = cache[script_name];
    return elem.script_hash == hash;
}

void ProcessingResultCache::reset_cache()
{
    cache.clear();
}
