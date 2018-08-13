#include "QMetaDataWidget.h"

#include <QtWidgets>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>

#include <itkMetaDataDictionary.h>
#include <itkMetaDataObject.h>
#include <itkImageFileReader.h>
#include <itkMetaImageIO.h>
#include <itkImage.h>

QMetaDataWidget::QMetaDataWidget(const QString& path, QWidget *parent) : QWidget(parent, Qt::WindowStaysOnTopHint)
{
  this->window()->setWindowTitle("QMetaDataWidget");
  this->setMinimumSize(QSize(600, 300));
  this->Table = new QTableView(this);
  this->Source = new QStandardItemModel();
  this->Filter = new QSortFilterProxyModel();
  this->Filter->setSourceModel(this->Source);
  this->Filter->setFilterKeyColumn(-1);
  this->Filter->setFilterCaseSensitivity(Qt::CaseInsensitive);
  this->Table->setModel(this->Filter);
  this->Path = path;
  this->SetupLayout();
  this->SetupTable();
}


void QMetaDataWidget::SetupLayout(void)
{
  QVBoxLayout* Layout = new QVBoxLayout();
  Layout->setSpacing(5);

  QLineEdit* lineedit = new QLineEdit();
  lineedit->setPlaceholderText(tr("Search key..."));
  QObject::connect(lineedit, SIGNAL(textChanged(const QString&)), this->Filter, SLOT(setFilterRegExp(const QString&)));
  Layout->addWidget(lineedit);
  Layout->addSpacing(5);
  Layout->addWidget(this->Table, 1);
  this->setLayout(Layout);
}


void QMetaDataWidget::SetupTable()
{ 
  itk::MetaImageIO::Pointer io = itk::MetaImageIO::New();
  io->SetFileName(this->Path.toStdString().c_str());

  try
  {
    io->ReadImageInformation();
  }
  catch (itk::ExceptionObject &e)
  {
    return;
  }
  itk::MetaDataDictionary dic = io->GetMetaDataDictionary();

  std::vector<std::string> keys = dic.GetKeys();
  this->Source->setColumnCount(2);
  QStringList headerlabels;
  headerlabels << tr("Key") << tr("Value");
  this->Source->setHorizontalHeaderLabels(headerlabels);
  this->Source->setRowCount(keys.size());
  for (unsigned int i=0; i<keys.size(); i++)
  {
    std::string value;
    bool valid = itk::ExposeMetaData<std::string>(dic, keys[i], value);
    if (!valid)
      continue;
    QStandardItem *keyitem = new QStandardItem(keys[i].c_str());
    QStandardItem *valueitem = new QStandardItem(value.c_str());
    this->Source->setItem(i, 0, keyitem);
    this->Source->setItem(i, 1, valueitem);
  }
  this->Table->resizeColumnToContents(0);
  this->Table->setColumnWidth(1, 300);
  this->update();
}


void QMetaDataWidget::showEvent(QShowEvent* event)
{
  QPoint p = QCursor::pos(); QPoint diff(5,5);
  p -= diff;
  this->move(p);  
}
