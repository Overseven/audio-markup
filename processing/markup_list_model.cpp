#include "markup_list_model.h"

MarkupListModel::MarkupListModel(QObject *parent)
    : QStringListModel{parent}
{

}

void MarkupListModel::insert_markup(MarkupKey markup_key, QString description, QCPAbstractItem *item)
{
    if (markups_map.contains(markup_key)){
        auto [row, _] = markups_map[markup_key];
        auto list = stringList();
        list[row] = description;
        setStringList(list);
        markups_map.insert(markup_key, {row, item});
    } else {
        auto list = stringList();
        int row = list.length();
        list.append(description);
        setStringList(list);
        markups_map.insert(markup_key, {row, item});
    }
}

void MarkupListModel::clear_markup_data()
{
    markups_map.clear();
    removeRows(0, rowCount());
}

std::optional<MarkupKey> MarkupListModel::get_markup_key(const QCPAbstractItem *item)
{
    auto keys = markups_map.keys();
    for (const auto &markup_key : keys){
        auto [model_idx, m_item] = markups_map[markup_key];
        if (item == m_item) {
            return markup_key;
        }
    }
    return {};
}

std::optional<MarkupKey> MarkupListModel::get_markup_key(const QModelIndex &index)
{
    auto row = index.row();
    auto keys = markups_map.keys();
    for (const auto &markup_key : keys){
        auto [model_idx, item] = markups_map[markup_key];
        if (row == model_idx) {
            return markup_key;
        }
    }
    return {};
}

std::optional<QModelIndex> MarkupListModel::index_from_markup_key(MarkupKey markup_key)
{
    if (!markups_map.contains(markup_key)){
        return {};
    }
    auto [model_key, _] = markups_map[markup_key];
    return index(model_key);
}

std::optional<QCPAbstractItem *> MarkupListModel::get_item(MarkupKey markup_key)
{
    if (!markups_map.contains(markup_key)){
        return {};
    }
    auto [_, rect] = markups_map[markup_key];
    return rect;
}
