#ifndef XM_RUBY_FACTORY
#define XM_RUBY_FACTORY

#include <klibloader.h>
#include <kinstance.h>

class XMRubyFactory : public KLibFactory
{
  Q_OBJECT
public:
  XMRubyFactory( QObject *parent = 0, const char *name = 0 );
  ~XMRubyFactory();

  virtual QObject* createObject(QObject* parent = 0,
			const char* name = 0, 
			const char* classname = "QObject", 
			const QStringList &args = QStringList()); 

private:
  //  static KInstance* s_instance;s
};

#endif
