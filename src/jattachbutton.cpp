#include "jattachbutton.h"
#include "global.h"
#include <QApplication>
#include <QDrag>
#include <QMimeData>
#include <QFileInfo>

JAttachButton::JAttachButton(QString attachpath, QWidget *parent) : QPushButton(parent)
{
    m_attachpath = attachpath;
    hide();

    connect(this, SIGNAL(clicked()), this, SLOT(onOpen()));
    connect(parent, SIGNAL(toDeleteAttach()), this, SLOT(onDelete()));
    connect(parent, SIGNAL(toAttachFile(QString)), this, SLOT(onAttach(QString)));
}

void JAttachButton::onOpen()
{
    emit toAttachOpen(text());
}

void JAttachButton::onAttach(QString file)
{
    message(DEBUG, "JAttachWidget::onAttach()", QString("attached file: %1").arg(file));

    setText(file);
    show();
}

void JAttachButton::onDelete()
{
    setText("");
    hide();
}

void JAttachButton::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
        m_dragStartPosition = e->pos();

    QPushButton::mousePressEvent(e);
}

void JAttachButton::dragEnterEvent(QDragEnterEvent* e)
{
    if (e->mimeData()->hasUrls())
    {
        e->acceptProposedAction();
    }
}


void JAttachButton::mouseMoveEvent(QMouseEvent *e)
{
    if (!(e->buttons() & Qt::LeftButton))
        return;
    if ((e->pos() - m_dragStartPosition).manhattanLength() < QApplication::startDragDistance())
        return;

    QFileInfo fi(m_attachpath + text());
    qDebug() << fi.absoluteFilePath();
    QUrl url = QUrl::fromLocalFile(fi.absoluteFilePath());

    QMimeData *mimeData = new QMimeData;
    mimeData->setUrls(QList<QUrl>() << url);

    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);
   // drag->exec(Qt::CopyAction | Qt::MoveAction, Qt::CopyAction);
  // drag->exec(Qt::CopyAction, Qt::CopyAction);
    Qt::DropAction dropAction = drag->exec(Qt::CopyAction);

    QPushButton::mouseMoveEvent(e);
}
