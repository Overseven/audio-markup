#include "js_case_window.h"
#include "ui_js_case_window.h"
#include <QJSEngine>

JsCaseWindow::JsCaseWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::JsCaseWindow),
    func_provider(std::make_shared<JsFunctionsProvider>()
) {
    ui->setupUi(this);
    init_case_tree();
    func_provider->load_from_files();
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
    return ui->textBrowser_case_code->placeholderText();
}

ExecutionResult JsCaseWindow::execute()
{
    auto executor = Executor();
    return executor.execute(this, func_provider.get());
}

void JsCaseWindow::init_case_tree()
{
    auto case_dir = QDir::currentPath() + "/js/cases";
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
    qDebug() << "on_treeView_case_activated: " << index.data() << file_path;
    load_case(file_path);
}
