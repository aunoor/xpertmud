#ifndef XM_PYTHON_FACTORY
#define XM_PYTHON_FACTORY

#include <klibloader.h>
#include <kinstance.h>

class XMPythonFactory : public KLibFactory
{
  Q_OBJECT
public:
  XMPythonFactory( QObject *parent = 0, const char *name = 0 );
  ~XMPythonFactory();

  virtual QObject* createObject(QObject* parent = 0,
			const char* name = 0, 
			const char* classname = "QObject", 
			const QStringList &args = QStringList()); 

private:
  //  static KInstance* s_instance;
};

#endif
