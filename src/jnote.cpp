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
#include "importdlg.h"

JNote::JNote(QWidget *parent) : QWidget(parent)
{
    m_findwords="";

    // read settings
    m_basepath = readSettings("basepath").toString();

    qDebug() << "JNote() basepath: " << m_basepath;

    if (m_basepath.isEmpty())
        m_basepath = BASE_PATH;
    m_attachpath = m_basepath + ATTACH_DIR + SP;

    // create dirs if they are not exist
    QDir().mkdir(m_basepath);
    QDir().mkdir(m_attachpath);
    QDir(m_basepath).mkdir(MEMO_DIR);

    dbopen();
    gui();    
    onLoadData(0);

    // read settings
    restoreGeometry(readSettings("geometry").toByteArray());
    m_splitter->restoreState(readSettings("splitter").toByteArray());
    int fontsize = readSettings("fontsize").toInt();
    m_fontsize = (fontsize < FONT_DEFAULT_SIZE) ? FONT_DEFAULT_SIZE : fontsize;

    emit toFontResize(m_fontsize);
}

void JNote::dbopen()
{
    qDebug() << "basepath" << m_basepath;\

    JDB db;

    bool okDB = false;
    QString dbfile = m_basepath + DB_FILE;

    if (!QFile(dbfile).exists()) {
        okDB = db.createDB(dbfile);
    }
    else {
        // remove existing backup file and copy
        QString backupFile = dbfile + ".bak";
        removeFile(backupFile);
        copyFile(dbfile, backupFile);
       okDB = db.open(dbfile);
    }

    // if the db is old, upgrade it.
    if (okDB) {
        message(DEBUG, "JDB", QString("open() DB version: %1").arg(db.dbversion()));
        if (db.dbversion() < DB_VERSION) {
            message(DEBUG, "JDB", "DB upgrade...");
            db.close();
            if (!db.upgrade(dbfile)) {
                message(ERROR, "JNote", "db upgrade was failed");
                //return;
            }
            message(INFO, "JNote", "DB upgraded successfully!");
            db.open(dbfile);
        }
    }
}

void JNote::gui()
{
    m_titlelist = new JListWidget(this);
    m_attachlist = new JListWidget(this);
    m_text = new JTextEdit("", "", this);
    m_memo = new JTabWidget(m_basepath + MEMO_DIR, this);
    m_splitter = new QSplitter();
    m_category = new CategoryCB(this);

    QToolBar *toolbar = new QToolBar();
    menu(toolbar);

    //left panel
    QWidget *leftwg = new QWidget();
    QVBoxLayout *lvbox = new QVBoxLayout();
    lvbox->addWidget(m_category);
    lvbox->addWidget(m_titlelist);
    lvbox->setMargin(0);
    lvbox->setSpacing(5);
    leftwg->setLayout(lvbox);

    // right panel
    QWidget *rightwg = new QWidget();
    QVBoxLayout *rvbox= new QVBoxLayout;
    rvbox->addWidget(m_text);
    rvbox->setMargin(1);
    rvbox->setSpacing(1);
    rightwg->setLayout(rvbox);

    m_splitter->addWidget(leftwg);
    m_splitter->addWidget(rightwg);
    m_splitter->addWidget(m_attachlist);
    m_attachlist->hide(); // hide at the begining.

    QSplitter *vSplitter = new QSplitter(Qt::Vertical);
    vSplitter->addWidget(m_splitter);
    vSplitter->addWidget(m_memo);

    // note counter
    m_counter = new QLabel("");
    //m_counter->setMaximumHeight(attachdelbt->height());

    QHBoxLayout *hbox = new QHBoxLayout();
    hbox->addWidget(m_counter, 0, Qt::AlignLeft);
    hbox->addStretch();

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(toolbar);
    layout->addWidget(vSplitter);
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addLayout(hbox);

    resize(MAIN_DIALOG_SIZE);

    setLayout(layout);

    connect(m_text, SIGNAL(toChangedData(QString, QString)), this, SLOT(onNoteChanged(QString, QString)));
    connect(m_text, SIGNAL(toDropFileList(QList<QUrl>)), this, SLOT(onDropFileList(QList<QUrl>)));
    connect(m_titlelist, SIGNAL(toItemChanged(int)), this, SLOT(onTitleSelected(int)));
    connect(m_titlelist, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onTitleContextMenu(QPoint)));
    connect(m_category, SIGNAL(toItemChanged(int)), this, SLOT(onCategoryChanged(int)));
  //  connect(m_category, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onCategoryContextMenu(QPoint)));
    connect(m_attachlist, SIGNAL(toItemDoubleClicked(int, QString)), this, SLOT(onAttachOpen(int, QString)));
    connect(m_attachlist, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onAttachContextMenu(QPoint)));
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

    QAction *actImport = new QAction(this);
    actImport->setIcon(QIcon(":import"));
    actImport->setIconText(tr("Import"));

    QAction *actSettings = new QAction(this);
    actSettings->setIcon(QIcon(":settings"));
    actSettings->setIconText(tr("Settings"));

    toolbar->addAction(actAdd);
    toolbar->addAction(actRemove);
    toolbar->addSeparator();
    toolbar->addAction(actBigger);
    toolbar->addAction(actSmaller);
    toolbar->addSeparator();
    toolbar->addAction(actImport);
    toolbar->addSeparator();
    toolbar->addAction(actSettings);
    toolbar->addSeparator();
    toolbar->addWidget(new QLabel(tr("Find:")));
    toolbar->addWidget(m_find);

    connect(actAdd, SIGNAL(triggered()), this, SLOT(onAddNote()));
    connect(actRemove, SIGNAL(triggered()), this, SLOT(onRemoveNote()));
    connect(actBigger, SIGNAL(triggered()), this, SLOT(onFontBigger()));
    connect(actSmaller, SIGNAL(triggered()), this, SLOT(onFontSmaller()));
    connect(actImport, SIGNAL(triggered()), this, SLOT(onImportDlg()));
    connect(actSettings, SIGNAL(triggered()), this, SLOT(onSettings()));

    connect(m_find, SIGNAL(textChanged(QString)), this, SLOT(onFindNotes(QString)));
}

