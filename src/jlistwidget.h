#ifndef JLISTWIDGET_H
#define JLISTWIDGET_H

#include "global.h"
#include "jdb.h"
#include <QObject>
#include <QListWidgetItem>
#include <QListWidget>
#include <QList>

class JListWidget : public QListWidget
{
    Q_OBJECT

public:
    explicit JListWidget(QWidget *parent = nullptr);
    void updateAllData(IdMap map);
    void addData(int id, QString str);
    void removeSelectedData();
    void updateCurrentData(QString str);
    int currentID();
    IdMap selectedData();

signals:
    void toItemChanged(int);
    void toItemDoubleClicked(int, QString);

public slots:
    void onCurrentItemChanged(QListWidgetItem*, QListWidgetItem*);
    void onFontResize(int);
    void onItemDoubleClicked(QListWidgetItem*);
};
#endif // JLISTWIDGET_H
