#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , mThreadManager(new ThreadManager(this))
{
    ui->setupUi(this);
    ui->stopButton->setEnabled(false);
    ui->regenerateButton->setEnabled(false);
    ui->requestButton->setEnabled(false);
    fillGeneralThreadsCount();
    createComboBoxValues();
    createSpinBoxValues();

    connect(ui->startButton, &QPushButton::clicked, this, &MainWindow::startServer);
    connect(ui->stopButton, &QPushButton::clicked, this, &MainWindow::stopServer);
    connect(ui->regenerateButton, &QPushButton::clicked, this, &MainWindow::regenerateFiles);
    connect(ui->requestButton, &QPushButton::clicked, this, &MainWindow::requestedFileId);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::fillGeneralThreadsCount()
{
    mGeneralThreadsCount.resize(6);
    std::iota(mGeneralThreadsCount.begin(), mGeneralThreadsCount.end(), 1);
}

void MainWindow::createComboBoxValues()
{
    for(int i = 0; i < mGeneralThreadsCount.size(); i++)
    {
        ui->comboBox->addItem(QString::number(mGeneralThreadsCount[i]));
    }
    ui->comboBox->setCurrentIndex(1);
}

void MainWindow::createSpinBoxValues()
{
    ui->spinBox->setMinimum(1);
    ui->spinBox->setMaximum(4);
    ui->spinBox->setValue(1);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if(!mThreadManager->threadsStopped())
    {
        mThreadManager->stopAllThreads();
        event->accept();
    }
}

void MainWindow::startServer()
{
    int generalThreadsCount = ui->comboBox->currentText().toInt();
    int requestedThreadsCount = ui->spinBox->value();
    mThreadManager->startThreads(generalThreadsCount, requestedThreadsCount);

    ui->startButton->setEnabled(false);
    ui->stopButton->setEnabled(true);
    ui->regenerateButton->setEnabled(false);
    ui->requestButton->setEnabled(true);
    ui->comboBox->setEnabled(false);
    ui->spinBox->setEnabled(false);

    qDebug() << "Pushed button Start";
    qDebug() << "generalThreadsCount = " << generalThreadsCount << " , requestedThreadsCount = " << requestedThreadsCount;
}

void MainWindow::stopServer()
{
    mThreadManager->stopAllThreads();

    ui->stopButton->setEnabled(false);
    ui->startButton->setEnabled(true);
    ui->regenerateButton->setEnabled(true);
    ui->requestButton->setEnabled(false);
    ui->comboBox->setEnabled(true);
    ui->spinBox->setEnabled(true);

    qDebug() << "Pushed button Stop";
}

void MainWindow::regenerateFiles()
{
    int generalThreadsCount = ui->comboBox->currentText().toInt();
    int requestedThreadsCount = ui->spinBox->value();
    mThreadManager->regenerateFiles(generalThreadsCount, requestedThreadsCount);

    ui->regenerateButton->setEnabled(false);
    ui->startButton->setEnabled(false);
    ui->stopButton->setEnabled(true);
    ui->requestButton->setEnabled(true);

    qDebug() << "Pushed button Regenerate";
}

void MainWindow::requestedFileId()
{
    int requestedFileId = ui->lineEdit->text().toInt();
    mThreadManager->addRequest(requestedFileId);

    qDebug() << "Pushed button Request";
    qDebug() << "requestedFileId = " << requestedFileId;
}
