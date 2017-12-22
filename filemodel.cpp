#include "filemodel.h"

FileModel::FileModel(QObject *parent) : QObject(parent)
{

}

const QVector<QVector<QString>> &FileModel::getData(void) const
{
    return m_data;
}

void FileModel::clear()
{
    for (auto i: m_data) {
        QVector().swap(j);
    }
    QVector().swap(m_data);
}

void FileModel::parseString(const QString &files)
{
    clear();
    for (auto user_files: files.splitRef(';')) {
        // user_files is id:f1!s1!d1:f2!s2!d2...
        QString id = user_files.takeFirst();
        for (auto file: user_files.split(':')) {
            // file is f1!s1!d1
            auto params = file.split('!');
            params.append(id);
            m_data.append(QVector::fromList(params));
        }
    }
}


