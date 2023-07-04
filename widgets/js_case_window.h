#ifndef JS_CASE_WINDOW_H
#define JS_CASE_WINDOW_H

#include <QWidget>
#include <QFileSystemModel>
#include "../interfaces/i_samples_provider.h"
#include "../interfaces/i_js_case_provider.h"
#include "../processing/js_functions_provider.h"

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
    void execute();

signals:
    void process_samples_requested();

private slots:
    void on_treeView_case_activated(const QModelIndex &index);
    void on_pushButton_execute_and_save_clicked();
    void on_pushButton_new_file_clicked();
    void on_pushButton_save_case_file_clicked();

private:
    Ui::JsCaseWindow *ui;
    QFileSystemModel *case_model;
    QString dir_path;
};

#endif // JS_CASE_WINDOW_H
