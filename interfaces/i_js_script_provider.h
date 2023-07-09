#ifndef I_JS_SCRIPT_PROVIDER_H
#define I_JS_SCRIPT_PROVIDER_H

#include <QObject>
#include <QString>
#include <QFileSystemModel>

struct IJsScriptProvider {
    virtual ~IJsScriptProvider() {}
    virtual QVector<QString> get_all_scripts_filenames() = 0;
    virtual QString get_script_code(const QString &filename) = 0;
    virtual QString get_root_dir() = 0;

    virtual void save_script(QString filename, QString code) = 0;

    virtual bool connect_to_signal_script_list_changed(QObject* pReceiver, const char* pszSlot, bool bConnect) const = 0;
};

#endif // I_JS_SCRIPT_PROVIDER_H
