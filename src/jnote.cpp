 #include "jnote.h"
#include "jdb.h"
#include "jattachbutton.h"
#include "jattachdelbutton.h"
#include "jsettingsdlg.h"
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

JDB db;

JNote::JNote(QWidget *parent) : QWidget(parent)
{
    m_findwords="";
}

void JNote::open()
{
    qDebug() << "JNote::open()";
    gui();
    readSettings();

    if (m_basepath.isEmpty())
        m_basepath = BASE_PATH;

    qDebug() <<"m_basepath=" <<m_basepath << dbfile();

    //create new db if it's not exist.
    if (!QFile(dbfile()).exists()) {
        db.createDB(dbfile());
        QDir(m_basepath).mkdir(ATTACH_DIR);
    }
    else {
        // remove existing backup file and copy
        QString backupFile = dbfile() + ".bak";
        removeFile(backupFile);
        copyFile(dbfile(), backupFile);
        db.open(dbfile());
    }

    // if the db is old, upgrade it.
    message(DEBUG, "JDB", QString("open() DB version: %1").arg(db.dbversion()));
    if (db.dbversion() < DB_VERSION) {
        message(DEBUG, "JDB", "DB upgrade...");
        db.close();
        if (!db.upgrade(dbfile())) {
            message(ERROR, "JNote", "db upgrade was failed");
            return;
        }
        message(INFO, "JNote", "DB upgraded successfully!");
        db.open(dbfile());
    }

    loadData();
    emit toFontResize(m_fontsize);
    emit toChangeAttachpath(attachpath());
}

void JNote::gui()
{
    m_splitter = new QSplitter();
    m_text = new JTextEdit("",this);
    m_titlelist = new JListWidget(this);
    m_category = new JCategoryCB(this);
   // m_titlelist->setContextMenuPolicy(Qt::CustomContextMenu);

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
   // QLineEdit *titleEd = new QLineEdit();
    QVBoxLayout *rvbox= new QVBoxLayout;
  //  rvbox->addWidget(titleEd);
    rvbox->addWidget(m_text);
    rvbox->setMargin(1);
    rvbox->setSpacing(1);
    rightwg->setLayout(rvbox);

    m_splitter->addWidget(leftwg);
    m_splitter->addWidget(rightwg);

    JAttachButton *attachbt = new JAttachButton(this);
    JAttachDelButton *attachdelbt = new JAttachDelButton(this);
    m_counter = new QLabel("");
    m_counter->setMaximumHeight(attachdelbt->height());

    QHBoxLayout *hbox = new QHBoxLayout();
    hbox->addWidget(m_counter, 0, Qt::AlignLeft);
    hbox->addStretch();
    hbox->addWidget(attachdelbt, 0, Qt::AlignRight);
    hbox->addWidget(attachbt, 0, Qt::AlignRight);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(toolbar);
    layout->addWidget(m_splitter);
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addLayout(hbox);

    resize(MAIN_DIALOG_SIZE);

    setLayout(layout);

    connect(m_text, SIGNAL(toChangedData(QString)), this, SLOT(onTextChanged(QString)));
    connect(m_text, SIGNAL(toDropFile(QString)), this, SLOT(onDropFile(QString)));
    connect(m_titlelist, SIGNAL(toItemChanged(int)), this, SLOT(onTitleSelected(int)));
    connect(m_titlelist, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onTitleContextMenu(QPoint)));
    connect(m_category, SIGNAL(toItemChanged(int)), this, SLOT(onCategoryChanged(int)));
    connect(m_category, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onCategoryContextMenu(QPoint)));

    connect(attachdelbt, SIGNAL(toDeleteAttach()), this, SLOT(onDeleteAttach()));
    connect(attachbt, SIGNAL(toAttachOpen(QString)), this, SLOT(onAttachOpen(QString)));
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

    toolbar->addAction(actAdd);
    toolbar->addAction(actRemove);
    toolbar->addSeparator();
    toolbar->addAction(actBigger);
    toolbar->addAction(actSmaller);
    toolbar->addSeparator();
    toolbar->addAction(actSettings);
    toolbar->addSeparator();
    toolbar->addWidget(new QLabel(tr("Find:")));
    toolbar->addWidget(m_find);

    connect(actAdd, SIGNAL(triggered()), this, SLOT(onAddNote()));
    connect(actRemove, SIGNAL(triggered()), this, SLOT(onRemoveNote()));
    connect(actBigger, SIGNAL(triggered()), this, SLOT(onFontBigger()));
    connect(actSmaller, SIGNAL(triggered()), this, SLOT(onFontSmaller()));
    connect(actSettings, SIGNAL(triggered()), this, SLOT(onSettings()));

    connect(m_find, SIGNAL(textChanged(QString)), this, SLOT(onFindNotes(QString)));
}

