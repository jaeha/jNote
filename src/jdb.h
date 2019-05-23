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
//#include "global.h"

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

#define DB_VERSION      500
#define DB_FILE         "data.jnote"

typedef QMap<int, QString> IdMap;
typedef QMapIterator<int, QString> IdMapIterator;

typedef struct Data {
    int id;
    QString note;
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
    void close();

    /// jnote specific
    bool createTable();
    IdMap getTitles() { return recordMap("notes", "note"); }
    IdMap findNotes(QString words) { return recordMap("notes", "note",\
                                           QString("note like '%%1%'").arg(words)); }
    void setNote(int id, QString text) { update("notes", id, "note", text); }
    int insertEmptyNote() { return insert("notes", QStringList()<<""<<"1"<<""); }
    QString getNote(int id) { return record("notes", id, "note").toString(); }
    bool removeNote(int id) { return remove("notes", id); }
    bool setAttach(int id, QString value) { return update("notes", id, "attached", value); }
    QString getAttach(int id) { return record("notes", id, "attached").toString(); }


private:
    bool updateLastModified(QString table, int id);

    QStringList m_tables;
};

#endif // JDB_H
