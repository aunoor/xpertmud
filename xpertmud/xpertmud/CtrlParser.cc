#include "CtrlParser.h"

#ifdef WIN32
#include <sstream>
#endif
#include <iostream>
#include <algorithm>
using std::cout;
using std::endl;
using std::cerr;
#ifdef WIN32
# define max(x,y) ((x) > (y) ? (x):(y))
#else
using std::max;
#endif

//#define DEBUG(X)
#define DEBUG(X) cout << X

// vt100 docs are here...
// http://vt100.net/docs/vt100-ug/chapter3.html

// vt100 special characters, stolen from konsole ;-)
QChar CtrlParser::vt100_graphics[32] =
{ // 0/8     1/9    2/10    3/11    4/12    5/13    6/14    7/15
  0x0020, 0x25C6, 0x2592, 0x2409, 0x240c, 0x240d, 0x240a, 0x00b0,
  0x00b1, 0x2424, 0x240b, 0x2518, 0x2510, 0x250c, 0x2514, 0x253c,
  0xF800, 0xF801, 0x2500, 0xF803, 0xF804, 0x251c, 0x2524, 0x2534,
  0x252c, 0x2502, 0x2264, 0x2265, 0x03C0, 0x2260, 0x00A3, 0x00b7
};

// ANSI X3.64 Parser
// see http://members.tripod.com/~ilkerf/cdoc/ansix364.html
// (I coded this spec, let's hope it's correct)

static char SHARPCHARS[] = { // ESC # ?
  '8', '3', '4', '6', '5',
0 };
static char CHARSETCHARS[] = { // ESC ( ?
  'A', 'B', '0', '1', '2',
0 };
static char THRICECHARS[] = { // ESC ? Pn Pn
  'Y',
0 };
static char ONCECHARS[] = { // ESC ? Or should it be all?
  'T', 'P', '\\', 'b', 'W', 'G', 'I', 'H', 'D',
  'a', 'U', 'E', ']', 'K', 'L', '^', 'Q', 'R', 'M',
  'c', 'N', 'O', 'F', 'S', 'J', '=', '>', '<', '?',
  'A', 'B', 'C', 'Z',
0 };
static char ATTR1CHARS[] = { // ESC [ Pn ?
  'Z', 'G', 'U', 'E', 'F', 'D', 'B', 'C', 'A', 'Y',
  'c', 'P', 'M', 'X', '`', 'a', '@', 'L', 'U', 'V',
  'b', 'T', 'S', 'd', 'e',
  // attr2:
  'R', 'H', 'f', 'r',
  // attrn:
  'W', 'o', 'n', 'O', 'J', 'N', 'K', 'i', 'l', 'Q',
  'm', 'h', 'g', 'q', 'x', 'y',

0 };
static char SPACE1CHARS[] = { // ESC [ Pn Space ?
  'C', '@', 'A', 'E',
  'D', 'B', 'G', // space2
  'F', 'H', // spacen
0 };
static char ATTR2CHARS[] = {
  'R', 'H', 'f', 'r', 'y',
  // attrn:
  'W', 'o', 'n', 'O', 'J', 'N', 'K', 'i', 'l', 'Q',
  'm', 'h', 'g', 'q', 'x', 'y',
0 };
static char SPACE2CHARS[] = {
  'D', 'B', 'G',
  'F', 'H', // spacen
0 };
static char ATTRNCHARS[] = {
  'W', 'o', 'n', 'O', 'J', 'N', 'K', 'i', 'l', 'Q',
  'm', 'h', 'g', 'q', 'x', 'y',
0 };
static char SPACENCHARS[] = { 
  'F', 'H',
0 };

CtrlParser::CtrlParser():
  state(SNORMAL), mode(STRIP), callback(NULL),
  stripped(""), colored(""), unparsed(""),
  vt100special(false)
{
  fgColor = 7;
  bgColor = 0;
}

