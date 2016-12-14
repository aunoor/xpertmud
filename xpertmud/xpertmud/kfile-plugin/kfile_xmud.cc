#include "kfile_xmud.h"

#include <klocale.h>
#include <kstringvalidator.h>
#include <kgenericfactory.h>

#include <qvalidator.h>
#include <qdom.h>
#include <qfile.h>


typedef KGenericFactory<KXmudPlugin> XmudFactory;

K_EXPORT_COMPONENT_FACTORY( kfile_xmud, XmudFactory( "kfile_xmud" ) )

KXmudPlugin::KXmudPlugin( QObject *parent, const char *name,
                        const QStringList &preferredItems )
    : KFilePlugin( parent, name, preferredItems )
{

  KFileMimeTypeInfo* info = addMimeTypeInfo( "application/x-xpertmud-bookmark" );
  
  KFileMimeTypeInfo::GroupInfo* group;
  KFileMimeTypeInfo::ItemInfo* item;
 
  //////////////////////////////////////////////////////////////////////////

  group =addGroupInfo(info, "Connection", i18n("Connection"));
  //  setAttributes(group, KFileMimeTypeInfo::Addable);
  
  item=addItemInfo(group, "Host", i18n("Host"), QVariant::String);
  //  setAttributes(item, KFileMimeTypeInfo::Modifiable);

  item=addItemInfo(group, "Port", i18n("Port"), QVariant::String);
  //  setAttributes(item, KFileMimeTypeInfo::Modifiable);

  //////////////////////////////////////////////////////////////////////////
  
  group =addGroupInfo(info, "Description", i18n("Description"));
  setAttributes(group, KFileMimeTypeInfo::Addable);
  
  item=addItemInfo(group, "Name", i18n("Name"), QVariant::String);
  //setAttributes(item, KFileMimeTypeInfo::Modifiable);
  setHint(item,  KFileMimeTypeInfo::Name);
  
  //////////////////////////////////////////////////////////////////////////
  /*
  group =addGroupInfo(info, "Scripting", i18n("Scripting"));
  //  setAttributes(group, KFileMimeTypeInfo::Addable);

  item=addItemInfo(group, "Interpreter", i18n("Interpreter"), QVariant::String);
  //  setAttributes(item, KFileMimeTypeInfo::Modifiable);

  item=addItemInfo(group, "Script", i18n("Script"), QVariant::StringList);
  //  setAttributes(item, KFileMimeTypeInfo::Modifiable);
  */
}

bool KXmudPlugin::readInfo( KFileMetaInfo& info, uint )
{
  QFile f( info.path() );
  if ( !f.open( IO_ReadOnly ) ) { 
    return false;
  }
  QDomDocument doc( "xpertmudBookmark" );
  if ( !doc.setContent( &f ) ) {
    f.close();
    return false;
  }

  // TODO: Does this only find Toplevel Nodes or all?
  QDomNodeList conns=doc.elementsByTagName("Connection");

  // TODO: Multiple Groups aren't displayed...
  for (unsigned int i=0; i<conns.count(); ++i) {
    KFileMetaInfoGroup group = appendGroup(info, "Connection");
    
    QDomNode n = conns.item(i).firstChild();
    while( !n.isNull() ) {
      QDomElement e = n.toElement(); // try to convert the node to an element.
      if( !e.isNull() ) {
	if(e.tagName()=="Host") {
	  appendItem(group, "Host",e.text());
	} else if (e.tagName()=="Port") {
	  appendItem(group, "Port", e.text());
	}
      }
      n = n.nextSibling();
    }
  }

  // group = appendGroup(info, "Description");
  // appendItem(group, "Name", );

  /*
  group = appendGroup(info, "Scripting");
  appendItem(group, "Interpreter", cfg.readEntry("scripting","perl"));
  appendItem(group, "Script", 
	     QStringList::split('\n', // Non portable
				cfg.readEntry("script"),true)
	     );
  */
  return true;
}

//bool KXmudPlugin::writeInfo(const KFileMetaInfo& info) const {
  //  KSimpleConfig cfg(info.path(),false);
  //  cfg.setGroup("xpertmud Bookmark");
  //  cfg.writeEntry("host",info["Connection"]["Host"].value().toString());
  //  if (info["Connection"]["Port"].isValid())
  //   cfg.writeEntry("port",info["Connection"]["Port"].value().toUInt());
    
  
  //  cfg.writeEntry("name",info["Description"]["Name"].value().toString());
  //  cfg.writeEntry("scripting",info["Scripting"]["Interpreter"].value().toString());
  //  cfg.writeEntry("script",info["Scripting"]["Script"].value().toString());
//}

//QValidator* KXmudPlugin::createValidator(const QString& /* mimetype */,
//					 const QString& group, 
//					 const QString& key,
//					 QObject* parent, 
//					 const char* name) const {

//  if (key=="Port") {
//   return new QIntValidator(1, 0xFFFF, parent, name);
//  }
  /*
  if (key=="Interpreter") {
    QStringList list;
    //TODO: is this good?
    list+="perl";
    list+="python";
    return new KStringListValidator(list, false, true, parent, name);
  }
  */
//  return NULL;
//}
