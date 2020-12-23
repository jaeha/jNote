#ifndef JTABLEDLG_H
#define JTABLEDLG_H

#include <QObject>
#include <QDialog>

class JTableDlg : public QDialog
{
    Q_OBJECT
public:
    explicit JTableDlg(QString table, QWidget *parent = nullptr);

signals:

public slots:
};

#endif // JTABLEDLG_H
