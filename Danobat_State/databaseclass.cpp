#include "databaseclass.h"

DataBaseClass::DataBaseClass()
{

}

void DataBaseClass::conn_DB(QSqlDatabase *db, QString type, QString stringConn)
{
     *db = QSqlDatabase::addDatabase(type);
     db->setDatabaseName(stringConn);
}

bool DataBaseClass::open_conn_DB(QSqlDatabase *db)
{
    if (db->open()) {
        return true;
    }else return  false;
}

void DataBaseClass::close_conn_DB(QSqlDatabase *db)
{
   db->close();
}
