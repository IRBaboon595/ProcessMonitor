#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    time = QTime::currentTime();
    update_timer = new QTimer();

    processName = new QLabel;
    processPid = new QLabel;
    cpuLoadLabel = new QLabel;
    usedResident = new QLabel;
    usedVirtual = new QLabel;
    processActiveDiscriptors = new QLabel;

    QDir dir;
    dir.setFilter(QDir::Executable);

    f_d = new QFileDialog();

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(processName);
    layout->addWidget(processPid);
    layout->addWidget(cpuLoadLabel);
    layout->addWidget(usedResident);
    layout->addWidget(usedVirtual);
    layout->addWidget(processActiveDiscriptors);
    ui->centralwidget->setLayout(layout);

    connect(update_timer, SIGNAL(timeout()), this, SLOT(update()));
    connect(this->ui->actionOpen, SIGNAL(triggered()), this, SLOT(openClicked()));
}

void MainWindow::update()
{
    QStringList temp_strList;
    int brake_index = 0;
    QFile *file = new QFile();
    QString vms;
    QString rss;
    double temp_long = 0;
    int bracket_1_index = 0;
    int bracket_2_index = 0;
    QString uptime;          //1 /proc/uptime
    QString utime;           //14 /proc/pid/stat
    QString stime;           //15 /proc/pid/stat
    QString cutime;          //16 /proc/pid/stat
    QString cstime;          //17 /proc/pid/stat
    QString starttime;       //22 /proc/pid/stat
    double total_time = 0;
    double seconds = 0;
    double cpu_usage = 0;
    uint32_t cpu_freq = 0;
    processName->setText("Process name: " + fileName);
    processPid->setText("Process PID: "+ procPID);

    file->setFileName("/proc/uptime");
    if(!file->open(QIODevice::ReadOnly))
        qDebug() << "Unable to open kernel uptime";
    uptime = file->readAll();
    temp_strList = uptime.split(' ');
    uptime = temp_strList.value(0);
    file->close();

    file->setFileName("/proc/" + procPID.toUtf8() + "/stat");
    cpu_freq = sysconf(_SC_CLK_TCK);
    if(!file->open(QIODevice::ReadOnly))
        qDebug() << "Unable to open /proc/pid/stat";
    vms = file->readAll();
    bracket_1_index = vms.indexOf('(', 0);
    bracket_2_index = vms.indexOf(')', 0);
    brake_index = vms.indexOf(' ', bracket_1_index);
    if(brake_index < bracket_2_index)
    {
        vms.remove(brake_index, 1);
    }
    temp_strList = vms.split(' ');
    utime = temp_strList.value(13);
    stime = temp_strList.value(14);
    cutime = temp_strList.value(15);
    cstime = temp_strList.value(16);
    starttime = temp_strList.value(21);
    vms = temp_strList.value(22);
    temp_long = vms.toDouble() / 1000;
    vms = QString::number(uint64_t(temp_long));
    rss = temp_strList.value(23);
    temp_long = rss.toDouble() * 4;
    rss = QString::number(uint64_t(temp_long));
    usedResident->setText("Resident Set Size: " + rss);
    usedVirtual->setText("Virtual Set Size: " + vms);

    total_time = utime.toDouble() + stime.toDouble();
    //total_time += cutime.toDouble() + cstime.toDouble(); //if children proccesses are taken into account
    seconds = uptime.toDouble() - (starttime.toDouble() / cpu_freq);
    cpu_usage = 100 * ((total_time / cpu_freq) / seconds);
    cpuLoadLabel->setText("Process CPU usage in %: " + QString::number(cpu_usage));
    file->close();

    QDir *dir = new QDir("/proc/" + procPID + "/fd");
    processActiveDiscriptors->setText("Used discriptors: " + QString::number(dir->count()));
}

vector<float> MainWindow::readCpuStats()
{
    vector<float> ret;
    QFile *file = new QFile("/proc/stat");
    QString temp_1;
    QStringList temp_2;

    if(!file->open(QIODevice::ReadOnly))
    {
        qDebug() << "Unable to open /proc/stat";
        return ret;
    }

    temp_1 = file->readLine();
    temp_2 = temp_1.split(' ');

    for (int i = 0; i < 4; ++i)
    {
        ret.push_back(temp_2.at(i + 2).toFloat());
    }
    return ret;
}

int MainWindow::getCpuLoad(double dt)
{
    vector<float> stats1 = readCpuStats();
    QProcess::execute("sleep", QStringList() << QString::number(dt));
    vector<float> stats2 = readCpuStats();
    int size1 = stats1.size();
    int size2 = stats2.size();
    if (!size1 || !size2 || size1 != size2) return 2;
    for (int i = 0; i < size1; ++i) stats2[i] -= stats1[i];
    int sum = 1;
    for (int i = 0; i < size1; ++i) sum += stats2[i];
    int load = 100 - (stats2[size2 - 1] * 100 / sum);
    return load;
}

void MainWindow::kill()
{
    QList<QTableWidgetItem*> list = table->selectedItems();
    QTableWidgetItem* item = list.value(0);
    QString str = item->text();
    qDebug() << str;
    QProcess::execute("kill", QStringList() << str);
    update();
}

void MainWindow::openClicked()
{
    fileName = f_d->getOpenFileName(this, tr("Open executable"), tr("/home"), tr("Executable (*)"));
    qDebug() << "open clicked  " << fileName;
    QProcess *process = new QProcess();
    process->start(fileName);
    QDir *dir = new QDir("/proc");
    QStringList list;
    QStringList temp_list;
    QString temp;
    temp.clear();
    list = dir->entryList(QStringList("*"), QDir::AllDirs);
    temp_list = fileName.split('/');
    path = fileName;
    fileName = temp_list.last();
    foreach(QString str, list) {
        if(str.toInt()) {
            QFile file = {"/proc/" + str.toUtf8() + "/comm"};               //23-vms 24-rss pages (one page - 4kB)
            if(!file.open(QIODevice::ReadOnly))
                qDebug() << "PROCESS ERROR!" << "Unable to open entry";
            temp = file.readAll();
            temp.chop(1);
            if(temp == fileName)
            {
                procPID = str;
            }

        }
    }
    update_timer->start(1000);
}

MainWindow::~MainWindow()
{
    delete ui;
}

