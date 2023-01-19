#ifndef QMedBrowserSettingsDialog_h
#define QMedBrowserSettingsDialog_h

#include <QDialog>
#include <QSettings>
#include <QLineEdit>
#include <QLabel>

/**
 * @class QMedBrowserSettingsDialog
 * @author Nicolas Toussaint
 * @date 16/07/18
 * @brief Dialog to modify the settings of the application
 * @details
 * 
 * 
 * @see QSettings 
 * 
 */
class QMedBrowserSettingsDialog : public QDialog
{
  Q_OBJECT

public:
  explicit QMedBrowserSettingsDialog(QWidget *parent = 0);

public slots:
  void accept();

protected:

  void SetupLayout();

private:

  QLineEdit* FileFiltersEdit;
  QLabel* LegendFileLabel;
  QList<QLineEdit*> LabelValueEditList;
};


#endif
