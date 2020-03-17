#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , mThreadManager(new ThreadManager(this))
    , mTcpServer(new TcpServer(this))
    , mStopThreadsTimer(new QTimer(this))
{
    ui->setupUi(this);
    ui->stopButton->setEnabled(false);
    ui->regenerateButton->setEnabled(false);
    fillGeneralThreadsCount();
    createComboBoxValues();
    createSpinBoxValues();

    connect(ui->startButton, &QPushButton::clicked, this, &MainWindow::startServer);
    connect(ui->stopButton, &QPushButton::clicked, this, &MainWindow::stopServer);
    connect(ui->regenerateButton, &QPushButton::clicked, this, &MainWindow::regenerateFiles);
    connect(mStopThreadsTimer, &QTimer::timeout, this, &MainWindow::checkThreadsStops);
    connect(mTcpServer, &TcpServer::requestFile, mThreadManager, &ThreadManager::createRequestedFile);
    connect(mThreadManager, &ThreadManager::fileCreated, this, &MainWindow::handleCreatedFile);
    connect(mTcpServer, &TcpServer::requestHistoryMessage, ui->requestHistory, &QTextEdit::append);
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
    mThreadManager->stopAllThreads();
    event->accept();
}

void MainWindow::startServer()
{
    int generalThreadsCount = ui->comboBox->currentText().toInt();
    int requestedThreadsCount = ui->spinBox->value();
    mThreadManager->startThreads(generalThreadsCount, requestedThreadsCount);

    ui->startButton->setEnabled(false);
    ui->stopButton->setEnabled(true);
    ui->regenerateButton->setEnabled(false);
    ui->comboBox->setEnabled(false);
    ui->spinBox->setEnabled(false);

    qDebug() << "Pushed button Start";
    qDebug() << "generalThreadsCount = " << generalThreadsCount << " , requestedThreadsCount = " << requestedThreadsCount;
}

void MainWindow::stopServer()
{
    mThreadManager->stopAllThreads();
    mStopThreadsTimer->start(500);

    ui->stopButton->setEnabled(false);
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

    qDebug() << "Pushed button Regenerate";
}

void MainWindow::checkThreadsStops()
{
    int numRunningThreads = mThreadManager->getNumRuningThreads();
    if(numRunningThreads == 0)
    {
        ui->startButton->setEnabled(true);
        ui->regenerateButton->setEnabled(true);
        mStopThreadsTimer->stop();
    }
    qDebug() << "Timer trigered";
}

void MainWindow::handleCreatedFile(int fileId, QTime time, QString path)
{
    QString message = time.toString() + " : " + path;
    ui->generatedFilesHistory->append(message);
    mTcpServer->informClient(fileId, path);
}
