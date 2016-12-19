/* This file is part of the KDE libraries
   Copyright (C) 2000 Max Judin <novaprint@mtu-net.ru>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/
#include "kdockwidget.h"
#include "kdockwidget_private.h"
#include <kdocktabctl.h>

#include <QLayout>
#include <QPainter>
#include <QMouseEvent>
#include <QObjectList>
#include <QStringList>
#include <QCursor>

#include <QApplication>
#include <QMenu>

#define DOCK_CONFIG_VERSION "0.0.5"

static const char*close_xpm[]={
"5 5 2 1",
"# c black",
". c None",
"#...#",
".#.#.",
"..#..",
".#.#.",
"#...#"};

static const char*dockback_xpm[]={
"5 5 2 1",
"# c black",
". c None",
"#....",
".#...",
"..#.#",
"...##",
"..###"};

static const char*not_close_xpm[]={
"5 5 2 1",
"# c black",
". c None",
"#####",
"#...#",
"#...#",
"#...#",
"#####"};

/**
 * A special kind of KMainWindow that is able to have dockwidget child widgets.
 *
 * The main widget should be a dockwidget where other dockwidgets can be docked to
 * the left, right, top, bottom or to the middle.
 * Furthermore, the KDockMainWindow has got the KDocManager and some data about the dock states.
 *
 * @author Max Judin.
*/
KDockMainWindow::KDockMainWindow( QWidget* parent, const QString name, Qt::WindowFlags f, QList<Qt::WidgetAttribute> a)
:KMainWindow( parent, name, f, a )
{
  QString new_name = QString(name) + QString("_DockManager");
  dockManager = new KDockManager( this, new_name );
  mainDockWidget = 0L;
}

KDockMainWindow::~KDockMainWindow()
{
	delete dockManager;
}

void KDockMainWindow::setMainDockWidget( KDockWidget* mdw )
{
  if ( mainDockWidget == mdw ) return;
  mainDockWidget = mdw;
}

void KDockMainWindow::setView( QWidget *view )
{
  if ( QString(view->staticMetaObject.className()) ==  QString("KDockWidget") ){
    if ( view->parent() != this ) ((KDockWidget*)view)->applyToWidget( this );
  }

  QMainWindow::setCentralWidget(view);
}

KDockWidget* KDockMainWindow::createDockWidget( const QString& name, const QPixmap &pixmap, QWidget* parent, const QString& strCaption, const QString& strTabPageLabel)
{
  return new KDockWidget( dockManager, name, pixmap, parent, strCaption, strTabPageLabel );
}

void KDockMainWindow::makeDockVisible( KDockWidget* dock )
{
  if ( dock != 0L)
    dock->makeDockVisible();
}

void KDockMainWindow::makeDockInvisible( KDockWidget* dock )
{
  if ( dock != 0L)
    dock->undock();
}

void KDockMainWindow::makeWidgetDockVisible( QWidget* widget )
{
  makeDockVisible( dockManager->findWidgetParentDock(widget) );
}

void KDockMainWindow::writeDockConfig(QDomElement &base)
{
  dockManager->writeConfig(base);
}

void KDockMainWindow::readDockConfig(QDomElement &base)
{
  dockManager->readConfig(base);
}

#ifndef NO_KDE2
void KDockMainWindow::writeDockConfig( KConfig* c, QString group )
{
  dockManager->writeConfig( c, group );
}

void KDockMainWindow::readDockConfig( KConfig* c, QString group )
{
  dockManager->readConfig( c, group );
}
#endif

void KDockMainWindow::slotDockWidgetUndocked()
{
  QObject* pSender = (QObject*) sender();
  if (!pSender->inherits("KDockWidget")) return;
  KDockWidget* pDW = (KDockWidget*) pSender;
  emit dockWidgetHasUndocked( pDW);
}

/*************************************************************************/
KDockWidgetAbstractHeaderDrag::KDockWidgetAbstractHeaderDrag( KDockWidgetAbstractHeader* parent, KDockWidget* dock, const QString name )
:QFrame( parent )
{
  setObjectName(name);
  dw = dock;
  installEventFilter( dock->dockManager() );
}
/*************************************************************************/
KDockWidgetHeaderDrag::KDockWidgetHeaderDrag( KDockWidgetAbstractHeader* parent, KDockWidget* dock, const QString name )
:KDockWidgetAbstractHeaderDrag( parent, dock, name )
{
}

void KDockWidgetHeaderDrag::paintEvent( QPaintEvent* )
{
  QPixmap drawBuffer( width(), height() );
  QPainter paint;

  paint.begin( &drawBuffer );
  paint.fillRect( drawBuffer.rect(), QBrush( palette().background()) );

  paint.setPen( palette().light().color() );
  paint.drawLine( 1, 3, 1, 2 );
  paint.drawLine( 1, 2, width(), 2 );

  paint.setPen( palette().mid().color() );
  paint.drawLine( 1, 4, width(), 4 );
  paint.drawLine( width(), 4, width(), 3 );

  paint.setPen( palette().light().color() );
  paint.drawLine( 1, 6, 1, 5 );
  paint.drawLine( 1, 5, width(), 5 );

  paint.setPen( palette().mid().color() );
  paint.drawLine( 1, 7, width(), 7 );
  paint.drawLine( width(), 7, width(), 6 );

  paint.drawPixmap(0,0,drawBuffer,0,0,width(),height());
  paint.end();
}
/*************************************************************************/
KDockWidgetAbstractHeader::KDockWidgetAbstractHeader( KDockWidget* parent, const QString name )
:QFrame( parent )
{
  setObjectName(name);
}
/*************************************************************************/
KDockWidgetHeader::KDockWidgetHeader( KDockWidget* parent, const char* name )
:KDockWidgetAbstractHeader( parent, name )
{
  layout = new QHBoxLayout( this );
  layout->setSizeConstraint( QLayout::SetMinimumSize );

  drag = new KDockWidgetHeaderDrag( this, parent );

  closeButton = new KDockButton_Private( this, "DockCloseButton" );
  closeButton->setIcon(QPixmap(close_xpm));
  closeButton->setFixedSize(9,9);
  connect( closeButton, SIGNAL(clicked()), parent, SIGNAL(headerCloseButtonClicked()));
  connect( closeButton, SIGNAL(clicked()), parent, SLOT(undock()));

  stayButton = new KDockButton_Private( this, "DockStayButton" );
  stayButton->setCheckable(true);
  stayButton->setIcon(QPixmap(not_close_xpm));
  stayButton->setFixedSize(9,9);
  connect( stayButton, SIGNAL(clicked()), this, SLOT(slotStayClicked()));

  dockbackButton = new KDockButton_Private( this, "DockbackButton" );
  dockbackButton->setIcon(QPixmap(dockback_xpm));
  dockbackButton->setFixedSize(9,9);
  connect( dockbackButton, SIGNAL(clicked()), parent, SIGNAL(headerDockbackButtonClicked()));
  connect( dockbackButton, SIGNAL(clicked()), parent, SLOT(dockBack()));

  layout->addWidget( drag );
  layout->addWidget( dockbackButton );
  layout->addWidget( stayButton );
  layout->addWidget( closeButton );
  layout->activate();
  drag->setFixedHeight( layout->minimumSize().height() );
}

void KDockWidgetHeader::setTopLevel( bool isTopLevel )
{
  if ( isTopLevel ){
    KDockWidget* par = (KDockWidget*)parent();
    if( par) {
      if( par->isDockBackPossible())
        dockbackButton->show();
      else
        dockbackButton->hide();
    }
    stayButton->hide();
    closeButton->hide();
    drag->setEnabled( true );
  } else {
    dockbackButton->hide();
    stayButton->show();
    closeButton->show();
  }
  layout->activate();
  updateGeometry();
}

