#ifndef JTEXTEDIT_H
#define JTEXTEDIT_H

#include <QTextEdit>
#include "global.h"

class JTextEdit : public QTextEdit
{
    Q_OBJECT

public:
    explicit JTextEdit(QString filename="", QString data="", QWidget *parent = nullptr);
    void updateData(QString data, QString findStr="");
    void changeFontSize(int fontsize);

protected:
    void focusOutEvent(QFocusEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void mouseReleaseEvent(QMouseEvent *e);
    void mousePressEvent(QMouseEvent *e);


private:
    QColor jcolours[10] = {QColor("cyan"), QColor("magenta"), QColor("red"),
                          QColor("darkRed"), QColor("darkCyan"), QColor("darkMagenta"),
                          QColor("green"), QColor("darkGreen"), QColor("yellow"),
                          QColor("blue")};
    QStringList textFiles = QStringList() << "txt" << "out" << "log" << "sh" << "pl" << "py" << "c" << "h";
    void findString(QString str, QColor color);
    QString m_filename;

signals:
    void toChangedData(QString, QString);
    void toDropFilePath(QString);
    void toDropFileList(QList<QUrl>);

public slots:
    void onDataChanged();
    void onFontResize(int);
    void onSearchOpen();
    void onSearchInText(QString);
    void onFindText(QString);

};

#endif // JTEXTEDIT_H
