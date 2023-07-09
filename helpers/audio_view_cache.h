#ifndef AUDIOVIEWCACHE_H
#define AUDIOVIEWCACHE_H

#include <QVector>

class AudioViewCache
{
public:
    AudioViewCache();

    void reset_cache() noexcept;
    void set_audio_raw(QVector<double> _audio_raw) noexcept;

    QVector<double> get_raw() const noexcept;
    QVector<double> get_abs() noexcept;
    QVector<double> get_mean(int mean_window);

private:
    void update_abs();
    void update_mean(int window_len);

private:
    std::optional<QVector<double>> audio_raw;
    std::optional<QVector<double>> audio_abs;
    std::optional<std::tuple<QVector<double>, int>> audio_mean;
};

#endif // AUDIOVIEWCACHE_H
