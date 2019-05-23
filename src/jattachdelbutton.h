#ifndef JATTACHDELBUTTON_H
#define JATTACHDELBUTTON_H

#include <QObject>
#include <QPushButton>

class JAttachDelButton : public QPushButton
{
    Q_OBJECT
public:
    explicit JAttachDelButton(QWidget *parent = nullptr);

signals:
    void toDeleteAttach();

public slots:
    void onDelete();
    void onAttach(QString);
    void onHide();


};

#endif // JATTACHDELBUTTON_H
