#include "audio_file_selector.h"
#include "ui_audio_file_selector.h"

AudioFileSelector::AudioFileSelector(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AudioFileSelector)
{
    ui->setupUi(this);
    samples_model = new QStringListModel(this);
    ui->listView_audio_files->setModel(samples_model);
}

AudioFileSelector::~AudioFileSelector()
{
    delete ui;
}

void AudioFileSelector::set_sample_provider(std::shared_ptr<ISamplesProvider> _samples_provider)
{
    samples_provider = _samples_provider;
    samples_provider->connect_to_signal_samples_list_changed(this, SLOT(samples_list_changed()), true);
    samples_provider->connect_to_signal_selection_changed(this, SLOT(selection_changed()), true);
}

void AudioFileSelector::samples_list_changed()
{
    qDebug() << Q_FUNC_INFO;
    samples_model->removeRows(0, samples_model->rowCount());
    auto sample_files = samples_provider->get_files_list();
    samples_model->setStringList(sample_files);
}

void AudioFileSelector::selection_changed()
{
    auto selected_key_option = samples_provider->get_selected_file_key();
    if (!selected_key_option.has_value()) {
        return;
    }
    auto selected_key = selected_key_option.value();
    auto model_keys = samples_model->stringList();
    auto it = std::find(std::cbegin(model_keys), std::cend(model_keys), selected_key);
    size_t index = std::distance(std::cbegin(model_keys), it);
    if(index == static_cast<size_t>(model_keys.length()))
    {
        return;
    }
    ui->listView_audio_files->setCurrentIndex(samples_model->index(index));
}

void AudioFileSelector::on_listView_audio_files_clicked(const QModelIndex &index)
{
    auto sample_key = samples_model->data(index).toString();
    qDebug() << Q_FUNC_INFO << "index:" << index.row() << "sample_key:" << sample_key;
    samples_provider->select_sample(sample_key);
}
