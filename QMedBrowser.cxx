#include <QApplication>

#include "QMedBrowserWindow.h"

int main(int argc, char* argv[])
{
  QApplication app(argc, argv);
  QCoreApplication::setOrganizationName("toussaint-nicolas");
  QCoreApplication::setOrganizationDomain("https://github.com/ntoussaint/qmedbrowser");
  QCoreApplication::setApplicationName("QMedBrowser");
  QCoreApplication::setApplicationVersion("0.1.0");

  QMedBrowserWindow* window = new QMedBrowserWindow();
  window->show();
  return app.exec();
}
