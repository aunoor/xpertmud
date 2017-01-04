package XM; 
use diagnostics;
use strict; 

use vars qw($onTextEntered $onKeyPressed $onTextReceived $onTimer $onHelp
	    $statusWindow $onEcho $isEcho
	    $onLineReceived $writeAtOnceRegExp $dontWriteAtOnceRegExp

	    $aliasPreprocessing $keyPreprocessing $triggerPreprocessing

	    $showKeyNames
	   );

$isEcho = 1;
my @bBuffers;
my @curPos;

my @aliases;
my @keyBindings;
my @triggers;
my @timers;

my $time = 0.0;

sub BLACK   { return 0; }
sub RED     { return 1; }
sub GREEN   { return 2; }
sub YELLOW  { return 3; }
sub BLUE    { return 4; }
sub MAGENTA { return 5; }
sub CYAN    { return 6; }
sub WHITE   { return 7; }


sub addIncludeDir( $ ) {
  unshift @INC,shift;
}

############################################################
# Trigger functions
############################################################
sub addTrigger {
  my ( $name, $regExp, $toExp, $flag, $active ) = @_;

  if(scalar(@_) < 5) {
    $active = 1;
  }

  my $subRef = undef;
  my $desc = "$name: ";
  if(ref $toExp ne "CODE") {
    $desc .= "s/$regExp/$toExp/g";
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

  statusWindow()->print("Adding trigger [ $desc ].\n");
  unshift(@triggers, [ $name, $desc, $subRef, $active ]);
}

sub delTrigger {
  my $regExp = shift;
  my $l = $#triggers;

  @triggers = grep { $_->[0] !~ /$regExp/ } @triggers;

  my $diff = $l - $#triggers;
  statusWindow()->print("Deleted $diff triggers.\n");
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

  statusWindow()->print("Changed the state of $count triggers.\n");
}

sub listTriggers {
  statusWindow()->print("Trigger list:\n");
  foreach my $trigger (@triggers) {
    my ( $name, $desc, $subRef, $active ) = @$trigger;

    statusWindow()->print($desc);
    if($active) {
      statusWindow()->print(" *ACTIVE*\n");
    } else {
      statusWindow()->print(" *INACTIVE*\n");
    }
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
    if($doBreak) { last; }
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
  my $desc = "$name: ";
  if(ref $toExp ne "CODE") {
    $desc .= "s/$regExp/$toExp/g";
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

  statusWindow()->print("Adding alias [ $desc ].\n");
  unshift(@aliases, [ $name, $desc, $subRef, $active ]);
}

sub delAlias {
  my $regExp = shift;
  my $l = $#aliases;

  @aliases = grep { $_->[0] !~ /$regExp/ } @aliases;

  my $diff = $l - $#aliases;
  statusWindow()->print("Deleted $diff aliases.\n");
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

  statusWindow()->print("Changed the state of $count aliases.\n");
}

sub listAliases {
  statusWindow()->print("Alias list:\n");
  foreach my $alias (@aliases) {
    my ( $name, $desc, $subRef, $active ) = @$alias;

    statusWindow()->print($desc);
    if($active) {
      statusWindow()->print(" *ACTIVE*\n");
    } else {
      statusWindow()->print(" *INACTIVE*\n");
    }
  }
  statusWindow()->print("\n");
}

sub executeAliases {
  my $text = shift;

  foreach my $alias (@aliases) {
    my ( $name, $desc, $subRef, $active ) = @$alias;
    my $doBreak = undef;
    if($active) {
      ( my $doBreak, $text ) = &$subRef($text);
    }
    if($doBreak) { last; }
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

  my $desc = "$name: /$onKey/ -> ";
  if(ref $subRef ne "CODE") {
    $desc .= "\"".$subRef."\"";
    my $text = $subRef;
    $subRef = sub { wrap_onTextEntered("$text"); }
  } else {
    $desc .= "sub { ... }";
  }

  statusWindow()->print("Added key binding [ $desc ].\n");
  unshift(@keyBindings, [ $name, $desc, $onKey, $subRef, $active ]);
}

sub delKeyBinding {
  my $regExp = shift;
  my $l = $#keyBindings;

  @keyBindings = grep { $_->[0] !~ /$regExp/ } @keyBindings;

  my $diff = $l - $#keyBindings;
  statusWindow()->print("Deleted $diff keyBindings.\n");
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

  statusWindow()->print("Changed the state of $count key bindings.\n");
}

sub listKeyBindings {
  statusWindow()->print("Key binding list:\n");
  foreach my $keyBinding (@keyBindings) {
    my ( $name, $desc, $onKey, $subRef, $active ) = @$keyBinding;

    statusWindow()->print($desc);
    if($active) {
      statusWindow()->print(" *ACTIVE*\n");
    } else {
      statusWindow()->print(" *INACTIVE*\n");
    }
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
sub addTimer {
  my ( $name, $inc, $subRef, $active ) = @_;

  if(scalar(@_) < 4) {
    $active = 1;
  }

  my $desc = "$name: +$inc -> ";
  if(ref $subRef ne "CODE") {
    $desc .= "\"".$subRef."\"";
    my $text = $subRef;
    $subRef = sub { wrap_onTextEntered("$text"); }
  } else {
    $desc .= "sub { ... }";
  }

  statusWindow()->print("Added timer [ $desc ].\n");
  unshift(@timers, [ $name, $desc, $inc, $subRef, $active ]);
}

sub delTimer {
  my $regExp = shift;
  my $l = $#timers;

  @timers = grep { $_->[0] !~ /$regExp/ } @timers;

  my $diff = $l - $#timers;
  statusWindow()->print("Deleted $diff timers.\n");
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

  statusWindow()->print("Changed the state of $count timers.\n");
}

sub listTimers {
  statusWindow()->print("Timer list:\n");
  foreach my $timer (@timers) {
    my ( $name, $desc, $inc, $subRef, $active ) = @$timer;

    statusWindow()->print($desc);
    if($active) {
      statusWindow()->print(" *ACTIVE*\n");
    } else {
      statusWindow()->print(" *INACTIVE*\n");
    }
  }
  statusWindow()->print("\n");
}

sub getTime {
  return $time;
}



sub parse {
  my $fileName = shift;
  do "$fileName";
  if($@) {
    showPerlError($@);
  }
}

sub help {
  if(ref $onHelp eq "CODE") {
    &$onHelp(@_);
  }
}

$onHelp = sub {
  my $helpOn = shift;

  if(!$helpOn) {
    statusWindow()->printANSI("

xpertmud online help
********************

type
#help \"command\"

Commands:
  parse

");
  } elsif($helpOn eq "parse") {
    statusWindow()->printANSI("

Command: parse
**************

This is used to parse a script in \@INC by using
the perl do command. The only differnce is that
parse will print an error if perl reports errors
during do. So this is the recommended way to
load your scripts.

Example:
#parse \"myscript.pl\"

");
  }

  return undef;
};

sub sysinitInitialize {
  addIncludeDir(".");

  addAlias("Eval", "^#", sub {
	     my $text = shift;
	     eval(substr($text, 1));
	     if($@) { showPerlError($@); }
	     return "";
	   }, 1);

  addKeyBinding('Scroll-Up', '........ Prior', sub {
    statusWindow()->scrollLines(-statusWindow()->getLines()+2);  return 1; 
  });
  addKeyBinding('Scroll-Down', '........ Next', sub {
    statusWindow()->scrollLines(statusWindow()->getLines()-2);  return 1; 
  });

  statusWindow()->printANSI("

Welcome to xpertmud
*******************

type \"#help\"
for further information.

");
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

};

$onTextEntered=sub {
  my $text = shift;

  if(ref $aliasPreprocessing eq "CODE") {
    $text = &$aliasPreprocessing($text);
  }

  $text = executeAliases($text);

  if($text) {
    XM::send($text."\r\n");
    if($isEcho) {
      statusWindow()->setFGColor(CYAN);
      statusWindow()->print("$text\n");
      statusWindow()->resetAttributes();
    }
  }
};
#addTrigger("LOGIN:", "^By what name do you wish to be known?", sub { XM::send("Soranus\n"); return "Name specified\n"; }, 1);
#addTrigger("LOGIN:", "^Password:", sub { XM::send("globbi\n"); return "Password specified\n"; }, 1);
#addTrigger("HEALTH:", '^\* HP.*> $', sub { return undef; }, 1);
$onKeyPressed =sub {
  my $key = shift;
  my $ascii = shift;

  if($showKeyNames) {
    statusWindow()->setFGColor(2);
    statusWindow()->printANSI("\n$key");
    statusWindow()->resetAttributes();
  }

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
      XM::sendKey($key, $ascii);
      return 1;
    }
  }

  return undef;
};
#addTrigger("1", ".*", sub { statusWindow()->print("heheUIUIUIAAAAA\n"); }, undef, undef);

my $linePart = "";
$writeAtOnceRegExp = ".?";
$dontWriteAtOnceRegExp = "^\$";
$onTextReceived = sub {
  my $text=shift;

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
	statusWindow()->printANSI($line);

      } else {
	if(($line =~ /$writeAtOnceRegExp/) &&
	   ($line !~ /$dontWriteAtOnceRegExp/)) {

	  $line = executeTriggers($line);
	  statusWindow()->printANSI($line);

	} else {
	  $line = executeTriggers($line);
	  $linePart = $line;
	}
      }
    }
  }
};

$onEcho = sub {
  my $state = shift;

  $isEcho = $state;
  XM::echoMode($state);
};

sub wrap_onEcho {
  if (ref $onEcho eq 'CODE') {
    eval {
      &$onEcho(@_);
    };
    showPerlError($@) if $@;
  } 
}

sub wrap_onTextEntered {
  if (ref $onTextEntered eq 'CODE') {
    eval {
      &$onTextEntered(@_);
    };
    showPerlError($@) if $@;
  }
}

sub wrap_onTimer {
  if (ref $onTimer eq 'CODE') {
    eval {
      &$onTimer(@_);
    };
    showPerlError($@) if $@;
  }
}

sub wrap_onKeyPressed {
  if (ref $onKeyPressed eq 'CODE') {
    my $res;
    eval {
      $res = !!&$onKeyPressed(@_);
    };
    showPerlError($@) if $@;
    return $res;
  }
  return 0;
}

sub wrap_onTextReceived {
  if (ref $onTextReceived eq 'CODE') {
    eval {
      &$onTextReceived(@_);
    };
    showPerlError($@) if $@;
  }
}

sub statusWindow() {
  if (ref $statusWindow) {
    return $statusWindow;
  } else {
    my $tmp=0;
    $statusWindow= bless \$tmp,"XM::TextBufferWindow";
    return $statusWindow;
  }
}

sub sendKey {
  my $key = shift;
  my $ascii = shift;
  if($ascii =~ /^.$/) {
    XM::send("$ascii");
  }
}

package XM::Window;
use strict;

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
  

sub getPosition {
  my $self=shift;
  return ($self->getPositionX, $self->getPositionY);
}


sub DESTROY {
  my $self=shift;
  $self->close if $self->isValid();
}

package XM::TextWindow; 
use strict; 
use vars qw(@ISA);
@ISA= qw(XM::Window);

sub new { 
  my ($this,$title)=@_; 
  my $class = ref($this) || $this; 
  my $tmp= -1; 
  my $self=bless \$tmp,$class; 
  
  $$self = initialize(); 
  $self->setTitle($title) if $title; 
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

sub executeAnsiCmd {
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
    $curPos[$$self] = [ $self->getCursorPos() ];
  } elsif($text =~ s/^\x1b\[u//) {
#    print "restoring cursor\n";
    if(ref $curPos[$$self]) {
      $self->setCursor(@{$curPos[$$self]});
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
    } else {
      $backBuffer = $text;
      $text = "";
    }
  }
  return ($text, $backBuffer);
}

sub printANSI {
  my ($self, $text) = @_;
  my $backBuffer = undef;

  if(defined $bBuffers[$$self]) {
    $text = $bBuffers[$$self].$text;
  }

  while(length($text)) {

    if($text =~ s/^([^\x1b\t]+)//) {
      $self->print($1);
    }
    if($text =~ /^[\x1b\t]/) {
      ($text, $backBuffer) = $self->executeAnsiCmd($text);
    }
  }
  $bBuffers[$$self] = $backBuffer;
}

package XM::TextBufferWindow; 
use strict; 
use vars qw(@ISA);
@ISA= qw(XM::TextWindow);

sub new { 
  my ($this,$title)=@_; 
  my $class = ref($this) || $this; 
  my $tmp= -1; 
  my $self=bless \$tmp,$class; 
  
  $$self = initialize(); 
  $self->setTitle($title) if $title; 
  return $self; 
}


1;
