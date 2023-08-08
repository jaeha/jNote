#ifndef CATEGORYDLG_H
#define CATEGORYDLG_H

#include <QLineEdit>
#include <QComboBox>
#include <QDialog>

class CategoryDlg : public QDialog
{
    Q_OBJECT

public:
    explicit CategoryDlg(int cid, QWidget *parent = nullptr);

private:
    int m_cid;
    QLineEdit *m_name;
    QComboBox *m_type;

signals:

private slots:
    void onSave();
};

#endif // CATEGORYDLG_H
