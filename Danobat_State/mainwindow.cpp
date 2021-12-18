#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    initWidget();
    path = readIni();
    dbc.conn_DB(&db_dan,"QODBC", "DRIVER={Microsoft Access Driver (*.mdb)};FIL={MS Access};DBQ="+path+"\\Efficiency.mdb");
    QVector<double> barsData;
    QList<int> listType;
    customPlotDraw(barsData, true, listType);
    drawPizza(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initWidget()
{
    gridLayout = new QGridLayout();
    customPlot = new QCustomPlot(); // Инициализируем графическое полотно
    gridLayout->addWidget(customPlot,0,0,1,4);  // Устанавливаем customPlot в окно проложения

      but_Date = new QPushButton("ПОСТРОИТЬ ГРАФИК");
      but_Date->setStyleSheet("QPushButton"
                              "{"
                              "background-color: rgb(202, 214, 219); border-style: outset;border-width:"
                                                          " 1px; border-radius: 10px;border-color: rgb(0, 0, 0);"
                                                          "font: Arial 14px;min-width: 10em; padding: 6px;"
                              "}"
                              "QPushButton::pressed"
                              "{"
                              "background-color: rgb(113, 160, 183); border-style: outset;border-width: 1px;"
                              " border-radius: 10px;border-color: rgb(0, 0, 0);"
                              "font: Arial 14px;min-width: 10em; padding: 6px;"
                              "}"
                              );
      but_Date->setFixedHeight(60);
      but_Date->setFixedWidth(250);
      gridLayout->addWidget(but_Date,2,0,1,2);

      but_Save_Graphic = new QPushButton("СОХРАНИТЬ В ФАЙЛ");
      but_Save_Graphic->setStyleSheet("QPushButton"
                              "{"
                              "background-color: rgb(202, 214, 219); border-style: outset;border-width:"
                                                          " 1px; border-radius: 10px;border-color: rgb(0, 0, 0);"
                                                          "font: Arial 14px;min-width: 10em; padding: 6px;"
                              "}"
                              "QPushButton::pressed"
                              "{"
                              "background-color: rgb(113, 160, 183); border-style: outset;border-width: 1px;"
                              " border-radius: 10px;border-color: rgb(0, 0, 0);"
                              "font: Arial 14px;min-width: 10em; padding: 6px;"
                              "}"
                              );
      but_Save_Graphic->setFixedHeight(60);
      but_Save_Graphic->setFixedWidth(250);
      gridLayout->addWidget(but_Save_Graphic,2,2,1,2);

      label1 = new QLabel("Дата:");
      label1->setFixedWidth(50);
      label1->setFont(QFont("Arial", 12));
      gridLayout->addWidget(label1,1,0);

      dateEdit1 = new QDateEdit();
      dateEdit1->setFixedWidth(115);
      dateEdit1->setStyleSheet("background-color: rgb(235, 238, 240); border-style: outset;border-width:"
                               " 1px; border-radius: 5px;border-color: rgb(0, 0, 0);"
                               "font: Arial 14px;min-width: 10em; padding: 6px;");
      dateEdit1->setCalendarPopup(true);
      gridLayout->addWidget(dateEdit1,1,1,1,1);

//      label2 = new QLabel("Дата 2:");
//      label2->setFixedWidth(70);
//      gridLayout->addWidget(label2,1,2);

  //    dateEdit2 = new QDateEdit();
  //    dateEdit2->setFixedWidth(115);
  //    gridLayout->addWidget(dateEdit2,1,3,1,1);
  //    dateEdit2->setDate(QDate::currentDate());

  //    checkBox1 = new QCheckBox();
  //    checkBox1->setFixedWidth(250);
  //    gridLayout->addWidget(checkBox1,1,4,1,1);
  //    checkBox1->setChecked(false);
  //    checkBox1->setText("Выборка по диапазону дат");

      QDate date = QDate::currentDate();
      dateEdit1->setDate(date);

      ui->centralwidget->setLayout(gridLayout);

      connect(but_Date, SIGNAL(clicked()), this, SLOT(slotButClick()));
      connect(but_Save_Graphic, SIGNAL(clicked()), this, SLOT(saveToFile()));
}

QString MainWindow::readIni()
{
    QString path = QCoreApplication::applicationDirPath() + "\\ini.txt";
    QString pathDB;
       QFile ini(path);
      if(ini.open(QIODevice::ReadOnly)){
         while(!ini.atEnd()){
             QString str = ini.readLine();
             QStringList lst = str.split('=');

             if(lst[0]=="Path"){
                 pathDB = lst[1];
             }
         }
       }
      return pathDB;
}
void MainWindow::saveGraphic(QCustomPlot *graphic)
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), "",
        tr("JPEG (*.jpg);;PDF (*.pdf)"));

        if (fileName != "") {
            QFile file(fileName);
            if (!file.open(QIODevice::WriteOnly)) {
                // error message
            } else {
                graphic->saveJpg(fileName);
                file.close();
            }
        }
}
void MainWindow::readGeneralData(Times *times){
    uint tempTimeWork = 0;
    uint tempTimeStop = 0;
    uint tempTimeAlarm = 0;
    bool StopState = false;
    bool AlarmState = false;
    bool first_element = false;

    if(!dbc.open_conn_DB(&db_dan)){
        QMessageBox msg;
        msg.setText(db_dan.lastError().text());
        msg.exec();
    }

    QString date_Format = dateEdit1->date().toString("MM/dd/yyyy");
    QString date_Format_Priv = dateEdit1->date().addDays(-1).toString("MM/dd/yyyy");

    QSqlQuery query, query2;
    query.exec("SELECT * FROM STATE_EVENTS WHERE EVENT_DATE BETWEEN #"+date_Format+" 00:00:00"+"# AND #"+date_Format+" 23:59:59# ORDER BY ID;");
    query2.exec("SELECT * FROM STATE_EVENTS WHERE EVENT_DATE BETWEEN #"+date_Format_Priv+" 00:00:00"+"# AND #"+date_Format_Priv+" 23:59:59# ORDER BY ID;");

    if(query.lastError().isValid()){
       QMessageBox msg;
       msg.setText(query.lastError().text());
       msg.exec();
    }
    if(query2.lastError().isValid()){
       QMessageBox msg;
       msg.setText(query2.lastError().text());
       msg.exec();
    }

      while(query2.next()){
       if(query2.value(2).toInt()==14 && query2.value(4).toString()=="ON"){
          StopState = false;
          AlarmState = false;
       }
       if(query2.value(2).toInt()==14 && query2.value(4).toString()=="OFF"){
           StopState = true;
           AlarmState = false;
       }
       if(query2.value(2).toInt()==2 && query2.value(4).toString()=="ON"){
           StopState = false;
           AlarmState = true;
       }
      }

    while(query.next()){
        if(!first_element && (query.value(2).toInt()==14 || query.value(2).toInt()==2)){
            QString temp_str =  date_Format + " 00:00:00";
            QDateTime tempDT = QDateTime::fromString(temp_str, "MM/dd/yyyy hh:mm:ss");
            uint tempUTC = tempDT.toTime_t();

            if(query.value(2).toInt()==14 && query.value(4).toString()=="ON"){
                tempTimeWork = query.value(3).toDateTime().toTime_t();
                if(StopState)
                    if((tempTimeWork && tempTimeStop) != 0)
                       times->timeStop += tempTimeWork - tempUTC;
                if(AlarmState)
                     if((tempTimeWork && tempTimeAlarm) != 0)
                         times->timeAlarm += tempTimeWork - tempUTC;
            }
            if(query.value(2).toInt()==14 && query.value(4).toString()=="OFF" && !AlarmState){
                tempTimeStop = query.value(3).toDateTime().toTime_t();
                 times->timeWork += tempTimeStop - tempUTC;
            }
            if(query.value(2).toInt()==2 && query.value(4).toString()=="ON" && !StopState){
                tempTimeAlarm = query.value(3).toDateTime().toTime_t();
                times->timeWork += tempTimeAlarm - tempUTC;
            }

            StopState = false;
            AlarmState = false;
            first_element = true;
        }

        if(query.value(2).toInt()==14 && query.value(4).toString()=="ON"){
            tempTimeWork = query.value(3).toDateTime().toTime_t();
            if(StopState)
                if((tempTimeWork && tempTimeStop) != 0)
                   times->timeStop += tempTimeWork - tempTimeStop;
            if(AlarmState)
                 if((tempTimeWork && tempTimeAlarm) != 0)
                     times->timeAlarm += tempTimeWork - tempTimeAlarm;

            StopState = false;
            AlarmState = false;
        }
        if(query.value(2).toInt()==14 && query.value(4).toString()=="OFF" && !AlarmState && !StopState){
            tempTimeStop = query.value(3).toDateTime().toTime_t();
            StopState = true;
             if((tempTimeStop && tempTimeWork) != 0)
                  times->timeWork += tempTimeStop - tempTimeWork;
        }
        if(query.value(2).toInt()==2 && query.value(4).toString()=="ON" && !StopState && !AlarmState){
            tempTimeAlarm = query.value(3).toDateTime().toTime_t();
            AlarmState = true;
            if((tempTimeAlarm && tempTimeWork) != 0)
                   times->timeWork += tempTimeAlarm - tempTimeWork;
        }
    }

    QString temp_str =  date_Format + " 23:59:59";
    QDateTime tempDT = QDateTime::fromString(temp_str, "MM/dd/yyyy hh:mm:ss");
    uint tempUTC = tempDT.toTime_t();

    if(!StopState && !AlarmState)
         times->timeWork += tempUTC - tempTimeWork;
    if(StopState)
         times->timeStop += tempUTC - tempTimeStop;
    if(AlarmState)
         times->timeAlarm += tempUTC - tempTimeAlarm;

    dbc.close_conn_DB(&db_dan);
}
void MainWindow::readAllData(QVector <double> *listData, QList <int> *listType){
    uint tempTimeWork = 0;
    uint tempTimeStop = 0;
    uint tempTimeAlarm = 0;
    bool StopState = false;
    bool AlarmState = false;

    bool first_element = false;

    if(!dbc.open_conn_DB(&db_dan)){
        QMessageBox msg;
        msg.setText(db_dan.lastError().text());
        msg.exec();
    }

    QString date_Format = dateEdit1->date().toString("MM/dd/yyyy");
    QString date_Format_Priv = dateEdit1->date().addDays(-1).toString("MM/dd/yyyy");

    QSqlQuery query, query2;
    query.exec("SELECT * FROM STATE_EVENTS WHERE EVENT_DATE BETWEEN #"+date_Format+" 00:00:00"+"# AND #"+date_Format+" 23:59:59# ORDER BY ID;");
    query2.exec("SELECT * FROM STATE_EVENTS WHERE EVENT_DATE BETWEEN #"+date_Format_Priv+" 00:00:00"+"# AND #"+date_Format_Priv+" 23:59:59# ORDER BY ID;");

      if(query.lastError().isValid()){
         QMessageBox msg;
         msg.setText(query.lastError().text());
         msg.exec();
      }
      if(query2.lastError().isValid()){
         QMessageBox msg;
         msg.setText(query2.lastError().text());
         msg.exec();
      }

      while(query2.next()){
       if(query2.value(2).toInt()==14 && query2.value(4).toString()=="ON"){
          StopState = false;
          AlarmState = false;
       }
       if(query2.value(2).toInt()==14 && query2.value(4).toString()=="OFF"){
           StopState = true;
           AlarmState = false;
       }
       if(query2.value(2).toInt()==2 && query2.value(4).toString()=="ON"){
           StopState = false;
           AlarmState = true;
       }
      }

       while(query.next()){
          if(!first_element && (query.value(2).toInt()==14 || query.value(2).toInt()==2)){
              QString temp_str =  date_Format + " 00:00:00";
              QDateTime tempDT = QDateTime::fromString(temp_str, "MM/dd/yyyy hh:mm:ss");
              uint tempUTC = tempDT.toTime_t();

              if(query.value(2).toInt()==14 && query.value(4).toString()=="ON"){
                  tempTimeWork = query.value(3).toDateTime().toTime_t();
                  if(StopState)
                      if((tempTimeWork && tempTimeStop) != 0){
                          listData->push_back(tempTimeWork - tempUTC);
                          listType->push_back(2);
                      }
                         //timeStop += tempTimeWork - tempUTC;
                  if(AlarmState)
                       if((tempTimeWork && tempTimeAlarm) != 0){
                           listData->push_back(tempTimeWork - tempUTC);
                           listType->push_back(1);
                           //timeAlarm += tempTimeWork - tempUTC;
                       }
              }
              if(query.value(2).toInt()==14 && query.value(4).toString()=="OFF" && !AlarmState){
                  tempTimeStop = query.value(3).toDateTime().toTime_t();
                  listData->push_back(tempTimeStop - tempUTC);
                  listType->push_back(0);
                  // timeWork += tempTimeStop - tempUTC;
              }
              if(query.value(2).toInt()==2 && query.value(4).toString()=="ON" && !StopState){
                  tempTimeAlarm = query.value(3).toDateTime().toTime_t();
                  listData->push_back(tempTimeAlarm - tempUTC);
                  listType->push_back(0);
                  //timeWork += tempTimeAlarm - tempUTC;
              }

              StopState = false;
              AlarmState = false;
              first_element = true;
          }

          if(query.value(2).toInt()==14 && query.value(4).toString()=="ON"){
              tempTimeWork = query.value(3).toDateTime().toTime_t();
              if(StopState)
                  if((tempTimeWork && tempTimeStop) != 0){
                     listData->push_back(tempTimeWork - tempTimeStop);
                     listType->push_back(2);
                    // timeStop += tempTimeWork - tempTimeStop;
                  }
              if(AlarmState)
                   if((tempTimeWork && tempTimeAlarm) != 0){
                       listData->push_back(tempTimeWork - tempTimeAlarm);
                       listType->push_back(1);
                      //timeAlarm += tempTimeWork - tempTimeAlarm;
                   }

              StopState = false;
              AlarmState = false;
          }
          if(query.value(2).toInt()==14 && query.value(4).toString()=="OFF" && !StopState && !AlarmState){
              tempTimeStop = query.value(3).toDateTime().toTime_t();
              StopState = true;
               if((tempTimeStop && tempTimeWork) != 0){
                   listData->push_back(tempTimeStop - tempTimeWork);
                   listType->push_back(0);
                  //  timeWork += tempTimeStop - tempTimeWork;
               }
          }
          if(query.value(2).toInt()==2 && query.value(4).toString()=="ON" && !StopState && !AlarmState){
              tempTimeAlarm = query.value(3).toDateTime().toTime_t();
              AlarmState = true;
              if((tempTimeAlarm && tempTimeWork) != 0){
                  listData->push_back(tempTimeAlarm - tempTimeWork);
                  listType->push_back(0);
                  //timeWork += tempTimeAlarm - tempTimeWork;
              }
          }
      }

      QString temp_str =  date_Format + " 23:59:59";
      QDateTime tempDT = QDateTime::fromString(temp_str, "MM/dd/yyyy hh:mm:ss");
      uint tempUTC = tempDT.toTime_t();

      if(!StopState && !AlarmState){
          listData->push_back(tempUTC - tempTimeWork);
          listType->push_back(0);
          // timeWork += tempUTC - tempTimeWork;
      }
      if(StopState){
          listData->push_back(tempUTC - tempTimeStop);
          listType->push_back(2);
          // timeStop += tempUTC - tempTimeStop;
      }
      if(AlarmState){
          listData->push_back(tempUTC - tempTimeAlarm);
          listType->push_back(1);
          // timeAlarm += tempUTC - tempTimeAlarm;
      }

      dbc.close_conn_DB(&db_dan);
}
void MainWindow::drawPizza(bool start)
{
    qreal percentAl = 25;
    qreal percentSt = 25;
    qreal percentWr = 50;

  if(start){
    Times times;
    times.timeAlarm = 0;
    times.timeStop = 0;
    times.timeWork = 0;
    readGeneralData(&times);

    uint sum = times.timeAlarm + times.timeStop + times.timeWork;

    percentAl = 100.0 * times.timeAlarm / sum;
    percentSt = 100.0 * times.timeStop / sum;
    percentWr = 100.0 * times.timeWork / sum;
  }

    QPieSeries *series = new QPieSeries();
    QPieSlice *sl = new QPieSlice();
    series->setHoleSize(0.5);
    series->setPieSize(0.8);
    sl->setColor(QColor(229, 25, 25));
    sl->setLabel("Ошибки "+QString::number(percentAl) + "%");
    sl->setValue(percentAl);
    sl->setLabelFont(QFont("Arial", 10));
    sl->setLabelVisible();
    series->append(sl);

    QPieSlice *sl2 = new QPieSlice();
    sl2->setColor(QColor(0, 168, 140));
    sl2->setLabel("Работа "+QString::number(percentWr) + "%");
    sl2->setValue(percentWr);
    sl2->setLabelFont(QFont("Arial", 10));
    sl2->setLabelVisible();
    series->append(sl2);

    QPieSlice *sl3 = new QPieSlice();
    sl3->setColor(QColor(250, 170, 20));
    sl3->setLabel("Простои "+QString::number(percentSt) + "%");
    sl3->setValue(percentSt);
    sl3->setLabelFont(QFont("Arial", 10));
    sl3->setLabelVisible();
    series->append(sl3);

    QChartView *chartView = new QChartView();
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->chart()->setTitle("Статистика работы");
    chartView->chart()->setTitleFont(QFont("Arial", 16));
    chartView->chart()->addSeries(series);
    chartView->chart()->legend()->setAlignment(Qt::AlignBottom);
    //chartView->chart()->setTheme(QChart::ChartThemeBlueNcs);
    chartView->chart()->legend()->setFont(QFont("Arial", 11));
    chartView->setStyleSheet("background-color: rgb(178, 190, 195);");
  //  chartView->setFixedWidth(600);
    chartView->chart()->setBackgroundVisible(false);
    gridLayout->addWidget(chartView,0,5,3,2);

}
void MainWindow::customPlotDraw(QVector<double> barsData, bool start, QList<int> listType)
{
    QCPBarsGroup *group1 = nullptr;
    int numBars = barsData.size();

    customPlot->clearPlottables();

    // set dark background gradient:
    QLinearGradient gradient(0, 0, 0, 400);
    gradient.setColorAt(0, QColor(90, 90, 90));
    gradient.setColorAt(0.38, QColor(105, 105, 105));
    gradient.setColorAt(1, QColor(70, 70, 70));
    customPlot->setBackground(QBrush(gradient));

    // create empty bar chart objects:
    QCPBars *bar;
    group1 = new QCPBarsGroup(customPlot);
    for(int i=0; i<numBars; i++){
        bar = new QCPBars(customPlot->yAxis, customPlot->xAxis);
        switch (listType[i]){
        case 0: //work
            bar->setPen(QPen(QColor(0, 168, 140).lighter(130)));
            bar->setBrush(QColor(0, 168, 140)); break;
        case 1: //alarm
            bar->setPen(QPen(QColor(229, 25, 25).lighter(170)));
            bar->setBrush(QColor(229, 25, 25));break;
        case 2://stop
            bar->setPen(QPen(QColor(250, 170, 20).lighter(150)));
            bar->setBrush(QColor(250, 170, 20)); break;
        }

        bar->setAntialiased(false);
        bar->setStackingGap(0.6);
        bar->setWidth(0.4);
        group1->append(bar);
        //bar->setBarsGroup(group1);
    }

    // prepare x axis with country labels:
    QVector<double> ticks;
    QVector<QString> labels;
    ticks << 0.4 ;
    labels << "OP20B";
    QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
    textTicker->addTicks(ticks, labels);

    customPlot->yAxis->setTicker(textTicker);
    customPlot->yAxis->setTickLabelRotation(60);
    customPlot->yAxis->setSubTicks(false);
    customPlot->yAxis->setTickLength(0, 4);
    customPlot->yAxis->setRange(0, 1);
    customPlot->yAxis->setBasePen(QPen(Qt::white));
    customPlot->yAxis->setTickPen(QPen(Qt::white));
    customPlot->yAxis->grid()->setVisible(true);
    customPlot->yAxis->grid()->setPen(QPen(QColor(130, 130, 130), 0, Qt::DotLine));
    customPlot->yAxis->setTickLabelColor(Qt::white);
    customPlot->yAxis->setLabelColor(Qt::white);

    // prepare x axis:
    QSharedPointer<QCPAxisTickerTime> dataTimeTicker(new QCPAxisTickerTime);
    dataTimeTicker.data()->setTickCount(20);
    customPlot->xAxis->setTicker(dataTimeTicker);

    dataTimeTicker->setTimeFormat("%h:%m");
    customPlot->xAxis->setRange(0, 86400);

    customPlot->xAxis->setPadding(5); // a bit more space to the left border

    customPlot->xAxis->setLabel("Шкала времени");
    customPlot->xAxis->setBasePen(QPen(Qt::white));
    customPlot->xAxis->setTickPen(QPen(Qt::white));
    customPlot->xAxis->setSubTickPen(QPen(Qt::white));
    customPlot->xAxis->grid()->setSubGridVisible(true);
    customPlot->xAxis->grid()->setPen(QPen(QColor(130, 130, 130), 0, Qt::SolidLine));
    customPlot->xAxis->setTickLabelColor(Qt::white);
    customPlot->xAxis->setLabelColor(Qt::white);
    customPlot->xAxis->grid()->setSubGridPen(QPen(QColor(130, 130, 130), 0, Qt::DotLine));

    // Add data:
    QCPBars *cpBar;
    QCPBars *cpBar2;
    for(int i=0; i<numBars; i++){
       cpBar = group1->bars(i);
       if(i>0){
          cpBar2 = group1->bars(i-1);
          cpBar->moveAbove(cpBar2);
       }
      // if(i==1){
       //   cpBar2 = group1->bars(i-1);
       //   cpBar->moveBelow(cpBar2);
     //  }//else cpBar->moveAbove(cpBar);

       QVector<double> barsDataLoc;
       barsDataLoc << barsData[i];
       cpBar->setData(ticks, barsDataLoc);
      }

//    // setup legend:
//    customPlot->legend->setVisible(true);
//    customPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop|Qt::AlignHCenter);
//    customPlot->legend->setBrush(QColor(255, 255, 255, 100));
//    customPlot->legend->setBorderPen(Qt::NoPen);
//    QFont legendFont = font();
//    legendFont.setPointSize(10);
//    customPlot->legend->setFont(legendFont);
    customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

  //  customPlot->rescaleAxes();
     customPlot->replot();

}

void MainWindow::slotButClick()
{
    QVector <double> listData;
    QList <int> listType;    
    readAllData(&listData, &listType);
    customPlotDraw(listData,false,listType);
    drawPizza(true);
}
void MainWindow::saveToFile()
{
    saveGraphic(customPlot);
}
