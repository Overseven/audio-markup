#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "markup/markupwindow.h"
#include "common/markup.h"
#include "executor/executor.h"
#include <QVector>
#include <QSettings>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void open_dir();
    void load_samples_info();

private:
    Ui::MainWindow *ui;
    Markup::MarkupData *markup_data;
    std::shared_ptr<QSettings> settings;
    MarkupWindow *markup_window;
    Executor executor;
};
#endif // MAINWINDOW_H
