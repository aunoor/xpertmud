# xperthud.pl

# Example-Script that shows how to use the battletech-plugin

use XMBattleCore;
use XMBattleMapView;
use XMBattleContacts;
use XMBattleHeat;
use XMBattleWeapons;
use XMBattleSpeed;
use XMBattleStatus;

use strict;
use warnings;
use diagnostics;


# GLOBAL vars for stuff that should be accessible from cmd-line-evals
use vars qw($reportChannel $battleCore $Core $Map $Contacts $Status $Speed $Weapons);


# Print some little help...

statusWindow()->setWordWrapColumn(80);
statusWindow->print(ansi("%r%cy%chWelcome to xperthud!%cn%r"));
statusWindow->print(ansi("%cy%ch====================%cn%r%r"));
statusWindow->print(ansi("F6:         %cx%chActivate HUD%cn%r"));
statusWindow->print(ansi("F5:         %cx%chDeactivate HUD%cn%r"));
statusWindow->print(ansi("F1-F4:      %cx%chFiretic 1-4%cn%r"));
statusWindow->print(ansi("Alt-0-9:    %cx%chSight 0-9%cn%r"));
statusWindow->print(ansi("Ctrl-0-9:   %cx%chFire 0-9%cn%r"));
statusWindow->print(ansi("Numpad:     %cx%chChange heading (depending on NumLock)%cn%r"));
statusWindow->print(ansi("Many more keybindings available, please read the source :)%r%r%r"));


# I told you that this is just an example script. so don't complain!
# But if you don't mind, please let it in, it helps us to get an good guess on the current userbase

addDelayed("Advertisment",3*60,sub {
             XM::sendText('@force me=@doing %cb%chhttp://xpertmud.sf.net/%cn'."\n");
           });



$Core=$battleCore=new XMBattleCore("Core Debug");
$reportChannel="a";

setWriteRegExps(qr/^$/, qr/^#HUD/);

addTrigger("HUDINFO",qr/^#HUD/,sub {
	     my $line=shift;
	     $battleCore->parse($line);
	     return undef }
	   ,1);


if (0) { #hrm... 
  $battleCore->startSecondary();
addTrigger("BT_TEXTPARSER", qr/^.*$/, sub {
              my $line=shift;
              $battleCore->parseText($line);
              return $line;
           },0, 0); # don't break, inactive.

}

addTrigger("TEXTPARSER_SKIP", qr/^.?/, sub {
	      my $line=shift;
	      $battleCore->parseText($line);
	      return undef
	   },1, 0); # break, inactive.

addTrigger("TEXTPARSER_START", qr/^SSKIP$/, sub {
	     setTriggerState(qr/^TEXTPARSER_SKIP$/,1);
	     return undef;
	   });

addTrigger("TEXTPARSER_END", qr/^ESKIP$/, sub {
	     setTriggerState(qr/^TEXTPARSER_SKIP$/,0);
	     return undef;
	   });

$Map=new XMBattleMapView("Map");
$Map->setZoom(10);
$Contacts=new XMBattleContacts("Contacts");
$Speed=new XMBattleSpeed("Speed");
$Weapons=new XMBattleWeapons("Weapons");
$Status=new XMBattleStatus("Status");

##################### Mode/Window layout control ###########################
sub normalMode {
  setKeyBindingState(qr/^BT_/, 0);
  setAliasState(qr/^BT_/, 0);
  setTriggerState(qr/^BT_/, 0);
  setTimerState(qr/^BT_/, 0);
  statusWindow()->maximize();
  $Contacts->hide;
  $Speed->hide;
  $Map->hide;
  $Weapons->hide;
  $battleCore->hide;
  $Status->hide;
  $battleCore->setTacticalInterval(0);
  $battleCore->setStatusInterval(0);
  $battleCore->setContactsInterval(0);
  $battleCore->setWeaponsInterval(0);

# broken. waiting for fix
  statusWindow->scrollLines(5000);

}

