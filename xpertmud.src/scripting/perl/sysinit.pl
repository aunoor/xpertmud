use warnings;
no warnings qw/once/;
use strict;

use vars qw($onTextEntered $onKeyPressed $onTextReceived $onTimer $onHelp
	    $onConnectionEstablished $onConnectionDropped
	    $statusWindow $onEcho $isEcho $localEcho
	    $onLineReceived $writeAtOnceRegExp $dontWriteAtOnceRegExp

	    $aliasPreprocessing $keyPreprocessing $triggerPreprocessing

	   );


$isEcho = 1;

my @aliases;
my @keyBindings;
my @triggers;
my @timers;
my @delayed;

my $time = 0.0;

sub BLACK   { return 0; }
sub RED     { return 1; }
sub GREEN   { return 2; }
sub YELLOW  { return 3; }
sub BLUE    { return 4; }
sub MAGENTA { return 5; }
sub CYAN    { return 6; }
sub WHITE   { return 7; }
sub INTENSIVE { return 8; }

############## GLOBAL STUFF ######################
tie *STDOUT,"XMRedirector", statusWindow();
tie *STDERR,"XMRedirector",  statusWindow(),RED;
tie *STDIN,"XMRedirector", statusWindow(),MAGENTA;

$SIG{__WARN__}=sub { XM::showError($_[0]); };

END {
  # cleanup, to prevent warnings during global construction to scramble the stack
  untie *STDOUT;
  untie *STDERR;
  untie *STDIN;
  $SIG{__WARN__}='DEFAULT';
}
##################################################

############### HELP STUFF #######################
my $helpNode = new XMXmlNode("xmhelp");
my $helpAll = undef;

sub recreateHelpAll {
  $helpAll = new XMXmlNode("help");
  $helpAll->setAttribute("name", "Xpertmud Online Help");
  foreach my $node (@{ $helpNode->getSubNodes() }) {
    if($node->getTagName() eq "help") {
      foreach my $inner(@{ $node->getSubNodes() }) {
	$helpAll->addChild($inner);
      }
    }
  }
}

sub addHelp {
  my ($nodes) = @_;
  foreach my $node (@$nodes) {
    foreach my $nodeIn (@{ $helpNode->getSubNodes() }) {
      if($node->getAttribute('id') eq $nodeIn->getAttribute('id')) {
	$helpNode->removeChild($nodeIn);
      }
    }
    $helpNode->addChild($node);
  }
  recreateHelpAll();
}

sub parseHelp {
  my ($file) = @_;
  my $found = 0;
  foreach my $path (@INC) {
    my $helpParserDTD = new helpdtd();
    my $helpParser = XMXmlParser($helpParserDTD);
    my $f;
    if(open($f, $path . "/" . $file)) {
      $found = 1;
      while(my $line = <$f>) {
	$helpParser->parse($line);
      }
      addHelp($helpParser->getNodes());
      close $f;
      last;
    }
  }
  if(not $found) {
    XM::showError("File not found: \"$file\"\n");
  }
}


parseHelp("functions.xml");
parseHelp("topics.xml");
parseHelp("classes.xml");

##################################################

sub showKeyNames() {
  addKeyBinding("#SHOWKEYS", "^(.*)\$", sub {
		  statusWindow()->setFGColor(GREEN);
		  statusWindow()->print("$1\n");
		  statusWindow()->resetAttributes();
		  if($1 =~ /..... X/) {
		    delKeyBinding("^#SHOWKEYS\$");
		  }
		  return 1;
		}, 1);
  statusWindow()->print("Key name mode active, press \"x\" to\nget back to normal mode\n");
}

sub addIncludeDir( $ ) {
  push(@INC,shift);
}

my %muxcodes=
    (
     '%%'=>'%', '%t'=>"\t", '% '=>' ','%r'=>"\n", 
     '%cf'=>"\x1b[5m", '%ci'=>"\x1b[7m", 
     '%ch'=>"\x1b[1m", '%cn'=>"\x1b[0m", '%cu'=>"\x1b[4m", '%cx'=>"\x1b[30m", 
     '%cr'=>"\x1b[31m", '%cg'=>"\x1b[32m", '%cy'=>"\x1b[33m", '%cb'=>"\x1b[34m",
     '%cm'=>"\x1b[35m", '%cc'=>"\x1b[36m", '%cw'=>"\x1b[37m", '%cX'=>"\x1b[40m", 
     '%cR'=>"\x1b[41m", '%cG'=>"\x1b[42m", '%cY'=>"\x1b[43m", '%cB'=>"\x1b[44m",
     '%cM'=>"\x1b[45m", '%cC'=>"\x1b[46m", '%cW'=>"\x1b[47m"
     );