CtrlParser::CtrlParser(CtrlCallbackIface *callback):
  state(SNORMAL), mode(PARSE), callback(callback),
  stripped(""), colored(""), unparsed(""),
  vt100special(false)
{}

void CtrlParser::parse(const QString& s) {
  QString str;
  if(!unparsed.isEmpty()) {
    str = unparsed + s;
    unparsed.truncate(0);
  } else {
    str = s;
  }
  const QChar* from = str.unicode();
  const QChar* to;

  const QChar* strStart = str.unicode();
  const QChar* strEnd = str.unicode() + str.length();
  for(to=strStart; to != strEnd; ++to) {
    // leave this in for debuggin' purposes
    // if((unsigned char)*to < 32 || (unsigned char)*to > 127) {
    //cout << "Ctrl (" << (int)((unsigned char)*to) << ")" << endl;
    // }

    //std::cout << "<" << (int)(to-str.begin()) << ">";
    /*
    if((unsigned char)*to >= 32 && (unsigned char)*to <= 127) {
      std::cout << *to; // << "=" << (int)((unsigned char)*to);
    } else {
      std::cout << "(\\" << (int)((unsigned char)*to) << ")";
    }
    std::cout << std::flush;
    if(*to == '\n')
      std::cout << std::endl;
    */
    
    if(state != SNORMAL && *to == (char)0) {
    } else {
      switch(state) {
      case SNORMAL:
	if(param.size()) { param.clear(); }
	if(*to == (char)155) { // CSI
	  print(from, to);
	  from = to+1;
	  state = SATTR1;
	} else if(*to == (char)27) { // ESC ... could become CSI
	  print(from, to); 
	  from = to;
	  //	  recovery = to+1;
	  //	  std::cout << "<ESC>" << std::flush;
	  state = SESC; 
	} else if(mode == PARSE) {
	  /* if(*to == 0 || // NUL
	     *to == 1 || // SOH
	     *to == 2 || // STX
	     *to == 3 || // ETX
	     *to == 4 || // EOT
	     *to == 5 || // ENQ
	     *to == 6 || // ACK
	     
	     *to == 14 || // SO
	     *to == 15 || // SI
	     *to == 16 || // DLE
	     (*to >= 17 && *to <= 20) || // DC1 .. DC4
	     *to == 21 || // NAK
	     *to == 22 || // SYN
	     *to == 23 || // ETB
	     *to == 24 || // CAN
	     *to == 25 || // EM
	     *to == 26 || // SUB
	     (*to >= 28 && *to <= 31) // IS4 .. IS1
	     ) { for fine grained explanation purposes */
	  if(*to < (char)32 && 
	     (*to <= (char)6 || *to >= (char)14)) {
	    // std::cout << "<cut>" << std::flush;
	    print(from, to); from = to+1;
	  } else if(*to == (char)7) { // BEL
	    print(from, to); from = to+1;
	    callback->beep();
	  } else if(*to == (char)8) { // BS
	    print(from, to); from = to+1;
	    callback->backspace();
	  } else if(*to == (char)9) { // HT
	    print(from, to); from = to+1;
	    callback->tab();
	  } else if(*to == (char)10 || 
		    *to == (char)11 || 
		    *to == (char)12) { // LF, VT, FF
	    print(from, to); from = to+1;
	    callback->newline();
	  } else if(*to == (char)13) { // CR
	    print(from, to); from = to+1;
	    callback->carriageReturn();
	  }
	}
	break;
      case SESC:
	if(*to == '[') { // CSI
	  from = to+1;
	  state = SATTR1; 
	} else if(*to == '(') { // character set stuff
	  from = to+1;
	  state = SCHAR;
	} else if(*to == '#') { // line size commands
	  from = to+1;
	  state = SSHARP;
	} else if(in(*to, ONCECHARS)) { 
	  execute(*to, state);
	  from = to+1;
	  state = SNORMAL;
	} else if(in(*to, THRICECHARS)) { 
	  from = to+1;
	  state = STHRICE;
	} else {
	  DEBUG("Unknown ANSI Escape sequence: ESC ("
	       << (int)*to << ") \"" << *to << "\"" << endl);
	  //	  from = to = recovery;
	  state = SNORMAL;
	}
	break;
      case STHRICE:
	if(in(*to, THRICECHARS)) {
	  putparam(to, to+1);
	  from = to+1;
	  state = STHRICE_PARAM;
	} else {
	  DEBUG("Unknown ANSI Escape sequence: ESC ("
	       << (int)*to << ") \"" << *to << "\"" << endl);
	  //	  from = to = recovery;
	  state = SNORMAL;
	}
        break;
      case STHRICE_PARAM:
	if(from == to) {
	  putparam(from, to+1);
	} else if(from+1 == to) {
	  putparam(from+1, to+1);
	  execute(0, STHRICE); // the information is in param1,
	  // param2, param3 are the two parameters...
	  from = to+1;
	  state = SNORMAL;
	}
	break;
#define SPECIAL(CASE_SWITCH_, CHARSET_) \
      case CASE_SWITCH_: \
	if(in(*to, CHARSET_)) { \
	  execute(*to, state); \
	  from = to+1; \
	  state = SNORMAL; \
	} else { \
	  DEBUG("Unknown ANSI Escape sequence: ESC (" \
	       << (int)*to << ") \"" << *to << "\"" << endl); \
	  state = SNORMAL; \
	} \
        break;
#define PATTR(ATTR_, SPACE_, ATTR_N, ACHARS, SCHARS, ERRSTR) \
    case ATTR_: \
      if(*to == ';') { \
        putparam(from, to); \
	from = to+1; \
	state = ATTR_N; \
      } else if(*to == ' ') { \
	putparam(from, to); \
	state = SPACE_; \
      } else if(*to >= '0' && *to <= '9') { \
	/* NOP, no error, performance reasons of in() */ \
      } else if(in(*to, ACHARS)) { \
	putparam(from, to); \
	execute(*to, state); \
	from = to+1; \
	state = SNORMAL; \
      } else { \
	DEBUG(ERRSTR << " (" \
	     << (int)*to << ") \"" << *to << "\"" << endl); \
	state = SNORMAL; \
      } \
      break; \
    case SPACE_: \
      if(in(*to, SCHARS)) { \
	execute(*to, state); \
	from = to+1; \
	state = SNORMAL; \
      } else { \
	DEBUG(ERRSTR << "Space (" \
	     << (int)*to << ") \"" << *to << "\"" << endl); \
	state = SNORMAL; \
      } \
      break;

	SPECIAL(SCHAR, CHARSETCHARS);
	SPECIAL(SSHARP, SHARPCHARS);
	PATTR(SATTR1, SSPACE1, SATTR2, ATTR1CHARS, SPACE1CHARS,
	      "Unknown ANSI Escape sequence: ESC [ Pn ");
	PATTR(SATTR2, SSPACE2, SATTRN, ATTR2CHARS, SPACE2CHARS,
	      "Unknown ANSI Escape sequence: ESC [ Pn; Pn ");
	PATTR(SATTRN, SSPACEN, SATTRN, ATTRNCHARS, SPACENCHARS,
	      "Unknown ANSI Escape sequence: ESC [ Ps ");
      }
    }
  }
  if(state == SNORMAL) {
    print(from, to);
  } else {
    unparsed = QString(from, to-from);
  }
}

