#include "QMedImageListWidget.h"
#include "QMedImageItem.h"
#include "QMetaDataWidget.h"

#include <QtWidgets>
#include <iostream>
#include <QMenu>



QMedImageListWidget::QMedImageListWidget(const QSize& size, QWidget *parent) :
  QListWidget(parent)
{
  this->setViewMode(QListWidget::IconMode);
  this->setSelectionMode(QAbstractItemView::ExtendedSelection);
  this->setIconSize(size);
  this->setResizeMode(QListWidget::Adjust);
  this->setFlow(QListWidget::LeftToRight);
  this->setMovement(QListView::Static);
  this->setTabKeyNavigation(true);
  this->ContextMenu = new QMenu();
  this->UpdateMenu();
  this->setContextMenuPolicy(Qt::CustomContextMenu);
  QObject::connect(this, SIGNAL(customContextMenuRequested(const QPoint&)),
                   this, SLOT(ShowContextMenu(const QPoint&)));
                  
  for (unsigned int idx = 0; idx < 7; idx++)
  {
    this->LabelValues << QObject::tr("Label%1").arg(idx);
    this->Tags << QIcon(QObject::tr(":/label%1.png").arg(idx)).pixmap(45,45);
  }
}



void QMedImageListWidget::ShowContextMenu(const QPoint& pos)
{
  QPoint globalPos = this->viewport()->mapToGlobal(pos);

  QMenu menu;
  menu.addAction(QIcon::fromTheme("edit-copy"), "&Copy file link");

  this->ContextMenu->exec(globalPos);
}


void QMedImageListWidget::UpdateMenu(void)
{
  this->ContextMenu->clear();

  QAction* remove = new QAction(tr(" &Delete"), this);
  remove->setIcon(QIcon::fromTheme("edit-clear"));
  QObject::connect(remove, SIGNAL(triggered()), this, SLOT(DeleteSelected()));
  this->ContextMenu->addAction(remove);

  QAction* metadata = new QAction(tr(" &MetaData"), this);
  metadata->setIcon(QIcon::fromTheme("dialog-information"));
  QObject::connect(metadata, SIGNAL(triggered()), this, SLOT(ShowMetaData()));
  this->ContextMenu->addAction(metadata);
}


void QMedImageListWidget::CopySelected()
{
  QString text;
  foreach(QListWidgetItem* item, this->selectedItems())
  {
    QMedImageItem* meditem = reinterpret_cast<QMedImageItem*> (item);
    if (meditem)
      text.append(tr("%1 ").arg(meditem->path()));
  }

  QApplication::clipboard()->setText(text.simplified());
}


void QMedImageListWidget::DeleteSelected()
{
  foreach(QListWidgetItem* item, this->selectedItems())
  {
    QMedImageItem* meditem = reinterpret_cast<QMedImageItem*> (item);
    if (meditem)
    {
      QString path = meditem->path();
      QFile::remove(path);
      path.chop(4);
      QFile::remove(tr("%1.raw").arg(path));
    }
    delete item;
  }
}


void QMedImageListWidget::ShowMetaData(void)
{
  QList<QListWidgetItem*> items = this->selectedItems();
  if (!items.length())
    return;

  QMedImageItem* view = reinterpret_cast<QMedImageItem*> (items.at(0));
  if (!view)
    return;
  QMetaDataWidget* w = new QMetaDataWidget(view->path());
  w->show();
}



void QMedImageListWidget::keyPressEvent(QKeyEvent * event)
{

  if (   (event->key() == Qt::Key_Right)
      || (event->key() == Qt::Key_Left) 
      || (event->key() == Qt::Key_Up)
      || (event->key() == Qt::Key_Down)
      || (event->key() == Qt::Key_Delete))
  {
    int previous = this->currentRow();
    QListWidget::keyPressEvent(event);
    int current = this->currentRow();

    QItemSelectionModel::SelectionFlags flags = QItemSelectionModel::Clear | QItemSelectionModel::Current | QItemSelectionModel::Select;

    if(event->modifiers() & Qt::ShiftModifier)
      flags = QItemSelectionModel::Current | QItemSelectionModel::Select;
    if(event->modifiers() & Qt::ControlModifier)
      flags = QItemSelectionModel::Current;

    switch (event->key())
    {
      case Qt::Key_Delete:
        this->DeleteSelected();
        break;
      case Qt::Key_Right:
        if ( (current == previous) && (current < this->count() - 1) )
        {
          if( (event->modifiers() & Qt::ShiftModifier) && !(event->modifiers() & Qt::ControlModifier) )
            this->setCurrentRow(current, QItemSelectionModel::Select);
          this->setCurrentRow(current+1, flags);
        }
        break;
      case Qt::Key_Left:
        if ( (current == previous) && (current > 0) )
        {
          if( (event->modifiers() & Qt::ShiftModifier) && !(event->modifiers() & Qt::ControlModifier) )
            this->setCurrentRow(current, QItemSelectionModel::Select);
          this->setCurrentRow(current-1, flags);
        }
        break;
    }
  }
  else
  {
    switch (event->key())
    {
      case Qt::Key_0:
        this->ToggleLabelValue(0);
        break;
      case Qt::Key_1:
        this->ToggleLabelValue(1);
        break;
      case Qt::Key_2:
        this->ToggleLabelValue(2);
        break;
      case Qt::Key_3:
        this->ToggleLabelValue(3);
        break;
      case Qt::Key_4:
        this->ToggleLabelValue(4);
        break;
      case Qt::Key_5:
        this->ToggleLabelValue(5);
        break;
      case Qt::Key_6:
        this->ToggleLabelValue(6);
        break;
      default:
        break;
    }
  }
}

void QMedImageListWidget::ToggleLabelValue(unsigned int idx)
{
  if (this->LabelValues.count() < idx)
    return;
  QString totoggle = this->LabelValues.at(idx);
  foreach(QListWidgetItem* item, this->selectedItems())
  {
    QMedImageItem* meditem = reinterpret_cast<QMedImageItem*> (item);
    if (!meditem)
      continue;
    QStringList values = meditem->labelValues();
    if (values.contains(totoggle))
      values.removeAll(totoggle);
    else
      values.append(totoggle);
    meditem->setLabelValues(values);
    meditem->addOverlays(this->LabelsToPixmaps(values));
  }  
}

QList<QPixmap> QMedImageListWidget::LabelsToPixmaps(QStringList labelvalues)
{
  QList<QPixmap> ret;
  foreach (QString v, labelvalues)
  {
    int idx = this->LabelValues.indexOf(v);
    if (idx < 0)
      continue;
    QPixmap label = this->Tags.at(idx);
    ret << label;
  }
  return ret;
}
