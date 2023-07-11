#include "js_script_selector.h"
#include "ui_js_script_selector.h"

JsScriptSelector::JsScriptSelector(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::JsScriptSelector)
{
    ui->setupUi(this);
}

JsScriptSelector::~JsScriptSelector()
{
    delete ui;
}

void JsScriptSelector::set_js_script_provider(std::shared_ptr<IJsScriptProvider> _js_script_provider)
{
    js_script_provider = _js_script_provider;
    js_script_provider->connect_to_signal_script_list_changed(this, SLOT(update_tree()), true);
    update_tree();
}

QString JsScriptSelector::get_selected_script_filename()
{
    auto index = ui->treeWidget_scripts->currentIndex();
    auto item = ui->treeWidget_scripts->itemFromIndex(index);
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

void JsScriptSelector::on_treeWidget_scripts_activated(const QModelIndex &index)
{
    Q_UNUSED(index)
    emit selection_changed();
}

void JsScriptSelector::update_tree()
{
    qDebug() << Q_FUNC_INFO;
    ui->treeWidget_scripts->clear();
    auto filenames = js_script_provider->get_all_scripts_filenames();
    auto items = QMap<QString, QTreeWidgetItem*>();

    for (const auto &filename : filenames) {
        auto dirs = filename.split("/");
        for (int i = 0; i < dirs.length() - 1; i++) {
            QString path;
            if (i == 0){
                path = dirs.first();
                if (!items.contains(path)) {
                    auto new_item = new QTreeWidgetItem(ui->treeWidget_scripts);
                    new_item->setText(0, path);
                    items[path] = new_item;
                    ui->treeWidget_scripts->invisibleRootItem()->addChild(new_item);
                }
            } else {
                path = dirs.mid(0, i+1).join("/");
                if (!items.contains(path)) {
                    auto parent_path = dirs.mid(0, i).join("/");
                    auto parent_item = items[parent_path];
                    auto new_item = new QTreeWidgetItem(parent_item);
                    new_item->setText(0, dirs[i]);
                    parent_item->addChild(new_item);
                    items[path] = new_item;
                }
            }
        }
        auto parent_path = dirs.mid(0, dirs.length()-1).join("/");
        auto parent_item = items[parent_path];
        auto new_item = new QTreeWidgetItem(parent_item);
        new_item->setText(0, dirs.last());
        parent_item->addChild(new_item);
        items[filename] = new_item;
    }
}
