#ifndef DATABASECLASS_H
#define DATABASECLASS_H

#include <QSql>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QSqlError>

class DataBaseClass
{
public:
    DataBaseClass();
    void conn_DB(QSqlDatabase *db, QString type, QString stringConn);
    bool open_conn_DB(QSqlDatabase *db);
    void close_conn_DB(QSqlDatabase *db);

};

#endif // DATABASECLASS_H
