#ifndef IMPORTDLG_H
#define IMPORTDLG_H

#include <QDialog>
#include <QLineEdit>
#include <QListWidget>

class ImportDlg : public QDialog
{
    Q_OBJECT
private:
    QLineEdit *m_edImportDir;
    QListWidget *m_lvOut;
    QString m_basepath;

public:
    ImportDlg(QString basepath, QWidget *parent = nullptr);

signals:
    void toImportNote(QString);

public slots:
    void onFolderButton();
    void onImportButton();
};

#endif // IMPORTDLG_H
