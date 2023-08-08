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
    void updateAllData(IdMap map, bool isCheckable=false);
    void addData(int id, QString str, bool isCheckable=false);
    void removeSelectedData();
    void updateCurrentData(QString str);
    int currentID();
    IdMap selectedData();

private:
    void checkItem(QListWidgetItem *item, bool isChecked);

signals:
    void toItemChanged(int);
    void toItemChecked(int, bool);
    void toItemClicked(int, QString);
    void toItemDoubleClicked(int, QString);

public slots:
    void onCurrentItemChanged(QListWidgetItem*, QListWidgetItem*);
    void onFontResize(int);
    void onItemClicked(QListWidgetItem*);
    void onItemDoubleClicked(QListWidgetItem*);
};
#endif // JLISTWIDGET_H
