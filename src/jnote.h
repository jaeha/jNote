#ifndef JNOTE_H
#define JNOTE_H

#include "global.h"
#include "jtextedit.h"
#include "jlistwidget.h"
#include "jcategorycb.h"
#include <QWidget>
#include <QToolBar>
#include <QSplitter>
#include <QListWidget>
#include <QVBoxLayout>

class JNote : public QWidget
{
    Q_OBJECT

public:
    JNote(QWidget *parent = 0);
    void open();
    QString dbfile() { return m_basepath + DB_FILE; }  //
    QString attachpath() { return m_basepath + ATTACH_DIR + SP; }
    ~JNote();

private:
    JListWidget *m_titlelist;
    JTextEdit *m_text;
    JCategoryCB *m_category;
    QSplitter *m_splitter;
    QLineEdit *m_find;
    QLabel *m_counter;
    int m_fontsize;
    QString m_basepath;
    QString m_findwords;

    void gui();
    void loadData();
    void menu(QToolBar *toolbar);
    void writeSettings();
    void readSettings();


signals:
    void toFontResize(int);
    void toAttachFile(QString);
    void toHideAttachIcons();
    void toChangeAttachpath(QString);

private slots:
    void onDropFile(QString);
    void onTitleContextMenu(const QPoint&);
    void onCategoryContextMenu(const QPoint &pos);
    void onTitleSelected(int id);
    void onTextChanged(QString);
    void onAddNote();
    void onRemoveNote();
    void onDeleteAttach();
    void onAttachOpen(QString);
    void onFontBigger();
    void onFontSmaller();
    void onSettings();
    void onFindNotes(QString);
    void onNewCategory();
    void onDeleteCategory();
    void onRenameCategory();
    void onTitleChangeCategory(QAction*);
    void onCategoryChanged(int cid);
    void onImportData(QString);
    void onChangeBasepath(QString);
};

#endif // JNOTE_H
