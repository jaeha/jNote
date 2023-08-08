#ifndef JTOOLBUTTON_H
#define JTOOLBUTTON_H

#include <QToolButton>

class JToolButton : public QToolButton
{
    Q_OBJECT

private:
    QWidget *m_widget;

public:
    explicit JToolButton(QWidget *widget, QWidget *parent = nullptr);

signals:
    void toClicked(QWidget*);

public slots:
    void onClicked();

};
#endif // JTOOLBUTTON_H
