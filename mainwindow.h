#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QProgressBar>
#include <QBoxLayout>
#include <QTime>
#include <QDebug>
#include <QGroupBox>
#include <QTabWidget>
#include <QFile>
#include <QtGui>
#include <iostream>
#include <fstream>
#include <pwd.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <vector>
#include <QObject>
#include <QWidget>
#include <QTableWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QFileDialog>
#include <QStyleFactory>
#include <QTimer>
#include <QDir>

using namespace std;


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QTime time;
    QGroupBox *group;

private:
    vector<float> readCpuStats();
    int getCpuLoad(double dt);

    QLabel                      *processName;
    QLabel                      *processPid;
    QLabel                      *cpuLoadLabel;
    QLabel                      *usedResident;
    QLabel                      *usedVirtual;
    QLabel                      *processActiveDiscriptors;
    QString                     fileName;
    QString                     path;
    QString                     procPID;

    QTableWidget                *table;
    QHBoxLayout                 *H_layout_1;
    QPushButton                 *terminate_button;
    QPushButton                 *select_button;
    QVBoxLayout                 *layout;
    QTimer                      *update_timer;
    QFileDialog                 *f_d;
    Ui::MainWindow              *ui;

public slots:
    void openClicked();
    void update();
    void kill();
    void testClicked();

};
#endif // MAINWINDOW_H
