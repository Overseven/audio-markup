#ifndef MARKUPLISTMODEL_H
#define MARKUPLISTMODEL_H

#include <QStringListModel>
#include <QObject>
#include "../ui/qcustomplot/qcustomplot.h"
#include "../common/types.h"

class MarkupListModel : public QStringListModel
{
    Q_OBJECT
    typedef int ModelKey;
public:
    explicit MarkupListModel(QObject *parent = nullptr);

    void insert_markup(MarkupKey markup_key, QString description, QCPAbstractItem *item);
    void clear_markup_data();
    std::optional<MarkupKey> get_markup_key(const QCPAbstractItem *item);
    std::optional<MarkupKey> get_markup_key(const QModelIndex &index);
    std::optional<QModelIndex> index_from_markup_key(MarkupKey markup_key);
    std::optional<QCPAbstractItem*> get_item(MarkupKey markup_key);



private:
    // markup_key -> [ model_idx, rect ]
    QMap<MarkupKey, std::tuple<ModelKey, QCPAbstractItem*>> markups_map;
};

#endif // MARKUPLISTMODEL_H
