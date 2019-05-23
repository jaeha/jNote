#ifndef JNOTE_H
#define JNOTE_H

#include "global.h"
#include "jtextedit.h"
#include "jlistwidget.h"
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
    void doOpen(QString path="");
    QString basepath() { return m_basepath; }
    QString attachpath() { return m_basepath + "/" + ATTACH_PATH; }
    ~JNote();

private:
    JListWidget *m_titlelist;
    JTextEdit *m_text;
    QSplitter *m_splitter;
    QLineEdit *m_find;
    int m_fontsize;
    QString m_basepath;

    void gui();
    void loadTitleList();
    void menu(QToolBar *toolbar);
    void writeSettings();
    void readSettings();


signals:
    void toFontResize(int);
    void toAttachFile(QString);
    void toDeleteAttach();

private slots:
    void onDropFile(QString);
    void onTitleContextMenu(const QPoint&);
    void onTitleSelected(int id);
    void onTextChanged(QString);
    void onAddNote();
    void onRemoveNote();
    void onRemoveAttach();
    void onAttachOpen(QString);
    void onFontBigger();
    void onFontSmaller();
  //  void onOpen();
    void onFindNotes(QString);
};

#endif // JNOTE_H
