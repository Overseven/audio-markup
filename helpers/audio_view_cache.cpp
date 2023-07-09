#include "audio_view_cache.h"

AudioViewCache::AudioViewCache()
{

}

void AudioViewCache::reset_cache() noexcept
{
    audio_raw = {};
    audio_abs = {};
    audio_mean = {};
}

void AudioViewCache::set_audio_raw(QVector<double> _audio_raw) noexcept
{
    reset_cache();
    audio_raw = _audio_raw;
}

QVector<double> AudioViewCache::get_raw() const noexcept
{
    if (!audio_raw.has_value()) {
        return {};
    }
    return audio_raw.value();
}

QVector<double> AudioViewCache::get_abs() noexcept
{
    if (audio_abs.has_value()) {
        return audio_abs.value();
    }
    if (!audio_raw.has_value()) {
        return {};
    }

    update_abs();

    return audio_abs.value();
}

QVector<double> AudioViewCache::get_mean(int mean_window)
{
    if (audio_mean.has_value()) {
        const auto &[mean, window] = audio_mean.value();
        if (mean_window == window) {
            return mean;
        }
    }
    if (!audio_raw.has_value()) {
        return {};
    }
    if (!audio_abs.has_value()) {
        update_abs();
    }
    update_mean(mean_window);
    const auto &[mean, _] = audio_mean.value();
    return mean;
}

void AudioViewCache::update_abs()
{
    const auto &raw = audio_raw.value();
    auto samples_len = raw.length();
    QVector<double> data_abs(samples_len);
    for (auto i = 0; i < samples_len; i++) {
        data_abs[i] = std::abs(raw[i]);
    }

    audio_abs = data_abs;
}

void AudioViewCache::update_mean(int window_len)
{
    const auto &data_abs = audio_abs.value();
    auto samples_len = data_abs.length();
    QVector<double> data_mean(samples_len);
    for (auto i = 0; i < window_len/2 - 1; i++) {
        data_mean[i] = 0;
        data_mean[samples_len - i - 1] = 0;
    }
    for (auto i = window_len/2; i < samples_len - window_len/2; i++) {
        double sum = 0;
        for (auto j = i - window_len/2; j < i + window_len/2; j++) {
            sum += data_abs[j];
        }
        data_mean[i] = sum / window_len;
    }
    audio_mean = std::tuple<QVector<double>, int>(data_mean, window_len);
}
