#ifndef JSEARCHDLG_H
#define JSEARCHDLG_H

#include <QDialog>
#include <QWidget>
#include <QKeyEvent>


class JSearchDlg : public QDialog
{
    Q_OBJECT

public:
    explicit JSearchDlg(QWidget *parten = nullptr);

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

#endif // JSEARCHDLG_H
