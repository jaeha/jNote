#ifndef GLOBAL_H
#define GLOBAL_H

#include <QStandardPaths>
#include <QDebug>
#include <QMessageBox>
#include <QFile>
#include <QDir>
#include <QColor>
#include <QSettings>

#define PROGRAMMER      "Jaeha Lee"
#define COMPANY         "JTN Co."
#define COPYWRITE       "Copyright 2018 JTN Co. Ltd.. All rights reserved."
#define APP_TITLE       "JNote"
#define APP_VERSION     "5.0"
#define BUILT_DATE      "Thursday, 26th August 2021"
//#define SP     QDir::separator()
#define SP          "/"
#define BASE_PATH   QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)\
                    + SP + QString(APP_TITLE) + SP
#define ATTACH_DIR  "attach"
#define DB_FILE     "jnote.db"

#define MAIN_DIALOG_SIZE    QSize(500,800)
#define ICON_SIZE   QSize(24, 24)
#define DATE_LONG       "yyyy-MM-dd hh:mm:ss"
#define DATE_SHORT      "yyyy-MM-dd"
#define DATE_YYMM       "yyyy-MM"
#define DATE_LONG2      "yyyy-MM-dd hhmmss"
#define DATE_MONTHYEAR  "MMMM yyyy"


#define FONT_DEFAULT_SIZE       12
#define FONT_SIZE_MAX           100
#define FONT_SIZE_CHANGE        2
#define FONT_STYLE_LIST(fsize)  QString("font: normal %1px").arg(fsize)
#define FONT_STYLE_NOTE(fsize)  QString("font: normal %1px").arg(fsize)

#define NO_DATA     -1
#define ITEM_ID    1001

#define INT2STR(i)      QString::number(i)
#define FIRSTLINE(s)    s.split("\n").at(0)

enum CategoryType { NOTE_TYPE, JORUNAL_TYPE, TASK_TYPE };

enum LogLevel { ERROR, INFO, DEBUG };
inline void message(LogLevel level, const QString &title, const QString &msg="")
{
    QString str;
    switch (level) {
    case ERROR:
        QMessageBox::critical(0, title, msg, QMessageBox::Close);
        str = QString("ERROR: %1 %2").arg(title).arg(msg);
        break;
    case INFO:
        QMessageBox::warning(0, title, msg);
        str = QString("INFO: %1 %2").arg(title).arg(msg);
        break;
    case DEBUG:
        str = QString("DEBUG: %1 %2").arg(title).arg(msg);
        break;
    }
    qDebug() << str;
}

inline bool question(const QString &title, const QString &question)
{
    int button = QMessageBox::question(0, title, question,
                         QMessageBox::Yes|QMessageBox::No, QMessageBox::No);

    if (button == QMessageBox::No)
        return false;
    else
        return true;
}

inline QString readFile(QString path)
{
    QString data;
    QFile file(path);
    file.open(QFile::ReadOnly | QFile::Text);
    data = file.readAll();
    file.close();
    return data;
}

inline void writeFile(QString path, const char* data)
{
    QFile file(path);
    file.open(QFile::WriteOnly | QFile::Text);
    file.write(data);
    file.close();
}

inline bool removeFile(QString path)
{
    if (!QFile(path).exists()) {
        message(DEBUG, "removeFile()", path + " is not exist.");
        return false;
    }
    return QFile(path).remove();
}

inline bool copyFile(QString sPath, QString tPath)
{
    if (!QFile(sPath).exists()) {
        message(DEBUG, "global", "copyFile(): file is not exist!");
        return false;
    }

    QFileInfo sf(sPath);

    if (sf.isFile()) { // copy a single file
        if (!QFile(tPath).exists())
            QFile::copy(sPath, tPath);
        else
            message(DEBUG, "global", "copyFile(): file is already exist, " + tPath);
    } else if (sf.isDir()) {  // copy all the files on the directory
        QDir dir(sPath);
        foreach (QString f, dir.entryList(QDir::Files)) {
            QString tFile = tPath + SP +f;
           // qDebug() << tFile;
            if (!QFile(tFile).exists())
                QFile::copy(sPath + SP + f, tFile);
        }
    } else {
        message(ERROR, "global", "copyFile() was failed from " + sPath + " to " + tPath);
        return false;
    }

    message(DEBUG, "global", "copyFile() completed successfully!");

    return true;
}

inline void writeSettings(QString key, QVariant value)
{
    QSettings s(COMPANY, APP_TITLE);
    s.setValue(key, value);
}

inline QVariant readSettings(QString key)
{
    QSettings s(COMPANY, APP_TITLE);
    return s.value(key);
}


#endif // GLOBAL_H