void KDockWidgetHeader::setDragPanel( KDockWidgetHeaderDrag* nd )
{
  if ( !nd ) return;

  delete layout;
  layout = new QHBoxLayout( this );
  layout->setSizeConstraint( QLayout::SetMinimumSize );

  delete drag;
  drag = nd;

  layout->addWidget( drag );
  layout->addWidget( dockbackButton );
  layout->addWidget( stayButton );
  layout->addWidget( closeButton );
  layout->activate();
  drag->setFixedHeight( layout->minimumSize().height() );
}

void KDockWidgetHeader::slotStayClicked()
{
  setDragEnabled(!stayButton->isChecked());
}

bool KDockWidgetHeader::dragEnabled()
{
  return drag->isEnabled();
}

void KDockWidgetHeader::setDragEnabled(bool b)
{
  stayButton->setChecked(!b);
  closeButton->setEnabled(b);
  drag->setEnabled(b);
}

/*************************************************************************/
KDockWidget::KDockWidget( KDockManager* dockManager, const QString name, const QPixmap &pixmap, QWidget* parent, const QString& strCaption, const QString& strTabPageLabel, Qt::WindowFlags f)
: QWidget( parent, f )
  ,formerBrotherDockWidget(0L)
  ,currentDockPos(DockNone)
  ,formerDockPos(DockNone)
  ,pix(new QPixmap(pixmap))
  ,prevSideDockPosBeforeDrag(DockNone)
{
  setObjectName(name);
  d = new KDockWidgetPrivate();  // create private data

  d->_parent = parent;

  layout = new QVBoxLayout( this );
  layout->setSizeConstraint(QLayout::SetMinimumSize);

  manager = dockManager;
  manager->childDock->append( this );
  installEventFilter( manager );

  header = 0L;
  setHeader( new KDockWidgetHeader( this, "AutoCreatedDockHeader" ) );
  if( strCaption == 0L)
    setWindowTitle( name );
  else
    setWindowTitle( strCaption );

  if( strTabPageLabel == " ")
    setTabPageLabel( windowTitle() );
  else
    setTabPageLabel( strTabPageLabel);

  eDocking = DockFullDocking;
  sDocking = DockFullSite;

  isGroup = false;
  isTabGroup = false;

  setWindowIcon(pixmap);
  widget = 0L;

  QObject::connect(this, SIGNAL(hasUndocked()), manager->main, SLOT(slotDockWidgetUndocked()) );
  applyToWidget( parent, QPoint(0,0) );
}

KDockWidget::~KDockWidget()
{
  if ( !manager->undockProcess ){
    d->blockHasUndockedSignal = true;
    undock();
    d->blockHasUndockedSignal = false;
  }
  emit iMBeingClosed();
  manager->childDock->removeAll( this );
  delete pix;
  delete d; // destroy private data
}

void KDockWidget::setHeader( KDockWidgetAbstractHeader* h )
{
  if ( !h ) return;

  if ( header ){
    delete header;
    delete layout;
    header = h;
    layout = new QVBoxLayout( this );
    layout->setSizeConstraint( QLayout::SetMinimumSize );
    layout->addWidget( header );
     setWidget( widget );    
  } else {
    header = h;
    layout->addWidget( header );
  }
}

void KDockWidget::setEnableDocking( int pos )
{
  eDocking = pos;
  updateHeader();
}

void KDockWidget::updateHeader()
{
  if ( parent() ){
    if ( (parent() == manager->main) || isGroup || (eDocking == KDockWidget::DockNone) ){
      header->hide();
    } else {
      header->setTopLevel( false );
      header->show();
    }
  } else {
    header->setTopLevel( true );
    header->show();
  }
}

void KDockWidget::applyToWidget( QWidget* s, const QPoint& p )
{
  if ( parent() != s ){
    hide();
    this->setParent(s);
  }

  if ( s && s->inherits("KDockMainWindow") ){
    ((KDockMainWindow*)s)->setView( this );
  }

  if ( s == manager->main ){
      setGeometry( QRect(QPoint(0,0), manager->main->geometry().size()) );
  }

  if ( !s )
  {
    move(p);
  }
  updateHeader();

  setWindowIcon(*pix);
}

void KDockWidget::show()
{
  if ( parent() || manager->main->isVisible() ) {
      if (!parent()) {
          emit manager->setDockDefaultPos(this);
          emit setDockDefaultPos();
          if (parent()) {
              makeDockVisible();
          } else {
              QWidget::show();
          }
      } else {
          QWidget::show();
      }
  }
}

void KDockWidget::setDockWindowTransient (QWidget *parent, bool transientEnabled)
{
  d->_parent = parent;
  d->transient = transientEnabled;
  applyToWidget( parentWidget(), QPoint(0,0) );
}

bool KDockWidget::event( QEvent *event )
{
  switch ( event->type() )
  {
    #undef FocusIn
    case QEvent::FocusIn:
      if (widget && !d->pendingFocusInEvent) {
         d->pendingFocusInEvent = true;
         widget->setFocus();
      }
      d->pendingFocusInEvent = false;
      break;
    case QEvent::ChildRemoved:
      if ( widget == ((QChildEvent*)event)->child() ) widget = 0L;
      break;
    case QEvent::Show:
      if ( widget ) widget->show();
      emit manager->change();
      break;
    case QEvent::Hide:
      if ( widget ) widget->hide();
      emit manager->change();
      break;
    case QEvent::WindowTitleChange:
      if ( parentWidget() ){
        if ( parent()->inherits("KDockSplitter") ){
          ((KDockSplitter*)(parent()))->updateName();
        }
        if ( parentTabGroup() ){
          setDockTabName( parentTabGroup() );
          parentTabGroup()->setPageCaption( this, tabPageLabel() );
        }
      }
      break;
    case QEvent::Close:
      emit iMBeingClosed();
      break;
    default:
      break;
  }
  return QWidget::event( event );
}

