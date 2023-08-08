#include <QVBoxLayout>
#include <QToolButton>
#include <QLabel>
#include <QTabBar>
#include <QDateTime>
#include "global.h"
#include "jtabwidget.h"
#include "jtextedit.h"
#include "jtoolbutton.h"

JTabWidget::JTabWidget(QString path, QWidget *parent) : QTabWidget(parent)
{
    qDebug() << "JTabWidget(): " << path;
    m_path = path;

    gui();
    loadFiles();

    connect(parent, SIGNAL(toFontResize(int)), this, SLOT(onFontResize(int)));
}

void JTabWidget::loadFiles()
{
    if (!m_path.isEmpty()) {
        QDir dir(m_path);
        foreach (QFileInfo fi, dir.entryInfoList(QDir::NoFilter, QDir::Time|QDir::Reversed)) {
            if (fi.isFile())
                addTabData(fi.fileName(), readFile(fi.absoluteFilePath()));
        }
    }
}

void JTabWidget::gui()
{
    // add button
    QToolButton *tbAdd = new QToolButton();
    tbAdd->setText("+");
    addTab(new QLabel("Please Add a new tab by clicking +"), QString());
    setTabEnabled(0, false);
    tabBar()->setTabButton(0, QTabBar::RightSide, tbAdd);

    // SIGNAL
    connect(tbAdd, SIGNAL(clicked()), this, SLOT(onAddNewTab()));
}

void JTabWidget::onAddNewTab()
{
    QString title =  QDateTime::currentDateTime().toString(DATE_LONG2);
    addTabData(title, "");
}

void JTabWidget::addTabData(QString title, QString data)
{
    JTextEdit *edit = new JTextEdit(title, data, this);
    int index = addTab(edit, title);

    JToolButton *tbRemove= new JToolButton(edit, this);
    tbRemove->setText("x");

    tabBar()->setTabButton(index, QTabBar::RightSide, tbRemove);
    setCurrentIndex(index);
    edit->setFocus();

    connect(tbRemove, SIGNAL(toClicked(QWidget *)), this, SLOT(onRemoveTab(QWidget *)));
    connect(edit, SIGNAL(toDropFilePath(QString)), this, SLOT(onSetTabTitle(QString)));
    connect(edit, SIGNAL(toChangedData(QString, QString)), this, SLOT(onChangedData(QString, QString)));
}

void JTabWidget::onChangedData(QString filename, QString data)
{
    writeFile(m_path + SP + filename, data.toStdString().c_str());
}

void JTabWidget::onRemoveTab(QWidget *widget)
{
    int index = indexOf(widget);
    if (removeFile(m_path + SP + tabText(index)))
        removeTab(index);
}

void JTabWidget::onFontResize(int fontsize)
{
    setStyleSheet(FONT_STYLE_LIST(fontsize));
    emit toFontResize(fontsize);
}

void JTabWidget::onSetTabTitle(QString path)
{
    QString currentPath = m_path + SP + tabText(currentIndex());
    QFileInfo fi(path);
    if (QFile::rename(currentPath, m_path + SP + fi.fileName()))
        setTabText(currentIndex(), fi.fileName());
    else
        message(ERROR, "Tab title change", "Tab name cannot be chnaged as the same files are already exist!");

}

/*
void JTapTextWidget::addTab(QString title, QString data)
{
    JTextEdit *text = new JTextEdit(data);
 //   text->onFontResize(m_fontsize);
    int newIndex = m_tabs->addTab(text, title);
    m_tabs->setCurrentIndex(newIndex);
    text->setFocus();

  connect(m_find, SIGNAL(textChanged(QString)), text, SLOT(onFindText(QString)));
    connect(this, SIGNAL(toFontResize(int)), text, SLOT(onFontResize(int)));
    connect(text, SIGNAL(toDropFileName(QString)), this, SLOT(onTapTextChange(QString)));

}
*/
