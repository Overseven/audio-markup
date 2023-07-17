#ifndef AUDIO_FILE_MULTI_SELECTOR_H
#define AUDIO_FILE_MULTI_SELECTOR_H

#include <QWidget>
#include <QStringListModel>
#include <QTreeWidgetItem>
#include "../../interfaces/i_samples_provider.h"

namespace Ui {
class AudioFileMultiSelector;
}

class AudioFileMultiSelector : public QWidget
{
    Q_OBJECT

public:
    explicit AudioFileMultiSelector(QWidget *parent = nullptr);
    ~AudioFileMultiSelector();

    void set_sample_provider(std::shared_ptr<ISamplesProvider> _samples_provider);
    QVector<QString> get_selected_audio_filenames() const;

private slots:
    void update_list();
    void item_changed(QTreeWidgetItem *item, int column);

private:
    void set_item_checkbox_state_recursive_up(QTreeWidgetItem *item, int column, Qt::CheckState state);
    void set_item_checkbox_state_recursive_down(QTreeWidgetItem *item, int column, Qt::CheckState state);
    void collect_selected(QTreeWidgetItem *item, QVector<QString> &selected) const;

private:
    Ui::AudioFileMultiSelector *ui;
    std::shared_ptr<ISamplesProvider> samples_provider;
};

#endif // AUDIO_FILE_MULTI_SELECTOR_H
