#include "jsearchdlg.h"
#include <QHBoxLayout>
#include <QLineEdit>
#include <QLabel>

JSearchDlg::JSearchDlg(QWidget *parent) : QDialog(parent)
{
   // setModal(true);
  //  setWindowFlags(Qt::FramelessWindowHint);

    QLineEdit *searchEd = new QLineEdit();
    QHBoxLayout *hbox = new QHBoxLayout();
    hbox->addWidget(new QLabel("Search:"));
    hbox->addWidget(searchEd);

    setLayout(hbox);

    resize(500, 50);

    connect(searchEd, SIGNAL(textChanged(QString)), this, SLOT(onSearch(QString)));
}

void JSearchDlg::onSearch(QString word)
{
    emit toSearch(word);
}
