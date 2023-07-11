#ifndef JS_SCRIPT_WINDOW_H
#define JS_SCRIPT_WINDOW_H

#include <QWidget>
#include <QFileSystemModel>
#include "../../interfaces/i_samples_provider.h"
#include "../../interfaces/i_js_script_provider.h"
#include "../../providers/js_functions_provider.h"

namespace Ui {
class JsScriptWindow;
}

class JsScriptWindow : public QWidget
{
    Q_OBJECT

public:
    explicit JsScriptWindow(QWidget *parent, std::shared_ptr<IJsScriptProvider> _js_script_provider);
    ~JsScriptWindow();

    virtual QString get_script_code();
    void execute();

signals:
    void process_samples_requested();

private slots:
    void selection_changed();
    void on_pushButton_save_script_file_clicked();

private:
    Ui::JsScriptWindow *ui;
    std::shared_ptr<IJsScriptProvider> js_script_provider;
};

#endif // JS_SCRIPT_WINDOW_H
