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
    void dbopen();
    ~JNote();

private:
    JListWidget *m_titlelist;
    JListWidget *m_attachlist;
    JTextEdit *m_text;
    JTabWidget *m_memo;
    CategoryCB *m_category;
    QSplitter *m_splitter;
    QLineEdit *m_find;
    QLabel *m_counter;
    int m_fontsize;
    QString m_basepath;
    QString m_attachpath;
    QString m_findwords;

    void gui();
    void menu(QToolBar *toolbar);
    void deleteAttachments(IdMap map);
    QWidgetAction* getMenuTitleWidget(QString title);

signals:
    void toFontResize(int);
    void toAttachFile(QString);
    void toHideAttachIcons();
    void toChangeAttachpath(QString);

private slots:
    void onNoteChanged(QString, QString);
    void onAddNote();
    void onRemoveNote();
    void onFindNotes(QString);
    void onDropFileList(QList<QUrl>);
    void onChangeBasepath(QString);
    void onTitleContextMenu(const QPoint&);
    void onTitleSelected(int id);
    void onTitleChangeCategory(QAction*);
    void onNewCategory();
    void onCategoryContextMenu(const QPoint &pos);
    void onDeleteCategory();
    void onRenameCategory();
    void onCategoryChanged(int cid);
    void onAttachOpen(int, QString);
    void onAttachDelete();
    void onAttachContextMenu(const QPoint&);
    void onImportDlg();
    void onFontBigger();
    void onFontSmaller();
    void onSettings();
    void onLoadData(int);
};

#endif // JNOTE_H
