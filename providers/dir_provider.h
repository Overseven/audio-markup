#ifndef DIR_PROVIDER_H
#define DIR_PROVIDER_H

#include <QObject>
#include <QString>

class DirProvider : public QObject
{
    Q_OBJECT
public:
    DirProvider(QObject *parent = nullptr);
    void set_dir(const QString &_dir);
    std::optional<QString> get_dir() const noexcept;

signals:
    void dir_changed();

private:
    std::optional<QString> dir;
};

#endif // DIR_PROVIDER_H
