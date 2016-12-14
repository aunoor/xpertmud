#include "BattleStatus.h"
#include "BattleCore.h"
#include <qvariant.h>
#include <qwidget.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qimage.h>
#include <qstringlist.h>
#include <kglobal.h>
#include <kstandarddirs.h>

#include <algorithm>
#ifndef WIN32
using std::max;
using std::min;
#else
#define max(x,y) ((x) > (y)?(x):(y))
#define min(x,y) ((x) > (y)?(y):(x))
#endif

class BattleStatusDrawConfig {
public:
  QPixmap pixmap;
  QMap<QString,QPoint> armorLocations;
};


BattleStatusWidget::BattleStatusWidget(QWidget *parent, 
				   const char *name, 
				   const QStringList& args):
  QWidget(parent,name,WResizeNoErase| WRepaintNoErase ),
  core(BattleCore::getInstance()),hasArmorStatus(false),currentDrawConfig(NULL)
{
  setMaximumSize(150,150);
  connect(core, SIGNAL(queueMechInfoChange(const MechInfo&,
					   const MechInfo&)),
	  this, SLOT(slotUpdateMechInfo(const MechInfo&,
					const MechInfo&)));

   setEraseColor(black);

   QString path;
   path = 
      KGlobal::dirs()->findResource("appdata", 
				    "status/status.def");

   qDebug("Reading status defs from [%s]",(const char *)(path.local8Bit()));
   
   if (path.isEmpty()) return;
   QFile f(path);
   if (!f.open(IO_ReadOnly)) return;
   QTextStream s(&f);
   QString currentTemplate;
   BattleStatusDrawConfig * currentConfig=NULL;
   while (!s.atEnd()) {
     QString l=s.readLine();
     if (l.startsWith("TEMPLATE:")) {
       QString te=l.mid(9).simplifyWhiteSpace();
       if (!te.isEmpty()) {
         currentConfig=new BattleStatusDrawConfig;
         currentTemplate=te;
	 qDebug("CurrentTemplate: [%s]",(const char *)(currentTemplate.local8Bit()));
         drawConfig[currentTemplate]=currentConfig;
       }
     } else if (currentConfig && l.startsWith("IMAGE:")) {
       QString te=KGlobal::dirs()->findResource("appdata", l.mid(6).simplifyWhiteSpace());
       if (!te.isEmpty()) {
         QImage img(te);
         if (!img.isNull()) {
           currentConfig->pixmap=QPixmap(img/*.smoothScale(150,150)*/);
         }
       }
    } else if (currentConfig && l.startsWith("SECTION:")) {
      QString rem=l.mid(8).simplifyWhiteSpace();
      QTextStream li(&rem, IO_ReadOnly);
      int x,y;
      QString sec;
      li >> x >> y >> sec;
      if (!sec.isEmpty() && x>0 && y>0 && x<150 && y<150) {
        currentConfig->armorLocations[sec]=QPoint(x,y);
      }
    }
   }
}

BattleStatusWidget::~BattleStatusWidget() {
  BattleCore::returnInstance();
  for (QMap<QString,BattleStatusDrawConfig*>::iterator it=drawConfig.begin();
	it!=drawConfig.end(); ++it) {
     delete it.data();
  }
}


void BattleStatusWidget::slotFunctionCall(int, const QVariant &, QVariant &) {
}

void BattleStatusWidget::paintEvent ( QPaintEvent *e ) {
  QPainter painter;
  painter.begin(this);

  if (currentDrawConfig && !currentDrawConfig->pixmap.isNull()) {
     painter.drawPixmap(0,0,currentDrawConfig->pixmap);
  } else {
    erase();
  }

  painter.setPen(lightGray);
  QString pos=QString("(%1,%2,%3)").arg(ownPosition.getX()).arg(ownPosition.getY()).arg(ownPosition.getZ());
  painter.drawText(rect(),AlignTop|AlignHCenter|SingleLine,pos);

  if (currentDrawConfig && hasArmorStatus) {
    painter.save();
    QFont f=painter.font();
    f.setPixelSize(8);
    painter.setFont(f);

    for (QMap<QString,QPoint>::iterator it=currentDrawConfig->armorLocations.begin();
	 it != currentDrawConfig->armorLocations.end(); ++it) {
      if (currentArmor.contains(it.key())) {
        QPoint loc=it.data();
        QRect re(loc.x()-5,loc.y()-5,10,10);
        int armor=currentArmor[it.key()];
        int oarmor=originalArmor[it.key()];
        int per=oarmor?(armor*100)/oarmor:1;
        QString arm=QString::number(armor);
	if (per>90) {
          painter.setPen(green);
        } else if (per>75) {
           painter.setPen(darkGreen);
        } else if (per>45) {
           painter.setPen(yellow);
        } else {
           painter.setPen(red);
 	}
        painter.drawText(re,AlignCenter,arm);
      }
    }
    painter.restore();
  }

  // Overlay, draw last
  QStringList flags;
  if (ownFlags.contains("D"))
    flags.append("Destroyed");
  if (ownFlags.contains("F"))
    flags.append("Fallen");
  if (ownFlags.contains("f"))
    flags.append("Standing up");
  if (ownFlags.contains("S"))
    flags.append("Shutdown");
  if (ownFlags.contains("s"))
    flags.append("Startup");
  if (ownFlags.contains("J"))
    flags.append("Jumping");
  if (ownFlags.contains("+"))
    flags.append("Overheat");
  if (ownFlags.contains("I"))
    flags.append("Infernoed");
  if (ownFlags.contains("T"))
    flags.append("Towing");
  if (ownFlags.contains("H"))
    flags.append("Hulldown");

  if (!flags.isEmpty()) {
    painter.setPen(red);
    painter.drawText(rect(),AlignBottom|AlignHCenter,flags.join("\n"));
  }
  painter.end();
}


void BattleStatusWidget::slotUpdateMechInfo(const MechInfo &,const MechInfo & mechInfo) {
  if (mechInfo.isValid() && mechInfo.getId() == core->getOwnId()) {
    if (mechInfo.hasPos())
      ownPosition=mechInfo.getPos();
    if (mechInfo.hasStatus()) 
      ownFlags=mechInfo.getStatus();
    hasArmorStatus=mechInfo.hasOriginalArmor() && mechInfo.hasCurrentArmor();
    if (hasArmorStatus) {
      originalArmor=mechInfo.getOriginalArmor();
      currentArmor=mechInfo.getCurrentArmor();
    }
    currentDrawConfig=NULL;
    if (!originalArmor.contains("T") && drawConfig.contains(QString("%1-NT-%2").arg(mechInfo.getType()).arg(mechInfo.getName()))) {
      currentDrawConfig=drawConfig[QString("%1-NT-%2").arg(mechInfo.getType()).arg(mechInfo.getName())];
    } else if (!originalArmor.contains("T") && drawConfig.contains(QString("%1-NT").arg(mechInfo.getType()))) {
      currentDrawConfig=drawConfig[QString("%1-NT").arg(mechInfo.getType())];
    } else if (drawConfig.contains(QString("%1-%2").arg(mechInfo.getType()).arg(mechInfo.getName()))) {
      currentDrawConfig=drawConfig[QString("%1-%2").arg(mechInfo.getType()).arg(mechInfo.getName())];
    } else if (drawConfig.contains(QString(QChar(mechInfo.getType())))) {
      currentDrawConfig=drawConfig[QString(QChar(mechInfo.getType()))];
    }
    update();
  }
}
