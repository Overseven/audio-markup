#ifndef COMMON_UTILS_H
#define COMMON_UTILS_H

#include <QDir>

namespace Utils {

void recursive_scan_dir(QDir dir, QStringList filters, QStringList &output);

QByteArray hash(const QString &str);

}
#endif // COMMON_UTILS_H
