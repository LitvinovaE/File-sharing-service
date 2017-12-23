#ifndef FILEMODEL_H
#define FILEMODEL_H

#include <QObject>
#include <QVector>

class FileModel : public QObject
{
    Q_OBJECT
public:
    using DataType = QVector<QVector<QString>>;
    explicit FileModel(QObject *parent = nullptr);

    void clear(void);
    void parseString(const QString &files);
    const DataType &getData(void) const;


signals:

public slots:

private:
    DataType m_data;
};

#endif // FILEMODEL_H
