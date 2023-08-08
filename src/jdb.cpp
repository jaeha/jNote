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

int JDB::insert(QString table, QStringList list, bool isAutoID)
{
    int lastID = NO_DATA;
    QSqlQuery query;
    QString smt;
    if (isAutoID)
        smt = QString(tr("INSERT INTO %1 VALUES(NULL, %2 DATETIME('NOW'));").arg(table).arg(QString("?,").repeated(list.size())));
    else
        smt = QString(tr("INSERT INTO %1 VALUES(%2 DATETIME('NOW'));").arg(table).arg(QString("?,").repeated(list.size())));

    query.prepare(smt);
    for (int i=0; i<list.size(); i++) {
            query.addBindValue(list[i]);
    }

   // qDebug() << "insert() " << smt;

    if (query.exec())
        lastID = query.lastInsertId().toInt();
    else
        message(ERROR, "insert()", query.lastError().text());

    query.finish();
    return lastID;
}

bool JDB::remove(QString table, int id)
{
    qDebug() << "JDB::remove(): " << id;
    QString query = QString("DELETE FROM %1 WHERE id = %2").arg(table).arg(id);
    return execQuery(query);
}

bool JDB::update(QString table, int id, QString column, QVariant value, bool updateTime)
{
    if (id < 0) {
            message(ERROR, "JDB", QString("id(%1) is invalid.").arg(id));
            return false;
    }

    QSqlQuery query;
    QString sql = QString("UPDATE %1 SET %2 = :value").arg(table).arg(column);
    if (updateTime)
        sql = sql + QString(", lastmodified = DATETIME('NOW')");
    sql = sql + QString(" WHERE id = :id");

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

int JDB::removeDuplicated(QString table, QString column)
{
    QString where = QString("rowid NOT IN (SELECT min(rowid) FROM %1 GROUP BY %2)")
                            .arg(table).arg(column);
    int countDuplicated = counter("notes", where);
    QString sql = QString("DELETE FROM %1 WHERE %2").arg(table).arg(where);

    if (execQuery(sql))
        return countDuplicated;
    else
        return NO_DATA;
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

bool JDB::createDB(QString path, bool withDefault)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(path);
    db.open();

    // create tables
    QString sqlNoteTable = "CREATE TABLE notes (id INTEGER primary key, note TEXT, category_id int, " \
                           "tag VARCHAR(256), lastmodified DATE);";
    QString sqlCategoryTable = "CREATE TABLE category (id INTEGER primary key, desc varchar(128), lastmodified DATE);";
    QString sqlAttachmentTable = "CREATE TABLE attachment (id INTEGER primary key, filename varchar(256), note_id INTEGER, lastmodified DATE);";
    QString sqlInfoTable = "CREATE TABLE info (app_title VARCHAR(16), db_version INTEGER, db_created DATE);";
    QString sqlInfoInsert = QString("INSERT INTO info (app_title, db_version, db_created) \
                                        VALUES ('%1', %2, DATETIME('NOW'));").arg(APP_TITLE).arg(DB_VERSION);

    if (!execQuery(sqlNoteTable) ||
        !execQuery(sqlCategoryTable) ||
        !execQuery(sqlAttachmentTable )) {
        message(ERROR, "createTable()", "Failed create data table.");
        return false;
    }

    if (!execQuery(sqlInfoTable) || !execQuery(sqlInfoInsert)) {
        message(ERROR, "createTable()", "Failed create info table.");
        return false;
    }

    //default
    if (withDefault) {
        insertCategory("Presonal");
        insertCategory("Work");
    }
 //   db.close();
    return true;
}

bool JDB::upgrade(QString path)
{
    message(DEBUG, "JDB", "upgrade(): " + path);

    QString oldPath = path + ".before_upgrade";

    if (!QFile::rename(path, oldPath)) {
        message(ERROR, "JDB", "Failed to rename current db to " + oldPath);
        return false;
    }

    // this upgrade is only for previous version 3.2 (DB=600)

    // create new db
    createDB(path, false);

    QSqlDatabase odb = QSqlDatabase::addDatabase("QSQLITE","oldDB");
    odb.setDatabaseName(oldPath);
    odb.open();
    QSqlQuery query(odb);

    // category
    query.exec(QString("SELECT id, desc, lastmodified FROM category;"));
    while (query.next()) {
        int id = query.value(0).toInt();
        insert("category", QStringList()<<INT2STR(id)
                            <<query.value(1).toString(), false);
        update("category", id, "lastmodified", query.value(2).toString(), false);
    }

    // notes and attachments
    query.exec(QString("SELECT id, note, category_id, tag, attached, lastmodified FROM notes;"));
    while (query.next()) {
        int id = query.value(0).toInt();
        insert("notes", QStringList()<<INT2STR(id)
                            <<query.value(1).toString()
                            <<query.value(2).toString()
                            <<query.value(3).toString(), false);
        update("notes", id, "lastmodified", query.value(5).toString(), false);

        QString attachment = query.value(4).toString();
        if (!attachment.isEmpty())
            insertAttachment(attachment, id);
    }

    return true;
}

// import db
int JDB::import(QString path)
{
    message(DEBUG, "JDB", "import():" + path);

    // create temporary table for ids
    QString sqlTempIds = "CREATE TABLE tempids (id INTEGER, newid INTEGER, lastmodified DATE);";
    execQuery(sqlTempIds);

    int countInserted = 0;

    QSqlDatabase idb = QSqlDatabase::addDatabase("QSQLITE","importDB");
    idb.setDatabaseName(path);
    idb.open();
    QSqlQuery query(idb);

    // DB=600
   query.exec(QString("SELECT n.id, c.desc, n.note, n.tag, n.lastmodified FROM notes n JOIN category c \
                                        ON n.category_id=c.id"));
    while (query.next()) {
    QString cat = "imported-" + query.value(1).toString();
        int cid = desc2id("category", cat);
        if (cid <= 0)
            cid = insertCategory(cat);
        int newID = insert("notes", QStringList()<<query.value(2).toString()
                            <<INT2STR(cid)<<query.value(3).toString());
        if (newID > 0) {
            update("notes", newID, "lastmodified", query.value(3).toString());
            insert("tempids", QStringList()<<query.value(0).toString()<<INT2STR(newID), false);
            countInserted++;
        }
    }

    // attchments
    query.exec(QString("SELECT filename, note_id, lastmodified FROM attachment"));
    while (query.next()) {
        QString newID = record("tempids", query.value(1).toInt(), "newid").toString();
        insert("attachment", QStringList()<<query.value(0).toString()<<newID);
    }

    execQuery("drop table tempids");
    return countInserted;
}
