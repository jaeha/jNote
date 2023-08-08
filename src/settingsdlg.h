#ifndef SETTINGSDLG_H
#define SETTINGSDLG_H

#include <QObject>
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include "jlistwidget.h"

class SettingsDlg : public QDialog
{
    Q_OBJECT

private:
    QLabel *m_lbBasePath;
    QLabel *m_lbImportPath;
    void gui();
    void folderDlg(QString title, QLabel *label);

public:
    explicit SettingsDlg(QString basepath="", QWidget *parent = nullptr);

signals:
    void toChangeBasepath(QString);

public slots:
    void onBaseChange();
    void onImport();
    void onBaseFolderDlg();
    void onImportFolderDlg();

};

#endif // SETTINGSDLG_H
