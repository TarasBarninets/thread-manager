#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , mThreadManager(new ThreadManager(this))
{
    ui->setupUi(this);
    fillGeneralThreadsCount();
    createComboBoxValues();
    createSpinBoxValues();
    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::startServer);
    connect(ui->pushButton_2, &QPushButton::clicked, this, &MainWindow::stopServer);
    connect(ui->pushButton_3, &QPushButton::clicked, this, &MainWindow::regenerateFiles);
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

    ui->pushButton->setEnabled(false);
    ui->comboBox->setEnabled(false);
    ui->spinBox->setEnabled(false);
    ui->pushButton_2->setEnabled(true);
}

void MainWindow::stopServer()
{
    mThreadManager->stopAllThreads();
    ui->pushButton->setEnabled(true);
    ui->comboBox->setEnabled(true);
    ui->spinBox->setEnabled(true);
    ui->pushButton_2->setEnabled(false);
}

void MainWindow::regenerateFiles()
{
    int generalThreadsCount = ui->comboBox->currentText().toInt();
    mThreadManager->regenerateFiles(generalThreadsCount);
}
