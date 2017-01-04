# set encoding to utf-8
# that stuff is not very complete implemented afaik
# so uppercase and stuff is crap but regexpes work at least .. I think
$KCODE ="U";
require "weakref"

module XM;


$isEcho = 1;
$localEcho = 1;

$triggers = [];
$aliases =  [];
$keyBindings = [];
$timers = [];
$delayed = [];
$evalscope = binding();
$mtime = 0.0;

BLACK = 0
RED = 1
GREEN = 2
YELLOW = 3
BLUE = 4
MAGENTA = 5
CYAN = 6
WHITE = 7

def setLocalEcho(state)
    $localEcho = state;
end

def showKeyNames()
  rex = RegExp.compile('........ X');
  addKeyBinding('#SHOWKEYS', /^(.*)$/,
		(proc { |m|  
		  statusWindow().setFGColor(2);
		  statusWindow().write(m[1]+"\n");
		  statusWindow().resetAttributes();
		  if m[1] =~ rex
		    delKeyBinding('^#SHOWKEYS$');
		  end
		  true;
		}),  true);
  statusWindow().write("Key name mode active, press \"x\" to\n get back to normal mode\n");
end

def addIncludeDir(dir)
    $:.unshift(dir);
end

def prs(text)
    statusWindow().write(text);
end

def psw(text)
    statusWindow().write(text);
end
$__statusWindow = nil;
       

def connection(id=0)
    return XMConnection.getByID(id)
end

def sendText(text, id=0)
    connection(id).send(text)
end

def activeWindow()
    return XMTextWindow(XM.getActiveWindow())
end

def focusedInputLine()
    return XMInputLine.getByID(XM.getFocusedInputLine())
end

         
def statusWindow()
  if$__statusWindow == nil
    $__statusWindow = XMTextBufferWindow.new(0);
    p $__statusWindow;
  end
  
  return $__statusWindow;
end

############################################################
# Trigger functions
############################################################

def addTrigger(name, regExp, toExp, flag=false, active=true)
  rex = nil;
  if regExp.kind_of? Regexp
    rex = regExp;
  else
    rex = Regexp.new(regExp);
  end
  desc = "%s:\t" % name;
  subRef = nil;
  if toExp.kind_of? Proc
    tex = toExp;
    desc += "/%s/ -> function(): ..." % rex.source;
    subRef = proc { |text| 
      buf = text;
      ret = nil;
      if((buf.gsub!(rex) { |x| toExp.call(x) }) != nil)
	#            statusWindow().write("\"%s\"\n" % text)
	if buf != nil and buf =~ /^$/
	    buf = nil;
	end
	 ret = [ flag, buf ];
	
      else
	ret =  [ false, buf ];
      end
      ret;
    }
  else
    desc += "s/%s/%s/g" % [ rex.source, toExp.to_s ];
    subRef = proc { |text| 
      buf = text;
      ret = nil;
      if(buf.gsub!(rex,toExp) != nil)
	#            statusWindow().write("\"%s\"\n" % text)
	if buf != nil and buf =~ /^$/
	    buf = nil;
	end
	ret =  [ flag, buf ];
	
      else
	ret =  [ false, buf ];
      end
      ret;
    };
  end 
  $triggers.unshift  [ name, desc, subRef, active ];
end


def delTrigger(regExp)
  l = triggers.length;
  if not regExp.kind_of? Regexp
    regExp = Regexp.new(regExp);
  end
  $triggers.delete_if { |x| x[0] =~ regExp };
   return l- $triggers.length;
end


def setTriggerState(regExp, active)
  count = 0;
  if not regExp.kind_of? Regexp
    regExp = Regexp.new(regExp);
  end
  $triggers.map! { |x| 
    if x[0] =~ regExp
      ++count;
      x[3] = active;
    end
  };
  return count;
end


