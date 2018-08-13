#include <QApplication>

#include "QMedBrowserWindow.h"

int main(int argc, char* argv[])
{
  QApplication app(argc, argv);
  QMedBrowserWindow* window = new QMedBrowserWindow();
  window->show();
  return app.exec();
}