void JNote::onLoadData(int result)
{
    JDB db;
    m_titlelist->updateAllData(db.getNotes(0));
    m_category->updateAllData(db.getCategories());

    qDebug() << "LoadData()";
}

void JNote::onSettings()
{
    message(DEBUG, "JNote", "onSettings()");

    SettingsDlg *dlg = new SettingsDlg(this, m_basepath);
    dlg->show();

    connect(dlg, SIGNAL(toChangeBasepath(QString)), this, SLOT(onChangeBasepath(QString)));
    connect(dlg, SIGNAL(finished(int)), this, SLOT(onLoadData(int)));
}

void JNote::onImportDlg()
{
    ImportDlg *dlg = new ImportDlg(m_basepath, this);
    dlg->show();

    connect(dlg, SIGNAL(finished(int)), this, SLOT(onLoadData(int)));
}


void JNote::onChangeBasepath(QString path)
{
    m_basepath = path + SP;
    writeSettings("basepath", m_basepath);
    message(INFO, "onChangeBasepath", "Base Directory has been changed to " + path + " . Please restart JNote!");
    exit(1);
}

void JNote::onDropFileList(QList<QUrl> urlList)
{
    qDebug() << "JNote::onDropFileList()" << urlList;

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

        if (QFile::copy(path, newfile)) {
            QFileInfo fi2(newfile);
            QString fn = fi2.fileName();
            int cid = m_titlelist->currentID();
            m_attachlist->addData(cid, fn);
            db.insertAttachment(fn, cid);
        } else {
            message(ERROR, "JNote", QString("Failed to copy file from %1 to %2").arg(path).arg(newfile));
        }
    } //foreach
//    if (m_attachlist->count()>0)
        m_attachlist->show();
}

void JNote::onAttachOpen(int, QString filename)
{
    message(DEBUG, "onAttachOpen()", m_attachpath + filename);
    QDesktopServices::openUrl(QUrl("file:///" + m_attachpath + filename, QUrl::TolerantMode));
}

void JNote::onAttachContextMenu(const QPoint &pos)
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

void JNote::onAttachDelete()
{
    message(DEBUG,"JNote", "onAttachDelete()");

    IdMap map = m_attachlist->selectedData();

    int button = QMessageBox::question(this, tr("Question"),
                         QString("Do you really want to delete %1 files").arg(map.count()),
                         QMessageBox::Yes|QMessageBox::No, QMessageBox::No);
    if (button == QMessageBox::No)
        return;

    deleteAttachments(map);
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
            if (i.value() == m_category->currentText()) // skipped current category!
                continue;
            QAction *action = new QAction(i.value());
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
    menu->addAction("Rename", this , SLOT(onRenameCategory()));
    menu->addAction("Delete", this, SLOT(onDeleteCategory()));

    // Show context menu at handling position
    menu->exec(globalPos);
}

