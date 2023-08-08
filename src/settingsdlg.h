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
    JListWidget *m_lvCategory;
    void gui();

public:
    explicit SettingsDlg(QWidget *parent = nullptr, QString basepath="");

signals:
    void toChangeBasepath(QString);

public slots:
    void onBaseFolderDlg();
    void onCategoryAdd();
    void onCategoryRemove();
    void onCategoryChanged(QListWidgetItem*);

};

#endif // SETTINGSDLG_H
