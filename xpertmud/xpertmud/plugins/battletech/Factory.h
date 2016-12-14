#ifndef BATTLE_FACTORY_H
#define BATTLE_FACTORY_H
#include <klibloader.h>

class BattleFactory : public KLibFactory
{
  Q_OBJECT
public:
  BattleFactory( QObject *parent = 0, const char *name = 0 ):
    KLibFactory(parent,name) {}

  virtual QObject* createObject(QObject* parent = 0,
				const char* name = 0, 
				const char* classname = "QObject", 
				const QStringList &args = QStringList()); 

};

#endif
