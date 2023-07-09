#include "utils.h"

namespace Utils {

void recursive_scan_dir(QDir dir, QStringList filters, QStringList &output)
{
    dir.setNameFilters(filters);
    dir.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks);

    qDebug() << "Scanning: " << dir.path();

    QStringList filenames = dir.entryList();
    for (const auto &filename : filenames) {
        output << dir.absoluteFilePath(filename);
    }

    dir.setFilter(QDir::AllDirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);
    QStringList dirs = dir.entryList();
    for (const auto &dir_internal : dirs)
    {
        QString newPath = dir.absolutePath() + "/" + dir_internal;
        recursive_scan_dir(QDir(newPath), filters, output);
    }
}

}
