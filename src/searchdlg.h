#ifndef SEARCHDLG_H
#define SEARCHDLG_H

#include <QDialog>
#include <QWidget>
#include <QKeyEvent>


class SearchDlg : public QDialog
{
    Q_OBJECT

public:
    explicit SearchDlg(QWidget *parten = nullptr);

private:
    void keyPressEvent(QKeyEvent *evt)
    {
        if(evt->key() == Qt::Key_Enter || evt->key() == Qt::Key_Return)
            done(1);
        QDialog::keyPressEvent(evt);
    }

signals:
    void toSearch(QString);

private slots:
    void onSearch(QString);

};

#endif // SEARCHDLG_H
