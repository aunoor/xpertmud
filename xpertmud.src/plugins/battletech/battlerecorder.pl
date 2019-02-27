package BattleRec;
use strict;
use warnings;
use diagnostics;
use IO::File;

no warnings 'redefine';

my $recWindow=new XMTextBufferWindow("Recorder");
$recWindow->resizeChars(80,25);
$recWindow->show;

$recWindow->print(XM::ansi('%cr%ch%cuRecorder initialized%cn%r%r'));
$recWindow->print(XM::ansi('%crF10     -  Start/Stop recording%cn%r'));
$recWindow->print(XM::ansi('%crF9      -  Pause/Continue recording%cn%r'));
$recWindow->print(XM::ansi('%cr"playback <filename>"%cn%r%cr        -  Start Playback%cn%r'));
$recWindow->print(XM::ansi('%cr"stop"  -  Stop playback%cn%r'));
$recWindow->print(XM::ansi('%crF8      -  Faster playback%cn%r'));
$recWindow->print(XM::ansi('%crF7      -  Slower playback%cn%r'));

$recWindow->hide;
XM::statusWindow->maximize;


my $recFile;

my $recTime=0;


sub writeLog {
    my $line=shift;
    chomp($line);
    chomp($line);
    if ($recFile) {
	$recFile->print($recTime,':',$line,"\n");
    }
}

sub startRec {
    $recWindow->print(XM::ansi('%r%cr%chStarting Recorder%cn%r'));

    XM::setKeyBindingState(qr/^REC_START/,0);
    XM::setKeyBindingState(qr/^REC_STOP/,1);
    XM::setKeyBindingState(qr/^REC_PAUSE/,1);
    XM::setKeyBindingState(qr/^REC_UNPAUSE/,0);
    XM::setAliasState(qr/^PLAY_START/,0);

    my @d=localtime(time);
    my $fn=sprintf('BattleLog-%04d-%02d-%02d-%02d%02d%02d.log',
		   $d[5]+1900,$d[4]+1,$d[3],$d[2],$d[1],$d[0]);

    $recWindow->print(XM::ansi("Filename is: $fn\%r"));

    $recFile=new IO::File(">$fn");


    XM::setTimerState(qr/^REC_TIMER$/,1);
    XM::setTriggerState(qr/^REC_/,1);

    writeLog("#HUD:xperthud:KEY:R# Key set");

    return 1;
}

sub stopRec {
    $recWindow->print(XM::ansi('%r%cr%chStopping Recorder%cn%r'));

    XM::setKeyBindingState("REC_START",1);
    XM::setKeyBindingState("REC_STOP",0);
    XM::setKeyBindingState("REC_PAUSE",0);
    XM::setKeyBindingState("REC_UNPAUSE",0);

    XM::setTimerState(qr/^REC_TIMER$/,0);
    XM::setTriggerState(qr/^REC_/,0);
    XM::setAliasState(qr/^PLAY_START/,1);

    writeLog(XM::ansi('%cm%chLog end%cn'));

    $recFile->close();
    $recFile=undef;
    
    return 1;
}

sub pauseRec {
    $recWindow->print(XM::ansi('%r%cr%chPausing Recorder%cn%r'));

    XM::setKeyBindingState("REC_PAUSE",0);
    XM::setKeyBindingState("REC_UNPAUSE",1);

    XM::setTimerState(qr/^REC_TIMER$/,0);
    XM::setTriggerState(qr/^REC_/,0);

    writeLog(XM::ansi('%cm%chLog paused%cn'));

    $recTime+=3000;
    return 1;
}

sub unPauseRec {
    $recWindow->print(XM::ansi('%r%cr%chResuming Recorder%cn%r'));

    XM::setKeyBindingState("REC_PAUSE",1);
    XM::setKeyBindingState("REC_UNPAUSE",0);

    XM::setTimerState(qr/^REC_TIMER$/,1);
    XM::setTriggerState(qr/^REC_/,1);

    writeLog(XM::ansi('%cm%chLog resumed%cn'));

    return 1;
}


