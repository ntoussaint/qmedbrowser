#include "QMedBrowserWindow.h"
#include "QMedImageItem.h"

#include <QtWidgets>

#include <iostream>

QStringList listFiles(const QString& directory, QStringList filters)
{
  QStringList ret;
  QDirIterator it(directory, filters, QDir::Files, QDirIterator::Subdirectories);
  while (it.hasNext())
  {
    QString fname = it.next();
    ret.append(fname);
  }
  std::sort(ret.begin(),ret.end());
  return ret;
}


QMedBrowserWindow::QMedBrowserWindow(QWidget *parent) : QMainWindow(parent)
{
    QSettings settings;
    this->restoreGeometry(settings.value("MainWindow/geometry", this->saveGeometry()).toByteArray());
    this->restoreState(settings.value("MainWindow/windowState", this->saveState()).toByteArray());

  this->LegendDialog = nullptr;
  this->SettingsDialog = nullptr;

  this->SetupLayout();
  this->ApplySettings();
}

void QMedBrowserWindow::SetupLayout(void)
{
  QPushButton *open = new QPushButton();
  open->setToolTip("Open medical image folder");
  open->setIcon(QApplication::style()->standardIcon(QStyle::SP_DirOpenIcon));
  open->setIconSize(QSize(32,32));
  QObject::connect(open, SIGNAL(released()), this, SLOT(OpenFolderPressed()));
  QPushButton *save = new QPushButton();
  save->setToolTip("Overwrite labels in original files (metadata: Labels)");
  save->setIcon(QApplication::style()->standardIcon(QStyle::SP_DialogApplyButton));
  save->setIconSize(QSize(32,32));
  QObject::connect(save, SIGNAL(released()), this, SLOT(SaveFolderPressed()));
  QPushButton *load = new QPushButton();
  load->setToolTip("Load label spreadsheet file");
  load->setIcon(QApplication::style()->standardIcon(QStyle::SP_FileIcon));
  load->setIconSize(QSize(32,32));
  QObject::connect(load, SIGNAL(released()), this, SLOT(LoadFilePressed()));
  QPushButton *saveas = new QPushButton();
  saveas->setToolTip("Save labels into spreadsheet file");
  saveas->setIcon(QApplication::style()->standardIcon(QStyle::SP_DialogSaveButton));
  saveas->setIconSize(QSize(32,32));
  QObject::connect(saveas, SIGNAL(released()), this, SLOT(SaveAsFilePressed()));
  QPushButton *help = new QPushButton();
  help->setToolTip("Help: Open the help dialog");
  help->setIcon(QApplication::style()->standardIcon(QStyle::SP_DialogHelpButton));
  help->setIconSize(QSize(32,32));
  QObject::connect(help, SIGNAL(released()), this, SLOT(OpenHelpPressed()));
  QPushButton *config = new QPushButton();
  config->setToolTip("Config: Open the settings dialog");
  config->setIcon(QApplication::style()->standardIcon(QStyle::SP_FileDialogDetailedView));
  config->setIconSize(QSize(32,32));
  QObject::connect(config, SIGNAL(released()), this, SLOT(OpenPreferencesPressed()));

  this->HelpLabel = new QLabel("Press number keys ([1], [2], [3], etc) \nto assign a color-cored label to the selected images");
  this->HelpLabel->setVisible(false);
  this->ProgressBar = new QProgressBar();
  this->ProgressBar->setOrientation(Qt::Horizontal);
  this->ProgressBar->setRange(0, 100);
  this->ProgressBar->setVisible(false);

  this->LabelComboBox = new QComboBox();
  QObject::connect(this->LabelComboBox, SIGNAL(currentTextChanged(const QString&)), this, SLOT(SetItemFilter(const QString&)));
  this->ThumbnailSizeComboBox = new QComboBox();
  this->ThumbnailSizeComboBox->addItem("Thumbnail size");
  this->ThumbnailSizeComboBox->addItem("Small");
  this->ThumbnailSizeComboBox->addItem("Medium");
  this->ThumbnailSizeComboBox->addItem("Large");
  this->ThumbnailSizeComboBox->setCurrentIndex(0);
  QObject::connect(this->ThumbnailSizeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(SetThumbnailSize(int)));

  QHBoxLayout* toplayout = new QHBoxLayout();
  toplayout->setSpacing(5);
  toplayout->addWidget(open);
  toplayout->addWidget(load);
  toplayout->addWidget(save);
  toplayout->addWidget(saveas);
  toplayout->addWidget(this->ThumbnailSizeComboBox);
  toplayout->addStretch();
  toplayout->addWidget(this->HelpLabel);
  toplayout->addWidget(this->ProgressBar, 1);
  toplayout->addStretch();
  toplayout->addWidget(this->LabelComboBox);
  toplayout->addWidget(config);
  toplayout->addWidget(help);

  this->ListWidget = new QMedImageListWidget();

  QVBoxLayout* layout = new QVBoxLayout();
  layout->setSpacing(5);
  layout->addLayout(toplayout);
  layout->addWidget(this->ListWidget);
  
  QWidget* centralwidget = new QWidget(this);
  centralwidget->setLayout(layout);
  this->setCentralWidget(centralwidget);

  this->setStatusBar(new QStatusBar());
  this->statusBar()->showMessage(tr("Ready"));
}

