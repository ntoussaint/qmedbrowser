#include "QMedImageListWidget.h"
#include "QMedImageItem.h"
#include "QMetaDataWidget.h"

#include <QtWidgets>
#include <iostream>
#include <QMenu>
#include <QWheelEvent>
#include <QApplication>
#include <QScrollBar>
#include <QTimer>
#include <QDebug>
#include <QDateTime>
#include <QQueue>
#include <qmath.h>




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
  this->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
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

  /// smooth scrolling...
  lastWheelEvent = 0;
  smoothMoveTimer = new QTimer(this);
  connect(smoothMoveTimer, SIGNAL(timeout()), this, SLOT(slotSmoothMove()));

  m_fps = 60;
  m_duration = 400;
  m_acceleration = 2.5;

  m_smallStepModifier = Qt::SHIFT;
  m_smallStepRatio = 1.0 / 5.0;
  m_bigStepModifier = Qt::ALT;
  m_bigStepRatio = 5.0;
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


/// smooth scrolling...

void QMedImageListWidget::wheelEvent(QWheelEvent *e)
{
    // According to my experiment, a normal person is able to scroll his wheel
    // at the speed about 36 times per second in average.  Here we use a
    // conservative value 30: a user can achieve the maximum acceration when he
    // scrools his wheel at 30 times / second.
    static QQueue<qint64> scrollStamps;
    qint64 now = QDateTime::currentDateTime().toMSecsSinceEpoch();
    scrollStamps.enqueue(now);
    while (now - scrollStamps.front() > 500)
        scrollStamps.dequeue();
    double accerationRatio = qMin(scrollStamps.size() / 15.0, 1.0);

    if (!lastWheelEvent)
        lastWheelEvent = new QWheelEvent(*e);
    else
        *lastWheelEvent = *e;

    stepsTotal = m_fps * m_duration / 1000;
    double multiplier = 1.0 / 4.0;
    if (QApplication::keyboardModifiers() & m_smallStepModifier)
        multiplier *= m_smallStepRatio;
    if (QApplication::keyboardModifiers() & m_bigStepModifier)
        multiplier *= m_bigStepRatio;
    double delta = e->delta() * multiplier;
    if (m_acceleration > 0)
        delta += delta * m_acceleration * accerationRatio;

    stepsLeftQueue.push_back(qMakePair(delta, stepsTotal));
    smoothMoveTimer->start(1000 / m_fps);
}

void QMedImageListWidget::slotSmoothMove()
{
    double totalDelta = 0;

    for (QList< QPair<double, int> >::Iterator it = stepsLeftQueue.begin();
         it != stepsLeftQueue.end(); ++it)
    {
        totalDelta += subDelta(it->first, it->second);
        --(it->second);
    }
    while (!stepsLeftQueue.empty() && stepsLeftQueue.begin()->second == 0)
        stepsLeftQueue.pop_front();

    Qt::Orientation orientation = lastWheelEvent->orientation();
    // By default, when you press ALT, QT will scroll horizontally.  But if we
    // have defined the use of ALT key, we ignore this setting since horizontal
    // scroll is not so useful in okular
    if ((m_bigStepModifier & Qt::ALT) || (m_smallStepModifier & Qt::ALT))
        orientation = Qt::Vertical;

    QWheelEvent e(
                lastWheelEvent->pos(),
                lastWheelEvent->globalPos(),
                qRound(totalDelta),
                lastWheelEvent->buttons(),
                0,
                orientation
    );
    if (e.orientation() == Qt::Horizontal)
        QApplication::sendEvent(horizontalScrollBar(), &e);
    else
        QApplication::sendEvent(verticalScrollBar(), &e);

    if (stepsLeftQueue.empty()) {
        smoothMoveTimer->stop();
    }
}

double QMedImageListWidget::subDelta(double delta, int stepsLeft)
{
    double m = stepsTotal / 2.0;
    double x = abs(stepsTotal - stepsLeft - m);

    // some mathmatical integral result.
    return (cos(x * M_PI / m) + 1.0) / (2.0*m) * delta;
}
