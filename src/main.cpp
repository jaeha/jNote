#include "jnote.h"
#include <QApplication>
#include <QDir>
#include "jdb.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QCoreApplication::setApplicationName(APP_TITLE);
    QCoreApplication::setOrganizationName(COMPANY);
    QCoreApplication::setApplicationVersion(APP_VERSION);

    JNote w;
    JDB db;
 /*   QStringList args = app.arguments();
    QString path = BASE_PATH;
    if (args.count() >= 2) {
        path = args[1];
    }
*/
    // create base path & db if it's not already exist
    qDebug() <<"main()" <<BASE_PATH;
    if (!QDir(BASE_PATH).exists()) {
            message(INFO, "DB", BASE_PATH + " will be created.");
            QDir().mkdir(BASE_PATH);
            QDir().mkdir(BASE_PATH + ATTACH_DIR);
            if (!db.createDB(BASE_PATH + DB_FILE)) {
                message(ERROR, "DB", "Failed to create database!");
                return 1;
            }
    }

    w.open();
    w.show();
    return app.exec();
}
