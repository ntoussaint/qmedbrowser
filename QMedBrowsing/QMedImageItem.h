#ifndef QMedImageItem_h_
#define QMedImageItem_h_

#include <QListWidgetItem>
#include <QPixmap>
#include <QImage>


/**
 * @class QMedImageItem
 * @author Nicolas Toussaint
 * @brief QWidget to aid the selection of an image
 * 
 * @details
 * 
 * 
 */
class QMedImageItem : public QListWidgetItem
{

public:
  // Constructor/Destructor
  QMedImageItem(const QString &path, const QString &text, QListWidget * parent = 0, const QSize& size = QSize(100, 100), int type = QListWidgetItem::Type);
  ~QMedImageItem();

  const QImage& image(void) const 
  { return this->Image; }
  QString path(void) const
  { return this->Path; }

  void setLabelKey(QString arg)
  { this->LabelKey = arg; }
  QString labelKey(void) const
  { return this->LabelKey; }
  void setLabelValues(QStringList arg)
  { this->LabelValues = arg; }
  QStringList labelValues(void) const
  { return this->LabelValues; }

  void write(void);
  void addOverlays(QList<QPixmap> overlays);


protected:

  void loadImage(const QString& path);
  QImage readImage(const QString& path);

private:
  QImage Image;
  QString Path;
  QSize   Size;
  QPixmap Pixmap;
  QString LabelKey;
  QStringList LabelValues;
};

#endif // QMedImageItem_h_
