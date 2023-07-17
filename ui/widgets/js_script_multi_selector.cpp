#include "js_script_multi_selector.h"
#include "ui_js_script_multi_selector.h"

JsScriptMultiSelector::JsScriptMultiSelector(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::JsScriptMultiSelector)
{
    ui->setupUi(this);
}

JsScriptMultiSelector::~JsScriptMultiSelector()
{
    delete ui;
}

void JsScriptMultiSelector::set_js_script_provider(std::shared_ptr<IJsScriptProvider> _js_script_provider)
{
    js_script_provider = _js_script_provider;
    js_script_provider->connect_to_signal_script_list_changed(this, SLOT(update_tree()), true);
    update_tree();
    connect(ui->treeWidget_scripts, &QTreeWidget::itemChanged, this, &JsScriptMultiSelector::dir_item_changed);
}

QVector<SelectedScript> JsScriptMultiSelector::get_selected_scripts_filenames() const
{
    auto selected = QVector<SelectedScript>();

    auto root = ui->treeWidget_scripts->invisibleRootItem();
    if (root->childCount() == 0) {
        return {};
    }
    collect_selected(root, selected);

    return selected;
}

void JsScriptMultiSelector::on_treeWidget_scripts_activated(const QModelIndex &index)
{
    Q_UNUSED(index)
    emit selection_changed();
}

void JsScriptMultiSelector::dir_item_changed(QTreeWidgetItem *item, int column)
{
    qDebug() << Q_FUNC_INFO;
    if (column == 0) {
        return;
    }
    auto state = item->checkState(column);
    qDebug() << "column" << column << "state" << state;

    ui->treeWidget_scripts->blockSignals(true);

    set_item_checkbox_state_recursive_down(item, column, state);
    if (state == Qt::CheckState::Unchecked) {
        set_item_checkbox_state_recursive_up(item, column, state);
    }

    ui->treeWidget_scripts->blockSignals(false);
}

void JsScriptMultiSelector::collect_selected(QTreeWidgetItem *item, QVector<SelectedScript> &selected) const
{
    auto check_and_add_to_selected = [this, &selected](QTreeWidgetItem *it) {
        if (it == ui->treeWidget_scripts->invisibleRootItem()) {
            return;
        }

        auto item_name = it->text(0);
        if (!item_name.endsWith(".js")) {
            return;
        }
        bool is_selected = it->checkState(1);
        if (is_selected) {
            auto filename = get_item_path(it);
            selected.push_back({filename});
        }
    };

    check_and_add_to_selected(item);
    auto childCount = item->childCount();
    for (int i = 0; i < childCount; i++){
        auto it = item->child(i);
        collect_selected(it, selected);
    }
}

QString JsScriptMultiSelector::get_item_path(QTreeWidgetItem *item) const
{
    auto parent_item = item->parent();
    auto root_item = ui->treeWidget_scripts->invisibleRootItem();
    QStringList dir_path;

    while (parent_item != root_item && parent_item != nullptr) {
        dir_path << parent_item->text(0);
        parent_item = parent_item->parent();
    }
    std::reverse(std::begin(dir_path), std::end(dir_path));
    auto filename = dir_path.join("/") + "/" + item->text(0);
    return filename;
}

void JsScriptMultiSelector::set_item_checkbox_state_recursive_up(QTreeWidgetItem *item, int column, Qt::CheckState state)
{
    item->setCheckState(column, state);
    auto it = item->parent();
    while (it != ui->treeWidget_scripts->invisibleRootItem() && it != nullptr){
        it->setCheckState(column, state);
        it = it->parent();
    }
}

void JsScriptMultiSelector::set_item_checkbox_state_recursive_down(QTreeWidgetItem *item, int column, Qt::CheckState state)
{
    item->setCheckState(column, state);
    auto childCount = item->childCount();
    for (int i = 0; i < childCount; i++){
        auto it = item->child(i);
        set_item_checkbox_state_recursive_down(it, column, state);
    }
}

void JsScriptMultiSelector::update_tree()
{
    ui->treeWidget_scripts->clear();
    auto filenames = js_script_provider->get_all_scripts_filenames();
    auto items = QMap<QString, QTreeWidgetItem*>();

    auto add_item = [&items](QTreeWidgetItem* new_item, QTreeWidgetItem* parent, const QString &path, const QString &item_name) {
        new_item->setText(0, item_name);
        new_item->setCheckState(1, Qt::CheckState::Unchecked);
        items[path] = new_item;
        parent->addChild(new_item);
    };

    for (const auto &filename : filenames) {
        auto dirs = filename.split("/");
        for (int i = 0; i < dirs.length() - 1; i++) {
            QString path;
            if (i == 0){
                path = dirs.first();
                if (!items.contains(path)) {
                    auto new_item = new QTreeWidgetItem(ui->treeWidget_scripts);
                    add_item(new_item, ui->treeWidget_scripts->invisibleRootItem(), path, path);
                }
            } else {
                path = dirs.mid(0, i+1).join("/");
                auto item_name =  dirs[i];
                if (!items.contains(path)) {
                    auto parent_path = dirs.mid(0, i).join("/");
                    auto parent_item = items[parent_path];
                    auto new_item = new QTreeWidgetItem(parent_item);
                    add_item(new_item, parent_item, path, item_name);
                }
            }
        }
        auto parent_path = dirs.mid(0, dirs.length()-1).join("/");
        auto parent_item = items[parent_path];
        auto new_item = new QTreeWidgetItem(parent_item);
        add_item(new_item, parent_item, filename, dirs.last());
    }
}
