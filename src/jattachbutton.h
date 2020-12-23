#ifndef JATTACHBUTTON_H
#define JATTACHBUTTON_H

#include <QObject>
#include <QPushButton>
#include <QMouseEvent>
#include <QPoint>

class JAttachButton : public QPushButton
{
        Q_OBJECT
public:
    explicit JAttachButton(QWidget *parent = nullptr);
private:
    QString m_attachpath;
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void dragEnterEvent(QDragEnterEvent* e);
    QPoint m_dragStartPosition;

signals:
    void toAttachOpen(QString);

public slots:
    void onOpen();
    void onHide();
    void onAttach(QString);
    void onChangePath(QString);
};

#endif // JATTACHBUTTON_H
