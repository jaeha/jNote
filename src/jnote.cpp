#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QListWidget>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QFileDialog>
#include <QMenu>
#include <QLabel>
#include <QScrollArea>
#include <QDesktopServices>
#include <QUrl>
#include <QInputDialog>
#include "jnote.h"
#include "jdb.h"
#include "settingsdlg.h"
#include "attachdlg.h"
#include "categorydlg.h"

JNote::JNote(QWidget *parent) : QWidget(parent)
{
    m_findwords="";

    // read settings
    m_basepath = readSettings("basepath").toString();
    if (m_basepath.isEmpty())
        m_basepath = BASE_PATH;
    m_attachpath = m_basepath + ATTACH_DIR + SP;

    // create dirs if they are not exist
    QDir().mkdir(m_basepath);
    QDir().mkdir(m_attachpath);

    qDebug() << "JNote() base path: " << m_basepath << ", attach pahe: " << m_attachpath;

    gui();

    // read settings
    restoreGeometry(readSettings("geometry").toByteArray());
    m_splitter->restoreState(readSettings("splitter").toByteArray());
    int fontsize = readSettings("fontsize").toInt();
    m_fontsize = (fontsize < FONT_DEFAULT_SIZE) ? FONT_DEFAULT_SIZE : fontsize;

    emit toFontResize(m_fontsize);

}

bool JNote::openDB()
{
    JDB db;
    QString dbfile = m_basepath + DB_FILE;

    if (QFile(dbfile).exists()) {
        // remove existing backup file and copy
        QString backupFile = dbfile + ".bak";
        removeFile(backupFile);
        copyFile(dbfile, backupFile);
        if (db.open(dbfile)) {
            int dbversion = db.dbversion();
            message(DEBUG, "JDB", QString("open() DB version: %1").arg(dbversion));
            if (dbversion < DB_VERSION && question("Upgarde", QString("Found DB version is %1 on %2. Do you want to upgrade?")
                                                   .arg(dbversion).arg(m_basepath))) {
                db.close();
                int ret = 0;
                switch (dbversion) {
                    case 600:
                        ret = db.upgrade700(dbfile);
                    case 700:
                        ret = db.upgrade800(dbfile);
                        break;
                }
                if (!ret) {
                    message(ERROR, "JNote", "db upgrade was failed");
                    return false;
                }

                message(INFO, "JNote", QString("DB was upgraded to verson=%1 successfully!").arg(DB_VERSION));
                db.open(dbfile);
            }
        } else
            message(ERROR, "openDB", QString("Failed to open %1 !!").arg(dbfile));
    } else {
        //dbfile does not exist
        if (!db.createDB(dbfile)) {
            message(ERROR, "openDB", QString("Failed to create DB on %1 !!").arg(dbfile));
            return false;
        }
        message(INFO, "openDB", QString("Successfully created new DB on %1 !!").arg(dbfile));
    }

    onLoadData();

    return true;
}

void JNote::gui()
{
    m_titlelist = new JListWidget(this);
    m_text = new JTextEdit("", "", this);
    m_splitter = new QSplitter(Qt::Horizontal);
    m_category = new CategoryCB(this);

    QToolBar *toolbar = new QToolBar();
    menu(toolbar);

    //left panel
    QWidget *leftwg = new QWidget();
    QVBoxLayout *lvbox = new QVBoxLayout();
    lvbox->addWidget(m_category);
    lvbox->addWidget(m_titlelist);
    lvbox->setSpacing(0);
    lvbox->setContentsMargins(0,0,0,0);
    leftwg->setLayout(lvbox);

    // right panel
    QWidget *rightwg = new QWidget();
    QVBoxLayout *rvbox= new QVBoxLayout;
    rvbox->addWidget(m_text);
    rvbox->setSpacing(0);
     rvbox->setContentsMargins(0,0,0,0);
    rightwg->setLayout(rvbox);

    m_splitter->addWidget(leftwg);
    m_splitter->addWidget(rightwg);
    m_splitter->setHandleWidth(1);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(toolbar);
    layout->addWidget(m_splitter);

    resize(MAIN_DIALOG_SIZE);

    setLayout(layout);

    connect(m_text, SIGNAL(toChangedData(QString, QString)), this, SLOT(onNoteChanged(QString, QString)));
    connect(m_text, SIGNAL(toDropFileList(QList<QUrl>)), this, SLOT(onDropFileList(QList<QUrl>)));
    connect(m_titlelist, SIGNAL(toItemChecked(int,bool)), this, SLOT(onTitleChecked(int, bool)));
    connect(m_titlelist, SIGNAL(toItemChanged(int)), this, SLOT(onTitleSelected(int)));
    connect(m_titlelist, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onTitleContextMenu(QPoint)));
    connect(m_category, SIGNAL(toItemChanged(int)), this, SLOT(onCategoryChanged(int)));
    connect(m_category, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onCategoryContextMenu(QPoint)));
}