sub recTimer {
    $recTime+=250;
# FIX:
#    $recWindow->clearLine;
    $recWindow->print(XM::ansi("Recording at timestamp: %cy%ch$recTime%cn%r"));
}


sub recLogger {
    my $line=shift;
    if (length($line)) {
	writeLog($line);
    }
    return $line;
}

XM::delKeyBinding(qr/^REC_/);
XM::addKeyBinding("REC_START",qr/0000.... F10/,\&startRec,1);
XM::addKeyBinding("REC_STOP",qr/0000.... F10/,\&stopRec,0);
XM::addKeyBinding("REC_PAUSE",qr/0000.... F9/,\&pauseRec,0);
XM::addKeyBinding("REC_UNPAUSE",qr/0000.... F9/,\&unPauseRec,0);

XM::delTimer(qr/^REC_/);
XM::addTimer("REC_TIMER",0,\&recTimer,0);

XM::delTrigger(qr/^REC_/);
XM::addTrigger("REC_LOG",qr/./,\&recLogger,0,0);

my $playTime;
my $currentTime;
my $currentLine;
my $playFile;
my $playSpeed=1.0;
my $fileKey;

sub endPlayback {
    $recWindow->print(XM::ansi('%r%cr%chPlayback finished.%cn%r'));
    $playFile->close;
    $playFile=undef;
    XM::setAliasState(qr/^PLAY_START/,1);
    XM::setAliasState(qr/^PLAY_STOP/,0);
    XM::setTimerState(qr/^PLAY_TIMER/,0);
    XM::setKeyBindingState(qr/^REC_START/,1);
    XM::setKeyBindingState(qr/^PLAY_/,0);
    foreach($::Core->getContacts) {
	$::Core->deleteContact($_);
    }
    ::normalMode;
}

sub arrangeWindows {
  my $conHeight=160;
  my $speedWidth=50;
  my $weaponsHeight=160;

  $recWindow->hideCaption;
  $recWindow->resizeChars(81,40);
  $recWindow->move(0,$conHeight);
  $recWindow->resize($recWindow->getSizeX, XM::mdiHeight()-$conHeight-$weaponsHeight);
  $recWindow->show;

  XM::statusWindow->hideCaption;
  XM::statusWindow->resizeChars(81,10);
  XM::statusWindow->move(0,$recWindow->getSizeY+$conHeight);
  XM::statusWindow->resize($recWindow->getSizeX, $weaponsHeight);
  XM::statusWindow->show;

  $::Contacts->hideCaption;
  $::Contacts->move(0,0);
  $::Contacts->resize($recWindow->getSizeX-$speedWidth, $conHeight);
  $::Contacts->show;

  $::Speed->hideCaption;
  $::Speed->move($recWindow->getSizeX-$speedWidth,0);
  $::Speed->resize($speedWidth,$conHeight);
  $::Speed->show;

  $::Map->hideCaption;
  $::Map->move($recWindow->getSizeX,0);
  $::Map->resize(XM::mdiWidth() - $recWindow->getSizeX,
	       XM::mdiHeight()-$weaponsHeight);
  $::Map->show;

  $::Weapons->hideCaption;
  $::Weapons->move($recWindow->getSizeX,$::Map->getSizeY);
  $::Weapons->resize($::Map->getSizeX,$weaponsHeight);
  $::Weapons->show;
}