sub ansi( $ ) {
    my $text=shift;
    my $out="";
    foreach my $chunk (split /(\%c[fihnuxrgybmcwXRGYBMCW]|\%\%|\%[rts ])/, $text) {
	if (exists $muxcodes{$chunk}) {
	    $out.=$muxcodes{$chunk};
	} else {
	    $out .= $chunk;
	}
    }

    return $out;
}

sub prs { statusWindow()->print(shift); }
sub psw { statusWindow()->print(shift); }

sub connection {
  my $id = shift || 0;
  return XMConnection::getByID($id);
}

sub sendText {
  my $text = shift;
  my $id = shift || 0;
  connection($id)->send($text);
}

sub activeWindow {
  return XMWindow::get(XM::getActiveWindow());
}

sub focusedInputLine {
  return XMInputLine::getByID(XM::getFocusedInputLine());
}

sub statusWindow {
  if (ref $statusWindow) {
    return $statusWindow;
  } else {
    my %tmp=(
	     _ID=>0
	    );
    $statusWindow= bless \%tmp,"XMTextBufferWindow";
    $statusWindow->{_CIRCULAR}=$statusWindow;
    return $statusWindow;
  }
}


############################################################
# Trigger functions
############################################################
sub addTrigger {
  my ( $name, $regExp, $toExp, $flag, $active, $execute) = @_;

  if(scalar(@_) < 5) {
    $active = 1;
  }

  my $subRef = undef;
  my $desc = "$name: \t";
  if(ref $toExp eq "CODE") {
      $desc .= "/$regExp/ -> sub { ... }";
      $desc =~ s/\n/\\n/g;
      $subRef = sub {
	  $_ = shift;
	  if(/$regExp/) {
	      $_ = &$toExp($_);
	      return ( $flag, $_ );
	  } else {
	      return ( undef, $_ );
	  }
      };
  } elsif ($execute) {
      $desc .= "/$regExp/ -> do($toExp)";
      $desc =~ s/\n/\\n/g;
      $subRef = sub {
	  my $line = shift;
	  if($line=~/$regExp/) {
	      wrap_onTextEntered($toExp);	      
	  }
	  return ( undef, $line );
      };
  } else {
      $desc .= "s/$regExp/$toExp/g";
      $desc =~ s/\n/\\n/g;
      $subRef = sub {
	  $_ = shift;
	  if(s/$regExp/$toExp/g) {
	      return ( $flag, $_ );
	  } else {
	      return ( undef, $_ );
	  }
      };
  }

  #statusWindow()->print("Adding trigger [ $desc ].\n");
  unshift(@triggers, [ $name, $desc, $subRef, $active ]);
}

sub delTrigger {
  my $regExp = shift;
  my $l = $#triggers;

  @triggers = grep { $_->[0] !~ /$regExp/ } @triggers;

  my $diff = $l - $#triggers;
  #statusWindow()->print("Deleted $diff triggers.\n");
  return $diff;
}

sub setTriggerState {
  my ($regExp, $active) = @_;
  my $count = 0;
  map { 
    if($_->[0] =~ /$regExp/) {
      ++$count;
      $_->[3] = $active;
    }
  } @triggers;

  #statusWindow()->print("Changed the state of $count triggers.\n");
  return $count;
}

sub listTriggers {
  statusWindow()->setIntensive(1);
  statusWindow()->print("Trigger list:\n");
  statusWindow()->setIntensive(0);
  foreach my $trigger (@triggers) {
    my ( $name, $desc, $subRef, $active ) = @$trigger;

    statusWindow()->print($desc);
    if($active) {
      statusWindow()->setFGColor(GREEN);
      statusWindow()->print(" *ACTIVE*\n");
    } else {
      statusWindow()->setFGColor(RED);
      statusWindow()->print(" *INACTIVE*\n");
    }
    statusWindow()->resetAttributes();
  }
  statusWindow()->print("\n");
}

sub executeTriggers {
  my $line = shift;

  foreach my $trigger (@triggers) {
    my ( $name, $desc, $subRef, $active ) = @$trigger;
    my $doBreak = undef;
    if($active) {
      ( $doBreak, $line ) = &$subRef($line);
    }
    if($doBreak || !(defined $line)) { last; }
  }

  return $line;
}

