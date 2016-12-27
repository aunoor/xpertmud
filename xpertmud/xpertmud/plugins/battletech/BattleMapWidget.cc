#include "BattleMapWidget.h"
#include <QLayout>
#include <QSlider>
#include <QLabel>
#include <QTextStream>
#include <QAction>
#include <QStringList>
#include <QCursor>
#include <QMenu>

#include <kstandarddirs.h>
#include <kfiledialog.h>
#include <klocale.h>
#include <kglobal.h>
#include <kimageio.h>

#include <iostream>
using std::cout;
using std::endl;

BattleMapWidget::BattleMapWidget(QWidget *parent, 
				 const char *name, const QStringList& /* params*/):
  QWidget(parent),
  zoomLock(false),
  core(BattleCore::getInstance())
{
  setObjectName(name);
  /* Please leave in for debuggin purposes
  MechInfo m("ab", SubHEXPos(4, 6, M_PI, 0.3), M_PI/2.0);
  core->setMechInfo(m);
  MechInfo m2("ZY", SubHEXPos(2, 3, 2.42, 0.3), 2.42);
  core->setMechInfo(m2);
  */

  QVBoxLayout *l = new QVBoxLayout(this);
  l->setContentsMargins(0,0,0,0);
  //l->setAutoAdd(true);
  speedbar = new QWidget(this);
  l->addWidget(speedbar);
  QHBoxLayout *ls = new QHBoxLayout(speedbar);
  ls->setContentsMargins(0,0,0,0);
  //ls->setAutoAdd(true);
  //slider = new QSlider(0, 1000, 100, 0, QSlider::Horizontal,speedbar);
  slider = new QSlider(Qt::Horizontal, speedbar);
  slider->setMinimum(0);
  slider->setMaximum(1000);
  slider->setPageStep(100);
  slider->setValue(0);
  ls->addWidget(slider);
  centered = new QLabel("", speedbar);
  ls->addWidget(centered);
  speedbar->hide();
  view = new BattleMapView(this, "mapview");
  l->addWidget(view);
  centeredChanged();
  connect(view, SIGNAL(tileLeftClicked(QPoint, SubHEXPos)),
	  this, SLOT(tileLeftClicked(QPoint, SubHEXPos)));
  connect(view, SIGNAL(tileMiddlePressed(QPoint, SubHEXPos)),
	  this, SLOT(tileMiddlePressed(QPoint, SubHEXPos)));
  connect(view, SIGNAL(tileRightPressed(QPoint, SubHEXPos)),
	  this, SLOT(tileRightPressed(QPoint, SubHEXPos)));
  connect(view, SIGNAL(tileDoubleClicked(QPoint, SubHEXPos)),
	  this, SLOT(tileDoubleClicked(QPoint, SubHEXPos)));
  connect(view, SIGNAL(mechLeftClicked(QPoint, const QString&)),
	  this, SLOT(mechLeftClicked(QPoint, const QString&)));
  connect(view, SIGNAL(mechMiddlePressed(QPoint, const QString&)),
	  this, SLOT(mechMiddlePressed(QPoint, const QString&)));
  connect(view, SIGNAL(mechRightPressed(QPoint, const QString&)),
	  this, SLOT(mechRightPressed(QPoint, const QString&)));
  connect(view, SIGNAL(mechDoubleClicked(QPoint, const QString&)),
	  this, SLOT(mechDoubleClicked(QPoint, const QString&)));
  connect(view, SIGNAL(zoomChanged(double)),
	  this, SLOT(zoomChanged(double)));

  connect(slider, SIGNAL(valueChanged(int)), 
	  this, SLOT(slotValueChanged(int)));

  slider->setValue((int)((view->getZoom()-view->getMinZoom()) /
			 (view->getMaxZoom()-view->getMinZoom())*1000.0));

  settingsPopup = new QMenu(this);
  connect(settingsPopup, SIGNAL(aboutToShow()), 
	  this, SLOT(createSettingsMenu()));

  tilePopup = new QMenu(this);
  connect(tilePopup, SIGNAL(aboutToShow()), 
	  this, SLOT(createTileMenu()));

  mechPopup = new QMenu(this);
  connect(mechPopup, SIGNAL(aboutToShow()), 
	  this, SLOT(createMechMenu()));

  lockHexPopup = new QMenu(this);
  connect(lockHexPopup, SIGNAL(aboutToShow()), 
	  this, SLOT(createLockHexMenu()));
  //QAction ( const QString & text, const QString & menuText, QKeySequence accel, QObject * parent, const char * name = 0, bool toggle = FALSE )  (obsolete)
  //showBar = new QAction("Show Quickbar", "&Show Quickbar", 0, settingsPopup, 0, true);
  showBar = settingsPopup->addAction("Show Quickbar");
  //showBar->setOn(false);
  showBar->setCheckable(true);
  showBar->setEnabled(false);
  connect(showBar, SIGNAL(toggled(bool)), this, SLOT(slotShowBar(bool)));

  // TODO: Not needed for QT-only version
  KImageIO::registerFormats();
}

