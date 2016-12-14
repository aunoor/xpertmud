#include "XMPerlFactory.h"
#include "XMPerl.h"

// DEBUG only, TODO: remove
//#include <iostream>
//using namespace std;

// force real dlunload on this lib
int __kde_do_unload = 1;

XMPerlFactory::XMPerlFactory( QObject* parent, const char* name )
  : KLibFactory( parent, name )
{
  //  cout << "creating Factory" << endl;
  //  s_instance = new KInstance("KXMPerl");
}

XMPerlFactory::~XMPerlFactory() { 
  //  cout << "deleting Factory" << endl;
  //  delete s_instance;
}


QObject* XMPerlFactory::createObject(QObject* parent,
				     const char* name, 
				     const char* /* unused: classname */, 
				     const QStringList & /* unused: args */) {

  //cout << "Creating XMPerl" <<endl;
  return new XMPerl(parent,name);
} 

K_EXPORT_COMPONENT_FACTORY(libxmperlinterpreter,XMPerlFactory)



//KInstance *XMPerlFactory::s_instance = 0L;
