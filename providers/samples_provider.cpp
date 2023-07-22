#include "samples_provider.h"
#include <QDebug>
#include <QDir>

SamplesProvider::SamplesProvider(std::shared_ptr<DirProvider> _dir_provider, QObject *parent)
    : QObject{parent},
      dir_provider(_dir_provider)
{
    connect(dir_provider.get(), &DirProvider::dir_changed, this, &SamplesProvider::dir_changed);
}

void SamplesProvider::reset_selection()
{
    selected_file.reset();
    audio_file = AudioFile<double>();
}

void SamplesProvider::select_sample(const SampleKey &sample_key)
{
    qDebug() << Q_FUNC_INFO << sample_key;

    if (selected_file == sample_key) {
        return;
    }
    selected_file.reset();
    auto dir_option = dir_provider->get_dir();
    if (!dir_option.has_value()) {
        qDebug() << Q_FUNC_INFO << "dir not selected";
        return;
    }
    auto dir = dir_option.value();
    if (load_audio_file(dir, sample_key)){
        selected_file = sample_key;
        emit selection_changed();
    }
}

std::optional<SampleKey> SamplesProvider::get_selected_file_key()
{
    return selected_file;
}

QVector<double> SamplesProvider::get_selected_file_samples()
{
    if (audio_file.getNumChannels() == 0) {
        return {};
    }
    return audio_file.samples[0];
}

QVector<double> SamplesProvider::get_file_samples(const QString &filename)
{
    if (selected_file.has_value() && selected_file.value() == filename){
        return get_selected_file_samples();
    }

    auto dir_option = dir_provider->get_dir();
    if (!dir_option.has_value()) {
        qDebug() << Q_FUNC_INFO << "dir not selected";
        return {};
    }
    auto dir = dir_option.value();
    QString file_path = dir + "/" + filename;
    auto file = AudioFile<double>();
    if (!file.load(file_path.toStdString())) {
        qDebug() << Q_FUNC_INFO << "can't open file:" << file_path;
        return {};
    }
    if (audio_file.getNumChannels() == 0) {
        return {};
    }
    return audio_file.samples[0];
}

QList<std::tuple<SampleKey, QVector<double> > > SamplesProvider::get_all_files_samples()
{
    auto dir_option = dir_provider->get_dir();
    if (!dir_option.has_value()) {
        qDebug() << Q_FUNC_INFO << "dir not selected";
        return {};
    }
    auto dir = dir_option.value();
    auto files = get_files_list();

    QVector<std::tuple<SampleKey, QVector<double>>> result(files.count());
    for (const auto& filename : qAsConst(files)) {
        QString file = dir + "/" + filename;
        auto audio_file = AudioFile<double>();
        bool is_loaded = audio_file.load(file.toStdString());
        if (!is_loaded || audio_file.getNumChannels() == 0) {
            return {};
        }
        result.append({filename, audio_file.samples[0]});
    }
    return result;
}

QVector<SampleKey> SamplesProvider::get_files_list()
{
    auto dir_option = dir_provider->get_dir();
    if (!dir_option.has_value()) {
        qDebug() << Q_FUNC_INFO << "dir not selected";
        return {};
    }
    QString dir_name = dir_option.value();
    QDir dir = QDir(dir_name);
    dir.setNameFilters({"*.wav"});
    dir.setFilter(QDir::Files | QDir::NoDotAndDotDot);

    return dir.entryList();
}

bool SamplesProvider::connect_to_signal_samples_list_changed(
    QObject *pReceiver,
    const char *pszSlot,
    bool bConnect
) const {
    if(bConnect) {
       return connect(this, SIGNAL(samples_list_changed()), pReceiver, pszSlot);
    }
    return disconnect(this, SIGNAL(samples_list_changed()), pReceiver, pszSlot);
}

bool SamplesProvider::connect_to_signal_selection_changed(
    QObject *pReceiver,
    const char *pszSlot,
    bool bConnect
) const {
    if(bConnect) {
       return connect(this, SIGNAL(selection_changed()), pReceiver, pszSlot);
    }
    return disconnect(this, SIGNAL(selection_changed()), pReceiver, pszSlot);
}

void SamplesProvider::dir_changed()
{
    emit samples_list_changed();
}

bool SamplesProvider::load_audio_file(const QString &directory, const QString &filename)
{
    QString file = directory + "/" + filename;
    return audio_file.load(file.toStdString());
}