BattleMapWidget::~BattleMapWidget() {
  BattleCore::returnInstance();
}

void BattleMapWidget::slotFunctionCall(int func, const QVariant & args, 
				       QVariant & result) {
  result=true;
  if(func == 0) { // centerMech(string)
    view->setFollowedMech(args.toString());
    centeredChanged();
  } else if(func == 1) { // scroll(dx, dy)
    //QStringList l = QStringList::split(',', args.toString());
    QStringList l = args.toString().split(',', QString::SkipEmptyParts);
    if(l.count() >= 2) {
      view->scrollMap(l[0].toInt(), l[1].toInt());
    }
  } else if(func == 2) { // setZoom(double)
    // TODO: Set Slider!!!
    view->setZoom(args.toDouble());
  } else if(func == 3) { // showQuickbar()
    showBar->setEnabled(true);
  } else if(func == 4) { // hideQuickbar()
    showBar->setEnabled(false);
  } else if(func == 5) { // saveImage
      //QStringList a=QStringList::split(',', args.toString());
      QStringList a=args.toString().split(',', QString::SkipEmptyParts);
      if (a.count() >= 5) {
	  view->saveImage(KURL(a[0]),
			  HEXPos(a[1].toInt(),a[2].toInt()),
			  HEXPos(a[3].toInt(),a[4].toInt()));
      }
  } else if (func == 6) { // switchStyle
     view->switchStyle(args.toString());
  }
}

void BattleMapWidget::tileLeftClicked(QPoint p, SubHEXPos hex) {
  QVariant result;
  emit callback(0,
		QVariant(QString("%1 %2 %3").arg(hex.getX()).arg(hex.getY()).arg(hex.getZ())),
		result);
}

void BattleMapWidget::tileRightPressed(QPoint p, SubHEXPos hex) {
  currentHex = hex;
  tilePopup->exec(QCursor::pos());
}

void BattleMapWidget::tileMiddlePressed(QPoint p, SubHEXPos hex) {
  SubHEXPos myPos = core->getMechInfo(core->getOwnId()).getPos();
  SubPos dir = NormHexLayout::difference(myPos,hex);
  QString str;
  QTextStream stream(&str, QIODevice::WriteOnly);
  stream << "heading " << dir.getAngleDeg()  << endl;
  core->slotSend(str);
}

void BattleMapWidget::tileDoubleClicked(QPoint p, SubHEXPos hex) {
  currentHex = hex;
  tactCurrentHex();
}

void BattleMapWidget::mechLeftClicked(QPoint p, const QString& mech) {
  QVariant result;
  emit callback(1,
		QVariant(mech),
		result);
}

void BattleMapWidget::mechRightPressed(QPoint p, const QString& mech) {
  currentMech = mech;
  mechPopup->exec(QCursor::pos());
}

void BattleMapWidget::mechMiddlePressed(QPoint p, const QString& mech) {
}

void BattleMapWidget::mechDoubleClicked(QPoint p, const QString& mech) {
  currentMech = mech;
  lockCurrentMech();
}

void BattleMapWidget::zoomChanged(double zoom) {
  if(!zoomLock) {
    zoomLock = true;
    slider->setValue((int)((zoom-view->getMinZoom()) /
			   (view->getMaxZoom()-view->getMinZoom())*1000.0));
    zoomLock = false;
  }
}

