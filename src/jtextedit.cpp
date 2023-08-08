#include "jtextedit.h"
#include "searchdlg.h"
#include <QDragEnterEvent>
#include <QMimeData>
#include <QFileInfo>
#include <QFile>
#include <QShortcut>

JTextEdit::JTextEdit(QString filename, QString data, QWidget *parent) : QTextEdit(parent)
{
    m_filename = filename;
    setPlainText(data);

    setAcceptDrops(true);
    setAutoFillBackground(true);
    setAcceptRichText(false);
    QShortcut *searchtext = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_F), this);

    connect(searchtext, SIGNAL(activated()), this, SLOT(onSearchOpen()));
    connect(this, SIGNAL(textChanged()), this, SLOT(onDataChanged()));
    connect(parent, SIGNAL(toFontResize(int)), this, SLOT(onFontResize(int)));
}

void JTextEdit::updateData(QString data, QString findStr)
{
    setPlainText(data);
    findString(findStr, QColor(Qt::yellow).lighter(130));
}

void JTextEdit::findString(QString str, QColor color)
{
    if (str.isEmpty())
        return;

    QList<QTextEdit::ExtraSelection> foundList;
    moveCursor(QTextCursor::Start);

    while (find(str)) {
        QTextEdit::ExtraSelection extra;
        extra.format.setBackground(color);
        extra.cursor = textCursor();
        foundList.append(extra);
    }

    setExtraSelections(foundList);
}

void JTextEdit::onFindText(QString text)
{
    findString(text, QColor("darkCyan"));
}

void JTextEdit::onSearchOpen()
{
    SearchDlg *dlg = new SearchDlg;
    dlg->show();

    connect(dlg, SIGNAL(toSearch(QString)), this, SLOT(onSearchInText(QString)));
}

void JTextEdit::onSearchInText(QString str)
{
    findString(str, QColor(Qt::green).lighter(130));
}

void JTextEdit::onDataChanged()
{
    if (!toPlainText().isEmpty())
        emit toChangedData(m_filename, toPlainText());
}

void JTextEdit::onFontResize(int fontsize)
{
    qDebug() << "JTextEdit::onFontResize()" << fontsize;
    setStyleSheet(FONT_STYLE_LIST(fontsize));
}

void JTextEdit::focusOutEvent(QFocusEvent *event)
{
//    emit editingFinished();
}

void JTextEdit::dragEnterEvent(QDragEnterEvent *event)
{
 //    setText(tr("<drop content>"));
     setBackgroundRole(QPalette::Highlight);

     event->acceptProposedAction();
 //    emit toChangedDrop(event->mimeData());
}

void JTextEdit::dragMoveEvent(QDragMoveEvent *event)
{
     event->acceptProposedAction();
}

void JTextEdit::dropEvent(QDropEvent *event)
{
    message(DEBUG, "JTextEdit::dropEvent()");

    const QMimeData *mimeData = event->mimeData();
    if (!mimeData->hasUrls()) {
        QMessageBox::critical(this, tr("Error"), tr("Only one file can be attached for each note"));
       return;
    }

    QList<QUrl> urlList = mimeData->urls();

    emit toDropFileList(urlList);

    if (!m_filename.isEmpty() && urlList.count()==1) {
        QString url = urlList.at(0).path();

    #ifdef Q_OS_WIN32
        url = url.remove(0,1);
    #endif

        QFileInfo fi(url);
        QString fn = fi.absoluteFilePath();
        setPlainText(readFile(fn));
        emit toDropFilePath(fn);
    }

    setBackgroundRole(QPalette::Dark);
    event->acceptProposedAction();
}

void JTextEdit::dragLeaveEvent(QDragLeaveEvent *event)
{
     event->accept();
}

void JTextEdit::mouseReleaseEvent(QMouseEvent *e)
{
  //  onFindText(textCursor().selectedText());

    QTextEdit::mouseReleaseEvent(e);
}

void JTextEdit::mousePressEvent(QMouseEvent *e)
{
    QTextEdit::mousePressEvent(e);
}
