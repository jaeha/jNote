#include <QVBoxLayout>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QDesktopServices>
#include <QPoint>
#include <QMenu>
#include <QFileDialog>
#include "attachdlg.h"
#include "jdb.h"

AttachDlg::AttachDlg(int nid, QString attachPath, QWidget *parent) : QDialog(parent)
{
    m_attachpath = attachPath;
    m_currentId = nid;

  //  setWindowFlags(Qt::Window | Qt::FramelessWindowHint);

    gui();
}

void AttachDlg::gui()
{
    JDB db;
    m_attachlist = new JListWidget();

    IdMapIterator i(db.getAllAttachment(m_currentId));
    while (i.hasNext()) {
        i.next();
        qDebug() <<"key: " <<i.key() <<", value: "<<i.value();
        m_attachlist->addData(i.key(), i.value().at(0));
    }

    QDialogButtonBox *okButton = new QDialogButtonBox(QDialogButtonBox::Ok);
    QPushButton *delButton = new QPushButton("Delete");
    QPushButton *saveButton = new QPushButton("Save");

    QHBoxLayout *hbox = new QHBoxLayout();
    hbox->addWidget(delButton);
    hbox->addWidget(saveButton);
    hbox->addStretch();
    hbox->addWidget(okButton);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(m_attachlist);
    layout->addLayout(hbox);

    setLayout(layout);

    connect(okButton, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(delButton, SIGNAL(clicked()), this, SLOT(onDelete()));
    connect(saveButton, SIGNAL(clicked()), this, SLOT(onSave()));
   // connect(m_attachlist, SIGNAL(toItemClicked(int, QString)), this, SLOT(onAttachOpen(int, QString)));
   // connect(m_attachlist, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onAttachContextMenu(QPoint)));
}

/*
void AttachDlg::onAttachContextMenu(const QPoint &pos)
{
    message(DEBUG, "onAttachContextMenu()", "add contect menu for attachment");

    // Handle global position
    QPoint globalPos = m_attachlist->mapToGlobal(pos);

    // Create menu and insert some actions
    QMenu *menu = new QMenu();
    menu->addSection("Attachment");
    menu->addAction("Delete", this, SLOT(onAttachDelete()));

    // Show context menu at handling position
    menu->exec(globalPos);
}
*/
void AttachDlg::onDelete()
{
    message(DEBUG,"JNote", "onAttachDelete()");

    IdMap map = m_attachlist->selectedData();

    int button = QMessageBox::question(this, tr("Question"),
                         QString("Do you really want to delete the selected file?").arg(map.count()),
                         QMessageBox::Yes|QMessageBox::No, QMessageBox::No);
    if (button == QMessageBox::No)
        return;

    deleteAttachments(map);
}

void AttachDlg::onSave()
{
    message(DEBUG, "onSave()", "start");
    IdMap map = m_attachlist->selectedData();

    if (map.count() <=0 ) {
        message(ERROR, "Attachment Dialog", "No files were selected!");
        return;
    }

    QString targetdir;
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::DirectoryOnly);
   //  dialog.setOption(QFileDialog::DontUseNativeDialog, true);
    dialog.setOption(QFileDialog::ShowDirsOnly, false);
   if (dialog.exec()) {
       targetdir = dialog.selectedFiles().first();
   }
   if (targetdir.isEmpty()) {
       message(ERROR, "Attachment Dialog", "Failed to get a target directory!");
       return;
   }

    IdMapIterator i(map);
    while (i.hasNext()) {
        i.next();
        QString filename = i.value().at(0);
        QFile f(m_attachpath + filename);
        if (f.copy(targetdir + "/" + filename)) {
            qDebug() << "saved to " << targetdir + "/" + filename;
        }
        else {
            message(ERROR, "Save attachment", "Failed to save Attachment file, " + f.fileName());
            return;
        }
    }
    message(INFO, "Save Attachments", "Successfully saved to " + targetdir);
}

void AttachDlg::deleteAttachments(IdMap map) {
    JDB db;
    IdMapIterator i(map);
    while (i.hasNext()) {
        i.next();
        QFile f(m_attachpath + i.value().at(0));
        if (f.remove()) {
            qDebug() << "removed " << f.fileName();
            db.removeAttachment(i.key());
        }
        else {
            message(ERROR, "Remove attachment", "Failed to remove Attachment file, " + f.fileName());
            return;
        }
    }
    m_attachlist->removeSelectedData();
}

