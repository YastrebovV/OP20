#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QDateEdit>
#include <QGridLayout>
#include "qcustomplot.h"
#include "databaseclass.h"
#include <QDebug>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QFile>
#include <QMessageBox>
QT_CHARTS_USE_NAMESPACE

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private:
    struct Times{
        uint timeStop = 0;
        uint timeWork = 0;
        uint timeAlarm = 0;
    };
    void initWidget();
    void customPlotDraw(QVector<double> barsData, bool Start, QList<int> listType);
    void readGeneralData(Times *times);
    void readAllData(QVector <double> *listData, QList <int> *listType);
    void drawPizza(bool start);
    QString readIni();
    void saveGraphic(QCustomPlot *graphic);


private:
    Ui::MainWindow *ui;
     QCustomPlot *customPlot;    // Объявляем графическое полотно
     QPushButton *but_Date;
     QPushButton *but_Save_Graphic;
     QLabel *label1;
     QDateEdit *dateEdit1;
     QGridLayout *gridLayout;
     QLabel *label2;
     QDateEdit *dateEdit2;
     QTableView *tableView1;
     QCheckBox *checkBox1;
     QString path;
     DataBaseClass dbc;
     QSqlDatabase db_dan;

public:

private slots:
    void slotButClick();
    void saveToFile();
};
#endif // MAINWINDOW_H
