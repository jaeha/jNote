#ifndef ATTACHDLG_H
#define ATTACHDLG_H

#include <QDialog>
#include "jlistwidget.h"

class AttachDlg : public QDialog
{
    Q_OBJECT
public:
    explicit AttachDlg(int nid, QString attachPath, QWidget *parent = nullptr);

private:
    JListWidget *m_attachlist;
    QString m_attachpath;
    int m_currentId;

    void gui();
    void deleteAttachments(IdMap map);

signals:

private slots:
  //  void onAttachOpen(int, QString filename);
  //  void onAttachContextMenu(const QPoint &pos);
    void onDelete();
    void onSave();

};

#endif // ATTACHDLG_H
