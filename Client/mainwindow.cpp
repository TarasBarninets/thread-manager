#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , mClient(new TcpClient(this))
{
    ui->setupUi(this);
    ui->pathRequestedFile->setReadOnly(true);
    QString windowTitle("Thread Manager Client");
    this->setWindowTitle(windowTitle);
    connect(ui->getPathButton, &QPushButton::clicked, this, &MainWindow::requestedFileId);
    connect(mClient, &TcpClient::responseFromServer, ui->pathRequestedFile, &QTextEdit::append);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::requestedFileId()
{
    QString request = ui->requestedFileId->text();
    std::string str = request.toStdString();
    const char* requestedFileId = str.c_str();
    mClient->requestToServer(requestedFileId);
}