############################################################
# Alias functions
############################################################
sub addAlias {
  my ($name, $regExp, $toExp, $flag, $active) = @_;

  if(scalar(@_) < 5) {
    $active = 1;
  }

  my $subRef = undef;
  my $desc = "$name: \t";
  if(ref $toExp ne "CODE") {
    $desc .= "s/$regExp/$toExp/g";
    $desc =~ s/\n/\\n/g;
    $subRef = sub {
      $_ = shift;
      if(s/$regExp/$toExp/g) {
	return ( $flag, $_ );
      } else {
	return ( undef, $_ );
      }
    }
  } else {
    $desc .= "/$regExp/ -> sub { ... }";
    $desc =~ s/\n/\\n/g;
    $subRef = sub {
      $_ = shift;
      if(/$regExp/) {
	$_ = &$toExp($_);
	return ( $flag, $_ );
      } else {
	return ( undef, $_ );
      }
    }
  }

  #statusWindow()->print("Adding alias [ $desc ].\n");
  unshift(@aliases, [ $name, $desc, $subRef, $active ]);
}

sub updateAlias {
  addAlias(@_);
  my $alias=shift @aliases;

  my $count = 0;
  map { 
    if($_->[0] eq $alias->[0]) {
      ++$count;
      $_->[1] = $alias->[1];
      $_->[2] = $alias->[2];
      $_->[3] = $alias->[3];
    }
  } @aliases;

  if ($count==0) {
      unshift @aliases,$alias;
      ++$count;
  }
  #statusWindow()->print("Updated $count aliases.\n");
  return $count;
}

sub delAlias {
  my $regExp = shift;
  my $l = $#aliases;

  @aliases = grep { $_->[0] !~ /$regExp/ } @aliases;

  my $diff = $l - $#aliases;
  #statusWindow()->print("Deleted $diff aliases.\n");
  return $diff;
}

sub setAliasState {
  my ($regExp, $active) = @_;
  my $count = 0;
  map { 
    if($_->[0] =~ /$regExp/) {
      ++$count;
      $_->[3] = $active;
    }
  } @aliases;

  #statusWindow()->print("Changed the state of $count aliases.\n");
  return $count;
}

sub listAliases {
  statusWindow()->setIntensive(1);
  statusWindow()->print("Alias list:\n");
  statusWindow()->setIntensive(0);
  foreach my $alias (@aliases) {
    my ( $name, $desc, $subRef, $active ) = @$alias;

    statusWindow()->print($desc);
    if($active) {
      statusWindow()->setFGColor(GREEN);
      statusWindow()->print(" *ACTIVE*\n");
    } else {
      statusWindow()->setFGColor(RED);
      statusWindow()->print(" *INACTIVE*\n");
    }
    statusWindow()->resetAttributes();
  }
  statusWindow()->print("\n");
}

sub executeAliases {
  my $text = shift;

  foreach my $alias (@aliases) {
    my ( $name, $desc, $subRef, $active ) = @$alias;
    my $doBreak = undef;
    if($active) {
      ( $doBreak, $text ) = &$subRef($text);
    }
    if($doBreak or !defined $text) { last; }
  }

  return $text;
}

############################################################
# Key binding functions
############################################################
sub addKeyBinding {
  my ( $name, $onKey, $subRef, $active ) = @_;

  if(scalar(@_) < 4) {
    $active = 1;
  }

  my $desc = "$name: /$onKey/\t  -> \t";
  if(ref $subRef ne "CODE") {
    $desc .= "\"".$subRef."\"";
    my $text = $subRef;
    $subRef = sub { wrap_onTextEntered($text); }
  } else {
    $desc .= "sub { ... }";
  }
  $desc =~ s/\n/\\n/g;

  #statusWindow()->print("Added key binding [ $desc ].\n");
  unshift(@keyBindings, [ $name, $desc, $onKey, $subRef, $active ]);
}

sub delKeyBinding {
  my $regExp = shift;
  my $l = $#keyBindings;

  @keyBindings = grep { $_->[0] !~ /$regExp/ } @keyBindings;

  my $diff = $l - $#keyBindings;
  #statusWindow()->print("Deleted $diff keyBindings.\n");
  return $diff;
}

sub setKeyBindingState {
  my ($regExp, $active) = @_;
  my $count = 0;
  map { 
    if($_->[0] =~ /$regExp/) {
      ++$count;
      $_->[4] = $active;
    }
  } @keyBindings;

  #statusWindow()->print("Changed the state of $count key bindings.\n");
  return $count;
}

