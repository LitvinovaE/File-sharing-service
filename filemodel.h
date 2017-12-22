#ifndef FILEMODEL_H
#define FILEMODEL_H

#include <QObject>
#include <QVector>

class FileModel : public QObject
{
    Q_OBJECT
public:
    explicit FileModel(QObject *parent = nullptr);

    void clear(void);
    void parseString(const QString &files);
    const QVector<QVector<QString>> &getData(void) const;


signals:

public slots:

private:
    QVector<QVector<QString>> m_data;
};

#endif // FILEMODEL_H