sub battleMode {
  my $conHeight=160;
  my $speedWidth=40;
  my $statusWidth=156;
  my $weaponsHeight=160;

  setKeyBindingState(qr/^BT_/, 1);
  setAliasState(qr/^BT_/, 1);
  setTriggerState(qr/^BT_/, 1);
  setTimerState(qr/^BT_/, 1);

  statusWindow->hideCaption;
  statusWindow->resizeChars(81,40);
  statusWindow->move(0,$conHeight);
  statusWindow->resize(statusWindow->getSizeX, XM::mdiHeight()-$conHeight);

  $Contacts->hideCaption;
  $Contacts->move(0,0);
  $Contacts->resize(statusWindow->getSizeX-$speedWidth, $conHeight);
  $Contacts->show;

  $Speed->hideCaption;
  $Speed->move(statusWindow->getSizeX-$speedWidth,0);
  $Speed->resize($speedWidth,$conHeight);
  $Speed->show;


  $Map->hideCaption;
  $Map->move(statusWindow->getSizeX,0);
  $Map->resize(XM::mdiWidth() - statusWindow->getSizeX,
	       XM::mdiHeight()-$weaponsHeight);
  $Map->show;

  $Weapons->hideCaption;
  $Weapons->move(statusWindow->getSizeX,$Map->getSizeY);
  $Weapons->resize($Map->getSizeX-$statusWidth,$weaponsHeight);
  $Weapons->show;

  $Status->hideCaption;
  $Status->move(statusWindow->getSizeX+$Weapons->getSizeX,$Map->getSizeY);
  $Status->resize($statusWidth,$weaponsHeight);
  $Status->show;


  $battleCore->setTacticalInterval(4*60);
  $battleCore->setStatusInterval(2);
  $battleCore->setContactsInterval(3);
  $battleCore->setWeaponsInterval(12);

# broken, waiting for fix
  statusWindow->scrollLines(5000);
}
######################### Automatic Mode Switches #########################

$Core->{onLogInfo}=sub {
   my ($self, $log)=@_;
   if ($log=~/Not in a BattleTech unit/) {
      normalMode();
      statusWindow->print(ansi('%cc%chThe HUD only works from BattleTech units. Deactivating.%cn%r'));
   }
};


############################## Mode Switches ##############################
# SHIFT CONTROL ALT META ALTGR NUML CAPSL SCRL

addKeyBinding("Switch_to_Battle","0000.... F6",sub { battleMode(); return 1; });
addKeyBinding("Switch_to_Normal","0000.... F5",sub { normalMode(); return 1; });
		



#################################### IDLE ##################################

addDelayed("IDLE",25*60,sub {
	     XM::sendText("\@pemit me=The time is [time()].\n");
	     setDelayed(qr/^IDLE/,25*60);
	     normalMode();
	   });

########################### Movement keybindings ###########################
# SHIFT CONTROL ALT META ALTGR NUML CAPSL SCRL

addKeyBinding('BT_Heading-240', '.000.... KP_1',
	      sub { XM::sendText("heading 240\n"); return 1; });
addKeyBinding('BT_Heading-210', '.000.... KP_End',
	      sub { XM::sendText("heading 210\n"); return 1; });

addKeyBinding('BT_Heading-180', '.000.... KP_2',
	      sub { XM::sendText("heading 180\n"); return 1; });
addKeyBinding('BT_Heading-180', '.000.... KP_Down',
	      sub { XM::sendText("heading 180\n"); return 1; });

addKeyBinding('BT_Heading-120', '.000.... KP_3',
	      sub { XM::sendText("heading 120\n"); return 1; });
addKeyBinding('BT_Heading-150', '.000.... KP_PgDown',
	      sub { XM::sendText("heading 150\n"); return 1; });

addKeyBinding('BT_Heading-270', '.000.... KP_4',
	      sub { XM::sendText("heading 270\n"); return 1; });
addKeyBinding('BT_Heading-270', '.000.... KP_Left',
	      sub { XM::sendText("heading 270\n"); return 1; });

addKeyBinding('BT_Keypad_Stop', '.000.... KP_5',
	      sub { XM::sendText("speed stop\n"); return 1; });
addKeyBinding('BT_Keypad_Stop', '.000.... KP_Begin',
	      sub { XM::sendText("speed stop\n"); return 1; });

addKeyBinding('BT_Heading-90', '.000.... KP_6',
	      sub { XM::sendText("heading 90\n"); return 1; });
addKeyBinding('BT_Heading-90', '.000.... KP_Right',
	      sub { XM::sendText("heading 90\n"); return 1; });

addKeyBinding('BT_Heading-300', '.000.... KP_7',
	      sub { XM::sendText("heading 300\n"); return 1; });
addKeyBinding('BT_Heading-330', '.000.... KP_Home',
	      sub { XM::sendText("heading 330\n"); return 1; });

addKeyBinding('BT_Heading-0', '.000.... KP_8',
	      sub { XM::sendText("heading 0\n"); return 1; });
addKeyBinding('BT_Heading-0', '.000.... KP_Up',
	      sub { XM::sendText("heading 0\n"); return 1; });

addKeyBinding('BT_Heading-60', '.000.... KP_9',
	      sub { XM::sendText("heading 60\n"); return 1; });
