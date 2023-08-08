#ifndef CATEGORYCB_H
#define CATEGORYCB_H

#include <QWidget>
#include <QComboBox>
#include "jdb.h"

class CategoryCB : public QComboBox
{
    Q_OBJECT
public:
    explicit CategoryCB(QWidget *parent = nullptr);
    void updateAllData(IdMap map);
    int currentID();
    ~CategoryCB();

signals:
    void toItemChanged(int);

public slots:
    void onCurrentIndexChanged(int index);
    void onFontResize(int);
};

#endif // CATEGORYCB_H
