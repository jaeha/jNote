#include <QLineEdit>
#include <QFileDialog>
#include <QGridLayout>
#include <QPushButton>
#include <QFileIconProvider>
#include <QDialogButtonBox>
#include <QGroupBox>
#include "settingsdlg.h"
#include "global.h"
#include "jdb.h"


SettingsDlg::SettingsDlg(QWidget *parent, QString basepath) : QDialog(parent)
{
    m_lbBasePath = new QLabel();
    m_lbBasePath->setText(basepath);

    JDB db;
    m_lvCategory = new JListWidget();
    m_lvCategory->setSelectionMode(QAbstractItemView::SingleSelection);
    m_lvCategory->setEditTriggers(QAbstractItemView::DoubleClicked);
    m_lvCategory->updateAllData(db.getCategories());

    gui();
}

void SettingsDlg::gui()
{
    setWindowTitle("JNote Settings");
    // setModal(true);
   //  setWindowFlags(Qt::FramelessWindowHint);
    QPushButton *btCategoryAdd = new QPushButton("Add");
    QPushButton *btCategoryRemove = new QPushButton("Remove");
 //   m_lvCategory.

    QPushButton *baseFolderIcon = new QPushButton();
    QDialogButtonBox *okButton = new QDialogButtonBox(QDialogButtonBox::Ok);

    baseFolderIcon->setIcon(QIcon(":folder"));
    baseFolderIcon->setFocusPolicy(Qt::NoFocus);

    QGroupBox *versionGroup = new QGroupBox(tr("Version"));
    QGridLayout *grid1 = new QGridLayout();
    grid1->addWidget(new QLabel("Application: 4.0"), 0, 0);
    grid1->addWidget(new QLabel("Database: 700"), 0, 1);
    versionGroup->setLayout(grid1);

    QGroupBox *generalGroup = new QGroupBox(tr("General"));
    QGridLayout *grid2 = new QGridLayout();
    grid2->addWidget(new QLabel("Base Directory:"), 0, 0);
    grid2->addWidget(baseFolderIcon, 0, 1);
    grid2->addWidget(m_lbBasePath, 0, 2);
    generalGroup->setLayout(grid2);

    QGroupBox *categoryGroup = new QGroupBox(tr("Category"));
    QVBoxLayout *vbox = new QVBoxLayout();
    vbox->addWidget(btCategoryAdd);
    vbox->addWidget(btCategoryRemove);
    vbox->addStretch();
    QHBoxLayout *hbox = new QHBoxLayout();
    hbox->addWidget(m_lvCategory);
    hbox->addLayout(vbox);
    categoryGroup->setLayout(hbox);

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addWidget(versionGroup);
    mainLayout->addWidget(generalGroup);
    mainLayout->addWidget(categoryGroup);;
    mainLayout->addWidget(okButton);

    setLayout(mainLayout);

    layout()->setSizeConstraint( QLayout::SetFixedSize );

    connect(baseFolderIcon, SIGNAL(clicked()), this, SLOT(onBaseFolderDlg()));
    connect(btCategoryAdd, SIGNAL(clicked()), this, SLOT(onCategoryAdd()));
    connect(btCategoryRemove, SIGNAL(clicked()), this, SLOT(onCategoryRemove()));
    connect(m_lvCategory, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(onCategoryChanged(QListWidgetItem*)));
    connect(okButton, &QDialogButtonBox::accepted, this, &QDialog::accept);
}

void SettingsDlg::onBaseFolderDlg()
{
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::DirectoryOnly);
   //  dialog.setOption(QFileDialog::DontUseNativeDialog, true);
    dialog.setOption(QFileDialog::ShowDirsOnly, false);
   if (dialog.exec()) {
       QString basedir = dialog.selectedFiles().first();
       m_lbBasePath->setText(basedir);
       emit toChangeBasepath(basedir);
   }
}

void SettingsDlg::onCategoryAdd()
{
    JDB db;
    int newId = db.insertCategory("New..");
    m_lvCategory->addData(newId, "New..");
}

void SettingsDlg::onCategoryRemove()
{
    JDB db;
    IdMap map = m_lvCategory->selectedData();
    int id = map.keys().first();
    if (!db.getNotes(id).isEmpty()) {
        message(ERROR, "The selected category is not empty, so it cannot be removed!");
        return;
    }

    if (db.removeCategory(id))
        m_lvCategory->removeSelectedData();
}

void SettingsDlg::onCategoryChanged(QListWidgetItem* item)
{
    qDebug() << "SettingsDlg::onCategoryChanged()" << item->text();
    JDB db;
    int id = m_lvCategory->currentID();
    db.setCategory(id, item->text());
}
