//
// Created 24.12.16.
//


#include "kstandarddirs.h"

QStringList KStandardDirs::findAllResources(const QString& type,
                             const QString& cfilter,
                             bool, bool) {
    cout << "Searching for: " << type.toLocal8Bit().data() << "/" << cfilter.toLocal8Bit().data() << endl;
    QStringList ret;
    if(type == "module") {
        findModules("dll/interpreter", cfilter, ret);
        findModules("dll/plugins", cfilter, ret);
        findModules("dll/interpreter", "lib"+cfilter, ret);
        findModules("dll/plugins", "lib"+cfilter, ret);
    } else if(type == "icon") {
        QDir d(RESOURCES_PATH_PREFIX + "icons");
        d.setFilter( QDir::Files | QDir::Readable );

        QString filter = cfilter;
        const QFileInfoList fList = d.entryInfoList(QStringList() << filter);

        if(fList.isEmpty()) return ret;

        QListIterator<QFileInfo> it (fList);
        QFileInfo fi;

        while ( it.hasNext() ) {
            fi = it.next();
            QString name = fi.filePath();
            cout << "found icon: " << name.toLocal8Bit().data() << endl;
            ret.append(name);
        }
    }
    return ret;
}

void KStandardDirs::findModules(const QString& dir, const QString &cfilter,
                 QStringList& ret) {

    QDir d(DLL_PATH_PREFIX + dir);

    d.setFilter( QDir::Files | QDir::Readable );

    QString filter = cfilter;
    bool isDotLa = false;
    if(filter.indexOf(".la") != -1) {
#ifdef Q_OS_WIN
        filter.replace(QRegExp("\\.la"), ".dll");
#else
#ifdef Q_OS_MACOS
        filter.replace(QRegExp("\\.la"), ".dylib");
#else
        filter.replace(QRegExp("\\.la"), ".so");
#endif
#endif
        isDotLa = true;
    }
    cout << "Final Filter: " << filter.toLocal8Bit().data() << endl;
    const QFileInfoList fList = d.entryInfoList(QStringList() << filter);

    if(fList.isEmpty()) return;

    QListIterator<QFileInfo> it (fList);
    QFileInfo fi;

    while (it.hasNext()) {
        fi = it.next();
        QString name = fi.filePath();
        if(isDotLa)
#ifdef Q_OS_WIN
            name.replace(QRegExp("\\.dll"), ".la");
#else
#ifdef Q_OS_MACOS
            name.replace(QRegExp("\\.dylib"), ".la");
#else
        name.replace(QRegExp("\\.so"), ".la");
#endif
#endif
        cout << "module found: " << name.toLocal8Bit().data() << endl;
        ret.append(name);
    }
}