# Special Script for Frontiers (http://frontiermux.com)

my $version="2007-10-31";


# http://www.geocities.com/soho/7373/haloween.htm
my $pumpkins=q#%cy
                                              _
                              /\              )\
                _           __)_)__        .'`--`'.
                )\_      .-'._'-'_.'-.    /  ^  ^  \
             .'`---`'. .'.' /%cro%cy\'/%cro%cy\ '.'.  \ \/\/\/ /
            /  <> <>  \ : ._:  0  :_. : \  '------'       _J_
            |    A    |:   \\/\_/\//   : |     _/)_    .'`---`'.
            \  <\_/>  / :  :\/\_/\/:  : /   .'`----`'./.'b   d  \
           _?_._`"`_.'`'-:__:__:__:__:-'   /.'<\   /> \:   0    |
        .'`---`'.``  _/(              /\   |:,___A___,|' V===V  /
       /.'a . a  \.'`---`'.        __(_(__ \' \_____/ /'._____.'
       |:  ___   /.'/\ /\  \    .-'._'-'_.'-:.______.' _?_
       \'  \_/   |:   ^    |  .'.' (o\'/o) '.'.     .'`"""`'.
        '._____.'\' 'vvv'  / / :_/_:  A  :_\_: \   /   ^.^   \
                  '.__.__.' | :   \'=...='/   : |  \  `===`  /
               %cnjgs%cy           \ :  :'.___.':  : /    `-------`
                              '-:__:__:__:__:-'

              %cn---> %cg%chXpertmud Halloween Edition%cn <----

#;



statusWindow->print(ansi('%cm%cu%chXperthud script for Frontier Lands%cn %cx%chVersion '.$version.'%cn%r'));
addDelayed("Frontiers_AD",60,sub {
             XM::sendText('@force me=@doing %cm%chhttp://xpertmud.btmux.com/'."\n");
});


# Pre-Declare the globals from xperthud.pl
use vars qw($reportChannel $battleCore $Core $Map $Contacts $Status $Speed $Weapons);


setWriteRegExps(qr/^$/, qr/^\#HUD|^MOTD/);

addTrigger("STARTUP",qr/MOTD/,sub {
  parse("xperthud.pl");


### Remove this line if you prefer the older style
  $Map->switchStyle('Heavy');



  parse("battlerecorder.pl");
  parse("damagelog.pl");


  delDelayed(qr/^IDLE$/);
  addDelayed("IDLE",25*60,sub {
     setDelayed(qr/^IDLE/,25*60);
     normalMode();
  });
  addDelayed("version_check",5,sub {
     XM::sendText("xpertmud_version $version\n");  
  });

  addDelayed("halloween",6,sub {
    statusWindow->print(ansi($pumpkins));
  });

  delTrigger(qr/^STARTUP$/);
  delDelayed(qr/^Advertisment$/);
  
  addTimer("BT_HOUSEKEEP",61,sub {
    my @vars=qw/ID AC SE UT MN X Y Z RN BR SP VS HD JH RTC BTC TN HT FL AG/;
    foreach my $c ($battleCore->getContacts) {
      my @v=@vars;
      my %con=map {(shift @v)=>$_} split /\|/, $battleCore->getContact($c);
      if ($con{AG}>60 and $con{FL}=~/D/) {
        $battleCore->deleteContact($c);
      }
    }
  },0);

  my $oldOwnID="";
  addTimer("BT_AUTOCENTER",15,sub {
    my $oid=$battleCore->getOwnID();
    if (defined $oid and $oid ne $oldOwnID) {
      $oldOwnID=$oid;
      $Map->centerMech($oid);
    }
  },0);
  
  shift;
});

parse("completion.pl");

addTrigger("COMLETION",qr/\] (\w[\w\s]+\w):/,sub { addCompletionWord($1); shift; });
addTrigger("COMLETION",qr/^(\w[\w\s]+\w) says/,sub { addCompletionWord($1); shift; });
addTrigger("COMLETION",qr/^(\w[\w\s]+\w) has/,sub { addCompletionWord($1); shift; });


