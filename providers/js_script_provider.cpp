#include "js_script_provider.h"
#include "../common/utils.h"

JsScriptProvider::JsScriptProvider(QObject *parent) : QObject(parent)
{
    QString relative_path = "../../js/scripts";

    #ifdef Q_OS_DARWIN
        relative_path = "../../../" + relative_path;
    #endif

    QDir dir(QDir::currentPath() + relative_path);
    script_dir = dir.absolutePath();

    file_system_model = new QFileSystemModel();
    file_system_model->setRootPath(script_dir);
    file_system_model->setNameFilters(QStringList("*.js"));
    file_system_model->setNameFilterDisables(false);

    connect(file_system_model, &QFileSystemModel::fileRenamed, this, [this](){
        emit script_list_changed();
    });
}

QVector<QString> JsScriptProvider::get_all_scripts_filenames()
{
    QStringList filenames;
    QDir root_dir = QDir(script_dir);
    if (!root_dir.exists()) {
        throw "Script dir not found: ./js/scripts";
    }
    Utils::recursive_scan_dir(root_dir, QStringList("*.js"), filenames);

    auto files_count = filenames.length();
    for (int i = 0; i < files_count; i++) {
        filenames[i] = filenames[i].mid(script_dir.length() + 1);
    }

    return filenames;
}

QString JsScriptProvider::get_script_code(const QString &filename)
{
    QString absolute_path = script_dir + "/" + filename;
    if (!absolute_path.endsWith(".js")) {
        throw "file has invalid format";
    }
    auto file = QFile(absolute_path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        throw "file is not readable";
    }

    auto script_code = QString::fromUtf8(file.readAll());
    return script_code;
}

QString JsScriptProvider::get_root_dir()
{
    return script_dir;
}

void JsScriptProvider::save_script(QString filename, QString code)
{
    qDebug() << Q_FUNC_INFO << "filename: " << filename;
    if (!filename.endsWith(".js")){
        throw "Script filename must contains '.js' postfix";
    }
    if (!filename.endsWith(".js")){
        throw "Script code is empty";
    }
    auto file = QFile(script_dir + "/" + filename);
    if (!file.open(QIODevice::WriteOnly)) {
        throw "Can't open file";
    }
    if (file.write(code.toUtf8()) == -1){
        throw "Can't write file";
    }
    emit script_list_changed();
}

bool JsScriptProvider::connect_to_signal_script_list_changed(QObject *pReceiver, const char *pszSlot, bool bConnect) const
{
    if(bConnect) {
       return connect(this, SIGNAL(script_list_changed()), pReceiver, pszSlot);
    }
    return disconnect(this, SIGNAL(script_list_changed()), pReceiver, pszSlot);
}