void QMedBrowserWindow::closeEvent(QCloseEvent *event)
{
  const QMessageBox::StandardButton ret = QMessageBox::warning(this, tr("Confirm Exit"),
                                                               tr("Please make sure to save changes before closing. \n\nDo you really want to exit the application?"), 
                                                               QMessageBox::Ok | QMessageBox::Cancel);
  if (ret != QMessageBox::Ok)
  {
    event->ignore();
  }
  else
  {
    QSettings settings;
    settings.setValue("MainWindow/geometry", this->saveGeometry());
    settings.setValue("MainWindow/windowState", this->saveState());
    event->accept();
  }
}

void QMedBrowserWindow::OpenFolderPressed(void)
{
  QSettings settings;
  QString defaultpath = settings.value("DefaultFolder", QDir::currentPath()).toString();
  QString folder = QFileDialog::getExistingDirectory ( this, "Please select the folder containing images to review", defaultpath );
  if (!QFileInfo(folder).exists())
    return;

  settings.setValue("DefaultFolder", folder);

  settings.setValue("DefaultFile", tr("%1%2%3").arg(folder).arg(QDir::separator()).arg("labels.csv"));

  QStringList defaultfilters;
  defaultfilters << "*.jpg" << "*.png";
  QStringList filters = settings.value("FileFilters", defaultfilters.join(",")).toString().split(",");
  QStringList files_new = listFiles(folder, filters);

  QList<QMedImageItem*> items_old;
  for (int idx=0; idx<this->ListWidget->count(); idx++)
  {
    QMedImageItem* meditem = reinterpret_cast<QMedImageItem*> (this->ListWidget->item(idx));
    if (meditem)
      items_old.append(meditem);
  }

  QMutableListIterator<QMedImageItem*> it(items_old);
  while (it.hasNext())
  {
    QMedImageItem* meditem = it.next();
    it.remove();
    //this->ListWidget->removeItemWidget(meditem);
    delete meditem;
  }

  this->statusBar()->showMessage(tr("Opening %1 images...").arg(files_new.count()));

  this->ProgressBar->setRange(0, files_new.count());
  this->HelpLabel->setVisible(false);
  this->ProgressBar->setVisible(true);

  QSize thumbnailsize(300,300);
  
  for (int idx = 0; idx < files_new.count(); idx++)
  {
    QString f = files_new.at(idx);
    QMedImageItem* meditem = new QMedImageItem(f, tr("%1").arg(this->ListWidget->count()), this->ListWidget, thumbnailsize);
    QStringList values = meditem->labelValues();
    meditem->addOverlays(this->ListWidget->LabelsToPixmaps(values));
    this->ListWidget->addItem(meditem);
    this->ProgressBar->setValue(idx);
  }

  this->ProgressBar->setVisible(false);
  this->HelpLabel->setVisible(true);
  this->statusBar()->showMessage(tr("Total: %1 images").arg(files_new.count()));
  this->SetThumbnailSize(3);

  this->LabelComboBox->setCurrentIndex(0);
  this->ThumbnailSizeComboBox->setCurrentIndex(0);
}