KDockWidget* KDockWidget::manualDock( KDockWidget* target, DockPosition dockPos, int spliPos, QPoint pos, bool check, int tabIndex )
{
  if (this == target)
    return 0L;  // docking to itself not possible

  bool succes = true; // tested flag

  // check allowed this dock submit this operations
  if ( !(eDocking & (int)dockPos) ){
    succes = false;
  }

  // check allowed target submit this operations
  if ( target && !(target->sDocking & (int)dockPos) ){
    succes = false;
  }

  if ( parent() && !parent()->inherits("KDockSplitter") && !parentTabGroup() ){
    succes = false;
  }

  if ( !succes ){
    // try to make another manualDock
    KDockWidget* dock_result = 0L;
    if ( target && !check ){
      KDockWidget::DockPosition another__dockPos = KDockWidget::DockNone;
      switch ( dockPos ){
        case KDockWidget::DockLeft  : another__dockPos = KDockWidget::DockRight ; break;
        case KDockWidget::DockRight : another__dockPos = KDockWidget::DockLeft  ; break;
        case KDockWidget::DockTop   : another__dockPos = KDockWidget::DockBottom; break;
        case KDockWidget::DockBottom: another__dockPos = KDockWidget::DockTop   ; break;
        default: break;
      }
      dock_result = target->manualDock( this, another__dockPos, spliPos, pos, true, tabIndex );
    }
    return dock_result;
  }
  // end check block

  d->blockHasUndockedSignal = true;
  undock();
  d->blockHasUndockedSignal = false;

  if ( !target ){
    move( pos );
    show();
    emit manager->change();
    return this;
  }

  KDockTabGroup* parentTab = target->parentTabGroup();
  if ( parentTab ){
    // add to existing TabGroup
    applyToWidget( parentTab );
    parentTab->insertPage( this, tabPageLabel(), -1, tabIndex );
    parentTab->setPixmap( this,  !this->windowIcon().isNull() ? this->windowIcon().pixmap(QSize(22,22), QIcon::Normal) : QPixmap());
    setDockTabName( parentTab );
    if( !toolTipStr.isEmpty())
      parentTab->setToolTip( this, toolTipStr);

    currentDockPos = KDockWidget::DockCenter;
    emit manager->change();
    return (KDockWidget*)parentTab->parent();
  }

  // create a new dockwidget that will contain the target and this
  QWidget* parentDock = target->parentWidget();
  KDockWidget* newDock = new KDockWidget( manager, "tempName", QPixmap(""), parentDock );
  newDock->currentDockPos = target->currentDockPos;

  if ( dockPos == KDockWidget::DockCenter ){
    newDock->isTabGroup = true;
  } else {
    newDock->isGroup = true;
  }
  newDock->eDocking = (target->eDocking & eDocking) & (~(int)KDockWidget::DockCenter);

  newDock->applyToWidget( parentDock );

  if ( !parentDock ){
    // dock to a toplevel dockwidget means newDock is toplevel now
    newDock->move( target->frameGeometry().topLeft() );
    newDock->resize( target->geometry().size() );
    if ( target->isVisible() ) newDock->show();
  }

  // redirect the dockback button to the new dockwidget
  if( target->formerBrotherDockWidget != 0L) {
    newDock->formerBrotherDockWidget = target->formerBrotherDockWidget;
    if( formerBrotherDockWidget != 0L)
      QObject::connect( newDock->formerBrotherDockWidget, SIGNAL(iMBeingClosed()),
                        newDock, SLOT(loseFormerBrotherDockWidget()) );
      target->loseFormerBrotherDockWidget();
    }
  newDock->formerDockPos = target->formerDockPos;

  if ( dockPos == KDockWidget::DockCenter )
  {
    KDockTabGroup* tab = new KDockTabGroup( newDock, "_dock_tab");
    newDock->setWidget( tab );

    target->applyToWidget( tab );
    applyToWidget( tab );


    tab->insertPage( target, target->tabPageLabel() );
    tab->setPixmap( target, !target->windowIcon().isNull() ? (target->windowIcon().pixmap(QSize(22,22))) : QPixmap());
    if( !target->toolTipString().isEmpty())
      tab->setToolTip( target, target->toolTipString());

    tab->insertPage( this, tabPageLabel(), -1, tabIndex );
    tab->setPixmap( this, !windowIcon().isNull() ? windowIcon().pixmap(QSize(22,22)) : QPixmap());
    if( !toolTipString().isEmpty())
      tab->setToolTip( this, toolTipString());

    setDockTabName( tab );
    tab->show();

    currentDockPos = DockCenter;
    target->formerDockPos = target->currentDockPos;
    target->currentDockPos = DockCenter;
  }
  else {
    // if to dock not to the center of the target dockwidget,
    // dock to newDock
    KDockSplitter* panner = 0L;
    if ( dockPos == KDockWidget::DockTop  || dockPos == KDockWidget::DockBottom ) panner = new KDockSplitter( newDock, "_dock_split_", Qt::Horizontal, spliPos, manager->splitterHighResolution() );
    if ( dockPos == KDockWidget::DockLeft || dockPos == KDockWidget::DockRight  ) panner = new KDockSplitter( newDock, "_dock_split_", Qt::Vertical , spliPos, manager->splitterHighResolution() );
    newDock->setWidget( panner );

    panner->setOpaqueResize(manager->splitterOpaqueResize());
    panner->setKeepSize(manager->splitterKeepSize());
    panner->setFocusPolicy( Qt::NoFocus );
    target->applyToWidget( panner );
    applyToWidget( panner );
    target->formerDockPos = target->currentDockPos;
    if ( dockPos == KDockWidget::DockRight) {
      panner->activate( target, this );
      currentDockPos = KDockWidget::DockRight;
      target->currentDockPos = KDockWidget::DockLeft;
    }
    else if( dockPos == KDockWidget::DockBottom) {
      panner->activate( target, this );
      currentDockPos = KDockWidget::DockBottom;
      target->currentDockPos = KDockWidget::DockTop;
    }
    else if( dockPos == KDockWidget::DockTop) {
      panner->activate( this, target );
      currentDockPos = KDockWidget::DockTop;
      target->currentDockPos = KDockWidget::DockBottom;
    }
    else if( dockPos == KDockWidget::DockLeft) {
      panner->activate( this, target );
      currentDockPos = KDockWidget::DockLeft;
      target->currentDockPos = KDockWidget::DockRight;
    }
    target->show();
    show();
    panner->show();
  }

  if ( parentDock ){
    if ( parentDock->inherits("KDockSplitter") ){
      KDockSplitter* sp = (KDockSplitter*)parentDock;
      sp->deactivate();
      if ( sp->getFirst() == target )
        sp->activate( newDock, 0L );
      else
        sp->activate( 0L, newDock );
    }
  }

  newDock->show();
  emit target->docking( this, dockPos );
  emit manager->replaceDock( target, newDock );
  emit manager->change();

  return newDock;
}

KDockTabGroup* KDockWidget::parentTabGroup()
{
  if ( !parent() ) return 0L;
  QWidget* candidate = parentWidget()->parentWidget();
  if ( candidate && candidate->inherits("KDockTabGroup") ) return (KDockTabGroup*)candidate;
  return 0L;
}

