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
    explicit JAttachButton(QString attachpath, QWidget *parent = nullptr);
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
    void onDelete();
    void onAttach(QString);
};

#endif // JATTACHBUTTON_H
