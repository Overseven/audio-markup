#include "processing_result_provider.h"
#include <QDir>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

namespace json::processing_result {
static const QString filename = "processing_results.json";
static const QString label_ranges = "ranges";
static const QString label_left = "left";
static const QString label_right = "right";
}

ProcessingResultProvider::ProcessingResultProvider(std::shared_ptr<DirProvider> _dir_provider, QObject *parent)
    : QObject{parent},
      dir_provider(_dir_provider)
{
    connect(dir_provider.get(), &DirProvider::dir_changed, this, &ProcessingResultProvider::dir_changed);
}

void ProcessingResultProvider::load()
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

    dir.setNameFilters({json::processing_result::filename});
    QStringList fileList = dir.entryList();
    if (fileList.isEmpty()){
        auto empty_processing_result_file = ProcessingResultFile{dir_name, {}};
        save_processing_result_file(dir_name, json::processing_result::filename, empty_processing_result_file);
    }
    processing_result_file.reset();
    dir.setNameFilters({json::processing_result::filename});
    fileList = dir.entryList();

    processing_result_file = load_processing_result_file(dir_name, fileList.first());
    emit results_file_changed();
}

void ProcessingResultProvider::save()
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
    if (processing_result_file.has_value()) {
        save_processing_result_file(dir_name, json::processing_result::filename, processing_result_file.value());
    }
}

std::optional<ProcessingResultFile> ProcessingResultProvider::get_processing_result_file() const
{
    return processing_result_file;
}

std::optional<ProcessingResult> ProcessingResultProvider::get_processing_result(const SampleKey &sample_key, const JsScriptKey &js_script_key, const HashKey &hash_key) const
{
    if (processing_result_file.has_value()) {
        auto data = processing_result_file.value();
        if (data.results.contains(sample_key)) {
            if (data.results[sample_key].contains(js_script_key)) {
                if (data.results[sample_key][js_script_key].contains(hash_key)) {
                    return data.results[sample_key][js_script_key][hash_key];
                }
            }
        }
    }
    return {};
}

void ProcessingResultProvider::set_processing_result_file(const ProcessingResultFile &result_file)
{
    processing_result_file = result_file;
    emit results_file_changed();
}

void ProcessingResultProvider::set_processing_result(const SampleKey &sample_key, const JsScriptKey &js_script_key, const HashKey &hash_key, const ProcessingResult &result)
{
    if (processing_result_file.has_value()) {
        auto data = processing_result_file.value();
        auto script_map = QMap<JsScriptKey, QMap<HashKey, ProcessingResult>>();
        auto hash_map = QMap<HashKey, ProcessingResult>();
        if (data.results.contains(sample_key)) {
            script_map = data.results[sample_key];
        }

        if (script_map.contains(js_script_key)) {
            hash_map = script_map[js_script_key];
        }
        hash_map[hash_key] = result;
        script_map[js_script_key] = hash_map;
        data.results[sample_key] = script_map;
        processing_result_file = data;
        emit results_file_changed();
    }

}

void ProcessingResultProvider::remove_result(const SampleKey &sample_key, const JsScriptKey &js_script_key, const HashKey &hash_key)
{
    if (!processing_result_file.has_value()) {
        return;
    }
    auto file = processing_result_file.value();
    if (file.results.contains(sample_key)) {
        if (file.results[sample_key].contains(js_script_key)) {
            if (file.results[sample_key][js_script_key].contains(hash_key)) {
                file.results[sample_key][js_script_key].remove(hash_key);
                emit results_file_changed();
            }
        }
    }
}

bool ProcessingResultProvider::connect_to_signal_result_changed(QObject *pReceiver, const char *pszSlot, bool bConnect) const
{
    if(bConnect) {
       return connect(this, SIGNAL(dir_changed()), pReceiver, pszSlot);
    }
    return disconnect(this, SIGNAL(dir_changed()), pReceiver, pszSlot);
}

void ProcessingResultProvider::dir_changed()
{
    load();
}

/* JSON scheme:
{
<audio_filename>: {
    <script_name>: {
        <script_hash>: {
            "ranges": [
                {
                    "left": 90,
                    "right": 110
                }
            ]
        }
    },
*/

