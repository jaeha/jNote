#include "categorycb.h"
#include "jtabledlg.h"

CategoryCB::CategoryCB(QWidget *parent) : QComboBox(parent)
{
    setContextMenuPolicy(Qt::CustomContextMenu);

    connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(onCurrentIndexChanged(int)));
    connect(parent, SIGNAL(toFontResize(int)), this, SLOT(onFontResize(int)));
}

void CategoryCB::onCurrentIndexChanged(int index)
{
    message(DEBUG, "onCurrentIndexChanged", QString("index:%1").arg(index));
    emit toItemChanged(itemData(index).toInt());
}

void CategoryCB::updateAllData(IdMap map)
{
    clear();

    IdMapIterator i(map);
    while (i.hasNext()) {
        i.next();
        addItem(i.value(), i.key());
    }

    setCurrentIndex(readSettings("lastCategoryIndex").toInt());
}

int CategoryCB::currentID() {
    return currentData().toInt();
}

void CategoryCB::onFontResize(int fontsize)
{
 //   message(DEBUG, "onFontResize()", QString("new font size: %1").arg(fontsize));
    setStyleSheet(FONT_STYLE_LIST(fontsize));
}

CategoryCB::~CategoryCB()
{
    writeSettings("lastCategoryIndex", currentIndex());
}
