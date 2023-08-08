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

int JDB::insert(QString table, QStringList list, bool isAuto)
{
    int lastID = NO_DATA;
    QSqlQuery query;
    QString smt;
    QString values = QString("?,").repeated(list.size());
    values.chop(1);
    if (isAuto)
        smt = QString(tr("INSERT INTO %1 VALUES(NULL, %2 , DATETIME('NOW'));")\
                      .arg(table).arg(values));
    else
        smt = QString(tr("INSERT INTO %1 VALUES(%2);").arg(table).arg(values));

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

bool JDB::update(QString table, int id, QString column, QVariant value)
{
    if (id < 0) {
       //     message(ERROR, "JDB", QString("id(%1) is invalid.").arg(id));
            return false;
    }

    QSqlQuery query;
    QString sql = QString("UPDATE %1 SET %2 = :value").arg(table).arg(column);
   // if (updateTime)
   //     sql = sql + QString(", lastmodified = DATETIME('NOW')");
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

bool JDB::replace(QString table, QString column, QVariant oldValue, QVariant newValue)
{
    QSqlQuery query;
    QString sql = QString("UPDATE %1 SET %2 = :newValue WHERE %2 = :oldValue").arg(table).arg(column);

    query.prepare(sql);
    query.bindValue(":oldValue", oldValue);
    query.bindValue(":newValue", newValue);

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

IdMap JDB::recordMap(QString table, QStringList columns, QString where)
{
    message(DEBUG, "JDB::recordMap", "start..");

    QSqlQuery query;
    IdMap map;
    QString sql = QString("SELECT id,%1 FROM %2").arg(columns.join(",")).arg(table);
    if (!where.isEmpty())
            sql += QString(" WHERE %1").arg(where);
    sql += QString(" ORDER BY %1").arg(columns.at(0));

    qDebug() << sql;

    query.exec(sql);
    while (query.next() && query.isValid()) {
        QStringList list;
        int id = query.value(0).toInt();
        for (int i=1; i<=columns.length(); i++) {
            list << FIRSTLINE(query.value(i).toString());
        }
        map.insert(id, list); /// only add first line to list
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

bool JDB::isDuplicated(QString title, QDate lastmodified)
{
    QSqlQuery query;
    QString sql = QString("SELECT lastmodified FROM notes WHERE note like '%2%'").arg(title);

    query.exec(sql);
    query.first();
    if (query.isValid() && query.value(0).toDate() >= lastmodified)
        return true;
    else
        return false;
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
    // category type - 0:Note, 1:Journal, 2:Todo
    QString sqlNoteTable = "CREATE TABLE notes (id INTEGER primary key, note TEXT, category_id int, " \
                           "tag VARCHAR(64) UNIQUE, checked int, lastmodified DATE);";
    QString sqlCategoryTable = "CREATE TABLE category (id INTEGER primary key, desc varchar(128)," \
                            "type INTEGER, lastmodified DATE);";
    QString sqlAttachmentTable = "CREATE TABLE attachment (id INTEGER primary key, filename varchar(256)," \
                            "note_id INTEGER, lastmodified DATE);";
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
        insertCategory("Presonal", 0);
        insertCategory("Work", 0);
        insertCategory("Journal", 1);
        insertCategory("Task", 2);
    }
 //   db.close();
    return true;
}

bool JDB::upgrade700(QString path)
{
    message(DEBUG, "JDB", "upgrade700(): " + path);

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
    query.first();
    while (query.next()) {
        int id = query.value(0).toInt();
        insert("category", QStringList()<<INT2STR(id)
                            <<query.value(1).toString()<<query.value(2).toString(), false);
    }

    // notes and attachments
    query.exec(QString("SELECT id, note, category_id, tag, attached, lastmodified FROM notes;"));
    query.first();
    while (query.next()) {
        int id = query.value(0).toInt();
        insert("notes", QStringList()<<INT2STR(id)
                            <<query.value(1).toString()
                            <<query.value(2).toString()
                            <<query.value(3).toString()
                            <<query.value(5).toString(), false);

        QString attachment = query.value(4).toString();
        if (!attachment.isEmpty())
            insertAttachment(attachment, id);
    }

    return true;
}

// 1. added a new field for checked(int) to notes
// 2. add unique id to tag

bool JDB::upgrade800(QString path)
{
    message(DEBUG, "JDB", "upgrade800(): " + path);

    QString oldPath = path + ".before_upgrade";

    if (!QFile::rename(path, oldPath)) {
        message(ERROR, "JDB", "Failed to rename current db to " + oldPath);
        return false;
    }

    // create new db
    createDB(path, false);

    QSqlDatabase odb = QSqlDatabase::addDatabase("QSQLITE","oldDB");
    odb.setDatabaseName(oldPath);
    odb.open();
    QSqlQuery query(odb);
    QSqlRecord record;

    // category
    // 1. new column, type=0(Note type)
    int nCategory=0;
    query.exec(QString("SELECT id, desc, lastmodified FROM category;"));
    while (query.next()) {
        int id = query.value(0).toInt();
        if (insert("category", QStringList()<<INT2STR(id)
                            <<query.value(1).toString()
                            <<INT2STR(0)
                            <<query.value(2).toString(), false))
            nCategory++;
        else
            message(ERROR, "Upgrade800", QString("Failed to insert the record, ID=%1").arg(id));
    }

    // notes
    // 1. add uuid to tag
    // 2. set the new column checked=0
    int nNotes=0;
    query.exec(QString("SELECT id, note, category_id, lastmodified FROM notes;"));
    while (query.next()) {
        QString id = query.value(0).toString();
        if (insert("notes", QStringList()<<id
                            <<query.value(1).toString()
                            <<query.value(2).toString()
                            <<createTag()
                            <<INT2STR(0)
                            <<query.value(3).toString(), false))
            nNotes++;
        else
            message(ERROR, "Upgrade800", QString("Failed to insert the record, ID=%1").arg(id));
    }

    // attchments
    int nAttachment = 0;
    query.exec(QString("SELECT id, filename, note_id, lastmodified FROM attachment"));
    while (query.next()) {
        QStringList list;
        for (int i=0; i<4; i++)
            list.append(query.value(i).toString());
        if (insert("attachment", list, false))
            nAttachment++;
        else
            message(ERROR, "Upgrade800", QString("Failed to insert the record, ID=%1").arg(query.value(0).toString()));
    }

    message(INFO, "Upgrade800", QString("Upgraded Category(%1), Notes(%2) and Attachment(%3) tables.")
                                    .arg(nCategory).arg(nNotes).arg(nAttachment));

    return true;
}

// import db
int JDB::import(QString ipath, QString bpath)
{
    message(DEBUG, "JDB", "import():" + ipath + DB_FILE);

    // create temporary table for ids
    QString sqlTempIds = "CREATE TABLE tempids (id INTEGER, newid INTEGER, lastmodified DATE);";
    execQuery(sqlTempIds);

    int nImported = 0;

    QSqlDatabase idb = QSqlDatabase::addDatabase("QSQLITE","importDB");
    idb.setDatabaseName(ipath + DB_FILE);
    if (!idb.open()) {
        message(ERROR, "Import", QString("Failed open DB on %1, error:%2")
                .arg(ipath + DB_FILE).arg(idb.lastError().text()));
        return 0;
    }

    QSqlQuery query(idb);

    // DB=600 or above
    query.exec(QString("SELECT n.id, c.desc, n.note, n.tag, n.lastmodified FROM notes n JOIN category c \
                                        ON n.category_id=c.id"));
    while (query.next()) {
        if (!isDuplicated(FIRSTLINE(query.value(2).toString()), query.value(4).toDate())) {
            message(DEBUG, "Import", QString("importing note ID=%1").arg(query.value(0).toString()));
            QString cat = "imported-" + query.value(1).toString();
            int cid = desc2id("category", cat);
            if (cid <= 0) cid = insertCategory(cat, 0);
            QString tag = query.value(3).toString();
            if (tag.isEmpty()) tag = createTag();
            int newID = insert("notes", QStringList()<<query.value(2).toString()
                                <<INT2STR(cid)<<tag<<INT2STR(0));

            message(DEBUG, "import db","inserted note");

            if (newID > 0) {
                QString lastmodified = query.value(4).toString();
                update("notes", newID, "lastmodified", lastmodified);
                insert("tempids", QStringList()<<query.value(0).toString()
                                            <<INT2STR(newID)<<lastmodified, false);
                nImported++;
            }
        }
    }

    // attchments
    query.exec(QString("SELECT filename, note_id, lastmodified FROM attachment"));
    while (query.next()) {
        int newID = record("tempids", query.value(1).toInt(), "newid").toInt();
        if (newID > 0) {
            insert("attachment", QStringList()<<query.value(0).toString()<<INT2STR(newID));
            // copying attachments
            QString sfname = query.value(0).toString();
            QString tfname = INT2STR(newID) + "_" + sfname.mid(sfname.indexOf('_'));
            QString sfile = ipath + ATTACH_DIR + SP + sfname;
            QString tfile = bpath + ATTACH_DIR + SP + tfname;
            message(DEBUG, "Import", QString("Copying attachments from %1 to %2").arg(sfile).arg(tfile));
            copyFile(sfile, tfile);
        }
    }

    execQuery("drop table tempids");
    return nImported;
}
