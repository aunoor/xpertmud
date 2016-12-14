#include "XMRubyFactory.h"
#include "XMRuby.h"

// force real dlunload on this lib
int __kde_do_unload = 1;

XMRubyFactory::XMRubyFactory( QObject* parent, const char* name )
  : KLibFactory( parent, name )
{
}

XMRubyFactory::~XMRubyFactory() { 
}

QObject* XMRubyFactory::createObject(QObject* parent,
				       const char* name, 
				       const char* /* unused: classname */, 
				       const QStringList &/* unused: args */) {

  return new XMRuby(parent,name);
} 

K_EXPORT_COMPONENT_FACTORY(libxmrubyinterpreter,XMRubyFactory)