def listTriggers()
  statusWindow().setIntensive(true);
  statusWindow().write("Trigger list:\n");
  statusWindow().setIntensive(false);
  for trigger in $triggers do    
    statusWindow().write(trigger[1]);
    if(trigger[3])
      statusWindow().setFGColor(GREEN);
      statusWindow().write(" *ACTIVE*\n");
    else
      statusWindow().setFGColor(RED);
      statusWindow().write(" *INACTIVE*\n");
    end
    statusWindow().resetAttributes();
  end
  statusWindow().write("\n");
end


def executeTriggers(text)
  for trigger in $triggers do
    name, desc, subRef, active  = trigger;
    doBreak = false;
    if(active)
      doBreak, text = subRef.call(text)
    end
    if(doBreak)
      return text
    end
  end
  return text
end


############################################################
# Alias functions
############################################################

def addAlias(name, regExp, toExp, flag=false, active=true)
  desc = "#{name}:\t";
  if toExp.kind_of? Proc
    rex = regExp;
    desc += "/%s/ -> function(): ..." % regExp.source;
    subRef = proc { |text| 
      buf = text;
      ret = nil;
      if(buf != nil and (buf.gsub!(rex) { |x| toExp.call(x) }) != nil)
	#            statusWindow().write("\"%s\"\n" % text)
	if buf != nil and buf =~ /^$/
	    buf = nil;
	end
	ret =  [ flag, buf ];
	
      else
	ret = [ false, buf ];
      end
      ret;
    }
  else
    rex = regExp;
    desc += "s/%s/%s/g" % [ rex.source, toExp.to_s ];
    subRef = proc { |text| 
      buf = text;
      ret = nil;
      if(buf.gsub!(rex,toExp) != nil)
	#            statusWindow().write("\"%s\"\n" % text)
	if buf != nil and buf =~ /^$/
	    buf = nil;
	end
	ret = [ flag, buf ];
	
      else
	ret = [ false, buf ];
      end
      ret;
    };
  end 
  $aliases.unshift  [ name, desc, subRef, active ];
end


def delAlias(regExp)
  l = aliases.length;
  if not regExp.kind_of? Regexp
    regExp = Regexp.new(regExp);
  end
  $aliases.delete_if { |x| x[0] =~ regExp };
   return l - $aliases.length;
end


def setAliasState(regExp, active)
  count = 0;
  if not regExp.kind_of? Regexp
    regExp = Regexp.new(regExp);
  end
  $aliases.map! { |x| 
    if x[0] =~ regExp
      ++count;
      x[3] = active;
    end
  };
  return count;
end


def listAliases()
  statusWindow().setIntensive(true);
  statusWindow().write("Alias list:\n");
  statusWindow().setIntensive(false);
  for i in $aliases do
    name, desc, subRef, active  = i;
    statusWindow().write(desc);
    if(active)
      statusWindow().setFGColor(GREEN);
      statusWindow().write(" *ACTIVE*\n");
    else
      statusWindow().setFGColor(RED);
      statusWindow().write(" *INACTIVE*\n");
    end
    statusWindow().resetAttributes();
  end
  statusWindow().write("\n");
end


def executeAliases(text)
  for i in $aliases do
    name, desc, subRef, active  = i;
    doBreak = false;
    if(active)
      doBreak, text = subRef.call(text)
    end
    if(doBreak)
      return text
    end
  end
  return text
end



############################################################
# KeyBinding functions
############################################################

def addKeyBinding(name, onKey, action, active=true)
  if not onKey.kind_of? Regexp
    onKey = Regexp.new(onKey);
  end
  desc = "%s:\t" % name;
  subRef = nil;
  if action.kind_of? Proc
    desc += "/%s/ -> function(): ..." % onKey.source;
    subRef = action
  else
    desc += "/%s/ -> \"%s\"" % [ onKey.source, action ];
    subRef = proc { |match|  wrap_onTextEntered(action) };
  end
  $keyBindings.unshift([ name, desc, onKey, subRef, active ]);
