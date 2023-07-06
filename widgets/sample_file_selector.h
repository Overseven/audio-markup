#ifndef SAMPLE_FILE_SELECTOR_H
#define SAMPLE_FILE_SELECTOR_H

#include <QWidget>
#include <QStringListModel>
#include "../interfaces/i_samples_provider.h"

namespace Ui {
class SampleFileSelector;
}

class SampleFileSelector : public QWidget
{
    Q_OBJECT

public:
    explicit SampleFileSelector(QWidget *parent = nullptr);
    ~SampleFileSelector();

    void set_sample_provider(std::shared_ptr<ISamplesProvider> _samples_provider);

signals:
    void selection_changed();

private slots:
    void on_listView_audio_files_clicked(const QModelIndex &index);
    void samples_list_changed();

private:
    Ui::SampleFileSelector *ui;
    QStringListModel *samples_model;
    std::shared_ptr<ISamplesProvider> samples_provider;
};

#endif // SAMPLE_FILE_SELECTOR_H
