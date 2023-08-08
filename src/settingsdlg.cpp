#include <QLineEdit>
#include <QFileDialog>
#include <QGridLayout>
#include <QPushButton>
#include <QFileIconProvider>
#include <QDialogButtonBox>
#include <QGroupBox>
#include <QToolButton>
#include "settingsdlg.h"
#include "global.h"
#include "jdb.h"

SettingsDlg::SettingsDlg(QString basepath, QWidget *parent) : QDialog(parent)
{
    m_lbBasePath = new QLabel();
    m_lbBasePath->setText(basepath);

    m_lbImportPath = new QLabel();

    gui();
}

void SettingsDlg::gui()
{
    setWindowTitle("JNote Settings");
    // setModal(true);
   //  setWindowFlags(Qt::FramelessWindowHint);

    QDialogButtonBox *okButton = new QDialogButtonBox(QDialogButtonBox::Ok);

    QToolButton *baseFolderIcon = new QToolButton();
    baseFolderIcon->setIcon(QIcon(":folder"));
    baseFolderIcon->setFocusPolicy(Qt::NoFocus);

    QToolButton *importFolderIcon = new QToolButton();
    importFolderIcon->setIcon(QIcon(":folder"));
    importFolderIcon->setFocusPolicy(Qt::NoFocus);

    QGroupBox *versionGroup = new QGroupBox(tr("Version"));
    QGridLayout *grid = new QGridLayout();
    grid->addWidget(new QLabel(QString("Application: %1").arg(APP_VERSION)), 0, 0);
    grid->addWidget(new QLabel(QString("Database: %1").arg(DB_VERSION)), 0, 1);
    versionGroup->setLayout(grid);

    QGroupBox *baseGroup = new QGroupBox(tr("Base Directory"));
    QPushButton *baseButton = new QPushButton("Change");
    QHBoxLayout *hbox = new QHBoxLayout();
    hbox->addWidget(baseFolderIcon);
    hbox->addWidget(m_lbBasePath);
    hbox->addWidget(baseButton);
    baseGroup->setLayout(hbox);

    QGroupBox *importGroup = new QGroupBox(tr("Import Other Notes"));
    QPushButton *importButton = new QPushButton("Import");
    QHBoxLayout *hbox2 = new QHBoxLayout();
    hbox2->addWidget(importFolderIcon);
    hbox2->addWidget(m_lbImportPath);
    hbox2->addWidget(importButton);
    importGroup->setLayout(hbox2);

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addWidget(versionGroup);
    mainLayout->addWidget(baseGroup);
    mainLayout->addWidget(importGroup);
    mainLayout->addWidget(okButton);

    setLayout(mainLayout);

    layout()->setSizeConstraint( QLayout::SetFixedSize );

    connect(baseFolderIcon, SIGNAL(clicked()), this, SLOT(onBaseFolderDlg()));
    connect(baseButton, SIGNAL(clicked()), this, SLOT(onBaseChange()));
    connect(importFolderIcon, SIGNAL(clicked()), this, SLOT(onImportFolderDlg()));
    connect(importButton, SIGNAL(clicked()), this, SLOT(onImport()));
    connect(okButton, &QDialogButtonBox::accepted, this, &QDialog::accept);

}

void SettingsDlg::onBaseChange()
{
    emit toChangeBasepath(m_lbBasePath->text());
}

void SettingsDlg::onImport()
{
    if (m_lbImportPath->text().isEmpty())
        return;

    this->setCursor(Qt::WaitCursor);

    QString importPath =  m_lbImportPath->text();
    QString basePath = m_lbBasePath->text();
    QString dbfile = basePath + DB_FILE;

    // backup
    if (!copyFile(dbfile, dbfile + ".before_import")) {
        message(ERROR, "Failed to create a backup file.");
        return;
    }

    JDB db;
    int nImported = db.import(importPath, basePath);
    if (nImported > 0) {
        message(INFO, "Import", QString("%1 records were imported.").arg(nImported));
    }
    else
        message(INFO, "Import", "No records were imported");

    this->setCursor(Qt::ArrowCursor);

  //  connect(dlg, SIGNAL(finished(int)), this, SLOT(onLoadData(int)));
}

void SettingsDlg::onBaseFolderDlg()
{
    folderDlg("Base Direcory", m_lbBasePath);
}

void SettingsDlg::onImportFolderDlg()
{
    folderDlg("Import Direcory", m_lbImportPath);
}

void SettingsDlg::folderDlg(QString title, QLabel *label)
{
    QFileDialog dialog;
    QString dir;
    dialog.setFileMode(QFileDialog::Directory);
   //  dialog.setOption(QFileDialog::DontUseNativeDialog, true);
    dialog.setOption(QFileDialog::ShowDirsOnly, false);
   if (dialog.exec())
       dir = dialog.selectedFiles().first();

   if (!dir.isEmpty()) {
       QString dbfile = dir + SP + DB_FILE;
       if (QFile().exists(dbfile))
           label->setText(dir + SP);
       else
           message(ERROR, title, QString("Fail to find the %1 !").arg(dbfile));
   }
}