end



def delKeyBinding(regExp)
  l = $keyBindings.length;
  if not regExp.kind_of? Regexp
    regExp = Regexp.new(regExp);
  end
  $keyBindings.delete_if { |x| x[0] =~ regExp };
   return l - $keyBindings.length;
end



def setKeyBindingState(regExp, active)
  count = 0;
  if not regExp.kind_of? Regexp
    regExp = Regexp.new(regExp);
  end
  $keyBindings.map! { |x| 
    if x[0] =~ regExp
      ++count;
      x[4] = active;
    end
  };
  return count;
end


def listKeyBindings()
  statusWindow().setIntensive(true);
  statusWindow().write("Key binding list:\n");
  statusWindow().setIntensive(false);
  for keyBinding in $keyBindings do
    name, desc, onKey, subRef, active  = keyBinding;    
    statusWindow().write(desc);
    if active
      statusWindow().setFGColor(GREEN);
      statusWindow().write(" *ACTIVE*\n");
    else
      statusWindow().setFGColor(RED);
      statusWindow().write(" *INACTIVE*\n");
    end
    statusWindow().resetAttributes();
  end
  statusWindow().write("\n");
end


def executeKeyBindings(key)
  for keyBinding in $keyBindings do
    name, desc, onKey, subRef, active  = keyBinding;
    if active
      match = onKey.match(key);
      if match != nil
	subRef.call(match);
	return true;
      end
    end
  end
  return false;
end

############################################################
# Timer functions
############################################################

def addTimer(name, inc, action, active=true)
  desc = "%s: +%d ->" % [ name, inc.to_s ];
  subRef = nil;
  if not action.kind_of? Proc
    desc += "\"" + action + "\"";
    subRef = proc { wrap_onTextEntered(action); };
  else
    subRef = action;
    desc += "function(): ...";
  end
  $timers.unshift([ name, desc, inc, subRef, active, 0 ]);
end


def delTimer(regExp)
  l = $timers.length;
  if not regExp.kind_of? Regexp
    regExp = Regexp.new(regExp);
  end
  $timers.delete_if { |x| x[0] =~ regExp };
  return l - $timers.length;
end


def setTimerState(regExp, active)
  count = 0;
  if not regExp.kind_of? Regexp
    regExp = Regexp.new(regExp);
  end
  $timers.map! { |x| 
    if x[0] =~ regExp
      ++count;
      x[4] = active;
    end
  };
  return count;
end


def setTimerInc(regExp, newInc)
  count = 0;
  if not regExp.kind_of? Regexp
    regExp = Regexp.new(regExp);
  end
  $timers.map! { |x|
    if  x[0] =~ regExp
      ++count;
      x[2] = newInc;
      x[1] = "%s: +%d ->" % [ a[0], newInc ];
      a[1] += a[3].to_s;
    end
  }
  return count;
end


def listTimers()
  statusWindow().setIntensive(true);
  statusWindow().write("Timer list:\n");
  statusWindow().setIntensive(false);
  for timer in $timers do
    name, desc, inc, subRef, active, last  = timer;
    statusWindow().write(desc);
    if active
      statusWindow().setFGColor(GREEN);
      statusWindow().write(" *ACTIVE*\n");
    else
      statusWindow().setFGColor(RED);
      statusWindow().write(" *INACTIVE*\n");
    end
    statusWindow().resetAttributes();
  end
  statusWindow().write("\n");
end


def getTime()
    return $mtime;
end

# TODO locals will not be included ... I dont like this ... 
# maybe eval with global bindings ?
# Cant get the global binding .. so I guess this will have to do
# TODO change to use $evalscope
def parse(file)
  begin
    load(file);
  rescue LoadError
    XM.showError('File not found: "%s"\n' % file);
  end
end