void JNote::menu(QToolBar *toolbar)
{
    m_find = new QLineEdit();
    QAction *actAdd = new QAction(this);
    actAdd->setIcon(QIcon(":add"));
    actAdd->setIconText(tr("Add"));

    QAction *actRemove = new QAction(this);
    actRemove->setIcon(QIcon(":remove"));
    actRemove->setIconText(tr("Remove"));

    QAction *actBigger = new QAction(this);
    actBigger->setIcon(QIcon(":bigger"));
    actBigger->setIconText(tr("Bigger"));

    QAction *actSmaller = new QAction(this);
    actSmaller->setIcon(QIcon(":smaller"));
    actSmaller->setIconText(tr("Smaller"));

    QAction *actSettings = new QAction(this);
    actSettings->setIcon(QIcon(":settings"));
    actSettings->setIconText(tr("Settings"));

    m_actAttachment = new QAction(this);
    m_actAttachment->setIcon(QIcon(":attachment"));
    m_actAttachment->setIconText(tr("Attachment"));
    m_actAttachment->setVisible(false);

    toolbar->addAction(actAdd);
    toolbar->addAction(actRemove);
    toolbar->addSeparator();
    toolbar->addAction(actBigger);
    toolbar->addAction(actSmaller);
    toolbar->addSeparator();
    toolbar->addAction(actSettings);
    toolbar->addSeparator();
    toolbar->addWidget(new QLabel(tr("Search:")));
    toolbar->addWidget(m_find);
    toolbar->addSeparator();

    QMenu *menu = new QMenu(this);
    m_actAttachment->setMenu(menu);
    toolbar->addAction(m_actAttachment);

    connect(actAdd, SIGNAL(triggered()), this, SLOT(onAddNote()));
    connect(actRemove, SIGNAL(triggered()), this, SLOT(onRemoveNote()));
    connect(actBigger, SIGNAL(triggered()), this, SLOT(onFontBigger()));
    connect(actSmaller, SIGNAL(triggered()), this, SLOT(onFontSmaller()));
    connect(actSettings, SIGNAL(triggered()), this, SLOT(onSettings()));
    connect(m_find, SIGNAL(textChanged(QString)), this, SLOT(onFindNotes(QString)));
}

void JNote::onLoadData()
{
    JDB db;
    m_titlelist->updateAllData(db.getNotes(0));
    m_category->updateAllData(db.getCategories());

    qDebug() << "LoadData()";
}

void JNote::onSettings()
{
    message(DEBUG, "JNote", "onSettings()");

    SettingsDlg *dlg = new SettingsDlg(m_basepath, this);
    dlg->show();

    connect(dlg, SIGNAL(toChangeBasepath(QString)), this, SLOT(onChangeBasepath(QString)));
    connect(dlg, SIGNAL(finished(int)), this, SLOT(onLoadData(int)));
}

void JNote::onAttachDlg()
{
    int nid = m_titlelist->currentID();
    AttachDlg *dlg = new AttachDlg(nid, m_attachpath, this);

    //QPoint pos =  m_text->mapToGlobal(QPoint(m_text->geometry().y()+950, 0));
    //dlg->move(pos);
    dlg->show();

    connect(dlg, SIGNAL(finished(int)), this, SLOT(onCheckAttachment(int)));
}

void JNote::onChangeBasepath(QString path)
{
    if (m_basepath != path) {
        m_basepath = path;
        writeSettings("basepath", m_basepath);
        message(INFO, "onChangeBasepath", "Base Directory has been changed to " + path + " . Please restart JNote!");
        exit(1);
    }
}

