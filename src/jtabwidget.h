#ifndef JTABWIDGET_H
#define JTABWIDGET_H

#include <QObject>
#include <QWidget>
#include <QTabWidget>
#include <QToolButton>

class JTabWidget : public QTabWidget
{
    Q_OBJECT

private:
    QString m_path;
    void gui();
    void loadFiles();
    void addTabData(QString title, QString data);

public:
    explicit JTabWidget(QString path = "", QWidget *parent = nullptr);

signals:
    void toFontResize(int);

public slots:
    void onAddNewTab();
    void onChangedData(QString, QString);
    void onRemoveTab(QWidget *);
    void onFontResize(int);
    void onSetTabTitle(QString);
};

#endif // JTABWIDGET_H
