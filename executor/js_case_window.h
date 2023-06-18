#ifndef JS_CASE_WINDOW_H
#define JS_CASE_WINDOW_H

#include <QWidget>
#include <QFileSystemModel>
#include "executor.h"
#include "js_functions_provider.h"
#include "i_js_case_provider.h"

namespace Ui {
class JsCaseWindow;
}

class JsCaseWindow : public QWidget, public IJsCaseProvider
{
    Q_OBJECT

public:
    explicit JsCaseWindow(QWidget *parent = nullptr);
    ~JsCaseWindow();

    void init_case_tree();
    void load_case(const QString &file_path);

    virtual QString get_case_code();
    ExecutionResult execute();

private slots:
    void on_treeView_case_activated(const QModelIndex &index);

private:
    Ui::JsCaseWindow *ui;
    std::shared_ptr<JsFunctionsProvider> func_provider;
    std::shared_ptr<Executor> executor;
    QFileSystemModel *case_model;
};

#endif // JS_CASE_WINDOW_H
