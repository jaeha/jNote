#include "jtextedit.h"
#include "jsearchdlg.h"
#include <QDragEnterEvent>
#include <QMimeData>
#include <QFileInfo>
#include <QFile>
#include <QShortcut>

JTextEdit::JTextEdit(QString data, QWidget *parent) : QTextEdit(parent)
{
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

void JTextEdit::onSearchOpen()
{
    JSearchDlg *dlg = new JSearchDlg;
    dlg->show();

    connect(dlg, SIGNAL(toSearch(QString)), this, SLOT(onSearchInText(QString)));
}

void JTextEdit::onSearchInText(QString str)
{
    findString(str, QColor(Qt::green).lighter(130));
}

void JTextEdit::onDataChanged()
{
    //save to file
//    if (!m_filename.isEmpty() && !toPlainText().isEmpty())
//        writeFile(BASE_PATH + m_filename, toPlainText().toStdString().c_str());

    if (!toPlainText().isEmpty())
        emit toChangedData(toPlainText());
}

void JTextEdit::onFontResize(int fontsize)
{
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

// READ file
void JTextEdit::dropEvent(QDropEvent *event)
{
    message(DEBUG, "JTextEdit::dropEvent()");

    const QMimeData *mimeData = event->mimeData();

    QList<QUrl> urlList = mimeData->urls();

    if (urlList.count() > 1 || !mimeData->hasUrls()) {
        QMessageBox::critical(this, tr("Error"), tr("Only one file can be attached for each note"));
       return;
    }

    QString url = urlList.at(0).path();

#ifdef Q_OS_WIN32
    url = url.remove(0,1);
#endif

    QFileInfo fi(url);
   /* if (textFiles.contains(fi.suffix())) {
        QMessageBox msgBox;
        msgBox.setText("Dropped text file.  Would you like to add as note or attachment ?");
        QPushButton *noteButton = msgBox.addButton(tr("As Note"), QMessageBox::ActionRole);
        QPushButton *attachButton = msgBox.addButton(tr("As Attachment"), QMessageBox::ActionRole);
        QPushButton *cancelButton = msgBox.addButton(QMessageBox::Cancel);
        msgBox.exec();

        if (msgBox.clickedButton() == (QAbstractButton *)noteButton)
            updateData(readFile(fi.absoluteFilePath()));

        if (msgBox.clickedButton() == (QAbstractButton *)cancelButton)
            return;
    }
*/
    emit toDropFile(fi.absoluteFilePath());

    setBackgroundRole(QPalette::Dark);
    event->acceptProposedAction();
}

void JTextEdit::dragLeaveEvent(QDragLeaveEvent *event)
{
     event->accept();
}