void BattleMapWidget::lockCurrentHex() {
  QAction *ta = dynamic_cast<QAction*>(sender());
  if (ta==NULL) return;

  int mode = ta->property("name").toInt();

  QString s;
  QTextStream str(&s, QIODevice::WriteOnly);
  str << "lock " << currentHex.getX() << " " << currentHex.getY();

  if (mode==1)
    str << " b";
  if (mode==2)
    str << " c";
  if (mode==3)
    str << " i";
  if (mode==4)
    str << " h";

  str << endl;
  core->slotSend(s);
}

void BattleMapWidget::tactCurrentHex() {
  SubHEXPos ownP = core->getMechInfo(core->getOwnId()).getPos();
  SubPos p = NormHexLayout::difference
    (ownP, currentHex);
  QString s=QString("hudinfo t 20 %1 %2\n").arg((int)p.getAngleDeg()).arg((int)p.getDistance());
  cout << "TODO: send(" << s.toLatin1().data() << ")" << endl;
  core->slotSend(s);
}

void BattleMapWidget::jumpCurrentHex() {
  SubHEXPos ownP = core->getMechInfo(core->getOwnId()).getPos();
  SubPos p = NormHexLayout::difference
    (ownP, currentHex);
  QString s=QString("jump %1 %2\n").arg((int)p.getAngleDeg()).arg(p.getDistance());
  core->slotSend(s);
}

void BattleMapWidget::lockCurrentMech() {
  QString s;
  QTextStream str(&s, QIODevice::WriteOnly);
  str << "lock " << currentMech << endl;
  // Kein TODO, da das lock-command nix mit hudinfo zu tun hat
  //  cout << "TODO: send(" << s.latin1() << ")" << endl;
  core->slotSend(s);
}

void BattleMapWidget::scanCurrentMech() {
  QString s;
  QTextStream str(&s, QIODevice::WriteOnly);
  str << "scan " << currentMech << endl;
  // Kein TODO, da das scan-command nix mit hudinfo zu tun hat
  //  cout << "TODO: send(" << s.latin1() << ")" << endl;
  core->slotSend(s);
}

void BattleMapWidget::followCurrentMech() {
  if(view->getFollowedMech() == currentMech) 
    view->setFollowedMech("");
  else
    view->setFollowedMech(currentMech);
  centeredChanged();
}

void BattleMapWidget::followSelf() {
  if(view->getFollowedMech() == core->getOwnId())
    view->setFollowedMech("");
  else
    view->setFollowedMech(core->getOwnId());
  centeredChanged();
}

void BattleMapWidget::saveMap() {
  KURL url=KFileDialog::getSaveURL(KGlobal::dirs()->saveLocation("btech-maps"),
				   i18n("*.btmap|btech map (*.btmap)\n*|All Files"),
				   this, i18n("Save map as ..."));
  if(!url.isEmpty()) {
    if(!url.isLocalFile())
      return; // TODO: notify?
    QString fn = url.fileName();
    if(fn.indexOf('.') == -1) {
      url.setFileName(fn + ".btmap");
    }
    core->saveMap(url);
  }
}

void BattleMapWidget::loadMap() {
  KURL url=KFileDialog::getOpenURL(KGlobal::dirs()->saveLocation("btech-maps"),
				   i18n("*.btmap|btech map (*.btmap)\n*|All Files"),
				   this, i18n("Load map ..."));
  if(!url.isEmpty()) {
    core->loadMap(url);
  }
}
void BattleMapWidget::clearMap() {
  core->clearMap();
}
void BattleMapWidget::saveImage() {
  KURL url=KFileDialog::getSaveURL(QString(),
				   KImageIO::pattern(),
				   this, i18n("Save map image as ..."));
  if(!url.isEmpty()) {
    QString fn = url.fileName();
    if(fn.indexOf('.') == -1) {
      url.setFileName(fn + ".png");
    }
    view->saveImage(url,HEXPos(-1,-1),HEXPos(core->getMapWidth()+1,
					   core->getMapHeight()+1));
  }
}

void BattleMapWidget::slotValueChanged(int value) {
  if(!zoomLock) {
    zoomLock = true;
    view->setZoom(view->getMinZoom() + 
		  ((double)value/1000.0) * (view->getMaxZoom()-
					    view->getMinZoom()));
    zoomLock = false;
  }
}

void BattleMapWidget::slotShowBar(bool show) {
  if(show)
    speedbar->show();
  else
    speedbar->hide();
}

