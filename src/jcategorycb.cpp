#include "jcategorycb.h"
#include "jtabledlg.h"

JCategoryCB::JCategoryCB(QWidget *parent) : QComboBox(parent)
{
    setContextMenuPolicy(Qt::CustomContextMenu);

    connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(onCurrentIndexChanged(int)));
    connect(parent, SIGNAL(toFontResize(int)), this, SLOT(onFontResize(int)));
}

void JCategoryCB::onCurrentIndexChanged(int index)
{
    message(DEBUG, "onCurrentIndexChanged", QString("index:%1").arg(index));
    emit toItemChanged(itemData(index).toInt());
}

void JCategoryCB::updateAllData(IdMap map)
{
    clear();

    IdMapIterator i(map);
    while (i.hasNext()) {
        i.next();
        addItem(i.value(), i.key());
    }

    setCurrentIndex(0);
}

int JCategoryCB::currentID() {
    return currentData().toInt();
}

void JCategoryCB::onFontResize(int fontsize)
{
 //   message(DEBUG, "onFontResize()", QString("new font size: %1").arg(fontsize));
    setStyleSheet(FONT_STYLE_LIST(fontsize));
}
