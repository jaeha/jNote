#include "jlistwidget.h"


#define LIST_KEY    1001
#define ID(item)    item->data(LIST_KEY).toInt()

JListWidget::JListWidget(QWidget *parent) : QListWidget(parent)
{
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setContextMenuPolicy(Qt::CustomContextMenu);

   connect(this, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)),
           this, SLOT(onCurrentItemChanged(QListWidgetItem*, QListWidgetItem*)));
   connect(parent, SIGNAL(toFontResize(int)), this, SLOT(onFontResize(int)));
   connect(this, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(onItemClicked(QListWidgetItem *)));
   connect(this, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this,
              SLOT(onItemDoubleClicked(QListWidgetItem *)));
}

void JListWidget::onItemDoubleClicked(QListWidgetItem *item)
{
   // qDebug() << "JListWidget::onItemDoubleClicked()" << item->text() << item->data(LIST_KEY).toInt();
    item->setFlags (item->flags () | Qt::ItemIsEditable);
    emit toItemDoubleClicked(item->data(LIST_KEY).toInt(), item->text());
}

void JListWidget::onItemClicked(QListWidgetItem *item)
{
    message(DEBUG, "JListWidget::onItemClicked()", "start");

   if (item->flags() & Qt::ItemIsUserCheckable) {
        bool isChecked = item->checkState();
        checkItem(item, isChecked);
        emit toItemChecked(ID(item), isChecked);
   }
   else
       emit toItemClicked(ID(item), item->text());
}

void JListWidget::onCurrentItemChanged(QListWidgetItem* newItem, QListWidgetItem* oldItem)
{
    message(DEBUG, "JListWidget::onCurrentItemChanged()", QString("items: %1").arg(selectedItems().size()));

    if (newItem == nullptr) {
        emit toItemChanged(NO_DATA);
    } else {
        message(DEBUG, "onCurrentItemChanged()", "item changed");
        emit toItemChanged(ID(newItem));
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
        map.insert(ID(item), QStringList()<<item->text());

    return map;
}

void JListWidget::updateAllData(IdMap map, bool isCheckable)
{
  //  message(DEBUG, "updateAllData()", "start..");
  //  qDebug() << map;

    clear();

    if (map.count() == 0)
        return;

    IdMapIterator i(map);
    while (i.hasNext()) {
        i.next();
        QListWidgetItem *item = new QListWidgetItem(i.value().at(0));
        item->setData(LIST_KEY, i.key());
        if (isCheckable) {
            item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
            checkItem(item, i.value().at(1)=="1");
        }
        else {
            item->setFlags(item->flags() & ~Qt::ItemIsUserCheckable);
        }
        addItem(item);
    }

   // if (this->count() > 0)
   //     setCurrentRow(0);
}

void JListWidget::addData(int id, QString str, bool isCheckable)
{
    message(DEBUG, "JListWidget::addData()", "Start");
    qDebug() << id << str;

    selectionModel()->clearSelection();

    // add new item
    QListWidgetItem *item = new QListWidgetItem(str);
    item->setData(LIST_KEY, id);
    if (isCheckable) {
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(Qt::Unchecked);
    }  else {
        item->setFlags(item->flags() & ~Qt::ItemIsUserCheckable);
    }
    addItem(item);
    setCurrentItem(item);

    message(DEBUG, "JListWidget::addData()", "End");
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

void JListWidget::checkItem(QListWidgetItem *item, bool isChecked)
{
    message(DEBUG, "JListWidget::checkItem", QString("start isCghecked=%1").arg(isChecked));

    QFont f = item->font();
    if (isChecked) {
        f.setStrikeOut(true);
        item->setCheckState(Qt::Checked);
    }
    else {
        f.setStrikeOut(false);
        item->setCheckState(Qt::Unchecked);
    }
    item->setFont(f);
}
