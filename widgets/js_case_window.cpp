#include <QJSEngine>
#include <QMessageBox>
#include "js_case_window.h"
#include "ui_js_case_window.h"
#include "../processing/executor.h"

JsCaseWindow::JsCaseWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::JsCaseWindow)
{
    ui->setupUi(this);
    init_case_tree();
}

JsCaseWindow::~JsCaseWindow()
{
    delete ui;
}

void JsCaseWindow::load_case(const QString &file_path)
{
    auto file = QFile(file_path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug() << "file is not readable";
        return;
    }

    auto file_data = QString::fromUtf8(file.readAll());
    ui->textBrowser_case_code->setText(file_data);
}

QString JsCaseWindow::get_case_code()
{
    return ui->textBrowser_case_code->toPlainText();
}

void JsCaseWindow::execute()
{
    emit process_samples_requested();
}

void JsCaseWindow::init_case_tree()
{
    auto case_dir = QDir::currentPath() + "../../js/cases";
    case_model = new QFileSystemModel();
    case_model->setRootPath(case_dir);

    ui->treeView_case->setModel(case_model);
    ui->treeView_case->setRootIndex(case_model->index(case_dir));
}

void JsCaseWindow::on_treeView_case_activated(const QModelIndex &index)
{
    QString file_path = case_model->filePath(index);
    if (!file_path.endsWith(".js")) {
        return;
    }

    ui->lineEdit_filename->clear();

    auto filename_start_index = file_path.lastIndexOf('/');
    auto filename = file_path.mid(filename_start_index + 1);
    ui->lineEdit_filename->setText(filename.mid(0, filename.length()-3));
    dir_path = file_path.mid(0, filename_start_index);

    load_case(file_path);
}

void JsCaseWindow::on_pushButton_execute_and_save_clicked()
{
    execute();
//    auto samples = samples_provider->get_selected_file_samples();
//    auto result = execute(samples);
}

void JsCaseWindow::on_pushButton_new_file_clicked()
{
    auto index = ui->treeView_case->currentIndex();
    QString file_path = case_model->filePath(index);
    if (file_path.isEmpty()) {
        QMessageBox::warning(this,
            "Can't create case file",
            "Select dir or file to create new file",
            QMessageBox::Ok
        );
        return;
    }

    qDebug() << "FilePath: " << file_path;
    if (file_path.endsWith(".js")){
        auto last_index = file_path.lastIndexOf('/');
        file_path = file_path.mid(0, last_index);
        qDebug() << "FilePath: " << file_path;
    }
}


void JsCaseWindow::on_pushButton_save_case_file_clicked()
{
    if (dir_path.isEmpty()) {
        return;
    }

    QString filename = ui->lineEdit_filename->text();
    if (filename.isEmpty()) {
        QMessageBox::warning(this,
            "Can't create case file",
            "Write file name!",
            QMessageBox::Ok
        );
        return;
    }

    QString code = ui->textBrowser_case_code->toPlainText();
    if (code.isEmpty()) {
        QMessageBox::warning(this,
            "Can't create case file",
            "Write a case code!",
            QMessageBox::Ok
        );
        return;
    }
    QString filepath = dir_path + "/" + filename + ".js";
    auto file = QFile(filepath);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this,
            "Can't create case file",
            "Can't open file",
            QMessageBox::Ok
        );
        return;
    }
    file.write(code.toUtf8());
    qDebug() << "Case file wrote. Filename: " << filepath;
}

