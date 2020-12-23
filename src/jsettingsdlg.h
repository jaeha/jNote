#ifndef JSETTINGSDLG_H
#define JSETTINGSDLG_H

#include <QObject>
#include <QDialog>
#include <QLabel>
#include <QLineEdit>

class JSettingsDlg : public QDialog
{
    Q_OBJECT

private:
    QLabel *m_lbBasePath, *m_lbImportDataPath;

    void gui();

public:
    explicit JSettingsDlg(QWidget *parent = nullptr, QString basepath="");

signals:
    void toImportData(QString);
    void toChangeBasepath(QString);

public slots:
    void onBaseFolderDlg();
    void onImportFolderDlg();
    void onImportButton();
};

#endif // JSETTINGSDLG_H