void JNote::loadData()
{
    m_titlelist->updateAllData(db.getNotes(0));
    m_category->updateAllData(db.getCategories());
}

void JNote::onSettings()
{
    message(DEBUG, "JNote", "onSettings()");

    JSettingsDlg *dlg = new JSettingsDlg(this, m_basepath);
    dlg->show();

    connect(dlg, SIGNAL(toImportData(QString)), this, SLOT(onImportData(QString)));
    connect(dlg, SIGNAL(toChangeBasepath(QString)), this, SLOT(onChangeBasepath(QString)));
}

void JNote::onImportData(QString path)
{
    // category will not be imported, and all data will be under new category, 'imported'.

    QString importDB = path + SP + DB_FILE;
    if (!QFile(importDB).exists()) {
        message(ERROR, "JNote", importDB + " is not found!");
        return;
    }

    if (copyFile(dbfile(), dbfile() + ".before_import"))
        message(DEBUG, "JDB", "import() create backup:" + dbfile() + ".before_import");

    if (db.import(importDB)) {
        copyFile(path + SP + ATTACH_DIR, m_basepath + ATTACH_DIR);
        loadData();
        message(INFO, "JNote", "Import was completed successfully!");
    }
    else
        message(ERROR, "JNote", "onImportData() import was failed!");
}

void JNote::onChangeBasepath(QString path)
{
    m_basepath = path;
    message(INFO, "onChangeBasepath", "Changed base directory to " + path + " . Please copy all files into new directory and resatrt JNote!");
}

void JNote::onDropFile(QString path)
{
    message(DEBUG, "JNote()::onDropFile: ", path);

    QFileInfo fi(path);
    QString newfile = attachpath() + SP + QString("%1_%2").arg(m_titlelist->currentID()).arg(fi.fileName());

    if (QFile::copy(path, newfile)) {
        QFileInfo fi2(newfile);
        QString fn = fi2.fileName();
        emit toAttachFile(fn);
        db.setAttach(m_titlelist->currentID(), fn);
    } else {
        message(ERROR, "JNote", QString("Failed to copy file from %1 to %2").arg(path).arg(newfile));
    }
}

void JNote::onAttachOpen(QString path)
{
    message(DEBUG, "onAttachOpen()", path);
    QDesktopServices::openUrl(QUrl("file:///" + path, QUrl::TolerantMode));
}

void JNote::onDeleteAttach()
{
    message(DEBUG,"JNote", "onDeleteAttach()");

    int id = m_titlelist->currentID();
    QString fn = attachpath() + db.getAttach(id);
    qDebug() << "file remove: " << fn;
    QFile f(fn);
    if (f.remove()) {
        db.setAttach(id, "");
        emit toHideAttachIcons();
    } else
        message(ERROR, "Remove attachment", "Failed to remove Attachment file, " + fn);
}

