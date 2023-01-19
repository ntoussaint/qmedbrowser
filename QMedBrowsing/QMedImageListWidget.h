#ifndef QMedImageListWidget_h_
#define QMedImageListWidget_h_

#include <QList>
#include <QPair>
#include <Qt>
#include <QListWidget>

class QMenu;
class QDialog;
class QLabel;

/**
 * @class QMedImageListWidget
 * @author Nicolas Toussaint
 * @brief QWidget to aid the selection of an image
 * 
 * @details
 * 
 * 
 */
class QMedImageListWidget : public QListWidget
{
  Q_OBJECT

public:
  // Constructor/Destructor
  QMedImageListWidget(const QSize& size = QSize(100,100), QWidget *parent = 0);
  ~QMedImageListWidget(){};

  void setLabelValues(QStringList arg)
  { this->LabelValues = arg; }
  QStringList labelValues(void) const
  { return this->LabelValues; } 
  QList<QPixmap> LabelsToPixmaps(QStringList labelvalues);

public slots:

  void ShowContextMenu(const QPoint& pos);

  void CopySelected();
  void DeleteSelected();
  void ShowMetaData();

  void HandleItemChanged(QListWidgetItem* current, QListWidgetItem* previous);

signals:

  void Modified(const QString &path);

protected:

  void UpdateMenu(void);
  virtual void keyPressEvent(QKeyEvent * event);
  void ToggleLabelValue(unsigned int idx);
  /**
   * Double click event
   */
  void mouseDoubleClickEvent(QMouseEvent* event) override;

private:

  QMenu* ContextMenu;
  QStringList LabelValues;
  QList<QPixmap> Tags;

/// smooth scrolling...

protected:
  virtual void wheelEvent(QWheelEvent *event);

public slots:
  void slotSmoothMove();

private:
  double subDelta(double delta, int stepsLeft);

  QTimer *smoothMoveTimer;
  QWheelEvent *lastWheelEvent;

  int m_fps;
  int m_duration;

  double m_acceleration;
  double m_smallStepRatio;
  double m_bigStepRatio;
  Qt::Modifier m_smallStepModifier;
  Qt::Modifier m_bigStepModifier;

  int stepsTotal;
  QList< QPair<double, int> > stepsLeftQueue;

  QDialog* FullResolutionDialog;
  QLabel* FullResolutionLabel;
};

#endif // QMedImageListWidget_h_