void QMedBrowserWindow::SaveFolderPressed(void)
{
  if (!this->ListWidget->count())
    return;

  this->statusBar()->showMessage(tr("Saving %1 images...").arg(this->ListWidget->count()));

  this->ProgressBar->setRange(0, this->ListWidget->count());
  this->HelpLabel->setVisible(false);
  this->ProgressBar->setVisible(true);

  for (int idx=0; idx<this->ListWidget->count(); idx++)
  {
    QMedImageItem* meditem = reinterpret_cast<QMedImageItem*> (this->ListWidget->item(idx));
    if (!meditem)
      continue;
    meditem->write();
    /// @todo update a progress bar
    this->ProgressBar->setValue(idx);
  }
  this->HelpLabel->setVisible(true);
  this->ProgressBar->setVisible(false);
  this->statusBar()->showMessage(tr("Ready"));
}


void QMedBrowserWindow::LoadFilePressed(void)
{
  if (!this->ListWidget->count())
    return;
  QSettings settings;
  QString defaultpath = settings.value("DefaultFile", tr("%1%2%3").arg(QDir::currentPath()).arg(QDir::separator()).arg("labels.csv")).toString();
  QString filename = QFileDialog::getOpenFileName(this, tr("Open File"),
                           defaultpath,
                           tr("Spreadsheet (*.csv *.txt)"));
  if (!QFileInfo(filename).exists())
    return;

  settings.setValue("DefaultFile", filename);

  QFile* mapfile = new QFile(filename);

  if (!mapfile->open(QIODevice::ReadOnly))
    return;

  QTextStream in(mapfile);
  QString keys = in.readLine();

  unsigned long idx=0;
  QMap<QString, QStringList> labelsmap;
  while( !in.atEnd())
  {
    QStringList line = in.readLine().split(",");
    if (line.count() != 2)
      continue;
    QString filepath = line.at(0);
    QStringList labels = line.at(1).split(";");
    labelsmap.insert(filepath, labels);
  }
  mapfile->close();

  for (int idx=0; idx<this->ListWidget->count(); idx++)
  {
    QMedImageItem* meditem = dynamic_cast<QMedImageItem*> (this->ListWidget->item(idx));
    if (!meditem)
      continue;
    QString basename = QFileInfo(meditem->path()).fileName();
    
    QMap<QString, QStringList>::const_iterator found = labelsmap.find(basename);

    if (found == labelsmap.end())
      continue;

    meditem->setLabelValues(found.value());
    meditem->addOverlays(this->ListWidget->LabelsToPixmaps(found.value()));
  }
}


void QMedBrowserWindow::SaveAsFilePressed(void)
{
  if (!this->ListWidget->count())
    return;
  QSettings settings;
  QString defaultpath = settings.value("DefaultFile", tr("%1%2%3").arg(QDir::currentPath()).arg(QDir::separator()).arg("labels.csv")).toString();
  QString filename = QFileDialog::getSaveFileName(this, tr("Save File"),
                           defaultpath,
                           tr("Spreadsheet (*.csv *.txt)"));
  if (!filename.count())
    return;

  settings.setValue("DefaultFile", filename);

  QFile* mapfile = new QFile(filename);

  if (!mapfile->open(QIODevice::WriteOnly | QIODevice::Text | QFile::Truncate ))
    return;

  mapfile->resize(0);

  QTextStream out(mapfile);
  out << "Filename;Labels\n";

  for (int idx=0; idx<this->ListWidget->count(); idx++)
  {
    QMedImageItem* meditem = reinterpret_cast<QMedImageItem*> (this->ListWidget->item(idx));
    if (!meditem)
      continue;
    out << QFileInfo(meditem->path()).fileName() << "," << meditem->labelValues().join(";") << "\n";
  }

  mapfile->close();
}

