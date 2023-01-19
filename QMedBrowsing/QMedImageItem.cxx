#include "QMedImageItem.h"

#include <QFileInfo>
#include <QtWidgets>

#ifdef use_itk
#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkMetaDataDictionary.h>
#include <itkMetaDataObject.h>
#endif

QMedImageItem::QMedImageItem(const QString &path, const QString &text, QListWidget * parent, const QSize& size, int type) :
  QListWidgetItem(parent, type)
{
  this->setText(text);
  this->Size = size;
  this->Path = path;

  this->LabelKey = QObject::tr("Labels");

  QFileInfo finfo(path);
  if (finfo.exists())
    this->loadImage(path);
}

QMedImageItem::~QMedImageItem()
{
  ///@todo figure out why we have a leak here...
}

QImage QMedImageItem::readImage(const QString& path)
{
  QImageReader reader(path);
  reader.setAutoTransform(true);
  if (reader.canRead())
  {
      QImage ret;
      reader.read(&ret);
      
      return ret;
  }
  else
  {
#ifdef use_itk
    typedef itk::Image<unsigned int, 3> ImageType;
    itk::ImageFileReader<ImageType>::Pointer r = itk::ImageFileReader<ImageType>::New();
    r->SetFileName(path.toStdString().c_str());

    try
    {
      r->Update();
    }
    catch (itk::ExceptionObject& e)
    {
      qWarning() << QObject::tr("Cannot read file: %1").arg(path);
      return QImage();
    }

    ImageType::Pointer image = r->GetOutput();
    int width = image->GetLargestPossibleRegion().GetSize()[0];
    int height = image->GetLargestPossibleRegion().GetSize()[1];
    int depth = image->GetLargestPossibleRegion().GetSize()[2];
    int midslice = (int)((float)(depth) / 2.0);

    QImage qimage(width, height, QImage::Format_RGB32);
    QRgb* rgbPtr = reinterpret_cast<QRgb*>(qimage.bits()) + width * (height - 1);
    unsigned int* data_ptr = reinterpret_cast<unsigned int*>(image->GetBufferPointer());
    data_ptr += (width * height * midslice);

    for (int row = 0; row < height; row++)
    {
      for (int col = 0; col < width; col++)
      {
        *(rgbPtr++) = QColor(data_ptr[0], data_ptr[0], data_ptr[0]).rgb();
          data_ptr += image->GetNumberOfComponentsPerPixel();
      }
      rgbPtr -= width * 2;
    }

    itk::MetaDataDictionary dict = r->GetMetaDataDictionary();
    if (dict.HasKey(this->LabelKey.toStdString()))
    {
      std::string v;
      itk::ExposeMetaData<std::string>(dict, this->LabelKey.toStdString(), v);
      this->LabelValues = QString::fromStdString(v).split(",");
    }

    return qimage.mirrored();
#else
    return QImage();
#endif
  }
}

void QMedImageItem::loadImage(const QString& path)
{
  this->Image = readImage(path);

  float aspectratio = (float)(this->Image.height()) / (float)(this->Image.width());
  unsigned int px=0, py=0, sx= this->Image.width(), sy= this->Image.height();
  if (aspectratio > 1)
  {
    py = (unsigned int)((float)(sy-sx)/2.0);
    sy = sx;
  }
  else
  {
    px = (unsigned int)((float)(sx-sy)/2.0);
    sx = sy;
  }
  QRect rect(px, py, sx, sy);
  QImage selection = this->Image.copy(rect).scaled(this->Size, Qt::IgnoreAspectRatio);
  QPixmap pixmap = QPixmap::fromImage(selection);

  // getting size if the original picture is not square
  int size = qMax(pixmap.width(), pixmap.height());
  // creating a new transparent pixmap with equal sides
  this->Pixmap = QPixmap(size, size);
  this->Pixmap.fill(Qt::transparent);
  // creating circle clip area
  QPainterPath qpath;
  qpath.addRoundedRect(this->Pixmap.rect(), 10, 10);
  QPainter painter(&this->Pixmap);
  painter.setRenderHint(QPainter::Antialiasing);
  painter.setClipPath(qpath);
  // filling rounded area if needed
  painter.fillRect(this->Pixmap.rect(), Qt::black);
  // getting offsets if the original picture is not square
  painter.drawPixmap(0,0, pixmap.width(), pixmap.height(), pixmap);

  const QIcon icon(this->Pixmap);
  this->setIcon(icon);

}


void QMedImageItem::addOverlays(QList<QPixmap> overlays)
{

  QPixmap overlayed(this->Pixmap.width(), this->Pixmap.height());
  overlayed.fill(Qt::transparent); // force alpha channel
  QPainter painter(&overlayed);
  painter.setCompositionMode (QPainter:: CompositionMode_SourceOver);
  painter.drawPixmap(0, 0, this->Pixmap);

  for (int i=0; i<overlays.count(); i++)
  {
    painter.drawPixmap(this->Pixmap.width()-50, 10+i*50, overlays.at(i));
  }
  const QIcon icon(overlayed);

  this->setIcon(icon);
}



void QMedImageItem::write(void)
{
#ifdef use_itk
  typedef itk::Image<unsigned int, 3> ImageType;
  itk::ImageFileReader<ImageType>::Pointer r = itk::ImageFileReader<ImageType>::New();
  r->SetFileName(this->Path.toStdString());
  try
  {
    r->Update();
  }
  catch (itk::ExceptionObject &e)
  {
    qWarning() << QObject::tr("Cannot read file: %1").arg(this->Path);
    return;
  }

  itk::MetaDataDictionary dict = r->GetOutput()->GetMetaDataDictionary();

  if (!this->LabelValues.count())
    dict.Erase(this->LabelKey.toStdString());
  else
  {
    std::string v = this->LabelValues.join(",").toStdString();
    itk::EncapsulateMetaData<std::string>(dict, this->LabelKey.toStdString(), v);  
  }
  r->GetOutput()->SetMetaDataDictionary(dict);

  itk::ImageFileWriter<ImageType>::Pointer w = itk::ImageFileWriter<ImageType>::New();
  w->SetInput(r->GetOutput());
  w->SetFileName(this->Path.toStdString());

  try
  {
    w->Update();
  }
  catch (itk::ExceptionObject &e)
  {
    qWarning() << QObject::tr("Cannot write file: %1").arg(this->Path);
    return;
  }
#endif
}
