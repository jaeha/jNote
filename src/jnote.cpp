#include "jnote.h"
#include "jdb.h"
#include "jattachbutton.h"
#include "jattachdelbutton.h"
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

JDB db;

JNote::JNote(QWidget *parent) : QWidget(parent)
{
}

void JNote::doOpen(QString path)
{
    // clean up
    delete layout();
    db.close();

    m_basepath = path;

    // create base path if it's not already exist
    if (!QDir(basepath()).exists())
            QDir().mkdir(basepath());
    // attachment
    if (!QDir(attachpath()).exists())
            QDir().mkdir(attachpath());

    if (!db.open(path))
        message(ERROR, "JNote()", "Unable to open db!");

    gui();
    readSettings();
    loadTitleList();
    emit toFontResize(m_fontsize);
    setWindowTitle(QString("%1 v%2: %3").arg(APP_TITLE).arg(APP_VERSION).arg(basepath()));

    //remove directory from attachments
/*    IdMap map = db.recordMap("notes", "attached", "not attached=''");
    IdMapIterator i(map);
    while (i.hasNext()) {
        i.next();
        message(DEBUG, "attached", QString("%1 %2").arg(i.key()).arg(i.value()));
        QFileInfo f(i.value());
        message(DEBUG, "after", QString("%1 %2").arg(i.key()).arg(f.fileName()));
        db.update("notes", i.key(), "attached", f.fileName());
    }
*/
}

void JNote::gui()
{
    m_splitter = new QSplitter();
    m_text = new JTextEdit("",this);
    m_titlelist = new JListWidget(this);
   // m_titlelist->setContextMenuPolicy(Qt::CustomContextMenu);

    QToolBar *toolbar = new QToolBar();
    menu(toolbar);

    //left panel
    QWidget *leftwg = new QWidget();
    QVBoxLayout *vbox = new QVBoxLayout();
    vbox->addWidget(m_titlelist);
    vbox->setMargin(0);
    leftwg->setLayout(vbox);

    // right panel
    QWidget *rightwg = new QWidget();
    QVBoxLayout *rlayout = new QVBoxLayout;
    rlayout->addWidget(m_text);
    rlayout->setMargin(0);
    rightwg->setLayout(rlayout);

    m_splitter->addWidget(leftwg);
    m_splitter->addWidget(rightwg);

    JAttachButton *attachbt = new JAttachButton(attachpath(), this);
    JAttachDelButton *attachdelbt = new JAttachDelButton(this);
    QHBoxLayout *hbox = new QHBoxLayout();
    hbox->addStretch();
    hbox->addWidget(attachdelbt);
    hbox->addWidget(attachbt);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(toolbar);
    layout->addWidget(m_splitter);
    layout->setMargin(2);
    layout->setSpacing(2);
    layout->addLayout(hbox);

    resize(MAIN_DIALOG_SIZE);

    setLayout(layout);

    connect(m_text, SIGNAL(toChangedData(QString)), this, SLOT(onTextChanged(QString)));
    connect(m_text, SIGNAL(toDropFile(QString)), this, SLOT(onDropFile(QString)));
    connect(m_titlelist, SIGNAL(toItemChanged(int)), this, SLOT(onTitleSelected(int)));
   // connect(m_titlelist, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onTitleContextMenu(QPoint)));
    connect(attachdelbt, SIGNAL(toDeleteAttach()), this, SLOT(onRemoveAttach()));
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

  //  QAction *actOpen = new QAction(this);
  //  actOpen->setIcon(QIcon(":open"));
  //  actOpen->setIconText(tr("Open"));

    QAction *actBigger = new QAction(this);
    actBigger->setIcon(QIcon(":bigger"));
    actBigger->setIconText(tr("Bigger"));

    QAction *actSmaller = new QAction(this);
    actSmaller->setIcon(QIcon(":smaller"));
    actSmaller->setIconText(tr("Smaller"));

    toolbar->addAction(actAdd);
    toolbar->addAction(actRemove);
    toolbar->addSeparator();
    toolbar->addAction(actBigger);
    toolbar->addAction(actSmaller);
    toolbar->addSeparator();
  //  toolbar->addAction(actOpen);
  //  toolbar->addSeparator();
    toolbar->addWidget(new QLabel(tr("Find:")));
    toolbar->addWidget(m_find);

    connect(actAdd, SIGNAL(triggered()), this, SLOT(onAddNote()));
    connect(actRemove, SIGNAL(triggered()), this, SLOT(onRemoveNote()));
    connect(actBigger, SIGNAL(triggered()), this, SLOT(onFontBigger()));
    connect(actSmaller, SIGNAL(triggered()), this, SLOT(onFontSmaller()));
 //   connect(actOpen, SIGNAL(triggered()), this, SLOT(onOpen()));
    connect(m_find, SIGNAL(textChanged(QString)), this, SLOT(onFindNotes(QString)));
}

