#ifndef GLOBAL_H
#define GLOBAL_H

#include <QStandardPaths>
#include <QDebug>
#include <QMessageBox>
#include <QFile>
#include <QColor>

#define PROGRAMMER      "Jaeha Lee"
#define COMPANY         "JTN Co."
#define COPYWRITE       "Copyright 2018 JTN Co. Ltd.. All rights reserved."

#define APP_TITLE       "JNote"
#define APP_VERSION     "3.1.1"
#define BUILT_DATE      "Tuesday, 2nd Apr 2019"

#define BASE_PATH   QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)\
                    .append("/").append(APP_TITLE)
//#define ATTACH_PATH BASE_PATH.append("attach/")
#define ATTACH_PATH "attach/"

#define MAIN_DIALOG_SIZE    QSize(500,800)
#define ICON_SIZE   QSize(24, 24)
#define DATE_LONG       "yyyy-MM-dd hh:mm:ss"
#define DATE_SHORT      "yyyy-MM-dd"
#define DATE_YYMM       "yyyy-MM"
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


enum LogLevel {ERROR, INFO, DEBUG};
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

inline void removeFile(QString path)
{
    if (!QFile(path).exists()) {
        message(DEBUG, "removeFile()", path + " is not exist.");
        return;
    }
    QFile(path).remove();
}

inline void copyFile(QString source, QString target)
{
    if (!QFile(source).exists()) {
        message(DEBUG, "copyFile()", source + " is not exist.");
        return;
    }
    QFile(source).copy(target);
}


#endif // GLOBAL_H