void JNote::onTitleContextMenu(const QPoint &pos)
{
    message(DEBUG, "onTitleContextMenu()", "move items to different category");

   if (m_titlelist->currentID() > 0) {
        // Handle global position
        QPoint globalPos = m_titlelist->mapToGlobal(pos);
        // Create menu and insert some actions
        QMenu *menu = new QMenu();

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

    m_text->updateData("");
    m_titlelist->updateAllData(db.findNotes(m_findwords, cid));

    m_counter->setText(QString(" Records: %1").arg(db.counterNote(cid)));
}

void JNote::onTitleChangeCategory(QAction *action)
{
    message(DEBUG, "onTitleChangeCategory()", QString("action->text():%1").arg(action->text()));

    foreach (int nid, m_titlelist->selectedData()) {
       db.setNoteCategory(nid,  action->data().toInt());
       m_titlelist->removeSelectedData(); // remove from title list
    }
}

void JNote::onNewCategory()
{
    bool ok;
    QString newCategory = QInputDialog::getText(this, tr("Cagtegory"),
                                             tr("New Category:"), QLineEdit::Normal,
                                             "", &ok);
    if (ok && !newCategory.isEmpty()) {
        int cid = db.insertNewCategory(newCategory);
    }

    m_category->updateAllData(db.getCategories());
    m_category->setCurrentText(newCategory);
}

void JNote::onRenameCategory()
{
    bool ok;
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

    int nid = db.insertEmptyNote();
    m_titlelist->addData(nid, "");
    db.setNoteCategory(nid, m_category->currentID());
    m_text->setFocus();
}

void JNote::onTitleSelected(int id)
{
    message(DEBUG, "onTitleSelected()", QString("id = %1").arg(id));

    if (id == NO_DATA) {
        m_text->updateData("");
        return;
    }

    m_text->updateData(db.getNote(id), m_find->text());
    QString f = db.getAttach(id);
    if (!f.isEmpty()) {
        toAttachFile(f);
    }
    else
        emit toHideAttachIcons();
}


void JNote::onTextChanged(QString text)
{
    m_titlelist->updateCurrentData(FIRSTLINE(text));
    db.setNote(m_titlelist->currentID(), text);
}

void JNote::onRemoveNote()
{
    message(DEBUG, "onRemoveData()", "removing data..");

    int id = m_titlelist->currentID();
    if (id == NO_DATA) {
        message(ERROR, "Remove Note", "No data was selected!");
        return;
    }

    int button = QMessageBox::question(this, tr("Question"),
                         tr("Do you really want to delete the selected note ?"),
                         QMessageBox::Yes|QMessageBox::No, QMessageBox::No);
    if (button == QMessageBox::Yes) {
        if (!db.getAttach(id).isEmpty())
            onDeleteAttach();
        if (db.removeNote(id)) {
            m_titlelist->removeSelectedData();
            if (!db.getAttach(id).isEmpty())
                onDeleteAttach();
        }
        else
            message(ERROR, "Remove Note", "Failed to remove note!");
    }
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

/*
void JNote::onOpen()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                 QDir::homePath(),
                                 QFileDialog::ShowDirsOnly
                                 | QFileDialog::DontResolveSymlinks);

#ifdef Q_OS_WIN32
    dir = dir.remove(0,1);
#endif

    doOpen(dir);
}
*/

void JNote::onFindNotes(QString words)
{
    m_findwords = words;
    m_text->updateData("");
    m_titlelist->updateAllData(db.findNotes(words, m_category->currentID()));
}

void JNote::writeSettings()
{
    QSettings settings(COMPANY, APP_TITLE);

    settings.beginGroup("JNote");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("splitter", m_splitter->saveState());
    settings.setValue("fontsize", m_fontsize);
    settings.setValue("basepath", m_basepath);
    settings.endGroup();
}

void JNote::readSettings()
{
    QSettings settings(COMPANY, APP_TITLE);
    settings.beginGroup("JNote");
    restoreGeometry(settings.value("geometry").toByteArray());
    m_splitter->restoreState(settings.value("splitter").toByteArray());
    int fontsize = settings.value("fontsize").toInt();
    m_fontsize = (fontsize < FONT_DEFAULT_SIZE) ? FONT_DEFAULT_SIZE : fontsize;
    m_basepath = settings.value("basepath").toString();
    settings.endGroup();
}

JNote::~JNote()
{
    writeSettings();
}
