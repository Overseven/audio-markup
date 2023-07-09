#ifndef JS_SCRIPT_PROVIDER_H
#define JS_SCRIPT_PROVIDER_H

#include <QObject>
#include "../interfaces/i_js_script_provider.h"

class JsScriptProvider : public QObject, public IJsScriptProvider
{
    Q_OBJECT

public:
    explicit JsScriptProvider(QObject *parent = nullptr);

    virtual QVector<QString> get_all_scripts_filenames() override;
    virtual QString get_script_code(const QString &filename) override;
    virtual QString get_root_dir() override;

    virtual void save_script(QString filename, QString code) override;
    virtual bool connect_to_signal_script_list_changed(QObject* pReceiver, const char* pszSlot, bool bConnect) const override;

signals:
    void script_list_changed();

private:
    QFileSystemModel* file_system_model = nullptr;
    QString script_dir;
};

#endif // JS_SCRIPT_PROVIDER_H
