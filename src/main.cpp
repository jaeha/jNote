#include "jnote.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QCoreApplication::setApplicationName(APP_TITLE);
    QCoreApplication::setOrganizationName(COMPANY);
    QCoreApplication::setApplicationVersion(APP_VERSION);

    JNote w;
    QStringList args = app.arguments();
    QString path = BASE_PATH;
    if (args.count() >= 2) {
        path = args[1];
    }

    w.doOpen(path);
    w.show();

    return app.exec();
}
