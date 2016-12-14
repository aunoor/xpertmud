// -*- c++ -*-
#ifndef KFILE_XMUD_H
#define KFILE_XMUD_H

#include <kfilemetainfo.h>

class QStringList;

class KXmudPlugin: public KFilePlugin
{
    Q_OBJECT
    
public:
    KXmudPlugin( QObject *parent, const char *name, const QStringList& args );
    
    bool readInfo( KFileMetaInfo& info, uint what );
    
  //    bool writeInfo( const KFileMetaInfo& info ) const;
    

  //    QValidator* createValidator(const QString& /* mimetype */,
  //				const QString &group, const QString &key,
  //				QObject* parent, const char* name) const;

    
};

#endif