void QMedBrowserWindow::OpenHelpPressed(void)
{
  if (this->LegendDialog != nullptr)
  {
    if (this->LegendDialog->isVisible())
      return;
    else
    {
      delete this->LegendDialog;
      this->LegendDialog = nullptr;
    }
  }

  this->LegendDialog = new QDialog(this);
  QLabel* label = new QLabel();
  QSettings settings;
  QIcon icon = QIcon(settings.value("LegendFile", ":validate").toString());
  label->setPixmap(icon.pixmap(icon.availableSizes().at(0)));
  QLayout* layout = new QVBoxLayout();
  layout->setSpacing(5);
  layout->addWidget(label);
  this->LegendDialog->setLayout(layout);
  this->LegendDialog->setStyleSheet("background-color:black;");
  this->LegendDialog->setWindowTitle("Legend");
  this->LegendDialog->show();
}

void QMedBrowserWindow::OpenPreferencesPressed(void)
{
  if (!this->SettingsDialog)
  {
    this->SettingsDialog = new QMedBrowserSettingsDialog(this);
    QObject::connect(this->SettingsDialog, SIGNAL(accepted()), this, SLOT(ApplySettings()));
    this->SettingsDialog->resize(500,300);
  }
  this->SettingsDialog->show();
}

void QMedBrowserWindow::ApplySettings(void)
{
  QStringList defaultlabels;
  defaultlabels << "" << "" << "" << "" << "validated" << "" << "";
  QSettings settings;
  QStringList labels = settings.value("LabelValues", defaultlabels.join(",")).toString().split(",");
  this->ListWidget->setLabelValues(labels);
  this->LabelComboBox->clear();
  this->LabelComboBox->addItem("Filter label...");
  this->LabelComboBox->addItems(labels);
  this->LabelComboBox->addItem("Un-labelled");
  this->LabelComboBox->setCurrentIndex(0);

  int thumbnailsize_id = settings.value("ThumbnailSize", 1).toInt();
  QSize thumbnailsize(100,100);
  switch(thumbnailsize_id)
  {
    case 2:
      thumbnailsize.setHeight(200);
      thumbnailsize.setWidth(200);
      break;
    case 3:
      thumbnailsize.setHeight(300);
      thumbnailsize.setWidth(300);
      break;
    case 1:
    default:
      thumbnailsize.setHeight(100);
      thumbnailsize.setWidth(100);
      break;
  }
  this->ListWidget->setIconSize(thumbnailsize);
}


void QMedBrowserWindow::SetItemFilter(const QString& regexp)
{
  bool showitem = false;
  bool showunlabelled = regexp.compare("Un-labelled") == 0;
  bool showall = regexp.compare("Filter label...") == 0;
  unsigned long numberofimages = 0;
  QMedImageItem* meditem = nullptr;
  for (int idx=0; idx<this->ListWidget->count(); idx++)
  {
    meditem = reinterpret_cast<QMedImageItem*> (this->ListWidget->item(idx));
    if (!meditem)
      continue;

    if (showall)
      showitem = true;
    else if (showunlabelled)
      showitem = meditem->labelValues().count() == 0;
    else
      showitem = meditem->labelValues().contains(regexp);
    meditem->setHidden(!showitem);
    if (showitem)
      numberofimages++;
  }

  this->statusBar()->showMessage(tr("Total: %1 images").arg(numberofimages));
}

void QMedBrowserWindow::SetThumbnailSize(int sizeid)
{
  if (sizeid <= 0)
    return;
  QSettings settings;
  settings.setValue("ThumbnailSize", sizeid);
  QSize thumbnailsize(100,100);
  switch(sizeid)
  {
    case 2:
      thumbnailsize.setHeight(200);
      thumbnailsize.setWidth(200);
      break;
    case 3:
      thumbnailsize.setHeight(300);
      thumbnailsize.setWidth(300);
      break;
    case 1:
    default:
      thumbnailsize.setHeight(100);
      thumbnailsize.setWidth(100);
      break;
  }
  this->ListWidget->setIconSize(thumbnailsize);
}
