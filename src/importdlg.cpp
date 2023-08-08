#include <QGridLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QFileDialog>
#include <QLabel>
#include <QTextEdit>
#include <QDialogButtonBox>
#include "global.h"
#include "importdlg.h"
#include "jdb.h"

ImportDlg::ImportDlg(QString basepath, QWidget *parent) : QDialog(parent)
{
    m_basepath = basepath;
    m_edImportDir = new QLineEdit();
    m_lvOut = new QListWidget();
    m_edImportDir->setReadOnly(true);
    QPushButton *bnImport = new QPushButton("Import");
    QPushButton *bnFolder = new QPushButton();
    QDialogButtonBox *okButton = new QDialogButtonBox(QDialogButtonBox::Ok);

    bnFolder->setIcon(QIcon(":folder"));
    bnFolder->setFocusPolicy(Qt::NoFocus);
    //bnImport->setFocusPolicy(Qt::NoFocus);

    QGridLayout *grid = new QGridLayout();
    grid->addWidget(new QLabel("Importing directory:"), 0, 0);
    grid->addWidget(m_edImportDir, 0, 1, 1, 2);
    grid->addWidget(bnFolder, 0, 3);
    grid->addWidget(bnImport, 1, 0);
    grid->addWidget(new QLabel("Output:"), 2, 0);
    grid->addWidget(m_lvOut, 3, 0, 1, 4);
    grid->addWidget(okButton, 4, 3);

   QVBoxLayout *mainLayout = new QVBoxLayout();
   mainLayout->addLayout(grid);

   setLayout(mainLayout);

   connect(bnFolder, SIGNAL(clicked()), this, SLOT(onFolderButton()));
   connect(bnImport, SIGNAL(clicked()), this, SLOT(onImportButton()));
   connect(okButton, &QDialogButtonBox::accepted, this, &QDialog::accept);
}

void ImportDlg::onImportButton()
{
    m_lvOut->clear();

    QString importPath = m_edImportDir->text() + SP;
    QString importDB =  importPath + DB_FILE;
    QString dbfile = m_basepath + DB_FILE;

    if (!QFile(importDB).exists()) {
        m_lvOut->addItem(QString("ERROR: %1 is not found!").arg(importDB));
        return;
    }

    this->setCursor(Qt::WaitCursor);

    if (copyFile(dbfile, dbfile + ".before_import"))
        m_lvOut->addItem(QString("Created a backup file: %1.before_import").arg(dbfile));

    JDB db;
    m_lvOut->addItem(QString("Importing DB, %1").arg(dbfile));
    int countInserted = db.import(importDB);
    if (countInserted > 0)
        m_lvOut->addItem(QString("%1 records were imported.").arg(countInserted));
    int countDuplicated = db.removeDuplicated("notes", "note");
    if (countDuplicated > 0)
        m_lvOut->addItem(QString("%1 recored were duplicated and removed.").arg(countDuplicated));
    m_lvOut->addItem(QString("Copying attachments from %1 to %2")
                     .arg(importPath + ATTACH_DIR).arg(m_basepath + ATTACH_DIR));
    copyFile(importPath + ATTACH_DIR, m_basepath + ATTACH_DIR);
    m_lvOut->addItem(QString("Import was completed! Please check the imported category."));

    this->setCursor(Qt::ArrowCursor);
}

void ImportDlg::onFolderButton()
{
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::DirectoryOnly);
   //  dialog.setOption(QFileDialog::DontUseNativeDialog, true);
    dialog.setOption(QFileDialog::ShowDirsOnly, false);
   if (dialog.exec()) {
       m_edImportDir->setText(dialog.selectedFiles().first());
   }
}
