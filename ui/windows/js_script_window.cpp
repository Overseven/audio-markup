#include <QJSEngine>
#include <QMessageBox>
#include "js_script_window.h"
#include "ui_js_script_window.h"
#include "../../processing/executor.h"
#include "../widgets/js_script_selector.h"

JsScriptWindow::JsScriptWindow(QWidget *parent, std::shared_ptr<IJsScriptProvider> _js_script_provider) :
    QWidget(parent),
    ui(new Ui::JsScriptWindow),
    js_script_provider(_js_script_provider)
{
    ui->setupUi(this);
    ui->js_script_selector->set_js_script_provider(js_script_provider);

    connect(ui->js_script_selector, &JsScriptSelector::selection_changed, this, &JsScriptWindow::selection_changed);
}

JsScriptWindow::~JsScriptWindow()
{
    delete ui;
}

QString JsScriptWindow::get_script_code()
{
    return ui->textBrowser_script_code->toPlainText();
}

void JsScriptWindow::execute()
{
    emit process_samples_requested();
}

void JsScriptWindow::selection_changed()
{
    auto filename = ui->js_script_selector->get_selected_script_filename();
    try {
        auto script_code = js_script_provider->get_script_code(filename);
        ui->lineEdit_filename->setText(filename);
        ui->textBrowser_script_code->setText(script_code);
    }  catch (std::exception &exc) {
        qDebug() << Q_FUNC_INFO << "Can't load script code: " << exc.what();
    }
}

void JsScriptWindow::on_pushButton_save_script_file_clicked()
{
    auto filename = ui->lineEdit_filename->text();
    auto code = ui->textBrowser_script_code->toPlainText();

    try {
        js_script_provider->save_script(filename, code);
    }  catch (std::exception &exc) {
        QMessageBox::warning(this,
            "Can't save script file",
            exc.what(),
            QMessageBox::Ok
        );
        return;
    }
}

