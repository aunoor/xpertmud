#include "PerlInterpret.h"

// perl headers
extern "C" {
#include <EXTERN.h>
#include <perl.h>
#include <XSUB.h>
}

#include "GuiScriptingBindings.h"

#include "sysinit_pl.h"

#include <strstream>


// Stupid perl version incompatibilities...
#ifdef dXSTARG
#define FIXWARN
#else
#define dXSTARG
#endif

// TODO: add configure test! 5.6.1 compatibility mode as default
#undef FIXWARN

// Get rid of those stupid warnings
#ifdef FIXWARN
#define FIXPERLWARN 
                    // if(false) { cv+=0; Perl___notused+=0; }
#define FIXPERLWARN2 
                    // if (false) { targ+=0; }
#else
#define FIXPERLWARN
#define FIXPERLWARN2
#endif


// set from PerlInterpret while interpreting perl code
static GuiScriptingBindings *currentCallBack;


extern "C" {
  void boot_DynaLoader (CV* cv);
  /////////////////////////////////////////////////////////////////////
  // Implementation XM::Window

  XS(XM_Window_setTitle) {
    dXSARGS;
    FIXPERLWARN;

    if (items != 2)
      croak("Usage: XM::Window->setTitle($text)");

    
    int id = -1;
    if (SvROK(ST(0)))
      id=SvIV(SvRV(ST(0)));
    else
      croak("Usage: SCALAR=XM::Window->setTitle($text)");
    
    STRLEN len;
    char *title = SvPV(ST(1),len);
    dXSTARG;
     FIXPERLWARN2;


    currentCallBack->XM_Window_setTitle(id,title, len);
    XSRETURN_YES;
  }

  XS(XM_Window_show) {
    dXSARGS;
    FIXPERLWARN;

    if (items != 1)
      croak("Usage: XM::Window->show");

    int id = -1;
    if (SvROK(ST(0)))
      id=SvIV(SvRV(ST(0)));
    else
      croak("Usage: SCALAR=XM::Window->show()");
    dXSTARG;
     FIXPERLWARN2;

    currentCallBack->XM_Window_show(id);
    XSRETURN_YES;
  }

  XS(XM_Window_hide) {
    dXSARGS;
    FIXPERLWARN;

    if (items != 1)
      croak("Usage: XM::Window->hide");

    int id = -1;
    if (SvROK(ST(0)))
      id=SvIV(SvRV(ST(0)));
    else
      croak("Usage: SCALAR=XM::Window->hide()");
    dXSTARG;
     FIXPERLWARN2;

    currentCallBack->XM_Window_hide(id);
    XSRETURN_YES;
  }

  XS(XM_Window_close) {
    dXSARGS;
    FIXPERLWARN;

    if (items != 1)
      croak("Usage: XM::Window->close");

    int id = -1;
    if (SvROK(ST(0)))
      id=SvIV(SvRV(ST(0)));
    else
      croak("Usage: SCALAR=XM::Window->close()");
    dXSTARG;
     FIXPERLWARN2;

    currentCallBack->XM_Window_close(id);
    XSRETURN_YES;
  }


  XS(XM_Window_minimize) {
    dXSARGS;
    FIXPERLWARN;

    if (items != 1)
      croak("Usage: XM::Window->minimize");

    int id = -1;
    if (SvROK(ST(0)))
      id=SvIV(SvRV(ST(0)));
    else
      croak("Usage: SCALAR=XM::Window->minimize()");
    dXSTARG;
     FIXPERLWARN2;

    currentCallBack->XM_Window_minimize(id);
    XSRETURN_YES;
  }

  XS(XM_Window_maximize) {
    dXSARGS;
    FIXPERLWARN;

    if (items != 1)
      croak("Usage: XM::Window->maximize");

    int id = -1;
    if (SvROK(ST(0)))
      id=SvIV(SvRV(ST(0)));
    else
      croak("Usage: SCALAR=XM::Window->maximize()");
    dXSTARG;
     FIXPERLWARN2;

    currentCallBack->XM_Window_maximize(id);
    XSRETURN_YES;
  }
  
  XS(XM_Window_isValid) {
    dXSARGS;
    FIXPERLWARN;

    if (items != 1)
      croak("Usage: XM::Window->isValid");

    int id = -1;
    if (SvROK(ST(0)))
      id=SvIV(SvRV(ST(0)));
    else
      croak("Usage: SCALAR=XM::Window->isValid()");
    dXSTARG;
     FIXPERLWARN2;

    if (currentCallBack->XM_Window_isValid(id)) {
      XSRETURN_YES;
    } else {
      XSRETURN_NO;
    }
  }

   XS(XM_Window_move) {
    dXSARGS;
    FIXPERLWARN;

    if (items != 3)
      croak("Usage: XM::Window->move(x,y)");

    int id = -1;
    if (SvROK(ST(0)))
      id=SvIV(SvRV(ST(0)));
    else
      croak("Usage: SCALAR=XM::Window->move(x,y)");

    int x=SvIV(ST(1));
    int y=SvIV(ST(2));

    dXSTARG;
     FIXPERLWARN2;

    currentCallBack->XM_Window_move(id,x,y);

    XSRETURN_YES;
  }
 
   XS(XM_Window_resize) {
    dXSARGS;
    FIXPERLWARN;

    if (items != 3)
      croak("Usage: XM::Window->resize(x,y)");

    int id = -1;
    if (SvROK(ST(0)))
      id=SvIV(SvRV(ST(0)));
    else
      croak("Usage: SCALAR=XM::Window->resize(x,y)");

    int x=SvIV(ST(1));
    int y=SvIV(ST(2));

    dXSTARG;
     FIXPERLWARN2;

    currentCallBack->XM_Window_resize(id,x,y);

    XSRETURN_YES;
  }
 
  XS(XM_Window_raise) {
    dXSARGS;
    FIXPERLWARN;

    if (items != 1)
      croak("Usage: XM::Window->raise");

    int id = -1;
    if (SvROK(ST(0)))
      id=SvIV(SvRV(ST(0)));
    else
      croak("Usage: SCALAR=XM::Window->raise()");
    dXSTARG;
     FIXPERLWARN2;

    currentCallBack->XM_Window_raise(id);
    XSRETURN_YES;
  }

  XS(XM_Window_lower) {
    dXSARGS;
    FIXPERLWARN;

    if (items != 1)
      croak("Usage: XM::Window->lower");

    int id = -1;
    if (SvROK(ST(0)))
      id=SvIV(SvRV(ST(0)));
    else
      croak("Usage: SCALAR=XM::Window->lower()");
    dXSTARG;
     FIXPERLWARN2;

    currentCallBack->XM_Window_lower(id);
    XSRETURN_YES;
  }


  XS(XM_Window_getPositionX) {
    dXSARGS;
    FIXPERLWARN;

    if (items != 1)
      croak("Usage: XM::Window->getPositionX");

    int id = -1;
    if (SvROK(ST(0)))
      id=SvIV(SvRV(ST(0)));
    else
      croak("Usage: SCALAR=XM::Window->getPositionX()");
    dXSTARG;
     FIXPERLWARN2;

    int pos=currentCallBack->XM_Window_getPositionX(id);
    XSRETURN_IV(pos);
  }


  XS(XM_Window_getPositionY) {
    dXSARGS;
    FIXPERLWARN;

    if (items != 1)
      croak("Usage: XM::Window->getPositionY");

    int id = -1;
    if (SvROK(ST(0)))
      id=SvIV(SvRV(ST(0)));
    else
      croak("Usage: SCALAR=XM::Window->getPositionY()");
    dXSTARG;
     FIXPERLWARN2;

    int pos=currentCallBack->XM_Window_getPositionY(id);
    XSRETURN_IV(pos);
  }


  XS(XM_Window_getSizeX) {
    dXSARGS;
    FIXPERLWARN;

    if (items != 1)
      croak("Usage: XM::Window->getSizeX");

    int id = -1;
    if (SvROK(ST(0)))
      id=SvIV(SvRV(ST(0)));
    else
      croak("Usage: SCALAR=XM::Window->getSizeX()");
    dXSTARG;
     FIXPERLWARN2;

    int pos=currentCallBack->XM_Window_getSizeX(id);
    XSRETURN_IV(pos);
  }


  XS(XM_Window_getSizeY) {
    dXSARGS;
    FIXPERLWARN;

    if (items != 1)
      croak("Usage: XM::Window->getSizeY");

    int id = -1;
    if (SvROK(ST(0)))
      id=SvIV(SvRV(ST(0)));
    else
      croak("Usage: SCALAR=XM::Window->getSizeY()");
    dXSTARG;
     FIXPERLWARN2;

    int pos=currentCallBack->XM_Window_getSizeY(id);
    XSRETURN_IV(pos);
  }




  /////////////////////////////////////////////////////////////////////
  // Implementation XM::TextWindow

  XS(XM_TextWindow_initialize) {
    dXSARGS;
    FIXPERLWARN;

    if (items != 0)
      croak("Usage: XM::TextWindow::initialize()");

    dXSTARG;
     FIXPERLWARN2;

    int id = currentCallBack->XM_TextWindow_initialize();

    XSRETURN_IV(id);
  }

  XS(XM_TextWindow_newline) {
    dXSARGS;
    FIXPERLWARN;

    if (items != 1)
      croak("Usage: XM::TextWindow->newline");

    int id = -1;
    if (SvROK(ST(0)))
      id=SvIV(SvRV(ST(0)));
    else
      croak("Usage: SCALAR=XM::TextWindow->newline()");
    dXSTARG;
     FIXPERLWARN2;

    currentCallBack->XM_TextWindow_newline(id);
    XSRETURN_YES;
  }

  XS(XM_TextWindow_clear) {
    dXSARGS;
    FIXPERLWARN;

    if (items != 1)
      croak("Usage: XM::TextWindow->clear");

    int id = -1;
    if (SvROK(ST(0)))
      id=SvIV(SvRV(ST(0)));
    else
      croak("Usage: SCALAR=XM::TextWindow->clear()");
    dXSTARG;
     FIXPERLWARN2;

    currentCallBack->XM_TextWindow_clear(id);
    XSRETURN_YES;
  }
  XS(XM_TextWindow_clearLine) {
    dXSARGS;
    FIXPERLWARN;

    if (items != 1)
      croak("Usage: XM::TextWindow->clearLine");

    int id = -1;
    if (SvROK(ST(0)))
      id=SvIV(SvRV(ST(0)));
    else
      croak("Usage: SCALAR=XM::TextWindow->clearLine()");
    dXSTARG;
     FIXPERLWARN2;

    currentCallBack->XM_TextWindow_clearLine(id);
    XSRETURN_YES;
  }
  XS(XM_TextWindow_clearEOL) {
    dXSARGS;
    FIXPERLWARN;

    if (items != 1)
      croak("Usage: XM::TextWindow->clearEOL");

    int id = -1;
    if (SvROK(ST(0)))
      id=SvIV(SvRV(ST(0)));
    else
      croak("Usage: SCALAR=XM::TextWindow->clearEOL()");
    dXSTARG;
     FIXPERLWARN2;

    currentCallBack->XM_TextWindow_clearEOL(id);
    XSRETURN_YES;
  }

  XS(XM_TextWindow_resetAttributes) {
    dXSARGS;
    FIXPERLWARN;

    if (items != 1)
      croak("Usage: XM::TextWindow->resetAttributes");

    int id = -1;
    if (SvROK(ST(0)))
      id=SvIV(SvRV(ST(0)));
    else
      croak("Usage: SCALAR=XM::TextWindow->resetAttributes()");
    dXSTARG;
     FIXPERLWARN2;

    currentCallBack->XM_TextWindow_resetAttributes(id);
    XSRETURN_YES;
  }
   
  XS(XM_TextWindow_print) {
    dXSARGS;
    FIXPERLWARN;

    if (items != 2)
      croak("Usage: XM::TextWindow->print($text)");
    
    int id = -1;
    if (SvROK(ST(0)))
      id=SvIV(SvRV(ST(0)));
    else
      croak("Usage: SCALAR=XM::TextWindow->print($text)");
    
    STRLEN len;
    char *text = SvPV(ST(1),len);
    dXSTARG;
     FIXPERLWARN2;

    currentCallBack->XM_TextWindow_print(id,text, len);
    XSRETURN_YES;
  }

 
  XS(XM_TextWindow_setBGColor) {
    dXSARGS;
    FIXPERLWARN;

    if (items != 2)
      croak("Usage: XM::TextWindow->setBGColor(color)");

    int id = -1;
    if (SvROK(ST(0)))
      id=SvIV(SvRV(ST(0)));
    else
      croak("Usage: SCALAR=XM::TextWindow->setBGColor(color)");
    int color=SvIV(ST(1));
    dXSTARG;
     FIXPERLWARN2;

    currentCallBack->XM_TextWindow_setBGColor(id,color);
    XSRETURN_YES;
  }

  XS(XM_TextWindow_setDefaultBGColor) {
    dXSARGS;
    FIXPERLWARN;

    if (items != 2)
      croak("Usage: XM::TextWindow->setDefaultBGColor(color)");

    int id = -1;
    if (SvROK(ST(0)))
      id=SvIV(SvRV(ST(0)));
    else
      croak("Usage: SCALAR=XM::TextWindow->setDefaultBGColor(color)");
    int color=SvIV(ST(1));
    dXSTARG;
     FIXPERLWARN2;

    currentCallBack->XM_TextWindow_setDefaultBGColor(id,color);
    XSRETURN_YES;
  }

  XS(XM_TextWindow_setFGColor) {
    dXSARGS;
    FIXPERLWARN;

    if (items != 2)
      croak("Usage: XM::TextWindow->setFGColor(color)");

    int id = -1;
    if (SvROK(ST(0)))
      id=SvIV(SvRV(ST(0)));
    else
      croak("Usage: SCALAR=XM::TextWindow->setFGColor(color)");
    int color=SvIV(ST(1));
    dXSTARG;
     FIXPERLWARN2;

    currentCallBack->XM_TextWindow_setFGColor(id,color);
    XSRETURN_YES;
  }

  XS(XM_TextWindow_setDefaultFGColor) {
    dXSARGS;
    FIXPERLWARN;

    if (items != 2)
      croak("Usage: XM::TextWindow->setDefaultFGColor(color)");

    int id = -1;
    if (SvROK(ST(0)))
      id=SvIV(SvRV(ST(0)));
    else
      croak("Usage: SCALAR=XM::TextWindow->setDefaultFGColor(color)");
    int color=SvIV(ST(1));
    dXSTARG;
     FIXPERLWARN2;

    currentCallBack->XM_TextWindow_setDefaultFGColor(id,color);
    XSRETURN_YES;
  }

 XS(XM_TextWindow_scrollLines) {
    dXSARGS;
    FIXPERLWARN;

    if (items != 2)
      croak("Usage: XM::TextWindow->scrollLines(howmuch)");

    int id = -1;
    if (SvROK(ST(0)))
      id=SvIV(SvRV(ST(0)));
    else
      croak("Usage: SCALAR=XM::TextWindow->scrollLines(count)");
    int count=SvIV(ST(1));
    dXSTARG;
     FIXPERLWARN2;

    currentCallBack->XM_TextWindow_scrollLines(id,count);
    XSRETURN_YES;
  }

 XS(XM_TextWindow_scrollColumns) {
    dXSARGS;
    FIXPERLWARN;

    if (items != 2)
      croak("Usage: XM::TextWindow->scrollColumns(howmuch)");

    int id = -1;
    if (SvROK(ST(0)))
      id=SvIV(SvRV(ST(0)));
    else
      croak("Usage: SCALAR=XM::TextWindow->scrollColumns(count)");
    int count=SvIV(ST(1));
    dXSTARG;
     FIXPERLWARN2;

    currentCallBack->XM_TextWindow_scrollColumns(id,count);
    XSRETURN_YES;
  }

  
  XS(XM_TextWindow_setBold) {
    dXSARGS;
    FIXPERLWARN;

    if (items != 2)
      croak("Usage: XM::TextWindow->setBold(bool)");

    int id = -1;
    if (SvROK(ST(0)))
      id=SvIV(SvRV(ST(0)));
    else
      croak("Usage: SCALAR=XM::TextWindow->setBold(bool)");
    bool is_it=SvIV(ST(1));
    dXSTARG;
     FIXPERLWARN2;

    currentCallBack->XM_TextWindow_setBold(id,is_it);
    XSRETURN_YES;
  }

  XS(XM_TextWindow_setBlinking) {
    dXSARGS;
    FIXPERLWARN;

    if (items != 2)
      croak("Usage: XM::TextWindow->setBlinking(bool)");

    int id = -1;
    if (SvROK(ST(0)))
      id=SvIV(SvRV(ST(0)));
    else
      croak("Usage: SCALAR=XM::TextWindow->setBlinking(bool)");
    bool is_it=SvIV(ST(1));
    dXSTARG;
     FIXPERLWARN2;

    currentCallBack->XM_TextWindow_setBlinking(id,is_it);
    XSRETURN_YES;
  }

  XS(XM_TextWindow_setUnderline) {
    dXSARGS;
    FIXPERLWARN;

    if (items != 2)
      croak("Usage: XM::TextWindow->setUnderline(bool)");

    int id = -1;
    if (SvROK(ST(0)))
      id=SvIV(SvRV(ST(0)));
    else
      croak("Usage: SCALAR=XM::TextWindow->setUnderline(bool)");
    bool is_it=SvIV(ST(1));
    dXSTARG;
     FIXPERLWARN2;

    currentCallBack->XM_TextWindow_setUnderline(id,is_it);
    XSRETURN_YES;
  }

  XS(XM_TextWindow_setReverse) {
    dXSARGS;
    FIXPERLWARN;

    if (items != 2)
      croak("Usage: XM::TextWindow->setReverse(bool)");

    int id = -1;
    if (SvROK(ST(0)))
      id=SvIV(SvRV(ST(0)));
    else
      croak("Usage: SCALAR=XM::TextWindow->setReverse(bool)");
    bool is_it=SvIV(ST(1));
    dXSTARG;
     FIXPERLWARN2;

    currentCallBack->XM_TextWindow_setReverse(id,is_it);
    XSRETURN_YES;
  }
 
  XS(XM_TextWindow_setIntensive) {
    dXSARGS;
    FIXPERLWARN;

    if (items != 2)
      croak("Usage: XM::TextWindow->setIntensive(bool)");

    int id = -1;
    if (SvROK(ST(0)))
      id=SvIV(SvRV(ST(0)));
    else
      croak("Usage: SCALAR=XM::TextWindow->setIntensive(bool)");
    bool is_it=SvIV(ST(1));
    dXSTARG;
     FIXPERLWARN2;

    currentCallBack->XM_TextWindow_setIntensive(id,is_it);
    XSRETURN_YES;
  }


  XS(XM_TextWindow_setCursor) {
    dXSARGS;
    FIXPERLWARN;

    if (items != 3)
      croak("Usage: XM::TextWindow->setCursor(x,y)");

    int id = -1;
    if (SvROK(ST(0)))
      id=SvIV(SvRV(ST(0)));
    else
      croak("Usage: SCALAR=XM::TextWindow->setCursor(x,y)");
    unsigned int x=SvUV(ST(1));
    unsigned int y=SvUV(ST(2));
    dXSTARG;
     FIXPERLWARN2;

    currentCallBack->XM_TextWindow_setCursor(id,x,y);
    XSRETURN_YES;
  }

  XS(XM_TextWindow_getCursorX) {
    dXSARGS;
    FIXPERLWARN;

    if (items != 1)
      croak("Usage: XM::TextWindow->getCursorX");

    int id = -1;
    if (SvROK(ST(0)))
      id=SvIV(SvRV(ST(0)));
    else
      croak("Usage: SCALAR=XM::TextWindow->getCursorX()");
    dXSTARG;
     FIXPERLWARN2;

    int pos=currentCallBack->XM_TextWindow_getCursorX(id);
    XSRETURN_IV(pos);
  }

  XS(XM_TextWindow_getCursorY) {
    dXSARGS;
    FIXPERLWARN;

    if (items != 1)
      croak("Usage: XM::TextWindow->getCursorY");

    int id = -1;
    if (SvROK(ST(0)))
      id=SvIV(SvRV(ST(0)));
    else
      croak("Usage: SCALAR=XM::TextWindow->getCursorY()");
    dXSTARG;
     FIXPERLWARN2;

    int pos=currentCallBack->XM_TextWindow_getCursorY(id);
    XSRETURN_IV(pos);
  }
  


  XS(XM_TextWindow_getColumns) {
    dXSARGS;
    FIXPERLWARN;

    if (items != 1)
      croak("Usage: XM::TextWindow->getColumns");

    int id = -1;
    if (SvROK(ST(0)))
      id=SvIV(SvRV(ST(0)));
    else
      croak("Usage: SCALAR=XM::TextWindow->getColumns()");
    dXSTARG;
     FIXPERLWARN2;

    int pos=currentCallBack->XM_TextWindow_getColumns(id);
    XSRETURN_IV(pos);
  }
  


  XS(XM_TextWindow_getLines) {
    dXSARGS;
    FIXPERLWARN;

    if (items != 1)
      croak("Usage: XM::TextWindow->getLines");

    int id = -1;
    if (SvROK(ST(0)))
      id=SvIV(SvRV(ST(0)));
    else
      croak("Usage: SCALAR=XM::TextWindow->getLines()");
    dXSTARG;
     FIXPERLWARN2;

    int pos=currentCallBack->XM_TextWindow_getLines(id);
    XSRETURN_IV(pos);
  }
  
  XS(XM_TextWindow_setFont) {
    dXSARGS;
    FIXPERLWARN;

    if (items != 3)
      croak("Usage: XM::TextWindow->setFont($text,size)");
    
    int id = -1;
    if (SvROK(ST(0)))
      id=SvIV(SvRV(ST(0)));
    else
      croak("Usage: SCALAR=XM::TextWindow->setFont($text,size)");
    
    STRLEN len;
    char *text = SvPV(ST(1),len);

    int size=SvIV(ST(2));

    dXSTARG;
     FIXPERLWARN2;

     if (currentCallBack->XM_TextWindow_setFont(id,text, len,size)) {
       XSRETURN_YES;
     } else {
       XSRETURN_NO;
     }
  }


  XS(XM_TextWindow_getCharAt) {
    dXSARGS;
    FIXPERLWARN;
    
    if (items != 3)
      croak("Usage: XM::TextWindow->getCharAt(x,y)");
    
    int id = -1;
    if (SvROK(ST(0)))
      id=SvIV(SvRV(ST(0)));
    else
      croak("Usage: SCALAR=XM::TextWindow->getCharAt(x,y)");
    
    int x=SvIV(ST(1));
    int y=SvIV(ST(2));
    
    dXSTARG;
    FIXPERLWARN2;
    
    char str[2];
    str[0]=currentCallBack->XM_TextWindow_getCharAt(id,x,y);
    str[1]=0;
    // ifdef perl5.6    
    //    XSRETURN_PVN(str,1);
    // else
    XSRETURN_PV(str);
  }
  
  

  XS(XM_TextWindow_resizeChars) {
    dXSARGS;
    FIXPERLWARN;

    if (items != 3)
      croak("Usage: XM::TextWindow->resizeChars(x,y)");

    int id = -1;
    if (SvROK(ST(0)))
      id=SvIV(SvRV(ST(0)));
    else
      croak("Usage: SCALAR=XM::TextWindow->resizeChars(x,y)");

    int x=SvIV(ST(1));
    int y=SvIV(ST(2));

    dXSTARG;
     FIXPERLWARN2;

    currentCallBack->XM_TextWindow_resizeChars(id,x,y);

    XSRETURN_YES;
  }

  /////////////////////////////////////////////////////////////////////
  // Implementation XM::TextBufferWindow
  
  XS(XM_TextBufferWindow_initialize) {
    dXSARGS;
    FIXPERLWARN;

    if (items != 0)
      croak("Usage: XM::TextBufferWindow::initialize()");

    dXSTARG;
     FIXPERLWARN2;

    int id = currentCallBack->XM_TextBufferWindow_initialize();

    XSRETURN_IV(id);
  }


  /////////////////////////////////////////////////////////////////////
  // Implementation XM

  XS(XM_send) {
    dXSARGS;
    FIXPERLWARN;

    if (items != 1)
      croak("Usage: XM::send($text)");

    STRLEN len;
    char *input1 = SvPV(ST(0),len);
    dXSTARG;
     FIXPERLWARN2;

    currentCallBack->XM_send(input1, len);
    XSRETURN(1);
  }

  XS(XM_telnet) {
    dXSARGS;
    FIXPERLWARN;

    if (items != 2)
      croak("Usage: XM::telnet($host, $port)");

    STRLEN len;
    char *input1 = SvPV(ST(0),len);
    int input2 = SvIV(ST(1));
    dXSTARG;
     FIXPERLWARN2;

    currentCallBack->XM_telnet(input1, len, input2);
    XSRETURN(1);
  }

  XS(XM_setStatusMsg) {
    dXSARGS;
    FIXPERLWARN;

    if (items != 1)
      croak("Usage: XM::setStatusMsg($msg)");

    STRLEN len;
    char *input1 = SvPV(ST(0),len);
    dXSTARG;
     FIXPERLWARN2;

    currentCallBack->XM_setStatusMsg(input1,len);
    XSRETURN(1);
    
  }

  XS(XM_resetStatusMsg) {
    currentCallBack->XM_resetStatusMsg();
    if (false) { cv+=0; }
  }

  XS(XM_showPerlError) {
    dXSARGS;
    FIXPERLWARN;

    if (items != 1)
      croak("Usage: XM::setStatusMsg($msg)");

    STRLEN len;
    char *input1 = SvPV(ST(0),len);
    dXSTARG;
     FIXPERLWARN2;

    currentCallBack->XM_showPerlError(input1,len);
    XSRETURN(1);
  }

  XS(XM_echoMode) {
    dXSARGS;
    FIXPERLWARN;

    if (items != 1)
      croak("Usage: XM::echoMode($state)");

    bool state=(bool)SvIV(ST(0));
    dXSTARG;
     FIXPERLWARN2;

    currentCallBack->XM_echoMode(state);
    XSRETURN(1);
  }


  /////////////////////////////////////////////////////////////////////
  // Activation


  void xs_init(void) {
    newXS((char *)"DynaLoader::boot_DynaLoader", boot_DynaLoader, (char *)__FILE__); 


    newXS((char *)"XM::Window::setTitle",
	  XM_Window_setTitle , (char *)__FILE__); 
    newXS((char *)"XM::Window::show",
	  XM_Window_show , (char *)__FILE__); 
    newXS((char *)"XM::Window::hide",
	  XM_Window_hide , (char *)__FILE__); 
    newXS((char *)"XM::Window::close",
	  XM_Window_close , (char *)__FILE__); 
    newXS((char *)"XM::Window::minimize",
	  XM_Window_minimize , (char *)__FILE__); 
    newXS((char *)"XM::Window::maximize",
	  XM_Window_maximize , (char *)__FILE__); 
    newXS((char *)"XM::Window::move",
	  XM_Window_move , (char *)__FILE__); 
    newXS((char *)"XM::Window::getPositionX",
	  XM_Window_getPositionX , (char *)__FILE__); 
    newXS((char *)"XM::Window::getPositionY",
	  XM_Window_getPositionY , (char *)__FILE__); 
    newXS((char *)"XM::Window::getSizeX",
	  XM_Window_getSizeX , (char *)__FILE__);
    newXS((char *)"XM::Window::getSizeY",
	  XM_Window_getSizeY , (char *)__FILE__);
    newXS((char *)"XM::Window::resize",
	  XM_Window_resize , (char *)__FILE__);
    newXS((char *)"XM::Window::isValid",
	  XM_Window_isValid , (char *)__FILE__); 
    newXS((char *)"XM::Window::raise",
	  XM_Window_raise , (char *)__FILE__); 
    newXS((char *)"XM::Window::lower",
	  XM_Window_lower , (char *)__FILE__); 

    newXS((char *)"XM::TextWindow::initialize",
	  XM_TextWindow_initialize , (char *)__FILE__); 
    newXS((char *)"XM::TextWindow::newline",
	  XM_TextWindow_newline , (char *)__FILE__); 
    newXS((char *)"XM::TextWindow::clear",
	  XM_TextWindow_clear , (char *)__FILE__); 
    newXS((char *)"XM::TextWindow::clearLine",
	  XM_TextWindow_clearLine , (char *)__FILE__); 
    newXS((char *)"XM::TextWindow::clearEOL",
	  XM_TextWindow_clearEOL , (char *)__FILE__); 
    newXS((char *)"XM::TextWindow::resetAttributes",
	  XM_TextWindow_resetAttributes , (char *)__FILE__); 
    newXS((char *)"XM::TextWindow::print",
	  XM_TextWindow_print , (char *)__FILE__); 
    newXS((char *)"XM::TextWindow::setBGColor",
	  XM_TextWindow_setBGColor , (char *)__FILE__); 
    newXS((char *)"XM::TextWindow::setDefaultBGColor",
	  XM_TextWindow_setDefaultBGColor , (char *)__FILE__); 
    newXS((char *)"XM::TextWindow::setFGColor",
	  XM_TextWindow_setFGColor , (char *)__FILE__); 
    newXS((char *)"XM::TextWindow::setDefaultFGColor",
	  XM_TextWindow_setDefaultFGColor , (char *)__FILE__); 
    newXS((char *)"XM::TextWindow::scrollLines",
	  XM_TextWindow_scrollLines , (char *)__FILE__); 
    newXS((char *)"XM::TextWindow::scrollColumns",
	  XM_TextWindow_scrollColumns , (char *)__FILE__); 
   newXS((char *)"XM::TextWindow::setBold",
	  XM_TextWindow_setBold , (char *)__FILE__); 
   newXS((char *)"XM::TextWindow::setBlinking",
	  XM_TextWindow_setBlinking , (char *)__FILE__); 
   newXS((char *)"XM::TextWindow::setUnderline",
	  XM_TextWindow_setUnderline , (char *)__FILE__); 
   newXS((char *)"XM::TextWindow::setReverse",
	  XM_TextWindow_setReverse , (char *)__FILE__); 
   newXS((char *)"XM::TextWindow::setIntensive",
	  XM_TextWindow_setIntensive , (char *)__FILE__); 
   newXS((char *)"XM::TextWindow::setCursor",
	  XM_TextWindow_setCursor , (char *)__FILE__); 
   newXS((char *)"XM::TextWindow::getCursorX",
	  XM_TextWindow_getCursorX , (char *)__FILE__); 
   newXS((char *)"XM::TextWindow::getCursorY",
	  XM_TextWindow_getCursorY , (char *)__FILE__); 
   newXS((char *)"XM::TextWindow::resizeChars",
	  XM_TextWindow_resizeChars , (char *)__FILE__); 
   newXS((char *)"XM::TextWindow::getCharAt",
	  XM_TextWindow_getCharAt , (char *)__FILE__); 
   newXS((char *)"XM::TextWindow::setFont",
	  XM_TextWindow_setFont , (char *)__FILE__); 
   newXS((char *)"XM::TextWindow::getLines",
	  XM_TextWindow_getLines , (char *)__FILE__); 
   newXS((char *)"XM::TextWindow::getColumns",
	  XM_TextWindow_getColumns , (char *)__FILE__); 


   newXS((char *)"XM::TextBufferWindow::initialize",
	 XM_TextBufferWindow_initialize , (char *)__FILE__); 


    newXS((char *)"XM::send",
	  XM_send , (char *)__FILE__); 
    newXS((char *)"XM::telnet",
	  XM_telnet , (char *)__FILE__); 
    newXS((char *)"XM::setStatusMsg",
	  XM_setStatusMsg , (char *)__FILE__); 
    newXS((char *)"XM::resetStatusMsg",
	  XM_resetStatusMsg , (char *)__FILE__); 
    newXS((char *)"XM::showPerlError",
	  XM_showPerlError , (char *)__FILE__); 
    newXS((char *)"XM::echoMode",
	  XM_echoMode , (char *)__FILE__); 
    

  }
}



















