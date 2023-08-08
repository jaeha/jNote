#include "jtoolbutton.h"

JToolButton::JToolButton(QWidget *widget, QWidget *parent) : QToolButton(parent)
{
    m_widget = widget;
    connect(this, SIGNAL(clicked()), this, SLOT(onClicked()));
}

void JToolButton::onClicked()
{
    emit toClicked(m_widget);
}
