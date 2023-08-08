#ifndef JNOTE_H
#define JNOTE_H

#include <QWidget>
#include <QToolBar>
#include <QSplitter>
#include <QListWidget>
#include <QVBoxLayout>
#include <QWidgetAction>
#include "global.h"
#include "jtextedit.h"
#include "jlistwidget.h"
#include "categorycb.h"
#include "jtabwidget.h"

class JNote : public QWidget
{
    Q_OBJECT

public:
    JNote(QWidget *parent = 0);
    bool openDB();
    ~JNote();

private:
    JListWidget *m_titlelist;
    JTextEdit *m_text;
    CategoryCB *m_category;
    QSplitter *m_splitter;
    QLineEdit *m_find;
    QLabel *m_counter;
    int m_fontsize;
    QString m_basepath;
    QString m_attachpath;
    QString m_findwords;
    QAction *m_actAttachment;

    void gui();
    void menu(QToolBar *toolbar);
    QWidgetAction* getMenuTitleWidget(QString title);

signals:
    void toFontResize(int);
   // void toAttachFile(QString);
   // void toHideAttachIcons();
   // void toChangeAttachpath(QString);

private slots:
    void onNoteChanged(QString, QString);
    void onAddNote();
    void onRemoveNote();
    void onFindNotes(QString);
    void onDropFileList(QList<QUrl>);
    void onChangeBasepath(QString);
    void onTitleContextMenu(const QPoint&);
    void onTitleChecked(int id, bool isChecked);
    void onTitleSelected(int id);
    void onTitleChangeCategory(QAction*);
    void onNewCategory();
    void onCategoryContextMenu(const QPoint &pos);
    void onDeleteCategory();
    void onModifyCategory();
    void onCategoryChanged(int cid);
    void onFontBigger();
    void onFontSmaller();
    void onSettings();
    void onLoadData();
    void onAttachDlg();
    void onCheckAttachment(int nid=-1);
    void onAttachOpen(QAction*);
};

#endif // JNOTE_H
