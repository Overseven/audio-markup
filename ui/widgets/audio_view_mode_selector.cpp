#include "audio_view_mode_selector.h"
#include "ui_audio_view_mode_selector.h"
#include <QMessageBox>

AudioViewModeSelector::AudioViewModeSelector(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AudioViewModeSelector)
{
    ui->setupUi(this);
}

AudioViewModeSelector::~AudioViewModeSelector()
{
    delete ui;
}

std::tuple<AudioViewMode, std::optional<int> > AudioViewModeSelector::get_mode() const noexcept
{
    return {mode, mean_window};
}

void AudioViewModeSelector::on_pushButton_set_clicked()
{
    auto new_mode = AudioViewMode::Raw;
    std::optional<int> new_mean_window = {};

    if (ui->radioButton_abs->isChecked()) {
        new_mode = AudioViewMode::Abs;

    } else if (ui->radioButton_mean->isChecked()) {
        bool ok = true;
        int window_len = ui->lineEdit_mean->text().toInt(&ok);
        if (!ok) {
            QMessageBox::warning(this,
                "Window len error",
                "Window len can't parsed",
                QMessageBox::Ok
            );
            return;
        }
        if (window_len < 3) {
            QMessageBox::warning(this,
                "Window len error",
                "Window len must be >= 3",
                QMessageBox::Ok
            );
            return;
        }
        if (window_len % 2 == 0) {
            QMessageBox::warning(this,
                "Window len error",
                "Window len must be odd",
                QMessageBox::Ok
            );
            return;
        }
        new_mode = AudioViewMode::Mean;
        new_mean_window = window_len;
    }

    if (mode == new_mode && mean_window == new_mean_window) {
        return;
    }

    mode = new_mode;
    mean_window = new_mean_window;

    emit view_mode_changed(mode, mean_window);
}

