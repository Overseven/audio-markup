#ifndef JS_SCRIPT_MULTI_SELECTOR_H
#define JS_SCRIPT_MULTI_SELECTOR_H

#include <QWidget>
#include <QTreeWidgetItem>
#include "../../interfaces/i_js_script_provider.h"

namespace Ui {
class JsScriptMultiSelector;
}

struct SelectedScript {
    QString filename;
    bool is_hidden;
};

class JsScriptMultiSelector : public QWidget
{
    Q_OBJECT

public:
    explicit JsScriptMultiSelector(QWidget *parent = nullptr);
    ~JsScriptMultiSelector();

    void set_js_script_provider(std::shared_ptr<IJsScriptProvider> _js_script_provider);
    QVector<SelectedScript> get_selected_scripts_filenames() const;

signals:
    void selection_changed();

public slots:
    void update_tree();

private slots:
    void on_treeWidget_scripts_activated(const QModelIndex &index);
    void dir_item_changed(QTreeWidgetItem * item, int column);

private:
    void collect_selected(QTreeWidgetItem *item, QVector<SelectedScript> &selected) const;
    QString get_item_path(QTreeWidgetItem *item) const;
    void set_item_checkbox_state_recursive_up(QTreeWidgetItem *item, int column, Qt::CheckState state);
    void set_item_checkbox_state_recursive_down(QTreeWidgetItem *item, int column, Qt::CheckState state);

private:
    Ui::JsScriptMultiSelector *ui;
    std::shared_ptr<IJsScriptProvider> js_script_provider;
};

#endif // JS_SCRIPT_MULTI_SELECTOR_H