sub listKeyBindings {
  statusWindow()->setIntensive(1);
  statusWindow()->print("Key binding list:\n");
  statusWindow()->setIntensive(0);
  foreach my $keyBinding (@keyBindings) {
    my ( $name, $desc, $onKey, $subRef, $active ) = @$keyBinding;

    statusWindow()->print($desc);
    if($active) {
      statusWindow()->setFGColor(GREEN);
      statusWindow()->print(" *ACTIVE*\n");
    } else {
      statusWindow()->setFGColor(RED);
      statusWindow()->print(" *INACTIVE*\n");
    }
    statusWindow()->resetAttributes();
  }
  statusWindow()->print("\n");
}

sub executeKeyBindings {
  my $key = shift;

  foreach my $keyBinding (@keyBindings) {
    my ( $name, $descr, $onKey, $subRef, $active ) = @$keyBinding;
    if($active && ($key =~ /$onKey/)) {
      &$subRef();
      return 1;
    }
  }
  return undef;
}

############################################################
# Timer functions
############################################################
sub getTime {
  return $time;
}

sub addTimer {
  my ( $name, $inc, $subRef, $active ) = @_;

  if(scalar(@_) < 3 or !defined $name ) {
    #onHelp "addTimer" aufrufen!
    return;
  }
  if(scalar(@_) < 4) {
    $active = 1;
  }

  my $desc = "$name: +$inc\t  -> \t";
  if(ref $subRef ne "CODE") {
    $desc .= "\"".$subRef."\"";
    my $text = $subRef;
    $subRef = sub { wrap_onTextEntered($text); }
  } else {
    $desc .= "sub { ... }";
  }
  $desc =~ s/\n/\\n/g;

  #statusWindow()->print("Added timer [ $desc ].\n");
  unshift(@timers, [ $name, $desc, $inc, $subRef, $active, getTime() ]);
}

sub delTimer {
  my $regExp = shift;
  my $l = $#timers;

  @timers = grep { $_->[0] !~ /$regExp/ } @timers;

  my $diff = $l - $#timers;
  #statusWindow()->print("Deleted $diff timers.\n");
  return $diff;
}

sub setTimerState {
  my ($regExp, $active) = @_;
  my $count = 0;
  map {
    if($_->[0] =~ /$regExp/) {
      ++$count;
      $_->[4] = $active;
    }
  } @timers;

  #statusWindow()->print("Changed the state of $count timers.\n");
  return $count;
}


sub setTimerInc {
  my ($regExp, $newInc) = @_;
  my $count = 0;
  map {
    if($_->[0] =~ /$regExp/) {
      ++$count;
      $_->[2] = $newInc;
    }
  } @timers;

  #statusWindow()->print("Changed the increment of $count timers.\n");
  return $count;
}


sub listTimers {
  statusWindow()->setIntensive(1);
  statusWindow()->print("Timer list:\n");
  statusWindow()->setIntensive(0);
  foreach my $timer (@timers) {
    my ( $name, $desc, $inc, $subRef, $active, $lastrun ) = @$timer;

    statusWindow()->print($desc);
    if($active) {
      statusWindow()->setFGColor(GREEN);
      statusWindow()->print(" *ACTIVE*\n");
    } else {
      statusWindow()->setFGColor(RED);
      statusWindow()->print(" *INACTIVE*\n");
    }
    statusWindow()->resetAttributes();
  }
  statusWindow()->print("\n");
}


############################################################
# Delay functions
############################################################
sub addDelayed {
  my ( $name, $delay, $subRef ) = @_;
  if(scalar(@_) < 3 or !defined $name ) {
    #onHelp "addDelayed" aufrufen!
    return;
  }
  my $desc = "$name: \t";
  if(ref $subRef ne "CODE") {
    $desc .= "\"".$subRef."\"";
    my $text = $subRef;
    $subRef = sub { wrap_onTextEntered($text); }
  } else {
    $desc .= "sub { ... }";
  }

  $desc =~ s/\n/\\n/g;
  #statusWindow()->print("Added timer [ $desc ].\n");
  unshift(@delayed, [ $name, $desc, $delay+getTime(), $subRef ]);
}

sub delDelayed {
  my $regExp = shift;
  my $l = $#delayed;

  @delayed = grep { $_->[0] !~ /$regExp/ } @delayed;

  my $diff = $l - $#timers;
  #statusWindow()->print("Deleted $diff timers.\n");
  return $diff;
}