void JNote::onDropFileList(QList<QUrl> urlList)
{
    qDebug() << "JNote::onDropFileList()" << urlList;

    int id = m_titlelist->currentID();
    if (id < 0) {
        message(ERROR, "onDropFileList", "No selected note!");
        return;
    }

    JDB db;
    QString path;
    QUrl url;
    foreach (url, urlList) {
        path = url.path();

#ifdef Q_OS_WIN32
    path = path.remove(0,1);
#endif
        QFileInfo fi(path);
        QString newfile = m_attachpath + QString("%1_%2").arg(m_titlelist->currentID()).arg(fi.fileName());

        message(INFO, "Attachment", "Attaching " + newfile);

        if (QFile::copy(path, newfile)) {
            QFileInfo fi2(newfile);
            QString fn = fi2.fileName();
            int id = m_titlelist->currentID();
            db.insertAttachment(fn, id);
        } else {
            message(ERROR, "JNote", QString("Failed to copy file from %1 to %2").arg(path).arg(newfile));
        }
    } //foreach

    onCheckAttachment();
}


void JNote::onTitleContextMenu(const QPoint &pos)
{
    message(DEBUG, "onTitleContextMenu()", "move items to different category");

    JDB db;
    if (m_titlelist->currentID() > 0) {
        // Handle global position
        QPoint globalPos = m_titlelist->mapToGlobal(pos);
        // Create menu and insert some actions
        QMenu *menu = new QMenu();      
        menu->addAction(getMenuTitleWidget("Change Category"));

        IdMapIterator i(db.getCategories());
        while (i.hasNext()) {
            i.next();
            QString text = i.value().at(0);
            if (text == m_category->currentText()) // skipped current category!
                continue;
            QAction *action = new QAction(text);
            action->setData(i.key());
            menu->addAction(action);
        }
        connect(menu, SIGNAL(triggered(QAction*)), this, SLOT(onTitleChangeCategory(QAction *)));

        // Show context menu at handling position
        menu->exec(globalPos);
   }
}

void JNote::onCategoryContextMenu(const QPoint &pos)
{
    message(DEBUG, "onCategoryContextMenu()", "change category");

    // Handle global position
    QPoint globalPos = m_category->mapToGlobal(pos);

    // Create menu and insert some actions
    QMenu *menu = new QMenu();
    menu->addSection("Category");
    menu->addAction("New", this, SLOT(onNewCategory()));
    menu->addAction("Modify", this , SLOT(onModifyCategory()));
    menu->addAction("Delete", this, SLOT(onDeleteCategory()));

    // Show context menu at handling position
    menu->exec(globalPos);
}

void JNote::onCategoryChanged(int cid)
{
    message(DEBUG, "onCategoryChanged()", "start..");

    JDB db;
    m_text->updateData("");
    m_titlelist->updateAllData(db.findNotes(m_findwords, cid), db.getCategoryType(cid)==TASK_TYPE);

  //  m_counter->setText(QString(" Records: %1").arg(db.counterNote(cid)));
}

void JNote::onTitleChangeCategory(QAction *action)
{
    message(DEBUG, "onTitleChangeCategory()", QString("action->text():%1").arg(action->text()));

    JDB db;
    IdMapIterator i(m_titlelist->selectedData());
    while (i.hasNext()) {
       i.next();
       db.setNoteCategory(i.key(),  action->data().toInt());
       m_titlelist->removeSelectedData(); // remove from title list
    }
}

void JNote::onNewCategory()
{
    JDB db;
    CategoryDlg *dlg = new CategoryDlg(-1, this);
    dlg->show();

    connect(dlg, SIGNAL(accepted()), this, SLOT(onLoadData()));
}

void JNote::onModifyCategory()
{
    JDB db;
    CategoryDlg *dlg = new CategoryDlg(m_category->currentID(), this);
    dlg->show();

    connect(dlg, SIGNAL(accepted()), this, SLOT(onLoadData()));
}

void JNote::onDeleteCategory()
{
    JDB db;
    int cid = m_category->currentData().toInt();
    if (db.getNotes(cid).size() > 0) {
        message(ERROR, "Delete Category", "Unable to delete category if there is existing notes!");
        return;
    }

   if (db.removeCategory(cid))
        m_category->updateAllData(db.getCategories());
   else
       message(ERROR, "onDeleteCategory()", "Failed to delete category, " + m_category->currentText());
}

void JNote::onAddNote()
{
    message(DEBUG, "onAddNote()", "adding note.");
    qDebug() << m_category->currentIndex();

    if (m_category->currentIndex()<0) {
        message(ERROR, "onAddNote()", "Category is not selected");
        return;
    }

    JDB db;
    int cid = m_category->currentID();
    int nid = db.insertNewNote(cid);
    int ctype = db.getCategoryType(cid);
    m_titlelist->addData(nid, "", ctype == TASK_TYPE);
  //  db.setNoteCategory(nid, cid);

    //For journal
    if (ctype == JORUNAL_TYPE) {
        m_text->append(QDateTime::currentDateTime().toString(DATE_LONG));
        m_text->append("");
        m_text->append("");
    }

    m_text->setFocus();
}

