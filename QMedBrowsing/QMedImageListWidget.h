#ifndef QMedImageListWidget_h_
#define QMedImageListWidget_h_

#include <QListWidget>

class QMenu;

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

signals:

  void Modified(const QString &path);

protected:

  void UpdateMenu(void);
  virtual void keyPressEvent(QKeyEvent * event);
  void ToggleLabelValue(unsigned int idx);
private:

  QMenu* ContextMenu;
  QStringList LabelValues;
  QList<QPixmap> Tags;
};

#endif // QMedImageListWidget_h_