void BattleMapWidget::createMechMenu() {
  mechPopup->clear();
  mechPopup->addAction(i18n("&Lock"), this, SLOT(lockCurrentMech()));
  mechPopup->addAction(i18n("&Scan"), this, SLOT(scanCurrentMech()));
  QAction *ta=mechPopup->addAction(i18n("&Follow"), this, SLOT(followCurrentMech()));
  if(view->getFollowedMech() == currentMech)
    ta->setChecked(true);
  mechPopup->addSeparator();
  ta=mechPopup->addAction(i18n("Follow S&elf"), this, SLOT(followSelf()));
  if (view->getFollowedMech() != "" && view->getFollowedMech() == core->getOwnId())
    ta->setChecked(true);
  mechPopup->addSeparator();
  ta = mechPopup->addMenu(settingsPopup);
  ta->setText(i18n("&Misc"));
}

void BattleMapWidget::createTileMenu() {
  tilePopup->clear();
  QAction *ta=tilePopup->addMenu(lockHexPopup);;
  ta->setText(i18n("&Lock"));
  tilePopup->addAction(i18n("&Tactical"), this, SLOT(tactCurrentHex()));
  tilePopup->addAction(i18n("&Jump To"), this, SLOT(jumpCurrentHex()));
  tilePopup->addSeparator();
  ta=tilePopup->addAction(i18n("Follow S&elf"), this, SLOT(followSelf()));
  if (view->getFollowedMech() != "" && view->getFollowedMech() == core->getOwnId())
    //tilePopup->setItemChecked(2, true);
    ta->setChecked(true);
  tilePopup->addSeparator();
  ta = tilePopup->addMenu(settingsPopup);
  ta->setText(i18n("&Misc"));
}

void BattleMapWidget::createLockHexMenu() {
  lockHexPopup->clear();
  lockHexPopup->addAction(QString("(%1, %2)")
			   .arg(currentHex.getX())
			   .arg(currentHex.getY()));;
  lockHexPopup->addSeparator();
/*
  lockHexPopup->addAction(i18n("&Lock IDF"), this, SLOT(lockCurrentHex(int)),0,0);
  lockHexPopup->addAction(i18n("Lock &Building"), this, SLOT(lockCurrentHex(int)),0,1);
  lockHexPopup->addAction(i18n("Lock &Clear"), this, SLOT(lockCurrentHex(int)),0,2);
  lockHexPopup->addAction(i18n("Lock &Ignite"), this, SLOT(lockCurrentHex(int)),0,3);
  lockHexPopup->addAction(i18n("Lock &Hex"), this, SLOT(lockCurrentHex(int)),0,4);
*/
  QAction *ta;
  ta=lockHexPopup->addAction(i18n("&Lock IDF"), this, SLOT(lockCurrentHex(int)));
  ta->setProperty("id",0);
  ta=lockHexPopup->addAction(i18n("Lock &Building"), this, SLOT(lockCurrentHex(int)));
  ta->setProperty("id",2);
  ta=lockHexPopup->addAction(i18n("Lock &Clear"), this, SLOT(lockCurrentHex(int)));
  ta->setProperty("id",3);
  ta=lockHexPopup->addAction(i18n("Lock &Ignite"), this, SLOT(lockCurrentHex(int)));
  ta->setProperty("id",4);
  ta=lockHexPopup->addAction(i18n("Lock &Hex"), this, SLOT(lockCurrentHex(int)));
  ta->setProperty("id",5);
}

void BattleMapWidget::createSettingsMenu() {
  settingsPopup->clear();
  //showBar->addTo(settingsPopup);
  settingsPopup->addAction(showBar);
  settingsPopup->addAction(i18n("&Save Map"), this, SLOT(saveMap()));
  settingsPopup->addAction(i18n("&Load Map"), this, SLOT(loadMap()));
  settingsPopup->addAction(i18n("&Clear Map"), this, SLOT(clearMap()));
  settingsPopup->addAction(i18n("Save &Image"), this, SLOT(saveImage()));
  
}

void BattleMapWidget::centeredChanged() {
  QString mech = view->getFollowedMech();
  if(mech == "") {
    centered->setText("Free");
  } else {
    if(mech == core->getOwnId()) {
      centered->setText("[**]");
    } else {
      centered->setText(QString("[") + mech + "]");
    }
  }
}