void CtrlParser::execute(const char command, EState execState) {
  // TODO: Make this state aware...
  defaultParamMapT::iterator dp = defaultParams.find(command);
  std::vector<int> parameter;
  if(dp != defaultParams.end()) {
    for(unsigned int i=0; i<dp->second.size(); ++i) {
      if(param.size() > i && param[i].second) {
	parameter.push_back(param[i].first);
      } else {
	parameter.push_back(dp->second[i]);
      }
    }
  }
  for(unsigned int i=parameter.size(); i<param.size(); ++i) {
    parameter.push_back(param[i].first);
  }
  
  if(mode == PARSE && (execState == SATTR1 || execState == SSPACE1 ||
		       execState == SATTR2 || execState == SSPACE2 ||
		       execState == SATTRN || execState == SSPACEN ||
		       execState == SESC)) {
    if(command == 'm') {
      if(parameter.empty()) {
	callback->resetAttributes();
      } else if(parameter.size() == 3 &&
		parameter[0] == 38 && parameter[1] == 5) {
	callback->setFGColor(parameter[2]);
      } else if(parameter.size() == 3 &&
		parameter[0] == 48 && parameter[1] == 5) {
	callback->setBGColor(parameter[2]);
      } else {
	for(unsigned int i=0; i<parameter.size(); ++i) {
	  if(parameter[i] == 0)
	    callback->resetAttributes();
	  else if(parameter[i] == 1)
	    callback->setIntense(true);
	  else if(parameter[i] == 2)
	    callback->setFaint(true);
	  else if(parameter[i] == 3)
	    callback->setItalic(true);
	  else if(parameter[i] == 4)
	    callback->setUnderline(true);
	  else if(parameter[i] == 5)
	    callback->setBlinking(true);
	  else if(parameter[i] == 5)
	    callback->setRapidBlinking(true);
	  else if(parameter[i] == 7)
	    callback->setReverse(true);
	  else if(parameter[i] == 8)
	    callback->setConcealed(true);
	  else if(parameter[i] == 9)
	    callback->setStrikeout(true);
	  else if(parameter[i] == 20)
	    callback->setFraktur(true);
	  else if(parameter[i] == 21)
	    callback->setDoubleUnderline(true);
	  else if(parameter[i] == 22) {
	    callback->setFaint(false);
	    callback->setIntense(false);
	  } else if(parameter[i] == 23) {
	    callback->setItalic(false);
	    callback->setFraktur(false);
	  } else if(parameter[i] == 24)
	    callback->setUnderline(false);
	  else if(parameter[i] == 25) {
	    callback->setBlinking(false);
	    callback->setRapidBlinking(false);
	  } else if(parameter[i] == 27)
	    callback->setReverse(false);
	  else if(parameter[i] == 28)
	    callback->setConcealed(false);
	  else if(parameter[i] == 29)
	    callback->setStrikeout(false);
	  else if(parameter[i] >= 30 && parameter[i] <= 37)
	    callback->setFGColor(parameter[i]-30);
	  else if(parameter[i] == 39)
	    callback->resetFGColor();
	  else if(parameter[i] >= 40 && parameter[i] <= 47) 
	    callback->setBGColor(parameter[i]-40);
	  else if(parameter[i] == 49)
	    callback->resetBGColor();
	  else if(parameter[i] == 51)
	    callback->setFramed(true);
	  else if(parameter[i] == 52)
	    callback->setEncircled(true);
	  else if(parameter[i] == 53)
	    callback->setOverline(true);
	  else if(parameter[i] == 54) {
	    callback->setFramed(false);
	    callback->setEncircled(false);
	  } else if(parameter[i] == 55)
	    callback->setOverline(false);
	}
      }
    } else if(command == 'K') {
      if(!parameter.empty()) {
	for(unsigned int i=0; i<parameter.size(); ++i) {
	  if(parameter[i] == 0) {
	    callback->clearEOL();
	  } else if(parameter[i] == 1) {
	    callback->clearBOL();
	  } else if(parameter[i] == 2) {
	    callback->clearLine();
	  }
	}
      }
    } else if(command == 'J') {
      if(!parameter.empty()) {
	for(unsigned int i=0; i<parameter.size(); ++i) {
	  if(parameter[i] == 0) {
	    callback->clearEOS();
	  } else if(parameter[i] == 1) {
	    callback->clearBOS();
	  } else if(parameter[i] == 2) {
	    callback->clearWindow();
	  }
	}
      }
    } else if(command == 'H' || command == 'f') {
      int x=0;
      int y=0;
      if(parameter.size() >= 1) { y = parameter[0]-1; }
      if(parameter.size() >= 2) { x = parameter[1]-1; }
      callback->setCursor(x, y);
    } else if(command == 'D') {
      if(!parameter.empty()) {
	callback->setCursor(max((int)callback->getCursorX()-1, 0),
			    callback->getCursorY());
      }
    } else {
      DEBUG("Unimplemented ANSI Command: " << command << endl);
    }
  } else if(mode == PARSE && execState == SCHAR) {
    if(command == '0') {
      vt100special = true;
    } else {
      vt100special = false;
    }
  } else if(mode != PARSE) {
    if(command == 'm') {
      for(unsigned int i=0; i<parameter.size(); ++i) {
	if(parameter[i] >= 30 && parameter[i] <= 37)
	  fgColor = parameter[i]-30;
	else if(parameter[i] >= 40 && parameter[i] <= 47)
	  bgColor = parameter[i]-40;
      }
    }
  }

  /* testing...
  cout << "\x1b";
  if(!parameter.empty()) {
    cout << "[" << parameter[0];
    for(int i=1; i<parameter.size(); ++i) {
      cout << ";" << parameter[i];
    }
  }
  cout << command;

  cout << "Ctrl ";
  if(space) { cout << "Space "; }
  cout << command << "(";
  if(!parameter.empty()) {
    cout << parameter[0];
    for(int i=1; i<parameter.size(); ++i) {
      cout << ", " << parameter[i];
    }
  }
  cout << ")";
  */
}

