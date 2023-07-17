#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QSettings>
#include "markup_window.h"
#include "js_script_window.h"
#include "result_window.h"
#include "stats_window.h"
#include "../../providers/js_functions_provider.h"
#include "../../providers/dir_provider.h"

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
    std::shared_ptr<Markup::MarkupData> markup_data;
    std::shared_ptr<DirProvider> dir_provider;
    std::shared_ptr<ISamplesProvider> samples_provider;
    std::shared_ptr<IMarkupProvider> markup_provider;
    std::shared_ptr<IJsScriptProvider> js_script_provider;
    std::shared_ptr<JsFunctionsProvider> js_function_provider;
    std::shared_ptr<QSettings> settings;
    MarkupWindow *markup_window;
    JsScriptWindow *script_window;
    ResultWindow *result_window;
    StatsWindow *stats_window;
};

#endif // MAIN_WINDOW_H
