#include "jattachbutton.h"
#include "global.h"
#include <QApplication>
#include <QDrag>
#include <QMimeData>
#include <QFileInfo>

JAttachButton::JAttachButton(QWidget *parent) : QPushButton(parent)
{
    hide();

    connect(this, SIGNAL(clicked()), this, SLOT(onOpen()));
    connect(parent, SIGNAL(toHideAttachIcons()), this, SLOT(onHide()));
    connect(parent, SIGNAL(toAttachFile(QString)), this, SLOT(onAttach(QString)));
    connect(parent, SIGNAL(toChangeAttachpath(QString)), this, SLOT(onChangePath(QString)));
}

void JAttachButton::onOpen()
{
    emit toAttachOpen(m_attachpath + text());
}

void JAttachButton::onAttach(QString file)
{
    message(DEBUG, "JAttachWidget::onAttach()", QString("attached file: %1").arg(file));

    setText(file);
    show();
}

void JAttachButton::onHide()
{
    setText("");
    hide();
}

void JAttachButton::onChangePath(QString path)
{
    m_attachpath = path;
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
 //   qDebug() << fi.absoluteFilePath();
    QUrl url = QUrl::fromLocalFile(fi.absoluteFilePath());

    QMimeData *mimeData = new QMimeData;
    mimeData->setUrls(QList<QUrl>() << url);

    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);
    Qt::DropAction dropAction = drag->exec(Qt::CopyAction);

    QPushButton::mouseMoveEvent(e);
}
