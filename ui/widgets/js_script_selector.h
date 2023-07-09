#ifndef JS_SCRIPT_SELECTOR_H
#define JS_SCRIPT_SELECTOR_H

#include <QWidget>
#include "../../interfaces/i_js_script_provider.h"

namespace Ui {
class JsScriptSelector;
}

class JsScriptSelector : public QWidget
{
    Q_OBJECT

public:
    explicit JsScriptSelector(QWidget *parent = nullptr);
    ~JsScriptSelector();

    void set_js_script_provider(std::shared_ptr<IJsScriptProvider> _js_script_provider);
    QString get_selected_script_filename();

signals:
    void selection_changed();

public slots:
    void update_tree();

private slots:
    void on_treeWidget_scripts_activated(const QModelIndex &index);


private:
    Ui::JsScriptSelector *ui;
    std::shared_ptr<IJsScriptProvider> js_script_provider;
};

#endif // JS_SCRIPT_SELECTOR_H
