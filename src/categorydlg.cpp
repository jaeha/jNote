#include <QLabel>
#include <QGridLayout>
#include <QDialogButtonBox>
#include <QPushButton>
#include "categorydlg.h"
#include "jdb.h"


CategoryDlg::CategoryDlg(int cid, QWidget *parent) : QDialog(parent)
{
    JDB db;
    m_cid = cid;
    m_name = new QLineEdit();
    m_type = new QComboBox();
    m_type->addItems(QStringList()<<"Note"<<"Journal"<<"Task");

    if (m_cid >= 0) {
        m_name->setText(db.getCategoryDesc(m_cid));
        m_type->setCurrentIndex(db.getCategoryType(m_cid));
    }

    QPushButton *saveButton = new QPushButton("Save");
    QDialogButtonBox *cancelButton = new QDialogButtonBox(QDialogButtonBox::Cancel);

    QGridLayout *grid = new QGridLayout();
    grid->addWidget(new QLabel("Category Name: "), 0, 0);
    grid->addWidget(m_name, 0, 1);
    grid->addWidget(new QLabel("Category Type: "), 1, 0);
    grid->addWidget(m_type, 1, 1);
    grid->addWidget(saveButton, 2,0);
    grid->addWidget(cancelButton, 2, 1);

    connect(saveButton, SIGNAL(clicked()), this, SLOT(onSave()));
    connect(cancelButton, &QDialogButtonBox::rejected, this, &QDialog::reject);

    setLayout(grid);
}

void CategoryDlg::onSave()
{
    JDB db;
    QString newName = m_name->text();
    int newIndex = m_type->currentIndex();

    if (m_cid >= 0) {
        db.setCategoryDesc(m_cid, newName);
        db.setCategoryType(m_cid, newIndex);
    } else {
        db.insertCategory(m_name->text(), m_type->currentIndex());
    }

    accept();
  //  message(INFO, "Save", "Category was saved.");
}