void JNote::onTitleChecked(int id, bool isChecked)
{
    message(DEBUG, "onTitleChecked()", QString("active: %1").arg(isChecked));
    JDB db;
    db.setNoteChecked(id, isChecked);
}
void JNote::onTitleSelected(int id)
{
    message(DEBUG, "onTitleSelected()", QString("id = %1").arg(id));

    JDB db;

   if (id == NO_DATA) {
        m_text->updateData("");
        return;
    }

    m_text->updateData(db.getNote(id), m_find->text());
    m_text->setReadOnly(true);

    onCheckAttachment(id);
}


void JNote::onNoteChanged(QString filename, QString data)
{
    JDB db;
    m_titlelist->updateCurrentData(FIRSTLINE(data));
    db.setNote(m_titlelist->currentID(), data);
}

void JNote::onRemoveNote()
{
    message(DEBUG, "onRemoveData()", "removing data..");

    JDB db;
    IdMap map = m_titlelist->selectedData();

    if (map.isEmpty()) {
        message(ERROR, "Remove Note", "No data was selected!");
        return;
    }

    if (!question("Delete Notes", QString("%1 notes are selected. Do you really want to delete them?").arg(map.count())))
                 return;

    qDebug() << map;

    IdMapIterator i(map);
    while (i.hasNext()) {
        i.next();
        // remove attachments
        IdMap attMap = db.getAllAttachment(i.key());
        if (!attMap.isEmpty()) {
            message(ERROR, "onRemoveNote", QString("Unable to delete Note ID:%1 as there is attchments.").arg(i.key()));
            return;
        }

        // remove DB
        if (!db.removeNote(i.key())) {
            message(ERROR, "Remove Note", "Failed to remove note!");
            return;
        }
    }
    m_titlelist->removeSelectedData();
}

void JNote::onFontBigger()
{
    m_fontsize += FONT_SIZE_CHANGE;
    m_fontsize = (m_fontsize > FONT_SIZE_MAX) ? FONT_SIZE_MAX : m_fontsize;
    emit toFontResize(m_fontsize);
}

void JNote::onFontSmaller()
{
    m_fontsize -= FONT_SIZE_CHANGE;
    m_fontsize = (m_fontsize < FONT_DEFAULT_SIZE) ? FONT_DEFAULT_SIZE : m_fontsize;
    emit toFontResize(m_fontsize);
}

void JNote::onFindNotes(QString words)
{
    JDB db;
    m_findwords = words;
    m_text->updateData("");
    m_titlelist->updateAllData(db.findNotes(words, m_category->currentID()));
}

void JNote::onCheckAttachment(int res)
{
    JDB db;
    QMenu *menu = new QMenu();
    int nid = m_titlelist->currentID();

    IdMap map = db.getAllAttachment(nid);
    if (map.count() <= 0) {
        m_actAttachment->setVisible(false);
        return;
    }

    IdMapIterator i(map);
    while (i.hasNext()) {
        i.next();
        qDebug() << i.key() << i.value().at(0);
        QAction *act = new QAction(i.value().at(0));
        act->setData(i.key());
        menu->addAction(act);
    }
    menu->addAction("More..", this, SLOT(onAttachDlg()));
    m_actAttachment->setMenu(menu);
    m_actAttachment->setVisible(true);

    connect(menu, SIGNAL(triggered(QAction*)), this, SLOT(onAttachOpen(QAction *)));
}

void JNote::onAttachOpen(QAction *act)
{
    message(DEBUG, "onAttachOpen()", m_attachpath + act->text());
    QDesktopServices::openUrl(QUrl("file:///" + m_attachpath + act->text(), QUrl::TolerantMode));
}

JNote::~JNote()
{
    writeSettings("geometry", saveGeometry());
    writeSettings("splitter", m_splitter->saveState());
    writeSettings("fontsize", m_fontsize);
    writeSettings("basepath", m_basepath);
}

/// functions

QWidgetAction* JNote::getMenuTitleWidget(QString title)
{
    QWidgetAction *wg = new QWidgetAction(this);
    QLabel *lb = new QLabel(title);
    lb->setAlignment(Qt::AlignCenter);
    lb->setStyleSheet("QLabel {color:gray;}");
    wg->setDefaultWidget(lb);
    return wg;
}


