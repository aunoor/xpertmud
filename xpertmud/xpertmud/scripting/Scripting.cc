#include "Scripting.h"

// DEBUG
//#include <iostream>
//using namespace std;

XMScripting::XMScripting(QObject* parent,
			 const char* name ):
#ifndef NO_KDE
  DCOPObject("ScriptingIface"), // TODO: BUG: Warum wird das ignoriert??????
#endif
  QObject(parent,name)
{
  //  cout << "XMScripting name: " << name << endl;
  //  cout << "Empty? " << (QCString("ScriptingIface").isEmpty()) << endl;

  //  cout << "ObjectID: " << objId() << endl;

  // TODO: Check why this strange workaround is neccessary...
#ifndef NO_KDE
  setObjId("ScriptingIface");
#endif

  //  cout << "ObjectID: " << objId() << endl;
}

XMScripting::~XMScripting() {

}