void KDockWidget::undock()
{
  QWidget* parentW = parentWidget();
  if ( !parentW ){
    hide();
    if (!d->blockHasUndockedSignal)
      emit hasUndocked();
    return;
  }

  formerDockPos = currentDockPos;
  currentDockPos = KDockWidget::DockDesktop;

  manager->blockSignals(true);
  manager->undockProcess = true;

  bool isV = parentW->isVisible();

  KDockTabGroup* parentTab = parentTabGroup();
  if ( parentTab ){
    d->index = parentTab->index( this); // memorize the page position in the tab widget
    parentTab->removePage( this );
    formerBrotherDockWidget = (KDockWidget*)parentTab->getFirstPage();
    QObject::connect( formerBrotherDockWidget, SIGNAL(iMBeingClosed()),
                      this, SLOT(loseFormerBrotherDockWidget()) );
    applyToWidget( 0L );
    if ( parentTab->pageCount() == 1 ){

      // last subdock widget in the tab control
      KDockWidget* lastTab = (KDockWidget*)parentTab->getFirstPage();
      parentTab->removePage( lastTab );
      lastTab->applyToWidget( 0L );
      lastTab->move( parentTab->mapToGlobal(parentTab->frameGeometry().topLeft()) );

      // KDockTabGroup always have a parent that is a KDockWidget
      KDockWidget* parentOfTab = (KDockWidget*)parentTab->parent();
      delete parentTab; // KDockTabGroup

      QWidget* parentOfDockWidget = parentOfTab->parentWidget();
      if ( parentOfDockWidget == 0L ){
          if ( isV ) lastTab->show();
      } else {
        if ( parentOfDockWidget->inherits("KDockSplitter") ){
          KDockSplitter* split = (KDockSplitter*)parentOfDockWidget;
          lastTab->applyToWidget( split );
          split->deactivate();
          if ( split->getFirst() == parentOfTab ){
            split->activate( lastTab );
            if ( ((KDockWidget*)split->parent())->splitterOrientation == Qt::Vertical )
              emit ((KDockWidget*)split->getAnother(parentOfTab))->docking( parentOfTab, KDockWidget::DockLeft );
            else
              emit ((KDockWidget*)split->getAnother(parentOfTab))->docking( parentOfTab, KDockWidget::DockTop );
          } else {
            split->activate( 0L, lastTab );
            if ( ((KDockWidget*)split->parent())->splitterOrientation == Qt::Vertical )
              emit ((KDockWidget*)split->getAnother(parentOfTab))->docking( parentOfTab, KDockWidget::DockRight );
            else
              emit ((KDockWidget*)split->getAnother(parentOfTab))->docking( parentOfTab, KDockWidget::DockBottom );
          }
          split->show();
        } else {
          lastTab->applyToWidget( parentOfDockWidget );
        }
        lastTab->show();
      }
      manager->blockSignals(false);
      emit manager->replaceDock( parentOfTab, lastTab );
      lastTab->currentDockPos = parentOfTab->currentDockPos;
      emit parentOfTab->iMBeingClosed();
      manager->blockSignals(true);
      delete parentOfTab;

    } else {
      setDockTabName( parentTab );
    }
  } else {
/*********************************************************************************************/
    if ( parentW->inherits("KDockSplitter") ){
      KDockSplitter* parentSplitterOfDockWidget = (KDockSplitter*)parentW;
      d->splitPosInPercent = parentSplitterOfDockWidget->separatorPos();

      KDockWidget* secondWidget = (KDockWidget*)parentSplitterOfDockWidget->getAnother( this );
      KDockWidget* group        = (KDockWidget*)parentSplitterOfDockWidget->parentWidget();
      formerBrotherDockWidget = secondWidget;
      applyToWidget( 0L );
      group->hide();

      if( formerBrotherDockWidget != 0L)
        QObject::connect( formerBrotherDockWidget, SIGNAL(iMBeingClosed()),
                          this, SLOT(loseFormerBrotherDockWidget()) );

      if ( !group->parentWidget() ){
        secondWidget->applyToWidget( 0L, group->frameGeometry().topLeft() );
        secondWidget->resize( group->width(), group->height() );
      } else {
        QWidget* obj = group->parentWidget();
        secondWidget->applyToWidget( obj );
        if ( obj->inherits("KDockSplitter") ){
          KDockSplitter* parentOfGroup = (KDockSplitter*)obj;
          parentOfGroup->deactivate();

          if ( parentOfGroup->getFirst() == group )
            parentOfGroup->activate( secondWidget );
          else
            parentOfGroup->activate( 0L, secondWidget );
        }
      }
      secondWidget->currentDockPos = group->currentDockPos;
      secondWidget->formerDockPos  = group->formerDockPos;
      delete parentSplitterOfDockWidget;
      manager->blockSignals(false);
      emit manager->replaceDock( group, secondWidget );
      emit group->iMBeingClosed();
      manager->blockSignals(true);
      delete group;

      if ( isV ) secondWidget->show();
    } else {
      applyToWidget( 0L );
    }
/*********************************************************************************************/
  }
  manager->blockSignals(false);
  if (!d->blockHasUndockedSignal)
    emit manager->change();
  manager->undockProcess = false;

  if (!d->blockHasUndockedSignal)
    emit hasUndocked();
}

void KDockWidget::setWidget( QWidget* mw )
{
  if ( !mw ) return;

  if ( mw->parent() != this ){
    mw->setParent(this);
  }

  widget = mw;
  delete layout;

  layout = new QVBoxLayout( this );
  layout->setSizeConstraint( QLayout::SetMinimumSize );

  layout->addWidget( header );
  layout->addWidget( widget,1 );
}

void KDockWidget::setDockTabName( KDockTabGroup* tab )
{
  QString listOfName;
  QString listOfCaption;
  for ( QWidget* w = tab->getFirstPage(); w; w = tab->getNextPage( w ) ){
    listOfCaption.append( w->windowTitle() ).append(",");
    listOfName.append( w->objectName() ).append(",");
  }
  listOfCaption.remove( listOfCaption.length()-1, 1 );
  listOfName.remove( listOfName.length()-1, 1 );

  tab->parentWidget()->setObjectName( listOfName );
  tab->parentWidget()->setWindowTitle( listOfCaption );

  tab->parentWidget()->repaint(); // KDockWidget->repaint
  if ( tab->parentWidget()->parent() )
    if ( tab->parentWidget()->parent()->inherits("KDockSplitter") )
      ((KDockSplitter*)(tab->parentWidget()->parent()))->updateName();
}

bool KDockWidget::mayBeHide()
{
  bool f = (parent() != manager->main);
  return ( !isGroup && !isTabGroup && f && isVisible() && ( eDocking != (int)KDockWidget::DockNone ) );
}

bool KDockWidget::mayBeShow()
{
  bool f = (parent() != manager->main);
  return ( !isGroup && !isTabGroup && f && !isVisible() );
}

void KDockWidget::changeHideShowState()
{
  if ( mayBeHide() ){
    undock();
    return;
  }

  if ( mayBeShow() ){
    if ( manager->main->inherits("KDockMainWindow") ){
      ((KDockMainWindow*)manager->main)->makeDockVisible(this);
    } else {
      makeDockVisible();
    }
  }
}

void KDockWidget::makeDockVisible()
{
  if ( parentTabGroup() ){
    parentTabGroup()->setVisiblePage( this );
  }
  if ( isVisible() ) return;

  QWidget* p = parentWidget();
  while ( p ){
    if ( !p->isVisible() )
      p->show();
    p = p->parentWidget();
  }
  if( parent() == 0L) // is undocked
    dockBack();
  show();
}

void KDockWidget::loseFormerBrotherDockWidget()
{
  if( formerBrotherDockWidget != 0L)
    QObject::disconnect( formerBrotherDockWidget, SIGNAL(iMBeingClosed()),
                         this, SLOT(loseFormerBrotherDockWidget()) );
  formerBrotherDockWidget = 0L;
  repaint();
}

void KDockWidget::dockBack()
{
  if( formerBrotherDockWidget) {
    // search all children if it tries to dock back to a child
    bool found = false;

    QList<KDockWidget*> cl = this->findChildren<KDockWidget*>();
    foreach(QWidget *widg, cl) {
      if (widg==formerBrotherDockWidget) {
          found = true;
          break;
      }
    }

    if( !found) {
      // can dock back to the former brother dockwidget
      manualDock( formerBrotherDockWidget, formerDockPos, d->splitPosInPercent, QPoint(0,0), false, d->index);
      formerBrotherDockWidget = 0L;
      makeDockVisible();
      return;
    }
  }

  // else dockback to the dockmainwindow (default behaviour)
  manualDock( ((KDockMainWindow*)manager->main)->getMainDockWidget(), formerDockPos, d->splitPosInPercent, QPoint(0,0), false, d->index);
  formerBrotherDockWidget = 0L;
  if (parent())
    makeDockVisible();
}

bool KDockWidget::isDockBackPossible()
{
  if( (formerBrotherDockWidget == 0L) || !(formerBrotherDockWidget->dockSite() & formerDockPos))
    return false;
  else
    return true;
}

/**************************************************************************************/

class KDockManager::KDockManagerPrivate
{
public:
  /**
   * This rectangle is used to highlight the current dockposition. It stores global screen coordinates.
   */
  QRect dragRect;

  /**
   * This rectangle is used to erase the previously highlighted dockposition. It stores global screen coordinates.
   */
  QRect oldDragRect;

  /**
   * This flag stores the information if dragging is ready to start. Used between mousePress and mouseMove event.
   */
  bool readyToDrag;

  /**
   * This variable stores the offset of the mouse cursor to the upper left edge of the current drag widget.
   */
  QPoint dragOffset;

  /**
   * These flags store information about the splitter behaviour
   */
  bool splitterOpaqueResize;
  bool splitterKeepSize;
  bool splitterHighResolution;
};