/////////////////////////////////////////////////////////////////////
// Implementation class PerlInterpret
static PerlInterpreter *perlInt = perl_alloc();
int PerlInterpret::iNumber = 0;

PerlInterpret::PerlInterpret(GuiScriptingBindings *callBack):
  callBack(callBack)
{
  currentCallBack = callBack;

  
  if (!perlInt)
    exit(42);

  if(iNumber == 0) {
    //PL_perl_destruct_level = 1;
    perl_construct(perlInt);
    
    const char *args[]={"",
			"-w",
			"-e", "0"};
    int ret = perl_parse(perlInt,xs_init,4,(char **)args,0);
    if(!ret)
      perl_run(perlInt);
    else 
      exit(23);
    iNumber++;
  }
  
  eval_pv("package XM_Interpret;\n", false);
  eval_pv(SYSINIT, true);

}

PerlInterpret::~PerlInterpret() {
  currentCallBack = callBack;
  
  strstream str;
  str << "use Symbol qw(delete_package); delete_package('XM_Interpret');" << endl << ends;
  eval_pv(str.str(), false);
  //perl_destruct(perlInt);
  //perl_free(perlInt);
}

void PerlInterpret::textEntered(const char * const text) {
  currentCallBack = callBack;
  const char *args[]={text,0};

  ENTER;
  SAVETMPS;
  perl_call_argv("XM::wrap_onTextEntered",G_DISCARD,(char **)args);
  FREETMPS;
  LEAVE;
}