void JNote::loadTitleList()
{
    m_titlelist->updateAllData(db.getTitles());
}

void JNote::onDropFile(QString path)
{
    message(DEBUG, "JNote()::onDropFile: ", path);

    QFileInfo fi(path);

    if (fi.suffix() == "jnote") {
        doOpen(fi.absolutePath());
    } else {
        QString newfile = QString("%1%2_%3").arg(attachpath()).arg(m_titlelist->currentID()).arg(fi.fileName());

        if (QFile::copy(path, newfile)) {
            QFileInfo fi2(newfile);
            QString fn = fi2.fileName();
            emit toAttachFile(fn);
            db.setAttach(m_titlelist->currentID(), fn);
        } else
            message(ERROR, "JNote", QString("Failed to copy file from %1 to %2").arg(path).arg(newfile));
    }
}

void JNote::onAttachOpen(QString file)
{
    message(DEBUG, "onAttachOpen()", attachpath()+file);
    QDesktopServices::openUrl(QUrl("file:///" + attachpath() + file, QUrl::TolerantMode));
}

void JNote::onRemoveAttach()
{
    int id = m_titlelist->currentID();
    QString fn = attachpath() + db.getAttach(id);
    QFile f(fn);
    if (f.remove()) {
        db.setAttach(id, "");
        emit toDeleteAttach();
    } else
        message(ERROR, "Remove attachment", "Failed to remove Attachment file, " + fn);
}

void JNote::onTitleContextMenu(const QPoint &pos)
{
    // Handle global position
    QPoint globalPos = m_titlelist->mapToGlobal(pos);

    // Create menu and insert some actions
    QMenu menu;
    menu.addAction("Change Category", this, SLOT(onTitleChangeCategory()));

    // Show context menu at handling position
    menu.exec(globalPos);
}

void JNote::onAddNote()
{
    message(DEBUG, "onAddData()", "adding data..");

    m_titlelist->addData(db.insertEmptyNote(), "");
    m_text->setFocus();
}

void JNote::onTitleSelected(int id)
{
    message(DEBUG, "onTitleSelected()", QString("id = %1").arg(id));

    if (id < 0)
        return;

    m_text->updateData(db.getNote(id), m_find->text());
    QString f = db.getAttach(id);
    if (!f.isEmpty()) {
        toAttachFile(f);
    }
    else
        emit toDeleteAttach();
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
        message(ERROR, "Rome Note", "No data was selected!");
        return;
    }

    int button = QMessageBox::question(this, tr("Question"),
                         tr("Do you really want to delete the selected note ?"),
                         QMessageBox::Yes|QMessageBox::No, QMessageBox::No);
    if (button == QMessageBox::Yes) {
        if (!db.getAttach(id).isEmpty())
            onRemoveAttach();
        if (db.removeNote(id)) {
            m_titlelist->takeItem(m_titlelist->currentRow());
            if (!db.getAttach(id).isEmpty())
                onRemoveAttach();
        }
        else
            message(ERROR, "Remove Note", "Failed to remove note!");
    }

    if (m_titlelist->count() == 0)
        m_text->updateData("");
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
    m_text->updateData("");
    m_titlelist->updateAllData(db.findNotes(words));
}

void JNote::writeSettings()
{
    QSettings settings(COMPANY, APP_TITLE);

    settings.beginGroup("JNote");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("splitter", m_splitter->saveState());
    settings.setValue("fontsize", m_fontsize);
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
    settings.endGroup();
}

JNote::~JNote()
{
    writeSettings();
}
