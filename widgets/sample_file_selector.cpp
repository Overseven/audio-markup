#include "sample_file_selector.h"
#include "ui_sample_file_selector.h"

SampleFileSelector::SampleFileSelector(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SampleFileSelector)
{
    ui->setupUi(this);
    samples_model = new QStringListModel(this);
    ui->listView_audio_files->setModel(samples_model);
}

SampleFileSelector::~SampleFileSelector()
{
    delete ui;
}

void SampleFileSelector::set_sample_provider(std::shared_ptr<ISamplesProvider> _samples_provider)
{
    samples_provider = _samples_provider;
    samples_provider->connect_to_signal_samples_list_changed(this, SLOT(samples_list_changed()), true);
}

void SampleFileSelector::samples_list_changed()
{
    qDebug() << Q_FUNC_INFO;
    samples_model->removeRows(0, samples_model->rowCount());
    auto sample_files = samples_provider->get_files_list();
    samples_model->setStringList(sample_files);
}

void SampleFileSelector::on_listView_audio_files_clicked(const QModelIndex &index)
{
    auto sample_key = samples_model->data(index).toString();
    qDebug() << Q_FUNC_INFO << "index:" << index.row() << "sample_key:" << sample_key;
    samples_provider->select_sample(sample_key);
}