addKeyBinding('BT_Heading-30', '.000.... KP_PgUp',
	      sub { XM::sendText("heading 30\n"); return 1; });

addKeyBinding('BT_Keypad_Run', '.000.... KP_0',
	      sub { XM::sendText("speed flank\n"); return 1; });
addKeyBinding('BT_Keypad_Run', '.000.... KP_Ins',
	      sub { XM::sendText("speed run\n"); return 1; });

addKeyBinding('BT_Keypad_Back', '.000.... KP_Del',
	      sub { XM::sendText("speed back\n"); return 1; });
addKeyBinding('BT_Keypad_Back', '.000.... KP_[,\.]',
	      sub { XM::sendText("speed back\n"); return 1; });


addKeyBinding('BT_Rottorso l', '.0(01|10).... KP_7',
	      sub { XM::sendText("rottorso l\n"); return 1; });
addKeyBinding('BT_Rottorso l', '.0(01|10).... KP_Home',
	      sub { XM::sendText("rottorso l\n"); return 1; });

addKeyBinding('BT_Rottorso c', '.0(01|10).... KP_8',
		  sub { XM::sendText("rottorso c\n"); return 1; });
addKeyBinding('BT_Rottorso c', '.0(01|10).... KP_Up',
		  sub { XM::sendText("rottorso c\n"); return 1; });

addKeyBinding('BT_Rottorso r', '.0(01|10).... KP_9',
	      sub { XM::sendText("rottorso r\n"); return 1; });
addKeyBinding('BT_Rottorso r', '.0(01|10).... KP_PgUp',
	      sub { XM::sendText("rottorso r\n"); return 1; });

########################## Targeting/Firing ################################
# SHIFT CONTROL ALT META ALTGR NUML CAPSL SCRL

addKeyBinding('BT_Target LL', '.100.... KP_1',
	      sub { XM::sendText("target LL\n"); return 1; });
addKeyBinding('BT_Target -',  '.100.... KP_2',
	      sub { XM::sendText("target -\n"); return 1; });
addKeyBinding('BT_Target RL', '.100.... KP_3',
	      sub { XM::sendText("target RL\n"); return 1; });
addKeyBinding('BT_Target LA', '.100.... KP_4',
	      sub { XM::sendText("target LA\n"); return 1; });
addKeyBinding('BT_Target CT', '.100.... KP_5',
	      sub { XM::sendText("target CT\n"); return 1; });
addKeyBinding('BT_Target RA', '.100.... KP_6',
	      sub { XM::sendText("target RA\n"); return 1; });
addKeyBinding('BT_Target LT', '.100.... KP_7',
	      sub { XM::sendText("target LT\n"); return 1; });
addKeyBinding('BT_Target H',  '.100.... KP_8',
	      sub { XM::sendText("target H\n"); return 1; });
addKeyBinding('BT_Target RT', '.100.... KP_9',
	      sub { XM::sendText("target RT\n"); return 1; });

addKeyBinding('BT_fire',  '.100.... (\d)$', #'
	      sub { XM::sendText("fire $1\n"); return 1; });
addKeyBinding('BT_sight', '.0(01|10).... (\d)$', #'
	      sub { XM::sendText("sight $2\n"); return 1; });

addKeyBinding('BT_fire0',  '.100.... asciicircum',
	      sub { XM::sendText("fire 0\n"); return 1; });
addKeyBinding('BT_fire0',  '.100.... \^',
	      sub { XM::sendText("fire 0\n"); return 1; });
addKeyBinding('BT_sight0', '.0(01|10).... asciicircum',
	      sub { XM::sendText("sight 0\n"); return 1; });
addKeyBinding('BT_sight0', '.0(01|10).... \^',
	      sub { XM::sendText("sight 0\n"); return 1; });

addKeyBinding('BT_firetic', qr/0000.... F([1234])$/, sub {
		my $tic = $1-1;
		XM::sendText("firetic $tic\n");
		return 1;
	      });
addKeyBinding('BT_listtic', qr/^1000.... F([1234])$/, sub {
		my $tic = $1-1;
		XM::sendText("listtic $tic\n");
		return 1;
	      });

########################### Status / Scanning ###############################
# SHIFT CONTROL ALT META ALTGR NUML CAPSL SCRL

addKeyBinding('BT_Status', '.000.... KP_/',
	      sub { XM::sendText("status\n"); return 1; });

addKeyBinding('BT_Status', '.000.... KP_\*',
	      sub { XM::sendText("contacts\n"); return 1; });

addKeyBinding('BT_Keypad_Tactial', '.000.... KP_-',
	      sub { $Map->setZoom(15); return 1; });