void CtrlParser::print(const QChar* from,
		       const QChar* to) {

  if(from != to) {
    if(mode == PARSE) {
      callback->printRaw(encodeCharset(from, to));
    } else {
      stripped += encodeCharset(from, to);
      for(const QChar* it = from; it != to; ++it) {
	colored += '0'+fgColor;
	colored += '0'+bgColor;
	colored += encodeCharset(it);
      }
    }
  }
}

void CtrlParser::putparam(const QChar* from,
			  const QChar* to) {
  if(from < to) {
    // TODO: perhaps take \0's out of the string?
    param.push_back(std::pair<int, bool>(QString(from, to-from).toInt(), true));
  } else {
    param.push_back(std::pair<int, bool>(0, false));
  }
}

void CtrlParser::initDefaultParams() {
  // a zero as default parameter needn't be given
  defaultParams['H'].push_back(1);
  defaultParams['H'].push_back(1);
  defaultParams['f'].push_back(1);
  defaultParams['f'].push_back(1);
  defaultParams['D'].push_back(1);
}

#ifdef TESTMAIN

#include <fstream>
#include <string>

int main() {
  CtrlParser p;
  std::string s;
  while(!std::cin.eof()) {
    std::cin >> s;
    //    cout << s;
    p.parse(s.c_str());
  }
  return 0;
}

