#ifndef AUDIO_VIEW_MODE_SELECTOR_H
#define AUDIO_VIEW_MODE_SELECTOR_H

#include <QWidget>

namespace Ui {
class AudioViewModeSelector;
}

enum AudioViewMode {
    Raw,
    Abs,
    Mean
};

class AudioViewModeSelector : public QWidget
{
    Q_OBJECT

public:
    explicit AudioViewModeSelector(QWidget *parent = nullptr);
    ~AudioViewModeSelector();

    std::tuple<AudioViewMode, std::optional<int>> get_mode() const noexcept;

signals:
    void view_mode_changed(AudioViewMode mode, std::optional<int> mean_window);

private slots:
    void on_pushButton_set_clicked();

private:
    AudioViewMode mode = AudioViewMode::Raw;
    std::optional<int> mean_window;
    Ui::AudioViewModeSelector *ui;
};

#endif // AUDIO_VIEW_MODE_SELECTOR_H