sub startPlayback {
    my $fn=shift;
    $playFile=new IO::File("<$fn");
    if ($playFile) {
	$recWindow->print(XM::ansi('%r%cr%chStarting Playback from "'.$fn.'".%cn%r'));
	::normalMode;
	foreach($::Core->getContacts) {
	    $::Core->deleteContact($_);
	}
	XM::setAliasState(qr/^PLAY_START/,0);
	XM::setAliasState(qr/^PLAY_STOP/,1);
	XM::setKeyBindingState(qr/^REC_START/,0);
	XM::setKeyBindingState(qr/^PLAY_PAUSE/,1);
	XM::setKeyBindingState(qr/^PLAY_FASTER/,1);
	XM::setKeyBindingState(qr/^PLAY_SLOWER/,1);
	$currentLine=$playFile->getline;
	if (!defined $currentLine) {
	    endPlayback;
	    return;
	}
	$currentLine=~s/^(\d+)://;
	$playTime=$1+0;
	$currentTime=$playTime;
	$currentLine=~s/HUD:(.*):KEY/HUD:xperthud:KEY/;
	$fileKey=$1;
	$playSpeed=1.0;
	XM::setTimerState(qr/^PLAY_TIMER/,1);
	arrangeWindows;
    } else {
	$recWindow->print(XM::ansi('%r%cr%chCan\'t open "'.$fn.'" for playback.%cn%r'));
    }
}

sub playTimer {
    $playTime+=250*$playSpeed;
    while ($playTime >= $currentTime) {
	if ($currentLine=~/^\#HUD/) {
	    $currentLine=~s/HUD:$fileKey:/HUD:xperthud:/;
	    $::Core->parse($currentLine);
	} else {
	    $recWindow->print($currentLine);
	}

	$currentLine=$playFile->getline;
	if (!defined $currentLine) {
	    endPlayback;
	    return;
	}
	$currentLine=~s/^(\d+)://;
	$currentTime=$1+0;
	
    }
}

sub changeSpeed {
    my $mod=shift;
    $playSpeed*=$mod;
    if ($playSpeed>=1.0) {
	$playSpeed=int($playSpeed*100.0+0.5)/100.0;
	$recWindow->print(XM::ansi('%r%cr%chChanging playback speed to '.($playSpeed).'.%cn%r'));
    } else {
	$playSpeed=1.0/(int((1.0/$playSpeed)*100.0+0.5)/100.0);
	$recWindow->print(XM::ansi('%r%cr%chChanging playback speed to 1/'.(1.0/$playSpeed).'.%cn%r'));
    }
}

sub playPause {
    $recWindow->print(XM::ansi('%r%cr%chPlayback paused at timestamp '.int($playTime).'.%cn%r'));
    XM::setTimerState(qr/^PLAY_TIMER/,0);
    XM::setKeyBindingState(qr/^PLAY_PAUSE/,0);
    XM::setKeyBindingState(qr/^PLAY_UNPAUSE/,1);
    return 1;
}

sub playResume {
    $recWindow->print(XM::ansi('%r%cr%chPlayback resumed.%cn%r'));
    XM::setTimerState(qr/^PLAY_TIMER/,1);
    XM::setKeyBindingState(qr/^PLAY_PAUSE/,1);
    XM::setKeyBindingState(qr/^PLAY_UNPAUSE/,0);
    return 1;
}

XM::delAlias(qr/^PLAY_/);
XM::addAlias("PLAY_START",qr/^playback (.+)$/,sub { startPlayback($1); return undef; },1,1);
XM::addAlias("PLAY_STOP",qr/^stop/,sub { endPlayback(); return undef; },1,0);

XM::delTimer(qr/^PLAY_/);
XM::addTimer("PLAY_TIMER",0,\&playTimer,0);


XM::delKeyBinding(qr/^PLAY_/);
XM::addKeyBinding("PLAY_FASTER",qr/0000.... F8/,sub { changeSpeed(2.0); return 1;},0);
XM::addKeyBinding("PLAY_SLOWER",qr/0000.... F7/,sub { changeSpeed(0.5); return 1;},0);
XM::addKeyBinding("PLAY_PAUSE",qr/0000.... F9/,\&playPause,0);
XM::addKeyBinding("PLAY_UNPAUSE",qr/0000.... F9/,\&playResume,0);
