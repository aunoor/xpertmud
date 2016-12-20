#include "Scripting.h"

// DEBUG
//#include <iostream>
//using namespace std;

XMScripting::XMScripting(QObject* parent,
			 const char* name ):
  QObject(parent)
{
	setObjectName(name);
  //  cout << "XMScripting name: " << name << endl;
  //  cout << "Empty? " << (QCString("ScriptingIface").isEmpty()) << endl;

  //  cout << "ObjectID: " << objId() << endl;

  // TODO: Check why this strange workaround is neccessary...

  //  cout << "ObjectID: " << objId() << endl;
}

XMScripting::~XMScripting() {

}
