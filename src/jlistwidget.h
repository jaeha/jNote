#ifndef JLISTWIDGET_H
#define JLISTWIDGET_H

#include "global.h"
#include "jdb.h"
#include <QObject>
#include <QListWidgetItem>
#include <QListWidget>

class JListWidget : public QListWidget
{
    Q_OBJECT

public:
    explicit JListWidget(QWidget *parent = nullptr);
    void updateAllData(IdMap map);
    void addData(int id, QString str);
    void removeCurrentData();
    void updateCurrentData(QString str);
    int currentID();

signals:
    void toItemChanged(int);

public slots:
    void onCurrentItemChanged(QListWidgetItem* oldItem, QListWidgetItem* newItem);
    void onFontResize(int);
};

#endif // JLISTWIDGET_H
