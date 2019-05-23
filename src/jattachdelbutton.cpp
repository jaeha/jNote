#include "jattachdelbutton.h"

#define ICON_SIZE   QSize(24, 24)

JAttachDelButton::JAttachDelButton(QWidget *parent) : QPushButton(parent)
{
    setIcon(QIcon(":delete_attach"));
    setIconSize(ICON_SIZE);
    setFixedSize(ICON_SIZE);
    setFlat(true);

    hide();

    connect(this, SIGNAL(clicked()), this, SLOT(onDelete()));
    connect(parent, SIGNAL(toAttachFile(QString)), this, SLOT(onAttach(QString)));
    connect(parent, SIGNAL(toDeleteAttach()), this, SLOT(onHide()));
}

void JAttachDelButton::onDelete()
{
    hide();
    emit toDeleteAttach();
}

void JAttachDelButton::onHide()
{
    hide();
}

void JAttachDelButton::onAttach(QString file)
{
    show();
}
