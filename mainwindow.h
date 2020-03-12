#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QCloseEvent>
#include "ThreadManager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void fillGeneralThreadsCount();
    void createComboBoxValues();
    void createSpinBoxValues();

protected:
    void closeEvent(QCloseEvent *event);

signals:
    void generalThreadsCount(int threadsCount);

private slots:
    void startServer();
    void stopServer();
    void regenerateFiles();
    void requestedFileId();

private:
    Ui::MainWindow *ui;
    ThreadManager* mThreadManager;
    QVector<int> mGeneralThreadsCount;
};
#endif // MAINWINDOW_H