KDockManager::KDockManager( QWidget* mainWindow , const QString name )
:QObject( 0 )
  ,main(mainWindow)
  ,currentDragWidget(0L)
  ,currentMoveWidget(0L)
  ,childDockWidgetList(0L)
  ,autoCreateDock(0L)
  ,storeW(0)
  ,storeH(0)
  ,draging(false)
  ,undockProcess(false)
  ,dropCancel(true)
{
  setObjectName(name);
  d = new KDockManagerPrivate;
  d->splitterOpaqueResize = false;
  d->splitterKeepSize = false;
  d->splitterHighResolution = false;

  main->installEventFilter( this );

  undockProcess = false;

  menuData = new QList<MenuDockData*>;

  menu = new QMenu();

  connect( menu, SIGNAL(aboutToShow()), SLOT(slotMenuPopup()) );
  connect( menu, SIGNAL(activated(int)), SLOT(slotMenuActivated(int)) );

  childDock = new QObjectList();
}

KDockManager::~KDockManager()
{
  qDeleteAll(*menuData);
  delete menuData;
  delete menu;

  qDeleteAll(*childDock);
  delete childDock;
  delete d;
}

void KDockManager::activate()
{
  foreach (QObject * obj, *childDock) {
    KDockWidget * dw;
    dw = dynamic_cast<KDockWidget *>(obj);
    if (dw==NULL) continue;

    if ( dw->widget ) dw->widget->show();
    if ( !dw->parentTabGroup() ){
        dw->show();
    }
  }

  if ( !main->inherits("QDialog") ) main->show();
}

bool KDockManager::eventFilter( QObject *obj, QEvent *event )
{
/*  if ( obj == main && event->type() == QEvent::Resize && main->children() ){
    QWidget* fc = (QWidget*)main->children()->getFirst();
    if ( fc )
      fc->setGeometry( QRect(QPoint(0,0), main->geometry().size()) );
  }
*/
  if ( obj->inherits("KDockWidgetAbstractHeaderDrag") ){
    KDockWidget* pDockWdgAtCursor = 0L;
    KDockWidget* curdw = ((KDockWidgetAbstractHeaderDrag*)obj)->dockWidget();
    switch ( event->type() ){
      case QEvent::MouseButtonPress:
        if ( ((QMouseEvent*)event)->button() == Qt::LeftButton ){
          if ( curdw->eDocking != (int)KDockWidget::DockNone ){
            dropCancel = true;
            curdw->setFocus();
            qApp->processEvents(QEventLoop::WaitForMoreEvents);

            currentDragWidget = curdw;
            currentMoveWidget = 0L;
            childDockWidgetList = new WidgetList();
            childDockWidgetList->append( curdw );
            findChildDockWidget( curdw, childDockWidgetList );

            d->oldDragRect = QRect();
            d->dragRect = QRect(curdw->geometry());
            QPoint p = curdw->mapToGlobal(QPoint(0,0));
            d->dragRect.moveTopLeft(p);
            drawDragRectangle();
            d->readyToDrag = true;

            d->dragOffset = QCursor::pos()-currentDragWidget->mapToGlobal(QPoint(0,0));
          }
        }
        break;
      case QEvent::MouseButtonRelease:
        if ( ((QMouseEvent*)event)->button() == Qt::LeftButton ){
          if ( draging ){
            if ( !dropCancel )
              drop();
            else
              cancelDrop();
          }
          if (d->readyToDrag) {
              d->readyToDrag = false;
              d->oldDragRect = QRect();
              d->dragRect = QRect(curdw->geometry());
              QPoint p = curdw->mapToGlobal(QPoint(0,0));
              d->dragRect.moveTopLeft(p);
              drawDragRectangle();
              currentDragWidget = 0L;
              delete childDockWidgetList;
              childDockWidgetList = 0L;
          }
          draging = false;
          dropCancel = true;
        }
        break;
      case QEvent::MouseMove:
        if ( draging ) {
          pDockWdgAtCursor = findDockWidgetAt( QCursor::pos() );
          KDockWidget* oldMoveWidget = currentMoveWidget;
          if ( currentMoveWidget  && pDockWdgAtCursor == currentMoveWidget ) { //move
            dragMove( currentMoveWidget, currentMoveWidget->mapFromGlobal( QCursor::pos() ) );
            break;
          } else {
            if (dropCancel && curdw) {
              d->dragRect = QRect(curdw->geometry());
              QPoint p = curdw->mapToGlobal(QPoint(0,0));
              d->dragRect.moveTopLeft(p);
            }else
              d->dragRect = QRect();

            drawDragRectangle();
          }

          if ( !pDockWdgAtCursor && (curdw->eDocking & (int)KDockWidget::DockDesktop) == 0 ){
              // just moving at the desktop
              currentMoveWidget = pDockWdgAtCursor;
              curPos = KDockWidget::DockDesktop;
          } else {
            if ( oldMoveWidget && pDockWdgAtCursor != currentMoveWidget ) { //leave
              currentMoveWidget = pDockWdgAtCursor;
              curPos = KDockWidget::DockDesktop;
            }
          }

          if ( oldMoveWidget != pDockWdgAtCursor && pDockWdgAtCursor ) { //enter pDockWdgAtCursor
            currentMoveWidget = pDockWdgAtCursor;
            curPos = KDockWidget::DockDesktop;
          }
        } else {
          if (d->readyToDrag) {
            d->readyToDrag = false;
          }
          if ( (((QMouseEvent*)event)->button() == Qt::LeftButton) &&
               (curdw->eDocking != (int)KDockWidget::DockNone) ) {
            startDrag( curdw);
          }
        }
        break;
      default:
        break;
    }
  }
  return QObject::eventFilter( obj, event );
}

KDockWidget* KDockManager::findDockWidgetAt( const QPoint& pos )
{
  dropCancel = true;

  if (!currentDragWidget)
    return 0L; // pointer access safety

  if (currentDragWidget->eDocking == (int)KDockWidget::DockNone ) return 0L;

  QWidget* p = QApplication::widgetAt( pos );
  if ( !p ) {
    dropCancel = false;
    return 0L;
  }
#if defined(_OS_WIN32_) || defined(Q_OS_WIN32)
  p = p->topLevelWidget();
#endif
  QWidget* w = 0L;
  findChildDockWidget( w, p, p->mapFromGlobal(pos) );
  if ( !w ){
    if ( !p->inherits("KDockWidget") ) {
      return 0L;
    }
    w = p;
  }
  if (w->findChild<KDockSplitter*>("_dock_split_") == NULL) return 0L;
  if (w->findChild<KDockTabGroup*>("_dock_tab") == NULL) return 0L;
  if (!childDockWidgetList) return 0L;
  if ( !childDockWidgetList->contains(w) ) return 0L;
  if ( currentDragWidget->isGroup && ((KDockWidget*)w)->parentTabGroup() ) return 0L;

  KDockWidget* www = (KDockWidget*)w;
  if ( www->sDocking == (int)KDockWidget::DockNone ) return 0L;

  KDockWidget::DockPosition curPos = KDockWidget::DockDesktop;
  QPoint cpos  = www->mapFromGlobal( pos );

  int ww = www->widget->width() / 3;
  int hh = www->widget->height() / 3;

  if ( cpos.y() <= hh ){
    curPos = KDockWidget::DockTop;
  } else
    if ( cpos.y() >= 2*hh ){
      curPos = KDockWidget::DockBottom;
    } else
      if ( cpos.x() <= ww ){
        curPos = KDockWidget::DockLeft;
      } else
        if ( cpos.x() >= 2*ww ){
          curPos = KDockWidget::DockRight;
        } else
            curPos = KDockWidget::DockCenter;

  if ( !(www->sDocking & (int)curPos) ) return 0L;
  if ( !(currentDragWidget->eDocking & (int)curPos) ) return 0L;
  if ( www->manager != this ) return 0L;

  dropCancel = false;
  return www;
}

