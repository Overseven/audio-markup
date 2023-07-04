#include "dir_provider.h"

DirProvider::DirProvider(QObject *parent) : QObject(parent)
{

}

void DirProvider::set_dir(const QString &_dir)
{
    dir = _dir;
    emit dir_changed();
}

std::optional<QString> DirProvider::get_dir() const noexcept
{
    return dir;
}
