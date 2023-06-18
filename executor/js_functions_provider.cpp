#include "js_functions_provider.h"
#include <QDir>

JsFunctionsProvider::JsFunctionsProvider()
{

}

int JsFunctionsProvider::load_from_files()
{
    const QString base_dir_name = QDir::currentPath() + "/js/functions";
    function_map = QMap<QString, JsFunction>();
    load_dir(base_dir_name);
    return function_map.count();
}

QString JsFunctionsProvider::get_all_functions() const noexcept
{
    QString result;
    for (const auto& func_name : function_map.keys()) {
        result.append(function_map[func_name].code);
    }
    return result;
}


void JsFunctionsProvider::load_dir(QString dir_name)
{
    QDir dir = QDir(dir_name);
    if (!dir.exists()) {
        qDebug() << "Functions dir not found: " << dir_name;
        return;
    }
    dir.setNameFilters({"*.js"});
    dir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
    QStringList file_list = dir.entryList();

    for (const auto &f : file_list) {
        load_file(dir_name + "/" + f);
    }

    dir.setNameFilters({});
    dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
    QStringList dir_list = dir.entryList();
    for (const auto &d : dir_list) {
        load_dir(dir_name + "/" + d);
    }
}

void JsFunctionsProvider::load_file(QString filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        throw std::invalid_argument((QString("Can't open function file: ") + filename).toStdString());
    }
    QString code = QString::fromUtf8(file.readAll()).trimmed();
    QString function_name = parse_function_name(filename, code);
    if (function_map.contains(function_name)) {
        auto func = function_map[function_name];
        throw std::invalid_argument((QString("function with name ") + func.name
            + " exist in multiple files: \n  1)" + func.filename
            + "\n  2) " + filename).toStdString());
    }
    function_map.insert(function_name, JsFunction{function_name, filename, code});
}

QString JsFunctionsProvider::parse_function_name(QString filename, QString code)
{
    int count = code.count("function ");
    if (count == 0){
        throw std::invalid_argument((QString("File has no any js function. File: ") + filename).toStdString());
    }
    if (count > 1){
        throw std::invalid_argument((QString("File must have only one js function. File: ") + filename).toStdString());
    }
    int start_idx = code.indexOf("function ") + 9;
    int end_idx = code.indexOf("(", start_idx);
    if (start_idx == -1 || end_idx == -1) {
        throw std::invalid_argument((QString("File contain not valid js function. File: ") + filename).toStdString());
    }
    auto function_name = code.sliced(start_idx, end_idx - start_idx).trimmed();
//    qDebug() << function_name;
    return function_name;
}