sub setDelayed {
  my ($regExp,$delay)=@_;
  $delay+=getTime();
  my $count = 0;
  map {
    if($_->[0] =~ /$regExp/) {
      ++$count;
      $_->[2] = $delay;
    }
  } @delayed;

  #statusWindow()->print("Changed the state of $count timers.\n");
  return $count;
}

sub listDelayed {
  statusWindow()->setIntensive(1);
  statusWindow()->print("Delayed Action list:\n");
  statusWindow()->setIntensive(0);
  foreach my $timer (@delayed) {
    my ( $name, $desc, $run, $subRef ) = @$timer;

    $run-=getTime();
    statusWindow()->print("$run\t  -> \t$desc\n");
  }
  statusWindow()->print("\n");

}

############################################################
# Misc functions
############################################################

sub parse {
  my $fileName = shift;
  do $fileName;
  if($@) {
    XM::showError($@);
  }
  if ($!) {
    XM::showError($! . " while parsing '$fileName'");
  }
}

sub help {
  if(ref $onHelp eq "CODE") {
    &$onHelp(@_);
  }
}

my $layoutGenerator = new helplayout('plain');
sub layout {
  my ($node, $subHelpText) = @_;
  $layoutGenerator->setSubHelpText($subHelpText);
  return $layoutGenerator->generate($node);
}

my $separator = "::";
$onHelp = sub {
  my ($topic) = @_;
  if((not defined $topic) || (ref $topic)) {
    $topic = '';
  }
  my $subHelpText = 'Use #help "';
  if($topic ne '') {
    $subHelpText .= $topic . $separator;
  }
  $subHelpText .= '<subtopic>" for help on a specific subtopic';
  if($topic eq '') {
    statusWindow()->print(layout($helpAll, $subHelpText));
    return;
  }
  my $search = $topic;
  my $finished = 0;
  my $node = $helpAll;
  my $found = 0;
  while(not $finished) {
    my $head;
    my $tail;
    if($search =~ /^(([^:]|(:[^:]))+)::(.*)$/) {
      $head = $1;
      $tail = $4;
    } else {
      $head = $search;
      $tail = "";
    }
    $found = 0;
    foreach my $child (@{ $node->getSubNodes() }) {
      if($child->hasAttribute('name') &&
	 $child->getAttribute('name') eq $head) {
	$search = $tail;
	$node = $child;
	if($tail eq '') {
	  my $helpMsg = layout($node, $subHelpText);
	  if($helpMsg ne "") {
	    $found = 1;
	    statusWindow()->print($helpMsg);
	  }
	  $finished = 1;
	} else {
	  $found = 1;
	}
	last;
      }
    }
    if(not $found) {
      $finished = 1;
    }
  }
  if(not $found) {
    statusWindow()->print("Couldn't find help on \"" . $topic . "\"\n");
  }

  return;
};

sub sysinitInitialize {

  addAlias("LOCAL_ECHO", qr/.?/, sub {
      my $text = shift;
      if($isEcho && defined $text) {
	  statusWindow()->setFGColor(CYAN);
	  statusWindow()->print("$text\n");
	  statusWindow()->resetAttributes();
      }
      return $text;
  });

  addAlias("Eval", qr/^\#/, sub {
	     my $text = shift;
	     eval(substr($text, 1));
	     if($@) { XM::showError($@); }
	     return undef;
	   }, 1);


  addKeyBinding('Scroll-Up', '.000.... PgUp', sub {
    statusWindow()->scrollLines(-int(statusWindow()->getLines()/2));  return 1;
  });
  addKeyBinding('Scroll-Down', '.000.... PgDown', sub {
    statusWindow()->scrollLines(int(statusWindow()->getLines()/2));  return 1;
  });

  statusWindow()->print("

Welcome to xpertmud
*******************

type \"#help\"
for further information.

");
}

sub setLocalEcho {
    my $state=shift;
    setAliasState(qr/^LOCAL_ECHO$/,$state);
}

$onTimer = sub {
  if($time == 0.0) {
    sysinitInitialize();
  }
  $time += 0.25;

  foreach my $timer (@timers) {
    my ( $name, $descr, $inc, $subRef, $active, $lastCall ) = @$timer;
    if($active && ($lastCall+$inc < $time)) {
      $timer->[5] = $time;
      &$subRef();
    }
  }
  foreach my $delayed (@delayed) {
    my ($name, $desc, $delay, $subRef) = @$delayed;
    if ($delay <= $time) {
      &$subRef();
    }
  }
# Delete in second step, so that a delayed function could reset
# itself or another delayed to a new time
  @delayed=grep { $_->[2] > $time } @delayed;
};

