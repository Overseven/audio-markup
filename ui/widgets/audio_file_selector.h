#ifndef AUDIO_FILE_SELECTOR_H
#define AUDIO_FILE_SELECTOR_H

#include <QWidget>
#include <QStringListModel>
#include "../../interfaces/i_samples_provider.h"

namespace Ui {
class AudioFileSelector;
}

class AudioFileSelector : public QWidget
{
    Q_OBJECT

public:
    explicit AudioFileSelector(QWidget *parent = nullptr);
    ~AudioFileSelector();

    void set_sample_provider(std::shared_ptr<ISamplesProvider> _samples_provider);

private slots:
    void on_listView_audio_files_clicked(const QModelIndex &index);
    void samples_list_changed();
    void selection_changed();

private:
    Ui::AudioFileSelector *ui;
    QStringListModel *samples_model;
    std::shared_ptr<ISamplesProvider> samples_provider;
};

#endif // AUDIO_FILE_SELECTOR_H
