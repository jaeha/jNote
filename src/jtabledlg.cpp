#include "jtabledlg.h"
#include "jlistwidget.h"
#include "jdb.h"
#include <QListWidget>
#include <QVBoxLayout>

JTableDlg::JTableDlg(QString table, QWidget *parent) : QDialog(parent)
{
    setModal(true);
    setGeometry(300, 300, 300, 200);

    JDB db;
    JListWidget *list = new JListWidget();
    list->updateAllData(db.getCategories());

    QVBoxLayout *vbox = new QVBoxLayout();
    vbox->addWidget(list);
    setLayout(vbox);
}
