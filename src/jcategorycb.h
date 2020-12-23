#ifndef JCATEGORYCB_H
#define JCATEGORYCB_H

#include <QWidget>
#include <QComboBox>
#include "jdb.h"

class JCategoryCB : public QComboBox
{
    Q_OBJECT
public:
    explicit JCategoryCB(QWidget *parent = nullptr);
    void updateAllData(IdMap map);
    int currentID();
private:


signals:
    void toItemChanged(int);

public slots:
    void onCurrentIndexChanged(int index);
    void onFontResize(int);
};

#endif // JCATEGORYCB_H
