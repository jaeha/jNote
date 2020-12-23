/*
 * JDB.cpp
 *
 *  Created on: Mar 09, 2016
 *      Author: jaeha
 *      version: 2.0
 */

#include "jdb.h"
#include "global.h"

JDB::JDB(QObject *parent) : QObject(parent)
{
}

bool JDB::upgrade(QString path)
{
    message(DEBUG, "JDB", "upgrade(): " + path);

    QString oldPath = path + ".before_upgrade";

    if (!QFile::rename(path, oldPath)) {
        message(ERROR, "JDB", "Failed to rename current db to " + oldPath);
        return false;
    }

    // create new db
    createDB(path);

    if (import(oldPath, "upgraded"))
        message(DEBUG, "JDB", "Upgrade was done successfully!");

    return true;
}

// import db
bool JDB::import(QString path, QString category)
{
    message(DEBUG, "JDB", "import():" + path);

    //db1: source
    QSqlDatabase sourceDB = QSqlDatabase::addDatabase("QSQLITE","sourceDB");
    sourceDB.setDatabaseName(path);
    sourceDB.open();
    QSqlQuery query_sourceDB(sourceDB);

    if (category.isEmpty())
        category = "imported";
    int cid = insertNewCategory(category);

    query_sourceDB.exec(QString("SELECT note, tag, attached FROM notes;"));
    while (query_sourceDB.next()) {
        insert("notes", QStringList() << query_sourceDB.value(0).toString()\
                                       << QString("%1").arg(cid)\
                                       << query_sourceDB.value(1).toString()\
                                       << query_sourceDB.value(2).toString());
    }

    message(DEBUG, "JDB", "import was done successfully!");

    sourceDB.close();
    sourceDB.removeDatabase("sourceDB");

    return true;
}

bool JDB::open(QString path)
{
    message(DEBUG, "JDB", "open() "+ path);

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(path);

   if (!db.open()) {
       message(ERROR, "JDB", QString("failed open %1 due to %2").arg(path).arg(db.lastError().text()));
       return false;
    }

    message(DEBUG, "JDB", QString("%1 opened successfully.").arg(path));
    return true;
}

bool JDB::execQuery(QString sql)
{
    QSqlQuery query;

    if (!query.exec(sql)) {
        message(ERROR, "execQuery()", QString("Failed to execute '%1' with error, %2").arg(sql).arg(query.lastError().text()));
        return false;
    }

    return true;
}

int JDB::insert(QString table, QStringList list)
{
    int lastID = NO_DATA;
    QSqlQuery query;
    QString smt = QString(tr("INSERT INTO %1 VALUES(NULL, %2 DATETIME('NOW'));").arg(table).arg(QString("?,").repeated(list.size())));

    query.prepare(smt);
    for (int i=0; i<list.size(); i++) {
            query.addBindValue(list[i]);
    }

    if (query.exec())
        lastID = query.lastInsertId().toInt();
    else
        message(ERROR, "insert()", query.lastError().text());

    query.finish();
    return lastID;
}

bool JDB::remove(QString table, int id)
{
    QString query = QString("DELETE FROM %1 WHERE id = %2").arg(table).arg(id);
    return execQuery(query);
}

bool JDB::update(QString table, int id, QString column, QString value)
{
    if (id < 0) {
            message(ERROR, "JDB", QString("id(%1) is invalid.").arg(id));
            return false;
    }

    QSqlQuery query;
    QString sql = QString("UPDATE %1 SET %2 = :value, lastmodified = DATETIME('NOW') WHERE id = :id").arg(table).arg(column);

    query.prepare(sql);
    query.bindValue(":id", id);
    query.bindValue(":value", value);

    if (!query.exec()) {
        message(ERROR, "update()", QString("Failed to update table(%1), %2").arg(table).arg(query.lastError().text()));
        return false;
    }

    query.finish();
    return true;
}

// list of record for single column
QStringList JDB::records(QString table, QString column)
{
    QSqlQuery query;
    QStringList list;
    QString sql = QString("SELECT %1 FROM %2 ORDER BY %1").arg(column).arg(table);

    query.exec(sql);
    while (query.next() && query.isValid())
            list.append(query.value(0).toString());

    return list;
}

QVariant JDB::record(QString table, int id, QString column)
{
    QSqlQuery query;
    QString smt = QString("SELECT %1 FROM %2 WHERE id=%3").arg(column).arg(table).arg(id);

    query.exec(smt);
    query.first();
    if (query.isValid())
        return query.value(0);
    else
        return "";
}

IdMap JDB::recordMap(QString table, QString column, QString where)
{
    QSqlQuery query;
    IdMap map;
    QString sql = QString("SELECT id,%1 FROM %2").arg(column).arg(table);
    if (!where.isEmpty())
            sql += QString(" WHERE %1").arg(where);
    sql += QString(" ORDER BY %1").arg(column);

    query.exec(sql);
    while (query.next() && query.isValid()) {
            int id = query.value(0).toInt();
            QString text = query.value(1).toString();
            map.insert(id, FIRSTLINE(text)); /// only add first line to list
    }
    return map;
}

int JDB::counter (QString table, QString where)
{
    QSqlQuery query;
    QString sql = QString("SELECT count(*) FROM %1").arg(table);
    if (!where.isEmpty())
            sql += QString(" WHERE %1").arg(where);

    qDebug() << sql;
    query.exec(sql);
    query.first();
    if (query.isValid())
        return query.value(0).toInt();
    else
        return NO_DATA;
}

int JDB::desc2id(QString table, QString desc)
{
    QSqlQuery query;
    QString sql = QString("SELECT id FROM %1 where desc = '%2'").arg(table).arg(desc);

    query.exec(sql);
    query.first();
    if (query.isSelect())
        return query.value(0).toInt();
    else
        return NO_DATA;
}

bool JDB::updateLastModified(QString table, int id)
{
    if (id < 0)
        return false;

    QString sql = QString("UPDATE %1 SET lastmodified = DATETIME('NOW') WHERE id = %2").arg(table).arg(id);
    return execQuery(sql);
}

void JDB::close()
{
    QSqlDatabase db = QSqlDatabase::database();
    if (db.isOpen())
        db.close();

    db.removeDatabase(db.connectionName());
}

JDB::~JDB()
{
}


/// jnote specific

bool JDB::createDB(QString path)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(path);
    db.open();

    // create tables
    QString sqlNoteTable = "CREATE TABLE notes (id INTEGER primary key, note TEXT, category_id int, " \
                           "tag VARCHAR(256), attached VARCHAR(256), lastmodified DATE);";
    QString sqlCategoryTable = "CREATE TABLE category (id INTEGER primary key, desc varchar(128), lastmodified DATE);";
    QString sqlInfoTable = "CREATE TABLE info (app_title VARCHAR(16), db_version INTEGER, db_created DATE);";
    QString sqlInfoInsert = QString("INSERT INTO info (app_title, db_version, db_created) \
                                        VALUES ('%1', %2, DATETIME('NOW'));").arg(APP_TITLE).arg(DB_VERSION);

    if (!execQuery(sqlNoteTable) || !execQuery(sqlCategoryTable)) {
        message(ERROR, "createTable()", "Failed create data table.");
        return false;
    }

    if (!execQuery(sqlInfoTable) || !execQuery(sqlInfoInsert)) {
        message(ERROR, "createTable()", "Failed create info table.");
        return false;
    }
 //   db.close();
    return true;
}
