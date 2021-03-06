#include "filemodel.h"

FileModel::FileModel(QObject *parent)
    : QObject(parent)
    , m_data()
{

}

const FileModel::DataType &FileModel::getData(void) const
{
    return m_data;
}

void FileModel::clear()
{
//    for (auto i: m_data) {
//        QVector().swap(j);
//    }
    DataType().swap(m_data);
}

void FileModel::parseString(const QString &files)
{
    clear();
    for (auto user_files_string: files.split(';')) {
        // user_files is id:f1!s1!d1:f2!s2!d2...
        auto user_files_list = user_files_string.split(':');
        QString id = user_files_list.takeFirst();
        for (auto file: user_files_list) {
            // file is f1!s1!d1
            auto params = file.split('!');
            params.append(id);
            m_data.append(QVector<QString>::fromList(params));
        }
    }
}