$onTextEntered=sub {
  my $text = shift;

  if(ref $aliasPreprocessing eq "CODE") {
    $text = &$aliasPreprocessing($text);
  }

  $text = executeAliases($text);

  if(defined $text) {
    sendText($text."\r\n");
  }
};

$onKeyPressed =sub {
  my $key = shift;
  my $ascii = shift;

  if(ref $keyPreprocessing eq "CODE") {
    ( $key, $ascii ) = &$keyPreprocessing($key, $ascii);
  }

  if(defined $key) {
    if(executeKeyBindings($key)) {
      return 1;
    }

    if($isEcho) {
      return undef;
    } else {
#      print ord($ascii) if defined $ascii;
      sendKey($key, $ascii);
      return 1;
    }
  }

  return undef;
};

$onConnectionEstablished = sub {
  my $id = shift;
  statusWindow()->print("Connection $id established.\n");
};

$onConnectionDropped = sub {
  my $id = shift;
  statusWindow()->print("Connection $id dropped.\n");
};

sub setWriteRegExps {
  $writeAtOnceRegExp = shift || $writeAtOnceRegExp;
  $dontWriteAtOnceRegExp = shift || $dontWriteAtOnceRegExp;
}

sub resetWriteRegExps {
  $writeAtOnceRegExp = ".?";
  $dontWriteAtOnceRegExp = "^\$";
}

my $linePart = "";
resetWriteRegExps();
$onTextReceived = sub {
  my $text=shift;
  my $id = shift;

  if(ref $triggerPreprocessing eq "CODE") {
    $text = &$triggerPreprocessing($text);
  }

  if (defined $text) {
    foreach my $line (split /(?<=\n)/, $linePart.$text) {
      $linePart = "";
      if($line =~ s/\r?\n$/\n/) {
	if(ref $onLineReceived eq "CODE") {
	  $line = &$onLineReceived($line);
	}
	$line = executeTriggers($line);
	if(defined $line && $line ne "") {
	  statusWindow()->print($line);
	}

      } else {
	if(($line =~ /$writeAtOnceRegExp/) &&
	   ($line !~ /$dontWriteAtOnceRegExp/)) {

	  $line = executeTriggers($line);
	  if(defined $line && $line ne "") {
	    statusWindow()->print($line);
	  }

	} else {
	  $linePart = $line;
	}
      }
    }
  }
};

$onEcho = sub {
  my $state = shift;
  my $id = shift;

  $isEcho = $state;
  XM::echoMode($state);
};

sub wrap_onEcho {
  if (ref $onEcho eq 'CODE') {
    eval {
      &$onEcho(@_);
    };
    XM::showError($@) if $@;
  }
}

sub wrap_onTextEntered {
  if (ref $onTextEntered eq 'CODE') {
    eval {
      &$onTextEntered(@_);
    };
    XM::showError($@) if $@;
  }
}

sub wrap_onTimer {
  if (ref $onTimer eq 'CODE') {
    eval {
      &$onTimer(@_);
    };
    XM::showError($@) if $@;
  }
}

sub wrap_onKeyPressed {
  if (ref $onKeyPressed eq 'CODE') {
    my $res;
    eval {
      $res = !!&$onKeyPressed(@_);
    };
    XM::showError($@) if $@;
    return $res;
  }
  return 0;
}

sub wrap_onTextReceived {
  if (ref $onTextReceived eq 'CODE') {
    eval {
      &$onTextReceived(@_);
    };
    XM::showError($@) if $@;
  }
}

sub wrap_onConnectionEstablished {
  if (ref $onConnectionEstablished eq 'CODE') {
    eval {
      &$onConnectionEstablished(@_);
    };
    XM::showError($@) if $@;
  }
}

sub wrap_onConnectionDropped {
  if (ref $onConnectionDropped eq 'CODE') {
    eval {
      &$onConnectionDropped(@_);
    };
    XM::showError($@) if $@;
  }
}

sub wrap_mouseDown {
  my ($id,$x,$y)=@_;
#  print "mouseDown on win $id, at ($x,$y)\n";
  my $win=XMWindow::get($id);
  if (defined $win and $win->isa("XMTextWindow")) {
    my $mdloop = $win->{_MOUSE_DOWN} || [];
    foreach my $funcRef (@{ $mdloop }) {
      eval {
	&$funcRef($win, $x, $y);
      };
      XM::showError($@) if $@;
    }
  }
}