void ProcessingResultProvider::save_processing_result_file(const QString &directory, const QString &filename, const ProcessingResultFile &processing_result_file) const
{
    auto data_json = QJsonObject();

    for (const auto &sample_key : processing_result_file.results.keys()) {
        auto scripts_json = QJsonObject();
        auto script_map = processing_result_file.results[sample_key];

        for (const auto &script_key : script_map.keys()) {
            auto hash_json = QJsonObject();
            auto hash_map = script_map[script_key];

            for (const auto &hash_key : hash_map.keys()) {
                auto processing_result = hash_map[hash_key];
                auto ranges_json = QJsonObject();
                auto ranges_arr_json = QJsonArray();

                for (const auto &range : processing_result.ranges) {
                    auto range_json = QJsonObject();
                    range_json.insert(json::processing_result::label_left, range.left);
                    range_json.insert(json::processing_result::label_right, range.right);
                    ranges_arr_json.append(range_json);
                }
                ranges_json.insert(json::processing_result::label_ranges, ranges_arr_json);
                hash_json.insert(hash_key, ranges_json);
            }
            scripts_json.insert(script_key, hash_json);
        }
        data_json.insert(sample_key, scripts_json);
    }

    qDebug() << Q_FUNC_INFO << directory + "/" + filename;
    QFile json_file(directory + "/" + filename);
    json_file.open(QFile::WriteOnly);

    auto document = QJsonDocument(data_json);
    json_file.write(document.toJson());
}

std::optional<ProcessingResultFile> ProcessingResultProvider::load_processing_result_file(const QString &directory, const QString &filename) const
{
    auto result = ProcessingResultFile{directory, {}};
    QFile file(directory + "/" + filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        throw std::invalid_argument("Can't open processing result file");
    }
    QJsonDocument loadDoc(QJsonDocument::fromJson(file.readAll()));

    QJsonObject json = loadDoc.object();
    auto samples_keys = json.keys();
    if (samples_keys.isEmpty()) {
        return {};
    }

    for (auto const &sample_key : samples_keys) {
        if (!json[sample_key].isObject()) {
            throw std::invalid_argument("Can't parse processing result (sample keys)");
        }
        auto sample_json = json[sample_key].toObject();
        result.results.insert(sample_key, {});

        for (auto const &script_key : sample_json.keys()) {
            if (!sample_json[script_key].isObject()) {
                throw std::invalid_argument("Can't parse processing result (script keys)");
            }
            auto script_json = sample_json[script_key].toObject();
            result.results[sample_key].insert(script_key, {});

            for (auto const &hash_key : script_json.keys()) {
                if (!script_json[hash_key].isObject()) {
                    throw std::invalid_argument("Can't parse processing result (hash keys)");
                }
                auto hash_json = script_json[hash_key].toObject();
                if (!hash_json.contains(json::processing_result::label_ranges)
                    || !hash_json[json::processing_result::label_ranges].isArray())
                {
                    throw std::invalid_argument("Can't parse processing result (ranges keys)");
                }
                auto ranges_json = hash_json[json::processing_result::label_ranges].toArray();
                result.results[sample_key][script_key].insert(hash_key, {});
                auto processing_result = ProcessingResult();

                for (int i = 0; i < ranges_json.count(); i++) {
                    if (!ranges_json[i].isObject()) {
                        throw std::invalid_argument("Can't parse processing result (range keys)");
                    }
                    auto range_json = ranges_json[i].toObject();
                    if (!range_json.contains(json::processing_result::label_left) || !range_json[json::processing_result::label_left].isDouble()) {
                        throw std::invalid_argument("Can't parse processing result (left keys)");
                    }
                    if (!range_json.contains(json::processing_result::label_right) || !range_json[json::processing_result::label_right].isDouble()) {
                        throw std::invalid_argument("Can't parse processing result (right keys)");
                    }
                    auto left = range_json[json::processing_result::label_left].toInt();
                    auto right = range_json[json::processing_result::label_right].toInt();
                    processing_result.ranges.append({left, right});
                }
                result.results[sample_key][script_key][hash_key] = processing_result;
            }
        }
    }
    return result;
}