void JNote::onCategoryChanged(int cid)
{
    message(DEBUG, "onCategoryChanged()", "start..");

    JDB db;
    m_text->updateData("");
    m_titlelist->updateAllData(db.findNotes(m_findwords, cid));

    m_counter->setText(QString(" Records: %1").arg(db.counterNote(cid)));
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
    bool ok;
    JDB db;
    QString newCategory = QInputDialog::getText(this, tr("Cagtegory"),
                                             tr("New Category:"), QLineEdit::Normal,
                                             "", &ok);
    if (ok && !newCategory.isEmpty()) {
        db.insertCategory(newCategory);
    }

    m_category->updateAllData(db.getCategories());
    m_category->setCurrentText(newCategory);
}

void JNote::onRenameCategory()
{
    bool ok;
    JDB db;
    QString renCategory = QInputDialog::getText(this, tr("Cagtegory"),
                                            QString("Rename Category(%1):").arg(m_category->currentText()), \
                                            QLineEdit::Normal,"", &ok);

    if (ok && !renCategory.isEmpty()) {
        int cid = m_category->currentData().toInt();
        db.setCategory(cid, renCategory);
        db.setNoteCategory(m_titlelist->currentID(), cid);
    }

    m_category->updateAllData(db.getCategories());
    m_category->setCurrentText(renCategory);
}

void JNote::onDeleteCategory()
{
    JDB db;
    int cid = m_category->currentData().toInt();
    if (db.getNotes(cid).size() > 0) {
        message(ERROR, "Delete Category", "Unable to delete Category if there is any note!");
        return;
    }

   if (db.removeCategory(cid))
        m_category->updateAllData(db.getCategories());
   else
       message(ERROR, "onDeleteCategory()", "Failed to delete category, " + m_category->currentText());
}

void JNote::onAddNote()
{
    message(DEBUG, "onAddData()", "adding data..");

    JDB db;
    int nid = db.insertEmptyNote();
    m_titlelist->addData(nid, "");
    db.setNoteCategory(nid, m_category->currentID());
    m_text->setFocus();
}

void JNote::onTitleSelected(int id)
{
 //   message(DEBUG, "onTitleSelected()", QString("id = %1").arg(id));

    JDB db;

    m_attachlist->clear();

    if (id == NO_DATA) {
        m_text->updateData("");
        return;
    }

    m_text->updateData(db.getNote(id), m_find->text());

    IdMapIterator i(db.getAllAttachment(id));
    while (i.hasNext()) {
        i.next();
     //   qDebug() <<"key: " <<i.key() <<", value: "<<i.value();
        m_attachlist->addData(i.key(), i.value());
    }

   // qDebug() << "onTitleSelected(): " <<m_attachlist->count();

    if (m_attachlist->count()>0)
        m_attachlist->show();
    else
        m_attachlist->hide();
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

    int button = QMessageBox::question(this, tr("Question"),
                         QString("%1 notes are selected. Do you really want to delete them?").arg(map.count()),
                         QMessageBox::Yes|QMessageBox::No, QMessageBox::No);

    if (button == QMessageBox::No)
        return;

    qDebug() << map;

    IdMapIterator i(map);
    while (i.hasNext()) {
        i.next();
        // remove attachments
        IdMap attMap = db.getAllAttachment(i.key());
        if (!attMap.isEmpty())
            deleteAttachments(attMap);

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

QWidgetAction* JNote::getMenuTitleWidget(QString title)
{
    QWidgetAction *wg = new QWidgetAction(this);
    QLabel *lb = new QLabel(title);
    lb->setAlignment(Qt::AlignCenter);
    lb->setStyleSheet("QLabel {color:gray;}");
    wg->setDefaultWidget(lb);
    return wg;
}

void JNote::deleteAttachments(IdMap map) {
    JDB db;
    IdMapIterator i(map);
    while (i.hasNext()) {
        i.next();
        QFile f(m_attachpath + i.value());
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

JNote::~JNote()
{
    writeSettings("geometry", saveGeometry());
    writeSettings("splitter", m_splitter->saveState());
    writeSettings("fontsize", m_fontsize);
    writeSettings("basepath", m_basepath);
}