############################################################
# Delayed functions
############################################################
def addDelayed(name, delay, action)
  desc = "%s: +%d -> " % [ name, delay ];
  subRef = nil;
  if not action.kind_of? Proc
    desc += "\"" + action + "\"";
    subRef = proc { wrap_onTextEntered(action) };
  else
    subRef = action;
    desc += "function(): ...";
  end       
  $delayed.unshift [ name, desc, delay+getTime(), subRef ];
end


def delDelayed(regExp)
  l = $delayed.length;
  $delayed.delete_if { |x| x[0] =~ regExp };
  return l - $ddiff.length;
end


def setDelayed(regExp,delay)
  count = 0;
  if not regExp.kind_of? Regexp
    regExp = Regexp.new(regExp);
  end
  $delayMap.each { |x|
    if x[0] =~ regExp
      count +=1;
      x[2] = delay+getTime()
      x[1] = "%s: +%d -> \"%s\"" % [ a[0], delay, a[3].to_s ];
    end
  };
  return count;
end;

def listDelayed()
  statusWindow().setIntensive(true);
  statusWindow().write("Delayed action list:\n");
  statusWindow().setIntensive(false);
  for dela in $delayed do
    a = dela;
    statusWindow().write( ("%s: +" % a[0]) + (a[2]-getTime()).to_s + " -> "  + "\"" + a[3].to_s + "\"" + "\n");
    statusWindow().resetAttributes();
    statusWindow().write("\n");
  end
end


############################################################
# Help
############################################################
$helps = {};

