#include "jsettingsdlg.h"
#include "global.h"
#include <QLineEdit>
#include <QFileDialog>
#include <QGridLayout>
#include <QPushButton>
#include <QFileIconProvider>
#include <QDialogButtonBox>

enum SETTINGS {S_BASE};
JSettingsDlg::JSettingsDlg(QWidget *parent, QString basepath) : QDialog(parent)
{
    m_lbBasePath = new QLabel();
    m_lbImportDataPath = new QLabel();

    m_lbBasePath->setText(basepath);

    gui();
}

void JSettingsDlg::gui()
{
    // setModal(true);
   //  setWindowFlags(Qt::FramelessWindowHint);

    QPushButton *importDataButton = new QPushButton("Import!");
    QPushButton *baseFolderIcon = new QPushButton();
    QPushButton *importFolderIcon = new QPushButton();
    QDialogButtonBox *okButton = new QDialogButtonBox(QDialogButtonBox::Ok);

    baseFolderIcon->setIcon(QIcon(":folder"));
    importFolderIcon->setIcon(QIcon(":folder"));
    baseFolderIcon->setFocusPolicy(Qt::NoFocus);
    importFolderIcon->setFocusPolicy(Qt::NoFocus);
    importDataButton->setFocusPolicy(Qt::NoFocus);

    QGridLayout *grid = new QGridLayout();
    grid->addWidget(new QLabel("Base Directory:"), 0, 0);
    grid->addWidget(baseFolderIcon, 0, 1);
    grid->addWidget(m_lbBasePath, 0, 2);

    grid->addWidget(new QLabel("Import Data File:"), 1, 0);
    grid->addWidget(importFolderIcon, 1, 1);
    grid->addWidget(m_lbImportDataPath, 1, 2);
    grid->addWidget(importDataButton,1,3);

    grid->addWidget(okButton, 2, 3);

    setLayout(grid);

    layout()->setSizeConstraint( QLayout::SetFixedSize );

    connect(baseFolderIcon, SIGNAL(clicked()), this, SLOT(onBaseFolderDlg()));
    connect(importFolderIcon, SIGNAL(clicked()), this, SLOT(onImportFolderDlg()));
    connect(importDataButton, SIGNAL(clicked()), this, SLOT(onImportButton()));
    connect(okButton, &QDialogButtonBox::accepted, this, &QDialog::accept);

}

void JSettingsDlg::onImportButton()
{
    emit toImportData(m_lbImportDataPath->text());
}

void JSettingsDlg::onBaseFolderDlg()
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

void JSettingsDlg::onImportFolderDlg()
{
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::DirectoryOnly);
   //  dialog.setOption(QFileDialog::DontUseNativeDialog, true);
    dialog.setOption(QFileDialog::ShowDirsOnly, false);
   if (dialog.exec())
       m_lbImportDataPath->setText(dialog.selectedFiles().first());
}
