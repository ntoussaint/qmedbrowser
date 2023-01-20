#ifndef QMedBrowserWindow_h_
#define QMedBrowserWindow_h_

#include <QMainWindow>
#include <QSettings>
#include <QProgressBar>
#include <QComboBox>
#include <QLabel>

#include "QMedImageListWidget.h"
#include "QMedBrowserSettingsDialog.h"

class QMedImageItem;

/**
 * @class QMedBrowserWindow
 * @author Nicolas Toussaint
 * @brief QMainWindow for the handling of medical image browsing
 * @details
 * 
 * 
 *
 * 
 * 
 * @see QMainWindow
 * 
 * 
 */
class QMedBrowserWindow : public QMainWindow
{ 
  Q_OBJECT

public:
  
  // Constructor/Destructor
  QMedBrowserWindow(QWidget *parent = 0);
  ~QMedBrowserWindow() {}  

  void emitImageRead(void) const;

public slots:


  void OpenFolderPressed(void);
  void SaveFolderPressed(void);
  void LoadFilePressed(void);
  void SaveAsFilePressed(void);
  void OpenHelpPressed(void);
  void OpenPreferencesPressed(void);

  void ApplySettings(void);

  void SetItemFilter(const QString& regexp);

  void SetThumbnailSize(int sizeid);

  void handleItem(QMedImageItem* item);

signals:
  void imageRead(int list_count) const;

protected:

  /**
   * @brief protected method to setup the layout
   * 
   */
  void SetupLayout(void);
  /**
   * @brief Overwritten close event to prompt for confirmation
   * @param event
   */
  void closeEvent(QCloseEvent * event);  

  /**
   * Reads an image and emits the signal: imageRead
   */
  QMedImageItem* readImage(const QString& filePath, QSize thumbnailsize) const;

private:

  QProgressBar* ProgressBar;
  QComboBox* LabelComboBox;
  QComboBox* ThumbnailSizeComboBox;
  QMedImageListWidget* ListWidget;
  QDialog* LegendDialog;
  QMedBrowserSettingsDialog* SettingsDialog;
  QLabel* HelpLabel;
};

#endif // QMetaDataWidget_h_