def help(helpOn = "", base = $helps, preDesc = "")
  prs("Sorry, not yet
");
end



############################################################
# Callbacks
############################################################

def sysinitInitialize()
  addAlias("Eval", /^.*$/, $__scripting, true);
  addAlias("Compose", /^.*$/, $__compose, false);
    
  addKeyBinding('Scroll-Up', '........ PgUp', proc { |m|
		  statusWindow().scrollLines(-1* statusWindow().getLines()+2);
		});
  addKeyBinding('Scroll-Down', '........ PgDown', proc { |m|
		  statusWindow().scrollLines(statusWindow().getLines()-2);
		});

  statusWindow().write("
    
Welcome to xpertmud
*******************

type \"#help()\"
for further information.
The # operator: '#help \"#\"'

");
end


$onTimer = proc do
  if($mtime == 0.0)
      sysinitInitialize();
  end
        
  $mtime += 0.25;

  for timer in $timers do
    name, desc, inc, action, active, lastCall  = timer;
    if(active and (lastCall + inc < $mtime))
      timer[5] = $mtime;
      action.call;
    end
  end
  for dela in $delayed do
    name, desc, delay, subRef  = dela;
    if(delay <= $mtime)
      subRef.call;
    end
  end
  $delayed.delete_if { |x| x[2] <= $mtime };
end
           

$aliasPreprocessing = nil;

$onTextEntered = proc do |text|
  if $aliasPreprocessing.kind_of? Proc
    text = $aliasPreprocessing.call(text);
  end
  text = executeAliases(text);
    
  if(text != nil and text != "")
    XM.Connection_send(0,text + "\r\n");
    if $isEcho and $localEcho
      statusWindow().setFGColor(CYAN);
      statusWindow().write(text + "\n");
      statusWindow().resetAttributes();
    end
  end
end


$keyPreprocessing = nil;

$onKeyPressed = proc do |key, ascii|
  ret = nil;
  if $keyPreprocessing.kind_of? Proc
    key, ascii  = $keyPreprocessing.call(key, ascii);
  end
  if key != "" 
    if executeKeyBindings(key)
      ret = true;
    end
    if $isEcho
      ret = false;
    else
      sendKey(key, ascii);
      ret = true;
    end
  else
    ret = false;
  end
  ret;
end

$onConnectionEstablished = proc do |id|
  statusWindow().write("Connection %d established.\n" % id);
end

$onConnectionDropped = proc do |id|
  statusWindow().write("Connection %d dropped.\n" % id);
end

$writeAtOnceRegExp = /.?/;
$dontWriteAtOnceRegExp = /^$/;


def setWriteRegExps(waoRegExp, dwaoRegExp)
  if waoRegExp != nil
    $writeAtOnceRegExp = waoRegExp;
  end
  if dwaoRegExp != nil
    $dontWriteAtOnceRegExp = dwaoRegExp;
  end
end

def resetWriteRegExps()
  $writeAtOnceRegExp = /.?/;
  $dontWriteAtOnceRegExp = /^$/;
end

$triggerPreprocessing = nil;
$onLineReceived = nil;
$linePart = "";
resetWriteRegExps();


$onTextReceived = proc do |text, id|
  if $triggerPreprocessing.kind_of? Proc
    text = $triggerPreprocessing.call(text);
  end

  toProcess = $linePart + text;
  if toProcess != nil and toProcess != "" 
    toProcess.each do |line|
      linePart = "";
      if line.gsub!(/\r?\n\r?/,"\n") != nil
	if $onLineReceived.kind_of? Proc
	  line = $onLineReceived.call(line);
	end
	
	line = executeTriggers(line);
	
	if line != nil  and line != "" 
	  
	  statusWindow().write(line);
	end
      else
	if $writeAtOnceRegExp =~ line and (not $dontWriteAtOnceRegExp =~ line)
	  line = executeTriggers(line);
	  if line != nil and line != ""
	    statusWindow().write(line);
	  end
	else
	  linePart = line
	end
      end
    end
  end
end


$onEcho = proc do |state, id|
  $isEcho = state;
  XM.echoMode(state);
end


# Ok, someone explain please what this regexp does ?
def sendKey(key, ascii)
  if ascii != nil and (/^.$/ =~ ascii)
     XM.Connection_send(0,ascii);
   end
end

def wrap_onEcho(state, id=0)
  begin
    $onEcho.call(state, id);
  rescue => error
    dumpError(error);
  end
end


def wrap_onTextEntered(text)
  begin
    $onTextEntered.call(text);
  rescue => error
    dumpError(error);
  end
end


def wrap_onTimer()
  begin
    $onTimer.call();
  rescue => error
    dumpError(error);
  end
end


def wrap_onKeyPressed(key, ascii)
  begin
    return $onKeyPressed.call(key, ascii);
  rescue => error
    dumpError(error);
  end
  return false;
end

def wrap_onTextReceived(line, id=0)
  begin
    $onTextReceived.call(line,id);
  rescue => error
    dumpError(error);
  end
end

def wrap_onConnectionEstablished(id)
  begin
    $onConnectionEstablished.call(id);;
  rescue => error
    dumpError(error);
  end
end

def wrap_onConnectionDropped(id)
  begin
    $onConnectionDropped.call(id);
  rescue => error
    dumpError(error);
  end
end


def wrap_mouseDown(id, x, y)
  win = getWindow(id);
  if win != nil
    win.onMouseDown(x, y);
  end
end

def wrap_mouseDrag(id, x, y)
  win = getWindow(id);
  if win != nil
    win.onMouseDrag(x, y);
  end
end

def wrap_mouseUp(id, x, y)
  win = getWindow(id);
  if win != nil
    win.onMouseUp(x, y);
  end
end

def wrap_pluginCall(id, function, args)
  win = getWindow(id);
  if win != nil
    return win.pluginCall(function, args);
  end
  return nil;
end


$allWindows = Hash.new;

def getWindow(id)
  if $allWindows.has_key? id
    begin
      v =  $allWindows[id];
      # just access the ref and see if it throws up
      v.id;
      return v;
    rescue
      # weakref dead... delete it
      $allWindows.delete(id);
      return nil;
    end
  else
    return nil;
  end
end

class XMWindow
# no finalizer in ruby ....
#    def __del__(self)
#      self.close();
#    end
  
  def initialize()
    @__id = nil;
  end
  
  def register()
    $allWindows[@__id] = WeakRef.new(self);
  end
  
  def setID(id)
    # to ensure this crap gets correctly sweeped ... geez    
    # GC's suck.. sometimes
    ObjectSpace.undefine_finalizer(self);
    @__id = id;
    ObjectSpace.define_finalizer(self,proc { XM.Window_close(id); }); 
  end
  
  def getID()
    return @__id;
  end
  
  def setTitle(text)
    XM.Window_setTitle(getID(), text);
  end
  
  def show()
    XM.Window_show(getID());
  end
  
  def hide()
    XM.Window_hide(getID());
  end
  
  def close()
    XM.Window_close(getID());
  end
  
  def minimize()
    XM.Window_minimize(getID());
  end
  
  def maximize()
    XM.Window_maximize(getID());
  end
  
  def move(x, y)
    XM.Window_move(getID(), x, y);
  end
  
  def resize(x, y)
    XM.Window_resize(getID(), x, y);
  end
  
  def isValid()
    return XM.Window_isValid(getID());
  end
  
  def raiseWin()
    XM.Window_raise(getID());
  end
  
  def lowerWin()
    XM.Window_lower(getID());
  end
  
  def getPositionX()
    return XM.Window_getPositionX(getID());
  end
  
  def getPositionY()
    return XM.Window_getPositionY(getID());
  end
  
  def getSizeX()
    return XM.Window_getSizeX(getID());
  end
  
  def getSizeY()
    return XM.Window_getSizeY(getID());
  end
  
  def showCaption()
    XM.Window_showCaption(getID());
  end
  
  def hideCaption()
    XM.Window_hideCaption(getID());
  end
end



class XMTextWindow < XMWindow

  def initialize(id = nil)
    super;
    if(id != nil and not id.kind_of? String)
      setID(id);
    else
      setID(XM.TextWindow_initialize());
      if(id != nil)
	setTitle(id);
      end
    end
    initTextWindow();
  end

  def initTextWindow()
    @onMouseDownFuncs = [];
    @onMouseDragFuncs = [];
    @onMouseUpFuncs = [];
    register();
  end

  def getCursorPos()
    return [ getCursorX(), getCursorY() ];
  end

  def getSizeChars()
    return [ getColumns(), getLines() ];
  end

  def registerMouseDownEvent(func)
    @onMouseDownFunc.push  func;
  end

  def registerMouseDragEvent(func)
    @onMouseDragFuncs.push func;
  end
  
  def registerMouseUpEvent(func)
    @onMouseUpFuncs.push func;
  end

  def onMouseDown(x, y)
    @onMouseDownFuncs.each { |func| func.call(x, y); }
  end
            
  def onMouseDrag(x, y)
    @onMouseDragFuncs.each { |func| func.call(x, y); }
  end

  def onMouseUp(x, y)
    @onMouseUpFuncs.each { |func| func.call(x, y); }
  end

  def newline()
    XM.TextWindow_newline(getID());
  end

  def clear()
    XM.TextWindow_clear(getID());
  end

  def clearLine()
    XM.TextWindow_clearLine(getID());
  end

  def clearEOL()
    XM.TextWindow_clearEOL(getID());
  end

  def clearBOL()
    XM.TextWindow_clearBOL(getID());
  end

  def clearEOS()
    XM.TextWindow_clearEOS(getID());
  end

  def clearBOS()
    XM.TextWindow_clearBOS(getID());
  end

  def resetAttributes()
    XM.TextWindow_resetAttributes(getID());
  end

  def writeRaw(text)
    XM.TextWindow_printRaw(getID(), text);
  end

  def write(text)
    XM.TextWindow_print(getID(), text);
  end

  def setBGColor(color)
    XM.TextWindow_setBGColor(getID(), color);
  end

  def setFGColor(color)
    XM.TextWindow_setFGColor(getID(), color);
  end

  def setDefaultBGColor(color)
    XM.TextWindow_setDefaultBGColor(getID(), color);
  end

  def setDefaultFGColor(color)
    XM.TextWindow_setDefaultFGColor(getID(), color);
  end

  def scrollLines(num)
    XM.TextWindow_scrollLines(getID(), num);
  end

  def scrollColumns(num)
    XM.TextWindow_scrollColumns(getID(), num);
  end

  def setBold(state)
    XM.TextWindow_setBold(getID(), state);
  end

  def setBlinking(state)
    XM.TextWindow_setBlinking(getID(), state);
  end

  def setUnderline(state)
    XM.TextWindow_setUnderline(getID(), state);
  end

  def setReverse(state)
    XM.TextWindow_setReverse(getID(), state);
  end

  def setIntensive(state)
    XM.TextWindow_setIntensive(getID(), state);
  end

  def resizeChars(x, y)
    XM.TextWindow_resizeChars(getID(), x, y);
  end

  def setCursor(x, y)
    XM.TextWindow_setCursor(getID(), x, y);
  end

  def getCursorX()
    return XM.TextWindow_getCursorX(getID());
  end

  def getCursorY()
    return XM.TextWindow_getCursorY(getID());
  end

  def getColumns()
    return XM.TextWindow_getColumns(getID());
  end

  def getLines()
    return XM.TextWindow_getLines(getID());
  end

  def getCharAt(x, y)
    return XM.TextWindow_getCharAt(getID(), x, y);
  end

  def setFont(name, size)
    return XM.TextWindow_setFont(getID(), name, size);
  end

  def getFGColorAt(x, y)
    return XM.TextWindow_getFGColorAt(getID(), x, y);
  end

  def getBGColorAt(x, y)
    return XM.TextWindow_getBGColorAt(getID(), x, y);
  end

  def enableMouseEvents()
    XM.TextWindow_enableMouseEvents(getID(), self);
  end

  def disableMouseEvents()
    XM.TextWindow_disableMouseEvents(getID(), self);
  end

  def getWordWrapColumn()
    XM.TextBufferWindow_getWordWrapColumn(getID());
  end

  def setWordWrapColumn(c)
    XM.TextBufferWindow_setWordWrapColumn(getID(), c);
  end
end


class XMTextBufferWindow < XMTextWindow
  def initialize(id = nil)
    # super not called, since it just overlaps
    # with setting the ID
    if(id != nil and not id.kind_of? String)
      setID(id);
    else
      setID(XM.TextBufferWindow_initialize());
      if(id != nil)
	setTitle(id);
      end
    end
    initTextWindow();
  end
end

class XMPlugin < XMWindow
  def initialize(lib, cls, title)
    super;
    id = XM.Plugin_initialize(lib, cls);
    if id < 0
      raise "Unable to create plugin";
    end
    setID(id);
    setTitle(title);
    register();
  end

  def pluginCall(functions, args)
    return nil;
  end
end




class XMConnection

  @@_instances = {};

  def initialize(host, port=0, onConnect="")
    if port == 0
      @id = XM.Connection_initialize(host, port, onConnect)
      @@__instances[id] = self
    else
      @id = host
    end
  end

  def getID()
    return @id
  end

  def self.getByID(id)
    if not @@__instances.has_key?(id)
      @@__instances[id] = new(id)
    end
    return @@__instances[id]
  end

#  def init(host,port,onConnect="")
#    return XM.Connection_initialize(getID(),host,port,onConnect)
#  end

  def open(host,port,onConnect="")
    XM.Connection_open(getID(),host,port,onConnect)
  end
  
  def send(string)
    XM.Connection_send(getID(),string)
  end

  def close()
    XM.Connection_close(getID())
  end

  def setInputEncoding(enc)
    XM.Connection_setInputEncoding(getID(),enc)
  end

  def setOutputEncoding(enc)
    XM.Connection_setOutputEncoding(getID(),enc)
  end
end


class XMInputLine
  @@__private = 0
  @@__instances = {}
  
  def initialize(id)
    if @@__private == 0
      raise "Private Constructor Error!"
    end
    @id = id
  end

  def getID()
    return @id
  end

  def self.getByID(id)
    if not @@__instances.has_key?(id)
      @@__private = 1
      @@__instances[id] = XMInputLine.new(id)
      @@__private = 0
    end
    return @@__instances[id]
  end
        

  def getCursorX()
    return XM.InputLine_getCursorX(getID())
  end

  def getCursorY()
    return XM.InputLine_getCursorY(getID())
  end

  def setCursorX(x)
    XM.InputLine_setCursorX(getID(),x)
  end

  def setCursorY(y)
    XM.InputLine_setCursorY(getID(),y)
  end

  def getText()
    return XM.InputLine_getText(getID())
  end

  def setText(text)
    XM.InputLine_setText(getID(),text)
  end

end



def dumpError(error)
  # Write a nice error
  #message; TODO: Nice output!
  
  errStr = "Traceback (most recent call last):\n" + error.message + "\n" + error.backtrace.join("\n");
            
  XM.showError(errStr);
end


$__scripting = proc do |match|
  ret = nil;
  if(match =~ Regexp.new('^#[^#]'))
    eval match[1...match.length], $evalscope;
    ret= "";
  else
    ret= match;
  end
  ret;
end


$composeBuffer = ""
$__compose = proc do |match|
  ret = nil;
  if match=~ Regexp.new('^##$');
    $composeBuffer += "\n";
    eval $composeBuffer, $evalscope;
    $composeBuffer = "";
    ret= "";
  elsif match =~ Regexp.new('^##')
    $composeBuffer += match[2...match.length] + "\n";
    statusWindow().setFGColor(BLUE);
    statusWindow().write("> " + match[2...match.length] + "\n");
    statusWindow().resetAttributes();
    ret = "";
  else
    ret = match;
  end
  ret;
end


module XMInputHistory
  @@commandList = []
  @@argumentList = []
  @@lresults = []

  def self.reset()
    @@connandList = []
    @@argumentList = []
  end

  def self.completionRequest(inputLine)
    line = inputLine.getText()
    words = line.split
    slist = nil
    if words.length == 0 or line =~ /\s+$/
      return nil
    elsif words.length == 1
      slist = @@commandList
    else
      slist = @@argumentList
    end
    word = words.pop;
    result = []
    slist.each { |template|
      if word.length < template.length and template.slice(0,word.length) == word
	result.push(template)
      end
    }
    if result.size == 1
      line.slice!(-1,word.length)
      inputLine.setText(line + result.pop)
    else
      shortest=word;
      result.each { |match|
       ns=$word;
       # hmm, geht das schneller?
       for(my $i=length($word); $i < length($shortest) and $i < length($match); ++$i) {
	 if (substr($shortest,$i,1) eq substr($match,$i,1)) {
	   $ns.=substr($shortest,$i,1);
	 } else {
	   last;
	 }
       }
       if (length($ns) < length($shortest)) {
	 $shortest=$ns;
       }
     }
      @lresults = result
    end
  end

  def self.setup
    addKeyBinding('COMPLETEREQUEST', '........ Tab', proc { |m|
XMInputHistory.completionRequest(XMInputLine.getByID(XM.getFocusedInputLine()))
		  });    
    addKeyBinding('COMPLETEDISPLAY', '.*', proc { |m|
		    setKeyBindingState(/^COMPLETEDISPLAY$/, false)
		    XMInputHistory.completionDisplay()
		  },false);    
    addAlias("COMPLETELEARN", /^.*$/, proc { |m|
	       XMInputHistory.addToHistory(m)
	     }, true);

  end

  def self.addToHistory(line)

  end
end

