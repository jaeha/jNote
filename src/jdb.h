/*
 * JDB.h
 *
 *  Created on: Mar 09, 2016
 *      Author: jaeha
 */

#ifndef JDB_H_
#define JDB_H_

/* desktop */
#include <QtCore>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QComboBox>
#include "global.h"

/* BlackBerry
#include <qobject.h>
#include <QtCore>
#include <QtSql/QtSql>
#include <bb/data/SqlConnection>
#include <bb/cascades/AbstractPane>
#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/OrientationSupport>
#include <bb/system/SystemDialog>

using namespace bb::cascades;
*/

#define DB_VERSION      600

typedef QMap<int, QString> IdMap;
typedef QMapIterator<int, QString> IdMapIterator;

typedef struct Data {
    int id;
    QString note;
    QString category;
    QString tags;
    QString attached;
    QString lastmodified;
} Datas;

class JDB : public QObject
{
    Q_OBJECT
public:
    explicit JDB(QObject *parent = 0);
    virtual ~JDB();

    bool open(QString path="");
    bool execQuery(QString sql);
    int insert(QString table, QStringList list);
    bool remove(QString table, int id);
    QStringList records(QString table, QString column);
    IdMap recordMap(QString table, QString column, QString where="");
    QVariant record(QString table, int id, QString column);
    int desc2id(QString table, QString desc);
    bool update(QString table, int id, QString column, QString value);
    int counter(QString table, QString where="");
    void close();
    int dbversion() { return records("info", "db_version").first().toInt(); }

    /// jnote specific
    bool createDB(QString path);
    bool upgrade(QString path);
    bool import(QString path, QString category="");

    IdMap getNotes(int cid) { return recordMap("notes", "note", QString("category_id=%1").arg(cid)); }
    IdMap findNotes(QString words, int cid) { return recordMap("notes", "note", QString("note like '%%1%' AND category_id=%2").arg(words).arg(cid)); }
    void setNote(int nid, QString text) { update("notes", nid, "note", text); }
    void setNoteCategory(int nid, int cid) { update("notes", nid, "category_id", QString("%1").arg(cid)); }
    int insertEmptyNote() { return insert("notes", QStringList()<<""<<"1"<<""<<""); }
    QString getNote(int nid) { return record("notes", nid, "note").toString(); }
    bool removeNote(int nid) { return remove("notes", nid); }
    bool setAttach(int nid, QString value) { return update("notes", nid, "attached", value); }
    QString getAttach(int nid) { return record("notes", nid, "attached").toString(); }
    int counterNote(int cid) { return counter("notes", QString("category_id=%1").arg(cid)); }

    IdMap getCategories() { return recordMap("category", "desc"); }
    void setCategory(int cid, QString category) { update("category", cid, "desc", category); }
    int insertNewCategory(QString category) { return insert("category", QStringList()<<category); }
    bool removeCategory(int cid) { return remove("category", cid); }

private:
    bool updateLastModified(QString table, int id);

    QStringList m_tables;
};

#endif // JDB_H