void KDockManager::findChildDockWidget( QWidget*& ww, const QWidget* p, const QPoint& pos )
{
  QObjectList ol = p->children();
  if (ol.isEmpty()) return;
  QObjectList::reverse_iterator it;
  for (it = ol.rbegin(); it != ol.rend(); ++it) {
      QObject *obj = *it;
      if ( !obj->isWidgetType() ) continue;
      QWidget *w = (QWidget*)*it;
      if ( w->isVisible() && w->geometry().contains(pos) ) {
          if ( w->inherits("KDockWidget") ) ww = w;
          findChildDockWidget( ww, w, w->mapFromParent(pos) );
          return;
      }
  }
  return;
}

void KDockManager::findChildDockWidget( const QWidget* p, WidgetList*& list )
{
  QObjectList ol = p->children();
  if (ol.isEmpty()) return;
  QObjectList::reverse_iterator it;
  for (it = ol.rbegin(); it != ol.rend(); ++it) {
       QObject *obj = *it;
        if ( obj->isWidgetType() ) {
            QWidget *w = (QWidget*)*it;
            if ( w->isVisible() ) {
                if ( w->inherits("KDockWidget") ) list->append( w );
                findChildDockWidget( w, list );
            }
        }
  }
  return;
}

void KDockManager::startDrag( KDockWidget* w )
{
  if(( w->currentDockPos == KDockWidget::DockLeft) || ( w->currentDockPos == KDockWidget::DockRight)
   || ( w->currentDockPos == KDockWidget::DockTop) || ( w->currentDockPos == KDockWidget::DockBottom)) {
    w->prevSideDockPosBeforeDrag = w->currentDockPos;

    if ( w->parentWidget()->inherits("KDockSplitter") ){
      KDockSplitter* parentSplitterOfDockWidget = (KDockSplitter*)(w->parentWidget());
      w->d->splitPosInPercent = parentSplitterOfDockWidget->separatorPos();
    }
  }

  curPos = KDockWidget::DockDesktop;
  draging = true;

  QApplication::setOverrideCursor(QCursor(Qt::SizeAllCursor));
}

void KDockManager::dragMove( KDockWidget* dw, QPoint pos )
{
  QPoint p = dw->mapToGlobal( dw->widget->pos() );
  KDockWidget::DockPosition oldPos = curPos;

  QSize r = dw->widget->size();
  if ( dw->parentTabGroup() ){
    curPos = KDockWidget::DockCenter;
    if ( oldPos != curPos ) {
      d->dragRect.setRect( p.x()+2, p.y()+2, r.width()-4, r.height()-4 );
    }
    return;
  }

  int w = r.width() / 3;
  int h = r.height() / 3;

  if ( pos.y() <= h ){
    curPos = KDockWidget::DockTop;
    w = r.width();
  } else
    if ( pos.y() >= 2*h ){
      curPos = KDockWidget::DockBottom;
      p.setY( p.y() + 2*h );
      w = r.width();
    } else
      if ( pos.x() <= w ){
        curPos = KDockWidget::DockLeft;
        h = r.height();
      } else
        if ( pos.x() >= 2*w ){
          curPos = KDockWidget::DockRight;
          p.setX( p.x() + 2*w );
          h = r.height();
        } else
          {
            curPos = KDockWidget::DockCenter;
            p.setX( p.x() + w );
            p.setY( p.y() + h );
          }

  if ( oldPos != curPos ) {
    d->dragRect.setRect( p.x(), p.y(), w, h );
    drawDragRectangle();
  }
}


void KDockManager::cancelDrop()
{
  QApplication::restoreOverrideCursor();

  delete childDockWidgetList;
  childDockWidgetList = 0L;

  d->dragRect = QRect();  // cancel drawing
  drawDragRectangle();    // only the old rect will be deleted
}


void KDockManager::drop()
{
  QApplication::restoreOverrideCursor();

  delete childDockWidgetList;
  childDockWidgetList = 0L;

  if ( dropCancel ) return;
  if ( !currentMoveWidget && ((currentDragWidget->eDocking & (int)KDockWidget::DockDesktop) == 0) ) {
    d->dragRect = QRect();  // cancel drawing
    drawDragRectangle();    // only the old rect will be deleted
    return;
  }
  if ( !currentMoveWidget && !currentDragWidget->parent() ) {
    currentDragWidget->move( QCursor::pos() - d->dragOffset );
  }
  else {
    int splitPos = currentDragWidget->d->splitPosInPercent;
    // do we have to calculate 100%-splitPosInPercent?
    if( (curPos != currentDragWidget->prevSideDockPosBeforeDrag) && (curPos != KDockWidget::DockCenter) && (curPos != KDockWidget::DockDesktop)) {
      switch( currentDragWidget->prevSideDockPosBeforeDrag) {
      case KDockWidget::DockLeft:   if(curPos != KDockWidget::DockTop)    splitPos = 100-splitPos; break;
      case KDockWidget::DockRight:  if(curPos != KDockWidget::DockBottom) splitPos = 100-splitPos; break;
      case KDockWidget::DockTop:    if(curPos != KDockWidget::DockLeft)   splitPos = 100-splitPos; break;
      case KDockWidget::DockBottom: if(curPos != KDockWidget::DockRight)  splitPos = 100-splitPos; break;
      default: break;
      }
    }
    currentDragWidget->manualDock( currentMoveWidget, curPos , splitPos, QCursor::pos() - d->dragOffset );
    currentDragWidget->makeDockVisible();
  }
}


static QDomElement createStringEntry(QDomDocument &doc, const QString &tagName, const QString &str)
{
    QDomElement el = doc.createElement(tagName);

    el.appendChild(doc.createTextNode(str));
    return el;
}


static QDomElement createBoolEntry(QDomDocument &doc, const QString &tagName, bool b)
{
    return createStringEntry(doc, tagName, QString::fromLatin1(b? "true" : "false"));
}


static QDomElement createNumberEntry(QDomDocument &doc, const QString &tagName, int n)
{
    return createStringEntry(doc, tagName, QString::number(n));
}


static QDomElement createRectEntry(QDomDocument &doc, const QString &tagName, const QRect &rect)
{
    QDomElement el = doc.createElement(tagName);

    QDomElement xel = doc.createElement("x");
    xel.appendChild(doc.createTextNode(QString::number(rect.x())));
    el.appendChild(xel);
    QDomElement yel = doc.createElement("y");
    yel.appendChild(doc.createTextNode(QString::number(rect.y())));
    el.appendChild(yel);
    QDomElement wel = doc.createElement("width");
    wel.appendChild(doc.createTextNode(QString::number(rect.width())));
    el.appendChild(wel);
    QDomElement hel = doc.createElement("height");
    hel.appendChild(doc.createTextNode(QString::number(rect.height())));
    el.appendChild(hel);

    return el;
}


static QDomElement createListEntry(QDomDocument &doc, const QString &tagName,
                                   const QString &subTagName, const QStringList &list)
{
    QDomElement el = doc.createElement(tagName);

    foreach(QString str, list) {
        QDomElement subel = doc.createElement(subTagName);
        subel.appendChild(doc.createTextNode(str));
        el.appendChild(subel);
    }

    return el;
}


static QString stringEntry(QDomElement &base, const QString &tagName)
{
    return base.namedItem(tagName).firstChild().toText().data();
}


static bool boolEntry(QDomElement &base, const QString &tagName)
{
    return base.namedItem(tagName).firstChild().toText().data() == "true";
}


static int numberEntry(QDomElement &base, const QString &tagName)
{
    return stringEntry(base, tagName).toInt();
}


static QRect rectEntry(QDomElement &base, const QString &tagName)
{
    QDomElement el = base.namedItem(tagName).toElement();

    int x = numberEntry(el, "x");
    int y = numberEntry(el, "y");
    int width = numberEntry(el, "width");
    int height = numberEntry(el,  "height");
    
    return QRect(x, y, width, height);
}


