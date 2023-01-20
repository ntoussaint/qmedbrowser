#pragma once

#include <QThread>
#include <QThread>
#include <QSize>

class QMedImageItem;

class QMedImageReaderThread : public QThread
{
Q_OBJECT
public:
    QMedImageReaderThread(const QString& filepath, const QSize& thumbnailsize);
    ~QMedImageReaderThread(void) {};

    void run() override;
signals:
    void itemReady(QMedImageItem* item);

private:
    QString filePath;
    QSize thumbnailSize;

};
