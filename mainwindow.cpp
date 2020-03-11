#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , mThreadManager(new ThreadManager(this))
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

void MainWindow::startServer()
{
    int generalThreadsCount = ui->comboBox->currentText().toInt();
    qDebug() << "generalThreadsCount = " << generalThreadsCount;
    mThreadManager->startGeneralThreads(generalThreadsCount);

    ui->startButton->setEnabled(false);
    ui->comboBox->setEnabled(false);
    ui->spinBox->setEnabled(false);
    ui->stopButton->setEnabled(true);
    ui->regenerateButton->setEnabled(true);
}

void MainWindow::stopServer()
{
    mThreadManager->stopAllThreads();

    ui->stopButton->setEnabled(false);
    ui->startButton->setEnabled(true);
    ui->comboBox->setEnabled(true);
    ui->spinBox->setEnabled(true);
}

void MainWindow::regenerateFiles()
{
    int generalThreadsCount = ui->comboBox->currentText().toInt();
    mThreadManager->regenerateFiles(generalThreadsCount);

    ui->regenerateButton->setEnabled(false);
    ui->startButton->setEnabled(false);
    ui->stopButton->setEnabled(true);
}