static QStringList listEntry(QDomElement &base, const QString &tagName, const QString &subTagName)
{
    QStringList list;
    
    QDomElement subel = base.namedItem(tagName).firstChild().toElement();
    while (!subel.isNull()) {
        if (subel.tagName() == subTagName)
            list.append(subel.firstChild().toText().data());
        subel = subel.nextSibling().toElement();
    }

    return list;
}


void KDockManager::writeConfig(QDomElement &base)
{
    // First of all, clear the tree under base
    while (!base.firstChild().isNull())
        base.removeChild(base.firstChild());
    QDomDocument doc = base.ownerDocument();

    QStringList nameList;
    QString mainWidgetStr;

    // collect widget names
    QStringList nList;
    foreach(QObject *obj,*childDock) {
        KDockWidget *obj1 = dynamic_cast<KDockWidget *>(obj);
        if (obj1==NULL) continue;
        if ( obj1->parent() == main )
                mainWidgetStr = obj1->objectName();
        nList.append(obj1->objectName());
    }


    nList.first();
    foreach(QString str, nList) {
        KDockWidget *obj = getDockWidgetFromName( str );
        if (obj->isGroup && (!nameList.contains(obj->firstName)
                             || !nameList.contains(obj->lastName))) {
            // Skip until children are saved (why?)
            continue;
        }

        QDomElement groupEl;
        
        if (obj->isGroup) {
            //// Save a group
            groupEl = doc.createElement("splitGroup");
            
            groupEl.appendChild(createStringEntry(doc, "firstName", obj->firstName));
            groupEl.appendChild(createStringEntry(doc, "secondName", obj->lastName));
            groupEl.appendChild(createNumberEntry(doc, "orientation", (int)obj->splitterOrientation));
            groupEl.appendChild(createNumberEntry(doc, "separatorPos", ((KDockSplitter*)obj->widget)->separatorPos()));
        } else if (obj->isTabGroup) {
            //// Save a tab group
            groupEl = doc.createElement("tabGroup");

            QStringList list;
            for ( QWidget *w = ((KDockTabGroup*)obj->widget)->getFirstPage();
                  w;
                  w = ((KDockTabGroup*)obj->widget)->getNextPage(w) ) {
                list.append( w->objectName() );
            }
            groupEl.appendChild(createListEntry(doc, "tabs", "tab", list));
            groupEl.appendChild(createNumberEntry(doc, "currentTab", ((KDockTabGroup*)obj->widget)->visiblePageId()));
        } else {
            //// Save an ordinary dock widget
            groupEl = doc.createElement("dock");
        }
        
        groupEl.appendChild(createStringEntry(doc, "name", obj->objectName()));
        groupEl.appendChild(createBoolEntry(doc, "hasParent", obj->parent()==NULL?false:true));
        if ( !obj->parent() ) {
            groupEl.appendChild(createRectEntry(doc, "geometry", QRect(main->frameGeometry().topLeft(), main->size())));
            groupEl.appendChild(createBoolEntry(doc, "visible", obj->isVisible()));
        }
        if (obj->header && obj->header->inherits("KDockWidgetHeader")) {
            KDockWidgetHeader *h = static_cast<KDockWidgetHeader*>(obj->header);
            groupEl.appendChild(createBoolEntry(doc, "dragEnabled", h->dragEnabled()));
        }
        
        base.appendChild(groupEl);    
        nameList.append(obj->objectName());
    }

    if (main->inherits("KDockMainWindow")) {
        KDockMainWindow *dmain = (KDockMainWindow*)main;
        QString centralWidgetStr = QString(dmain->centralWidget()? dmain->centralWidget()->objectName() : "");
        base.appendChild(createStringEntry(doc, "centralWidget", centralWidgetStr));
        QString mainDockWidgetStr = QString(dmain->getMainDockWidget()? dmain->getMainDockWidget()->objectName() : "");
        base.appendChild(createStringEntry(doc, "mainDockWidget", mainDockWidgetStr));
    } else {
        base.appendChild(createStringEntry(doc, "mainWidget", mainWidgetStr));
    }

    base.appendChild(createRectEntry(doc, "geometry", QRect(main->frameGeometry().topLeft(), main->size())));
}


void KDockManager::readConfig(QDomElement &base)
{
    if (base.namedItem("group").isNull()
        && base.namedItem("tabgroup").isNull()
        && base.namedItem("dock").isNull()) {
        activate();
        return;
    }

    autoCreateDock = new QObjectList();

    bool isMainVisible = main->isVisible();
    main->hide();

    foreach(QObject *obj, *childDock) {
        KDockWidget *obj1 = dynamic_cast<KDockWidget *>(obj);
        if (obj1==NULL) continue;
        if ( !obj1->isGroup && !obj1->isTabGroup ) {
          if ( obj1->parent() )
            obj1->undock();
          else
            obj1->hide();
        }
    }


    QDomElement childEl = base.firstChild().toElement();
    while (!childEl.isNull() ) {
        KDockWidget *obj = 0;

        if (childEl.tagName() == "splitGroup") {
            // Read a group
            QString name = stringEntry(childEl, "name");
            QString firstName = stringEntry(childEl, "firstName");
            QString secondName = stringEntry(childEl, "secondName");
            int orientation = numberEntry(childEl, "orientation");
            int separatorPos = numberEntry(childEl, "separatorPos");

            KDockWidget *first = getDockWidgetFromName(firstName);
            KDockWidget *second = getDockWidgetFromName(secondName);
            if (first && second) {
                obj = first->manualDock(second,
                                        (orientation == (int)Qt::Vertical)? KDockWidget::DockLeft : KDockWidget::DockTop,
                                        separatorPos);
                if (obj)
                    obj->setObjectName(name);
            }
        } else if (childEl.tagName() == "tabGroup") {
            // Read a tab group
            QString name = stringEntry(childEl, "name");
            QStringList list = listEntry(childEl, "tabs", "tab");
            QStringList::iterator it;
            it=list.begin();

            KDockWidget *d1 = getDockWidgetFromName( *it );
            ++it;
            KDockWidget *d2 = getDockWidgetFromName( *it );
            
            KDockWidget *obj = d2->manualDock( d1, KDockWidget::DockCenter );
            if (obj) {
                KDockTabGroup *tab = (KDockTabGroup*)obj->widget;
                ++it;
                while (it!=list.end() && obj) {
                    KDockWidget *tabDock = getDockWidgetFromName(*it);
                    obj = tabDock->manualDock(d1, KDockWidget::DockCenter);
                    ++it;
                }
                if (obj) {
                    obj->setObjectName(name);
                    tab->setVisiblePage(numberEntry(childEl, "currentTab"));
                }
            }
        } else if (childEl.tagName() == "dock") {
            // Read an ordinary dock widget
            obj = getDockWidgetFromName(stringEntry(childEl, "name"));
        }
        
        if (!boolEntry(childEl, "hasParent")) {
            QRect r = rectEntry(childEl, "geometry");
            obj = getDockWidgetFromName(stringEntry(childEl, "name"));
            obj->applyToWidget(0);
            obj->setGeometry(r);
            if (boolEntry(childEl, "visible"))
                obj->QWidget::show();
        }
        
        if (obj && obj->header && obj->header->inherits("KDockWidgetHeader")) {
            KDockWidgetHeader *h = static_cast<KDockWidgetHeader*>(obj->header);
            h->setDragEnabled(boolEntry(childEl, "dragEnabled"));
        }
        
        childEl = childEl.nextSibling().toElement();
    }

    if (main->inherits("KDockMainWindow")) {
        KDockMainWindow *dmain = (KDockMainWindow*)main;
        
        QString mv = stringEntry(base, "centralWidget");
        if (!mv.isEmpty() && getDockWidgetFromName(mv) ) {
            KDockWidget *mvd  = getDockWidgetFromName(mv);
            mvd->applyToWidget(dmain);
            mvd->show();
            dmain->setCentralWidget(mvd);
        }
        QString md = stringEntry(base, "mainDockWidget");
        if (!md.isEmpty() && getDockWidgetFromName(md)) {
            KDockWidget *mvd  = getDockWidgetFromName(md);
            dmain->setMainDockWidget(mvd);
        }
    } else {
        QString mv = stringEntry(base, "mainWidget");
        if (!mv.isEmpty() && getDockWidgetFromName(mv)) {
            KDockWidget *mvd  = getDockWidgetFromName(mv);
            mvd->applyToWidget(main);
            mvd->show();
        }
    }

    QRect mr = rectEntry(base, "geometry");
    main->setGeometry(mr);
    if (isMainVisible)
        main->show();

    qDeleteAll(*autoCreateDock);
    delete autoCreateDock;
    autoCreateDock = 0;
}