void PerlInterpret::addIncludeDir(const char * const inc) {
  currentCallBack = callBack;
  const char *args[]={inc,0};

  ENTER;
  SAVETMPS;
  perl_call_argv("XM::addIncludeDir",G_DISCARD,(char **)args);
  FREETMPS;
  LEAVE;
}

void PerlInterpret::textReceived(const char * const text) {
  currentCallBack = callBack;
  const char *args[]={text,0};

  ENTER;
  SAVETMPS;
  perl_call_argv("XM::wrap_onTextReceived",G_DISCARD,(char **)args);
  FREETMPS;
  LEAVE;
}

void PerlInterpret::timer() {
  currentCallBack = callBack;
  char *args[]={0};

    ENTER;
    SAVETMPS;
    perl_call_argv("XM::wrap_onTimer",G_DISCARD,args);
    FREETMPS;
    LEAVE;
}

void PerlInterpret::echo(bool state) {
  currentCallBack = callBack;
  const char *args[]={state?"1":"0",0};
  
  ENTER;
  SAVETMPS;
  perl_call_argv("XM::wrap_onEcho",G_DISCARD,(char **)args);
  FREETMPS;
  LEAVE;
}

bool PerlInterpret::keyPressed(const char * const key, 
			       const char * const ascii) {
  bool ret;
  dSP;
#ifdef FIXWARN
  if (false) { Perl___notused+=0; }
#endif
  currentCallBack = callBack;
  const char *args[]={key,ascii,0};

  ENTER;
  SAVETMPS;

  //  PUSHMARK(SP) ;
  //  PUTBACK;

  int count = perl_call_argv("XM::wrap_onKeyPressed",G_SCALAR,(char **)args);
  if(count != 1)
     croak("Big trouble\n") ;
  
  SPAGAIN;
  ret = POPi;

  PUTBACK;
  FREETMPS;
  LEAVE;

  return ret;
}
