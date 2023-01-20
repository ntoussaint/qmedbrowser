#include "QMedImageReaderThread.h"

#include "QMedImageItem.h"

QMedImageReaderThread::QMedImageReaderThread(const QString& filepath, const QSize& thumbnailsize)
	: filePath(filepath), thumbnailSize(thumbnailsize)
{
	qRegisterMetaType<QMedImageItem*>("QMedImageItem");

}

void QMedImageReaderThread::run(void)
{
	QMedImageItem* item = new QMedImageItem(this->filePath, "", nullptr, this->thumbnailSize);
	emit itemReady(item);
}