addKeyBinding('BT_Keypad_Navigate', '.000.... KP_\+',
	      sub { $Map->setZoom(42); return 1; });

########################### General Macro Key ###############################

addKeyBinding('BT_shortcut_LC', '^0100.... (.)$', sub { #'
		XM::sendText('.'.lc($1)."\n"); return 1; }
);

addKeyBinding('BT_shortcut_UC', '^1100.... (.)$', sub { #'
		XM::sendText('.'.uc($1)."\n"); return 1; }
);


########################### Enter-Modes ####################################
# SHIFT CONTROL ALT META ALTGR NUML CAPSL SCRL

addAlias("Enter_to_PR","\n","\%r",0,1);
addAlias("Enter_to_SP","\n"," ",0,0);

addKeyBinding('Switch_Enter_PR','1000.... Return', sub {
		setKeyBindingState("Switch_Enter_PR",0);
		setKeyBindingState("Switch_Enter_SP",1);
		setAliasState("Enter_to_PR",1);
		setAliasState("Enter_to_SP",0);
		statusWindow->setFGColor(BLUE);
		statusWindow->setIntensive(1);
		statusWindow->print("Enter will generate \%r codes now\n");
		statusWindow->resetAttributes();
		return 1;
},0);

addKeyBinding('Switch_Enter_SP','1000.... Return', sub {
		setKeyBindingState("Switch_Enter_PR",1);
		setKeyBindingState("Switch_Enter_SP",0);
		setAliasState("Enter_to_PR",0);
		setAliasState("Enter_to_SP",1);
		statusWindow->setFGColor(BLUE);
		statusWindow->setIntensive(1);
		statusWindow->print("Enter will be replaced with space now\n");
		statusWindow->resetAttributes();
		return 1;
},1);

############################# AutoScroll ###################################

my $oldOnTextEntered=$onTextEntered;
$onTextEntered=sub {
# broken. waiting for fix
  statusWindow->scrollLines(5000);
  &$oldOnTextEntered(@_);
  setDelayed("^IDLE\$",25*60);
};


############################### ConCaller ##################################

addAlias("DELETE_CONS","^clearcons",sub {
	   foreach($battleCore->getContacts) {
	     $battleCore->deleteContact($_);
	   }
	   return undef;
	 });

addAlias("BT_CONCALLER","^con\\s+",sub {
	   my $line=shift;
	   if ($line=~/^con\s+([A-Za-z][A-Za-z])/) {
	     my $mechid=$1;
	     my %me=$battleCore->getContact($mechid);
	     unless(defined $me{ID}) {
		statusWindow->setFGColor(BLUE);
		statusWindow->print("No Contact info on $mechid\n");
		statusWindow->resetAttributes();
		return undef;
	     }
	     my $s = sprintf(' [%s]%s %s x:%4d y:%4d z:%4d r:%5.1f '.
			     'b:%4d s:%5.1d h:%4d S: %s',
			     $me{ID},$me{UT},$me{MN},$me{X},$me{Y},
			     $me{Z},$me{RN},$me{BR},$me{SP},$me{HD},
			     $me{FL});
	     if($me{JH} ne "-") {
	       $s .= sprintf(' Jump h:%4d', $me{JH});
	     }
	     if($me{AG} > 0) {
	       $s .= sprintf(' Last Contact: %02d:%02d', int($me{AG}/60),
			     int($me{AG}%60));
	     } else {
	       $s .= " LOS maintained";
	     }
	     if(uc($me{ID}) eq $me{ID}) {
	       if($me{MN} eq "something") {
		 XM::sendText("sendchannel $reportChannel=* UNKNOWN * :".$s."\n");
	       } else {
		 XM::sendText("sendchannel $reportChannel=* ENEMY * :".$s."\n");
	       }
	     } else {
	       XM::sendText("sendchannel $reportChannel=FRIEND :".$s."\n");
	     }
	     return undef;
	   } else {
	     statusWindow->setFGColor(CYAN);
	     statusWindow->print("usage: con <mechid>\n");
	     statusWindow->resetAttributes();
	     return undef;
	   }
	 });

############################ Target Relock #################################

{
  my $lastLockedTarget="-";
  addTrigger("BT_ReLock",'Target set to .*\[(..)\]\.', sub { 
	       $lastLockedTarget=$1;
	       return shift;
	     },0,0);


  addKeyBinding('BT_ReLock', '.000.... KP_Enter', sub {
		  XM::sendText("lock $lastLockedTarget\n");
		  return 1;
		});

}
############################# Final inits ##################################

normalMode();


1;