/* old perl code
sub ____executeAnsiCmd {
  my ($self,$text)=@_;
  my $backBuffer = "";

  if($text =~ s/^\x1b\[(\d+(;\d+)*)?m//) {
    if(defined $1) {
      foreach my $attr (split /;/,$1) {
	if($attr == 0) { $self->resetAttributes; }
	elsif($attr == 1) { $self->setIntensive(1); }
	elsif($attr == 2) { $self->setIntensive(0); }
	elsif($attr == 4) { $self->setUnderline(1); }
	elsif($attr == 5) { $self->setBlinking(1); }
	elsif($attr == 7) { $self->setReverse(1); }
	elsif($attr >= 30 && $attr <= 37) { 
	  $self->setFGColor($attr-30); 
	} elsif($attr >= 40 && $attr <= 47) { 
	  $self->setBGColor($attr-40); 
	} else {
	  print "Warning: unknown ANSI-Attribute: $attr\n";
	}
      }
    } else {
      $self->resetAttributes;
    }
  } elsif($text =~ s/^\x1b\[K//) {
#    print "clearing EOL\n";
    $self->clearEOL;
  } elsif($text =~ s/^\x1b\[2K//) {
#    print "clearing line\n";
    $self->clearLine;
  } elsif($text =~ s/^\x1b\[2J//) {
#    print "clearing screen\n";
    $self->clear;
  } elsif($text =~ s/^\x1b\[J//) {
    my ($COL, $ROW) = $self->getCursorPos();
#    print "deleting downwards from ($COL, $ROW)\n";
    $self->clearEOL;
    my $y = $self->getCursorY()+1;
    while($y < $self->getLines()) {
      $self->setCursor($self->getCursorX(), $y);
      $self->clearLine();
      ++$y;
    }
    $self->setCursor($COL, $ROW);
  } elsif($text =~ s/^\x1b(D)//) {
    $self->scrollLines(1);
  } elsif($text =~ s/^\x1b(M)//) {
    $self->scrollLines(-1);
  } elsif($text =~ s/^\x1b\[s//) {
#    print "saving cursor\n";
    $self->{_SAVED_CURSOR} = [ $self->getCursorPos() ];
  } elsif($text =~ s/^\x1b\[u//) {
#    print "restoring cursor\n";
    if(ref $self->{_SAVED_CURSOR}) {
      $self->setCursor(@{$self->{_SAVED_CURSOR}});
    }
  } elsif($text =~ s/^\x1b\[c//) {
    # Query device code: 
    # XM::send("\x1b[xpertmud0c");
    XM::send("\x1b[ansi0c");
   } elsif($text =~ s/^\x1b\[6n//) {
     # Query Cursor Pos:
     my ( $COL, $ROW ) = $self->getCursorPos(); 
     ++$COL; ++$ROW;
     XM::send("\x1b\[${ROW};${COL}R");
   } elsif($text =~ s/^\x1b\[(\d+)d//) { #set y
     $self->setCursor(0,$1-1);
   } elsif($text =~ s/^\x1b\[(\d+)?A//) { #cursor up
     my $m=$1||1;
     $self->setCursor($self->getCursorX,$self->getCursorY-$m);
   } elsif($text =~ s/^\x1b\[(\d+)?B//) { #cursor down
     my $m=$1||1;
     $self->setCursor($self->getCursorX,$self->getCursorY+$m);
   } elsif($text =~ s/^\x1b\[(\d+)?D//) { #cursor left
     my $m=$1||1;
     $self->setCursor($self->getCursorX-$m,$self->getCursorY);
   } elsif($text =~ s/^\x1b\[(\d+)?C//) { #cursor right
     my $m=$1||1;
     $self->setCursor($self->getCursorX+$m,$self->getCursorY);
  } elsif($text =~ s/^\x1b\[(\d+;\d+)?[Hf]//) {
    if (defined $1) {
      my ($ROW, $COL) = split /;/,$1;
#      print "setting cursor to ($COL, $ROW)\n";

      $COL--; $ROW--;
      $self->setCursor($COL, $ROW);
    } else {
#      print "setting cursor to (00, 00)\n";
      $self->setCursor(0,0);
    }
  } elsif($text =~ s/^\t//) {
    my $cp=8 - $self->getCursorX % 8;
    $text = (' ' x $cp).$text;
  } else {
    if(length($text) > 10) {
      print "Warning: unknown ANSI-Code ",join(', ',map { sprintf '0x%x',ord } 
					       split//, $text),"\n";
      print "\"".substr($text, 1)."\"\n";
      $text=substr($text,1);
    } else {
      $backBuffer = $text;
      $text = "";
    }
  }
  return ($text, $backBuffer);
}
*/


#endif
