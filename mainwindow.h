#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QSettings>
#include "widgets/markup_window.h"
#include "widgets/js_case_window.h"
#include "widgets/result_window.h"
#include "processing/js_functions_provider.h"
#include "processing/dir_provider.h"

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

private slots:
    void process_samples();

private:
    Ui::MainWindow *ui;
    std::shared_ptr<Markup::MarkupData> markup_data;
    std::shared_ptr<DirProvider> dir_provider;
    std::shared_ptr<ISamplesProvider> samples_provider;
    std::shared_ptr<IMarkupProvider> markup_provider;
    std::shared_ptr<QSettings> settings;
    std::shared_ptr<JsFunctionsProvider> functions_provider;
    MarkupWindow *markup_window;
    JsCaseWindow *case_window;
    ResultWindow *result_window;
};

#endif // MAINWINDOW_H
