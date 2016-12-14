#ifndef XM_PERL_FACTORY
#define XM_PERL_FACTORY

#include <klibloader.h>

class XMPerlFactory : public KLibFactory
{
  Q_OBJECT
public:
  XMPerlFactory( QObject *parent = 0, const char *name = 0 );
  ~XMPerlFactory();

  virtual QObject* createObject(QObject* parent = 0,
			const char* name = 0, 
			const char* classname = "QObject", 
			const QStringList &args = QStringList()); 

private:
  //  static KInstance* s_instance;
};

#endif
