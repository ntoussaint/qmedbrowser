#ifndef QMetaDataWidget_h_
#define QMetaDataWidget_h_

#include <QWidget>

class QTableView;
class QStandardItemModel;
class QSortFilterProxyModel;

/**
 * @class QMetaDataWidget
 * @author Nicolas Toussaint
 * @brief QWidget for the display of metadata of a dnl::Image
 * @details
 * 
 * 
 * This class is dedicated to the (continuous) display of a dnl::Image's metadata.
 * 
 * 
 * @see dnl::Image dnlImageHandler dnlImageWidget
 * 
 * 
 */
class QMetaDataWidget : public QWidget
{ 
  Q_OBJECT

public:
  
  // Constructor/Destructor
  QMetaDataWidget(const QString& path, QWidget *parent = 0);
  ~QMetaDataWidget() {}  
  

protected:

  /**
   * @brief protected method to setup the layout
   * 
   */
  void SetupLayout(void);
  void SetupTable(void);

  void showEvent(QShowEvent * event);


private:

  QTableView* Table;
  QStandardItemModel* Source;
  QSortFilterProxyModel* Filter;
  QString Path;
};

#endif // QMetaDataWidget_h_