sub wrap_mouseDrag {
  my ($id,$x,$y)=@_;
  #print "mouseDrag on win $id, at ($x,$y)\n";
  my $win=XMWindow::get($id);
  if (defined $win and $win->isa("XMTextWindow")) {
    my $mdloop = $win->{_MOUSE_DRAG} || [];
    foreach my $funcRef (@{ $mdloop }) {
      eval {
	&$funcRef($win, $x, $y);
      };
      XM::showError($@) if $@;
    }
  }
}

sub wrap_mouseUp {
  my ($id,$x,$y)=@_;
  #print "mouseUp on win $id, at ($x,$y)\n";
  my $win=XMWindow::get($id);
  if (defined $win and $win->isa("XMTextWindow")) {
    my $mdloop = $win->{_MOUSE_UP} || [];
    foreach my $funcRef (@{ $mdloop }) {
      eval {
	&$funcRef($win, $x, $y);
      };
      XM::showError($@) if $@;
    }
  }
}

sub wrap_pluginCall {
  my ($id,$function,$args)=@_;
  my $win=XMWindow::get($id);
  if (defined $win and $win->isa("XMPlugin")) {
    my $result;
    eval {
      $result=$win->function($function,$args);
    };
    XM::showError($@) if $@;
    return $result;
  }
  warn "PluginCall on invalid window";
  return 42;
}

sub sendKey {
  my $key = shift;
  my $ascii = shift;
  if ($key =~ /KP_Enter/ or $key =~ /Return/) {
    sendText("\n");
  } elsif(defined $ascii && $ascii =~ /^.$/) {
    sendText("$ascii");
  }
}

# include here, else we need to put all the above
# stuff into a package, which we don't want to...
use xmlparser; #FIXED
use helplayout; #FIXED

############################################################
package XMWindow;
use strict;
use warnings;

{
  my @allWindows;

  sub register {
    my $self=shift;
    $allWindows[$self->getID()]=$self;
    XM::weaken($allWindows[$self->getID()]);
  }
  
  sub get { # "static"
    my $id=shift;
    if($id == 0) {
      return ::statusWindow();
    }
    return $allWindows[$id];
  }
}

sub getSize {
  my $self=shift;
  return ($self->getSizeX, $self->getSizeY);
}

sub getWidth {
  return shift->getSizeX;
}

sub getHeight {
  return shift->getSizeY;
}

sub getID {
  return shift->{_ID};
}

sub getPosition {
  my $self=shift;
  return ($self->getPositionX, $self->getPositionY);
}


sub DESTROY {
  my $self=shift;
  $self->close if $self->isValid();
}

############################################################
package XMTextWindow;
use strict;
use warnings;

use vars qw(@ISA);
@ISA= qw(XMWindow);

sub new {
  my ($this,$title)=@_;
  my $class = ref($this) || $this;
  my %tmp= (
	    _ID=>-1
	   );
  my $self=bless \%tmp,$class;

  $self->{_ID} = initialize();
  $self->setTitle($title) if defined $title;
  $self->register;
  return $self;
}

sub getCursorPos {
  my $self=shift;
  return ($self->getCursorX, $self->getCursorY);
}

sub getSizeChars {
  my $self=shift;
  return ($self->getColumns, $self->getLines);
}

sub registerMouseDownEvent {
  my $self = shift;
  my $funcRef = shift;
  push(@{ $self->{_MOUSE_DOWN}}, $funcRef);
}

sub registerMouseDragEvent {
  my $self = shift;
  my $funcRef = shift;
  push(@{ $self->{_MOUSE_DRAG}}, $funcRef);
}

sub registerMouseUpEvent {
  my $self = shift;
  my $funcRef = shift;
  push(@{ $self->{_MOUSE_UP}}, $funcRef);
}

sub printMux {
  $_[0]->print(ansi($_[1]));
}

############################################################
package XMTextBufferWindow;
use strict;
use warnings;

use vars qw(@ISA);
@ISA= qw(XMTextWindow);

sub new {
  my ($this,$title)=@_;
  my $class = ref($this) || $this;
  my %tmp= (
	    _ID=>-1
	   );
  my $self=bless \%tmp,$class;

  $self->{_ID} = initialize();
  $self->setTitle($title) if defined $title;
  $self->register;
  return $self;
}


############################################################
package XMPlugin;
use strict;
use warnings;