KDockWidget* KDockManager::getDockWidgetFromName( const QString& dockName )
{
  foreach(QObject *o, *childDock) {
      KDockWidget * obj = dynamic_cast<KDockWidget *>(o);
      if ( obj->objectName() == dockName ) return obj;
  }

  KDockWidget* autoCreate = 0L;
  if ( autoCreateDock ){
    autoCreate = new KDockWidget( this, dockName, QPixmap("") );
    autoCreateDock->append( autoCreate );
  }
  return autoCreate;
}
void KDockManager::setSplitterOpaqueResize(bool b)
{
  d->splitterOpaqueResize = b;
}

bool KDockManager::splitterOpaqueResize() const
{
  return d->splitterOpaqueResize;
}

void KDockManager::setSplitterKeepSize(bool b)
{
  d->splitterKeepSize = b;
}

bool KDockManager::splitterKeepSize() const
{
  return d->splitterKeepSize;
}

void KDockManager::setSplitterHighResolution(bool b)
{
  d->splitterHighResolution = b;
}

bool KDockManager::splitterHighResolution() const
{
  return d->splitterHighResolution;
}

void KDockManager::slotMenuPopup()
{
  menu->clear();
  menuData->clear();

  foreach(QObject *o, *childDock) {
    KDockWidget * obj = dynamic_cast<KDockWidget *>(o);
    if (obj==NULL) continue;
    if ( obj->mayBeHide() )
    {
      menu->addMenu( !obj->windowIcon().isNull() ? obj->windowIcon() : QPixmap(), QString("Hide ") + obj->windowTitle());
      menuData->append( new MenuDockData( obj, true ) );
    }

    if ( obj->mayBeShow() )
    {
      menu->addMenu( !obj->windowIcon().isNull() ? obj->windowIcon() : QPixmap(), QString("Show ") + obj->windowTitle() );
      menuData->append( new MenuDockData( obj, false ) );
    }
  }
}

void KDockManager::slotMenuActivated( int id )
{
  MenuDockData* data = menuData->at( id );
  data->dock->changeHideShowState();
}

KDockWidget* KDockManager::findWidgetParentDock( QWidget* w )
{
  KDockWidget * found = 0L;

  foreach(QObject *o, *childDock) {
      KDockWidget * dock = dynamic_cast<KDockWidget *>(o);
      if (dock==NULL) continue;
      if ( dock->widget == w ){
        found  = dock;
        break;
      }
  }

  return found;
}

void KDockManager::drawDragRectangle()
{
  if (d->oldDragRect == d->dragRect)
    return;

  int i;
  QRect oldAndNewDragRect[2];
  oldAndNewDragRect[0] = d->oldDragRect;
  oldAndNewDragRect[1] = d->dragRect;

  // 2 calls, one for the old and one for the new drag rectangle
  for (i = 0; i <= 1; i++) {
    if (oldAndNewDragRect[i].isEmpty())
      continue;

    KDockWidget* pDockWdgAtRect = (KDockWidget*) QApplication::widgetAt( oldAndNewDragRect[i].topLeft() );
    if (!pDockWdgAtRect)
      continue;

    bool isOverMainWdg = false;
    bool unclipped;
    KDockMainWindow* pMain = 0L;
    KDockWidget* pTLDockWdg = 0L;
    QWidget* topWdg;
    if (pDockWdgAtRect->topLevelWidget() == main) {
      isOverMainWdg = true;
      topWdg = pMain = (KDockMainWindow*) main;
      unclipped = pMain->testAttribute(Qt::WA_PaintUnclipped);
      pMain->setAttribute( Qt::WA_PaintUnclipped );
    }
    else {
      topWdg = pTLDockWdg = (KDockWidget*) pDockWdgAtRect->topLevelWidget();
      unclipped = pTLDockWdg->testAttribute(Qt::WA_PaintUnclipped);
      pTLDockWdg->setAttribute( Qt::WA_PaintUnclipped );
    }

    // draw the rectangle unclipped over the main dock window
    QPainter p;
    p.begin( topWdg );
      if ( !unclipped ) {
        if (isOverMainWdg)
          pMain->setAttribute( Qt::WA_PaintUnclipped, false );
        else
          pTLDockWdg->setAttribute( Qt::WA_PaintUnclipped, false );
      }
      // draw the rectangle
      //TODO: find equal for p.setRasterOp(Qt::NotXorROP);
      QRect r = oldAndNewDragRect[i];
      r.moveTopLeft( r.topLeft() - topWdg->mapToGlobal(QPoint(0,0)) );
      p.drawRect(r.x(), r.y(), r.width(), r.height());
    p.end();
  }

  // memorize the current rectangle for later removing
  d->oldDragRect = d->dragRect;
}


#ifdef _JOWENN_EXPERIMENTAL_

KDockArea::KDockArea( QWidget* parent, const QString name)
:QWidget( parent)
{
  setObjectName(name);
  QString new_name = QString(name) + QString("_DockManager");
  dockManager = new KDockManager( this, new_name );
  mainDockWidget = 0L;
}

KDockArea::~KDockArea()
{
	delete dockManager;
}

KDockWidget* KDockArea::createDockWidget( const QString& name, const QPixmap &pixmap, QWidget* parent, const QString& strCaption, const QString& strTabPageLabel)
{
  return new KDockWidget( dockManager, name, pixmap, parent, strCaption, strTabPageLabel );
}

void KDockArea::makeDockVisible( KDockWidget* dock )
{
  if ( dock != 0L)
    dock->makeDockVisible();
}

void KDockArea::makeDockInvisible( KDockWidget* dock )
{
  if ( dock != 0L)
    dock->undock();
}

void KDockArea::makeWidgetDockVisible( QWidget* widget )
{
  makeDockVisible( dockManager->findWidgetParentDock(widget) );
}

void KDockArea::writeDockConfig(QDomElement &base)
{
  dockManager->writeConfig(base);
}

void KDockArea::readDockConfig(QDomElement &base)
{
  dockManager->readConfig(base);
}

void KDockArea::slotDockWidgetUndocked()
{
  QObject* pSender = (QObject*) sender();
  if (!pSender->inherits("KDockWidget")) return;
  KDockWidget* pDW = (KDockWidget*) pSender;
  emit dockWidgetHasUndocked( pDW);
}

void KDockArea::resizeEvent(QResizeEvent *rsize)
{
  QWidget::resizeEvent(rsize);
  QWidgetList list = findChildren<QWidget*>();
  if (list.isEmpty()) return;

  foreach (QWidget *wdg, list) {
    wdg->setGeometry(QRect(QPoint(0,0),size()));
    //break;
  }
}

#endif

#ifndef NO_INCLUDE_MOCFILES // for Qt-only projects, because tmake doesn't take this name
#include "kdockwidget.moc"
#endif
