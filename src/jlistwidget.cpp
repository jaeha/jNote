#include "jlistwidget.h"


#define LIST_KEY    1001

JListWidget::JListWidget(QWidget *parent) : QListWidget(parent)
{
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setContextMenuPolicy(Qt::CustomContextMenu);
    setSpacing(2);

   /*
       QVBoxLayout *vbox = new QVBoxLayout();
       vbox->addWidget(m_list);
       vbox->setMargin(2);
       vbox->setSpacing(2);
       setLayout(vbox);
   */

   connect(this, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)),
           this, SLOT(onCurrentItemChanged(QListWidgetItem*, QListWidgetItem*)));
   connect(parent, SIGNAL(toFontResize(int)), this, SLOT(onFontResize(int)));
   connect(this, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this,
           SLOT(onItemDoubleClicked(QListWidgetItem *)));
}

void JListWidget::onItemDoubleClicked(QListWidgetItem *item)
{
   // qDebug() << "JListWidget::onItemDoubleClicked()" << item->text() << item->data(LIST_KEY).toInt();
    item->setFlags (item->flags () | Qt::ItemIsEditable);
    emit toItemDoubleClicked(item->data(LIST_KEY).toInt(), item->text());
}

void JListWidget::onCurrentItemChanged(QListWidgetItem* newItem, QListWidgetItem* oldItem)
{
   // message(DEBUG, "JListWidget::onCurrentItemChanged()", QString("items: %1").arg(selectedItems().size()));

    if (newItem == NULL) {
        emit toItemChanged(NO_DATA);
    } else {
      //  message(DEBUG, "onCurrentItemChanged()", "item changed");
        emit toItemChanged(newItem->data(LIST_KEY).toInt());
    }
}

void JListWidget::onFontResize(int fontsize)
{
 //   message(DEBUG, "onFontResize()", QString("new font size: %1").arg(fontsize));
    setStyleSheet(FONT_STYLE_LIST(fontsize));
}

IdMap JListWidget::selectedData()
{
    IdMap map;
    foreach (QListWidgetItem *item, selectedItems())
        map.insert(item->data(LIST_KEY).toInt(), item->text());

    return map;
}

void JListWidget::updateAllData(IdMap map)
{
    clear();

    if (map.count() == 0)
        return;

    IdMapIterator i(map);
    while (i.hasNext()) {
        i.next();
        QListWidgetItem *item = new QListWidgetItem(i.value());
        item->setData(LIST_KEY, i.key());
        addItem(item);
    }

    setCurrentRow(0);
}

void JListWidget::addData(int id, QString str)
{
    selectionModel()->clearSelection();

    // add new item
    QListWidgetItem *item = new QListWidgetItem(str);
    item->setData(LIST_KEY, id);
  //  item->setFlags(item->flags()|Qt::ItemIsUserCheckable);
  //  item->setCheckState(Qt::Unchecked);
    addItem(item);
    setCurrentItem(item);
}

void JListWidget::removeSelectedData()
{
    qDebug() << "JListWidget::removeSelectedData()" << selectedItems();
   // takeItem(currentRow());
    qDeleteAll(selectedItems());
}

void JListWidget::updateCurrentData(QString str)
{
    currentItem()->setText(str);
}

int JListWidget::currentID()
{
    if (currentItem())
        return currentItem()->data(LIST_KEY).toInt();
    else
        return NO_DATA;
}
