// QT includes


#include "DrosophilaOmmatidiaExplorer.h"
#include <QApplication>
extern int qInitResources_icons();

int main( int argc, char** argv )
{

  // QT Stuff
  QApplication app( argc, argv );


  QApplication::setStyle("fusion");

  qInitResources_icons();

  DrosophilaOmmatidiaExplorer myDrosophilaOmmatidiaExplorer;
  myDrosophilaOmmatidiaExplorer.show();

  return app.exec();
}
