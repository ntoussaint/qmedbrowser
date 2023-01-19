#include "QMedBrowserSettingsDialog.h"

#include <QtWidgets>
#include <QSettings>

QMedBrowserSettingsDialog::QMedBrowserSettingsDialog(QWidget *parent)
    : QDialog(parent)
{
  QString appName = QCoreApplication::applicationName() + " Preferences";
  this->setWindowTitle(appName);  
  this->SetupLayout();
}

void QMedBrowserSettingsDialog::SetupLayout(void)
{
  QSettings settings;
  QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel);
  QObject::connect(buttons, &QDialogButtonBox::accepted, this, &QMedBrowserSettingsDialog::accept);
  QObject::connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

  QLabel* filefilterslabel = new QLabel("File filters (comma separated):");
  filefilterslabel->setStyleSheet("font-weight: bold; color: black");
  QStringList defaultfilters;
  defaultfilters << "*.mha" << "*.mhd" << "*.nii" << "*.nii.gz";
  this->FileFiltersEdit = new QLineEdit();
  this->FileFiltersEdit->setPlaceholderText("Comma-separated filters");
  this->FileFiltersEdit->setText(settings.value("FileFilters", defaultfilters.join(",")).toString());

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->setSpacing(5);
  mainLayout->addWidget(filefilterslabel);
  mainLayout->addWidget(this->FileFiltersEdit);

  QFrame* line1 = new QFrame();
  line1->setFrameShape(QFrame::HLine);
  line1->setFrameShadow(QFrame::Sunken);
  mainLayout->addWidget(line1);

  QLabel* labelslabel = new QLabel("Control the label values");
  labelslabel->setStyleSheet("font-weight: bold; color: black");
  mainLayout->addWidget(labelslabel);

  QStringList defaultlabels;
  defaultlabels << "Background" << "Head" << "Thorax" << "Abdomen" << "Spine" << "Limbs" << "Placenta";
  QStringList labels = settings.value("LabelValues", defaultlabels.join(",")).toString().split(",");
  for (int i=0; i<defaultlabels.count(); i++)
  {
    QLabel* labellabel = new QLabel(tr("Label #%1").arg(i));
    QLabel* icon = new QLabel();
    icon->setPixmap(QIcon(QObject::tr(":/label%1.png").arg(i)).pixmap(15,15));
    QString l = i < labels.count() ? labels.at(i) : "";
    QLineEdit* edit = new QLineEdit();
    edit->setPlaceholderText("Unused");
    edit->setText(l);
    this->LabelValueEditList.append(edit);
  
    QHBoxLayout* hlayout = new QHBoxLayout();
    hlayout->setSpacing(5);
    hlayout->addWidget(icon);
    hlayout->addWidget(labellabel);
    hlayout->addWidget(edit);

    mainLayout->addLayout(hlayout);
  }

  QFrame* line2 = new QFrame();
  line2->setFrameShape(QFrame::HLine);
  line2->setFrameShadow(QFrame::Sunken);
  mainLayout->addWidget(line2);

  QLabel* legendfilelabel = new QLabel("Legend image file");
  legendfilelabel->setStyleSheet("font-weight: bold; color: black");
  mainLayout->addWidget(legendfilelabel);

  this->LegendFileLabel = new QLabel();
  this->LegendFileLabel->setText(settings.value("LegendFile", ":foetus.png").toString());
  QPushButton *filebutton = new QPushButton();
  filebutton->setToolTip(tr("Change legend image file"));
  filebutton->setIcon(QApplication::style()->standardIcon(QStyle::SP_DialogOpenButton));
  QObject::connect(filebutton, &QAbstractButton::released, [this]()
  { 
    QString filename = QFileDialog::getOpenFileName ( this, 
                                                      tr("Open legend image file"), 
                                                      QDir::currentPath(),
                                                      tr("Images (*.png *.xpm *.jpg)") );
    if (QFileInfo::exists(filename))
      this->LegendFileLabel->setText(filename);
  });
  QPushButton *resetbutton1 = new QPushButton();
  resetbutton1->setToolTip(tr("Reset to Foetus pictogram"));
  resetbutton1->setIcon(QApplication::style()->standardIcon(QStyle::SP_DialogResetButton));
  QObject::connect(resetbutton1, &QAbstractButton::released, [this]()
  { this->LegendFileLabel->setText(":foetus.png"); });
  QPushButton *resetbutton2 = new QPushButton();
  resetbutton2->setToolTip(tr("Reset to Foetus legend"));
  resetbutton2->setIcon(QApplication::style()->standardIcon(QStyle::SP_DialogResetButton));
  QObject::connect(resetbutton2, &QAbstractButton::released, [this]()
  { this->LegendFileLabel->setText(":legend.png"); });

  QHBoxLayout* legendlayout = new QHBoxLayout();
  legendlayout->addWidget(this->LegendFileLabel);
  legendlayout->addStretch();
  legendlayout->addWidget(filebutton);
  legendlayout->addWidget(resetbutton1);
  legendlayout->addWidget(resetbutton2);
  
  mainLayout->addLayout(legendlayout);

  mainLayout->addStretch();
  mainLayout->addWidget(buttons);
  this->setLayout(mainLayout);
}

void QMedBrowserSettingsDialog::accept()
{
  const QMessageBox::StandardButton ret = QMessageBox::warning(this, tr("Confirm preferences"), 
                                                               tr("The changes will take effect directly. \n\nDo you really want to apply the modifications?"), 
                                                               QMessageBox::Ok | QMessageBox::Cancel);
  if (ret != QMessageBox::Ok)
    return;
  QSettings settings;
  settings.setValue("FileFilters", this->FileFiltersEdit->text());

  QStringList labels;
  foreach(QLineEdit* edit, this->LabelValueEditList)
  {
    labels.append(edit->text());
  }
  settings.setValue("LabelValues", labels.join(","));

  settings.setValue("LegendFile", this->LegendFileLabel->text());
  
  QDialog::accept();
}
