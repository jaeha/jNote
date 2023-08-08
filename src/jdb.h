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

#define DB_VERSION      800

typedef struct Note {
    int id;
    QString note;
    int category_id;
    QString tag;
    QString lastmodified;
} NoteTable;

typedef QMap<int, QStringList> IdMap;
typedef QMapIterator<int, QStringList> IdMapIterator;

class JDB : public QObject
{
    Q_OBJECT
public:
    explicit JDB(QObject *parent = 0);
    virtual ~JDB();

    bool open(QString path="");
    bool execQuery(QString sql);
    int insert(QString table, QStringList list, bool isAuto=true);
    bool remove(QString table, int id);
    QStringList records(QString table, QString column);
    IdMap recordMap(QString table, QStringList columns, QString where="");
    QVariant record(QString table, int id, QString column);
    int desc2id(QString table, QString desc);
    bool update(QString table, int id, QString column, QVariant value);
    bool replace(QString table, QString column, QVariant oldValue, QVariant newValue);
    int counter(QString table, QString where="");
    void close();
    int dbversion() { return records("info", "db_version").first().toInt(); }
    QString desc(QString table, int id) { return record(table, id, "desc").toString(); }
    bool isDuplicated(QString title, QDate lastmodified);

    /// jnote specific
    bool createDB(QString path, bool withDefault=true);
    bool upgrade700(QString path);
    bool upgrade800(QString path);
    int import(QString ipath, QString bpath);
    QString createTag(){ return QUuid::createUuid().toString(); }

    //notes
    IdMap getNotes(int cid) { return recordMap("notes", QStringList()<<"note"<<"checked",
                            QString("category_id=%1").arg(cid)); }
    IdMap findNotes(QString words, int cid) { return recordMap("notes",
                            QStringList()<<"note"<<"checked", QString("note like '%%1%' AND category_id=%2").arg(words).arg(cid)); }
    void setNote(int nid, QString text) { update("notes", nid, "note", text); }
    QString getNote(int nid) { return record("notes", nid, "note").toString(); }
    bool removeNote(int nid) { return remove("notes", nid); }
    bool getNoteChecked(int nid) { return record("notes", nid, "checked").toBool(); }
    void setNoteChecked(int nid, bool isChecked) { update("notes", nid, "checked", isChecked?"1":"0"); }
    void setNoteCategory(int nid, int cid) { update("notes", nid, "category_id", QString("%1").arg(cid)); }
    QString getNoteTag(int nid) { return record("notes", nid, "tag").toString(); }
    void setNoteTag(int nid, QString tag) { update("notes", nid, "tag", tag); }
    int insertNewNote(int cid) { return insert("notes", QStringList()<<""<<INT2STR(cid)<<QUuid::createUuid().toString()<<"0"); }
    int counterNote(int cid) { return counter("notes", QString("category_id=%1").arg(cid)); }

    //category
    IdMap getCategories() { return recordMap("category", QStringList()<<"desc"); }
    int getCategoryType(int cid) { return record("category", cid, "type").toInt(); }
    void setCategoryType(int cid, int type) { update("category", cid, "type", INT2STR(type));  }
    QString getCategoryDesc(int cid) { return desc("category", cid); }
    void setCategoryDesc(int cid, QString desc) { update("category", cid, "desc", desc); }
    int insertCategory(QString category, int type) { return insert("category", QStringList()<<category<<INT2STR(type)); }
    bool removeCategory(int cid) { return remove("category", cid); }

    //attachment
    int insertAttachment(QString filename, int nid) { return insert("attachment", QStringList()<<filename<<INT2STR(nid));}
    IdMap getAllAttachment(int nid) { return recordMap("attachment", QStringList()<<"filename", QString("note_id=%1").arg(nid));}
    bool removeAttachment(int id) { return remove("attachment", id); }
    bool replaceAttachmentNote(int oldId, int newId) { return replace("attachment", "node_id", oldId, newId); }

private:
    bool updateLastModified(QString table, int id);

    QStringList m_tables;
};

#endif // JDB_H