use vars qw(@ISA);
@ISA= qw(XMWindow);

sub new {
  my ($this,$lib,$cls,$title)=@_;
  my $class = ref($this) || $this;
  my %tmp= (
	    _ID=>-1,
	    _LIBRARY=>$lib,
	    _CLASS=>$cls
	   );
  my $self=bless \%tmp,$class;

  $self->{_ID} = initialize($lib,$cls);

  return undef if ($self->getID()<0);

  $self->setTitle($title) if defined $title;
  $self->register;
  return $self;
}

sub function {
  my ($self,$id,$args)=@_;
  print "External function call on XMPlugin\n";
  print "FunctionID: $id, Argument: $args\n";
  print "Reimplement 'sub function' in your Plugin class!\n";
  return "Unimplemented Function";
}

############################################################
package XMConnection;
use strict;
use warnings;

sub getID {
  my ($self) = @_;
  return $self->{_ID};
}

{
  # TODO: weak refs?
  # remove in onConnectionDropped callback?
my $instances = {};

sub new {
  my ($this,$host,$port,$onConnect)=@_;
  my $class = ref($this) || $this;
  my %tmp= (
	    _ID=>-1,
	   );
  my $self=bless \%tmp,$class;

  $self->{_ID} = initialize($host,$port,$onConnect);

  return undef if ($self->getID()<0);

  $instances->{$self->getID()} = $self;

  return $self;
}

sub getByID {
  my ($id) = @_;
  if(not exists $instances->{$id}) {
    my $newCon = bless
      { _ID => $id }, "XMConnection";
    $instances->{$id} = $newCon;
  }
  return $instances->{$id};
}
}

############################################################
package XMInputLine;
use strict;
use warnings;

sub getID {
  my ($self) = @_;
  return $self->{_ID};
}

{
my $instances = {};

sub getByID {
  my ($id) = @_;
  if(not exists $instances->{$id}) {
    my $newLine = bless
      { _ID => $id }, "XMInputLine";
    $instances->{$id} = $newLine;
  }
  return $instances->{$id};
}
}

############################################################
package XMRedirector;
use strict;
use warnings;

sub TIEHANDLE {
  my $this=shift;
  my $class = ref $this || $this;

  my $win=shift;
  my $color=shift;
  my $self=bless {
		  win=>$win,
		 }, $class;
  if (defined $color) {
    $self->{color}=$color;
  }

  return $self;
}

sub changeWindow {
  my ($self,$win)=@_;
  $self->{win}=$win;
}

sub PRINT {
  my $self=shift;
  if (defined $self->{color}) {
    $self->{win}->setFGColor($self->{color});
  }
  foreach my $chunk (@_) {
    $self->{win}->print($chunk);
  }
  if (defined $self->{color}) {
    $self->{win}->resetAttributes;
  }
}


############################################################
package XM;
use strict;
use warnings;
no warnings qw/once/;

#conveniance/compatibility functions

*showKeyNames=\&::showKeyNames;
*addIncludeDir=\&::addIncludeDir;
*ansi=\&::ansi;
*prs=\&::prs;
*psw=\&::psw;
*connection=\&::connection;
*sendText=\&::sendText;
*activeWindow=\&::activeWindow;
*focusedInputLine=\&::focusedInputLine;
*statusWindow=\&::statusWindow;

*addTrigger=\&::addTrigger;
*delTrigger=\&::delTrigger;
*setTriggerState=\&::setTriggerState;
*listTriggers=\&::listTriggers;

*addAlias=\&::addAlias;
*updateAlias=\&::updateAlias;
*delAlias=\&::delAlias;
*setAliasState=\&::setAliasState;
*listAliases=\&::listAliases;

*addKeyBinding=\&::addKeyBinding;
*delKeyBinding=\&::delKeyBinding;
*setKeyBindingState=\&::setKeyBindingState;
*listKeyBindings=\&::listKeyBindings;

*addTimer=\&::addTimer;
*delTimer=\&::delTimer;
*setTimerState=\&::setTimerState;
*setTimerInc=\&::setTimerInc;
*listTimers=\&::listTimers;

*addDelayed=\&::addDelayed;
*delDelayed=\&::delDelayed;
*setDelayed=\&::setDelayed;
*listDelayed=\&::listDelayed;

*parse=\&::parse;
*setLocalEcho=\&::setLocalEcho;
*setWriteRegExps=\&::setWriteRegExps;
*resetWriteRegExps=\&::resetWriteRegExps;


1;
