#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QCloseEvent>
#include <QTimer>
#include "ThreadManager.h"
#include "TcpServer.h"

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
    void checkThreadsStops();
    void handleRequestedCreatedFile(int fileId, QString path, QDateTime dateTime);

private:
    Ui::MainWindow *ui;
    ThreadManager* mThreadManager;
    TcpServer* mTcpServer;
    QTimer *mStopThreadsTimer;
    QVector<int> mGeneralThreadsCount;
};
#endif // MAINWINDOW_H
