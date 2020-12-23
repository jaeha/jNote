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
    QList<int> selectedData();

signals:
    void toItemChanged(int);

public slots:
    void onCurrentItemChanged(QListWidgetItem* oldItem, QListWidgetItem* newItem);
    void onFontResize(int);
};

#endif // JLISTWIDGET_H
