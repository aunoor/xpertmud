setWriteRegExps(qr/^$/, qr/^#HUD|^MOTD/);
addTrigger("STARTUP",qr/MOTD/,sub { 
  parse("xperthud.pl");
  parse("battlerecorder.pl");
  delTrigger(qr/^STARTUP$/);
  shift;
});

################################# Claim Timer ##############################
{
  my $claimStart=0;
  my $claimTime=0;

  addTrigger("CLAIM_TIMER",
	     qr/^Complex claim initiated\. Claim will complete in \d+ minutes\./,
	     sub {
	       my $line = shift;
	       if($line =~ m/in (\d+)/) {
		 $claimTime=$1*60;
		 $claimStart=time;
		 statusWindow->setFGColor(BLUE);
		 statusWindow->print("Claim Noted.\n");
		 statusWindow->resetAttributes();
	       } $line;}, 0, 1);

  addAlias("CLAIM_TIMER",qr/^cclaim/,
	   sub {
	     if(time <  $claimStart + $claimTime) {
	       my $rest = $claimStart + $claimTime - time;
	       statusWindow->setFGColor(BLUE);
	       statusWindow->print(sprintf("Claim will be done in %02d:%02d\n",
					   (int($rest/60)),($rest%60)));
	       statusWindow->resetAttributes();
	     } else {
	       statusWindow->setFGColor(BLUE);
	       statusWindow->print("I dont know any ongoing Claims.\n");
	       statusWindow->resetAttributes();
	     }
	     return undef;
	   }, 1);

  addAlias("CLAIM_TIMER",qr/^setclaim \d+/,
	   sub {
	     my $line = shift;
	     if($line =~ m/setclaim (\d+)/) {
		 $claimTime=$1*60;
		 $claimStart=time;
		 statusWindow->setFGColor(BLUE);
		 statusWindow->print("Claim set.\n");
		 statusWindow->resetAttributes();
	       }
	     return undef;
	   } , 1);

  my $rclaimsub=sub {
    if(time <  $claimStart + $claimTime) {
      my $rest = $claimStart + $claimTime - time;
      XM::sendText(sprintf("sendchannel $reportChannel=TOC on Complex %02d:%02d\n",
		       (int($rest/60)),($rest%60)));
    } else {
      statusWindow->setFGColor(BLUE);
      statusWindow()->print("I dont know any ongoing Claims.\n");
      statusWindow->resetAttributes();
    }
    return undef;
  };

  addAlias("CLAIM_TIMER",qr/^rclaim/,$rclaimsub,1);

  addAlias("CLAIM_TIMER", qr/^autotoc(\s+\d+)?/,
	   sub {
	     my $interval=5*60;
	     my $line = shift;
	     if($line =~ /^autotoc\s+(\d+)/) {
	       $interval = $1*60;
	     }
	     if ($interval>0) {
	       if(time <  $claimStart + $claimTime) {
		 statusWindow->setFGColor(BLUE);
		 statusWindow->print("AutoTOC started. Will report every $interval seconds.\n");
		 statusWindow->resetAttributes();

		 delDelayed("^CLAIM_TIMER");
		 my $calltime = $claimStart + $claimTime-$interval;
		 while($calltime > time) {
		   addDelayed("CLAIM_TIMER",$calltime-time, $rclaimsub);
		   $calltime -= $interval;
		 }
	       } else {
		 statusWindow->setFGColor(BLUE);
		 statusWindow->print("I dont know any ongoing Claims.\n");
		 statusWindow->resetAttributes();
	       }
	     } else {
	       delDelayed("^CLAIM_TIMER");
	       statusWindow->setFGColor(BLUE);
	       statusWindow->print("AutoTOC disengaged.\n");
	       statusWindow->resetAttributes();
	     }
	     return undef;
	   } ,1);

}

################################# Frequencies ##############################

addTrigger("START_FREQ","================================= Frequencies ================================",sub {
	     setTriggerState("^FREQ_",1);
	     return $_;
	   }, 0, 1);

addTrigger("END_FREQ",  "==============================================================================",sub {
	     setTriggerState("^FREQ_",0);
	     return $_;
	   }, 0, 1);

addTrigger("FREQ_MAIN","Main:.+(\\d+)",sub {
	     my $line=shift;
	     if ($line=~/(\d\d\d\d+)/) {
	       XM::sendText("&F_MAIN me=$1\n");
	     }
	     return $line;
	   }, 0,0);

addTrigger("FREQ_BACKUP1","Backup1:.+(\\d+)",sub {
	     my $line=shift;
	     if ($line=~/(\d\d\d\d+)/) {
	       XM::sendText("&F_BACKUP1 me=$1\n");
	     }
	     return $line;
	   }, 0,0);

addTrigger("FREQ_BACKUP2","Backup2:.+(\\d+)",sub {
	     my $line=shift;
	     if ($line=~/(\d\d\d\d+)/) {
	       XM::sendText("&F_BACKUP2 me=$1\n");
	     }
	     return $line;
	   }, 0,0);

addTrigger("FREQ_SCREAM1","Scream1:.+(\\d+)",sub {
	     my $line=shift;
	     if ($line=~/(\d\d\d\d+)/) {
	       XM::sendText("&F_SCREAM1 me=$1\n");
	     }
	     return $line;
	   }, 0,0);

addTrigger("FREQ_SCREAM2","Scream2:.+(\\d+)",sub {
	     my $line=shift;
	     if ($line=~/(\d\d\d\d+)/) {
	       XM::sendText("&F_SCREAM2 me=$1\n");
	     }
	     return $line;
	   }, 0,0);

addTrigger("FREQ_STRIKE","Strike/Recon:.+(\\d+)",sub {
	     my $line=shift;
	     if ($line=~/(\d\d\d\d+)/) {
	       XM::sendText("&F_STRIKE me=$1\n");
	     }
	     return $line;
	   }, 0,0);

addTrigger("FREQ_HEAVY","Heavy/Tank:.+(\\d+)",sub {
	     my $line=shift;
	     if ($line=~/(\d\d\d\d+)/) {
	       XM::sendText("&F_HEAVY me=$1\n");
	     }
	     return $line;
	   }, 0,0);

addTrigger("FREQ_CHAT","Chat:.+(\\d+)",sub {
	     my $line=shift;
	     if ($line=~/(\d\d\d\d+)/) {
	       XM::sendText("&F_CHAT me=$1\n");
	     }
	     return $line;
	   }, 0,0);

{
  # Configure your prefered channel settings here
  my %mapping = (# primary channel, Modes, Backup channel, mode, backup slot
		 a=>["F_MAIN","DEG","F_BACKUP1","DER","f"],
		 b=>["F_SCREAM1","R","F_SCREAM2","R","g"],
		 c=>["F_CHAT","DEg"],
		 d=>["F_HEAVY","DEb"],
		 e=>["F_STRIKE","DEb"]
		);
  my $cmd="";
  foreach my $c (keys %mapping) {
    $cmd.="\@force me=setchannelfreq $c=[v($mapping{$c}->[0])]\n";
  }
  # TODO: keep them here, or set a character-script? or both?
  addAlias("SET_RADIO_FREQS","^setallfreqs",$cmd ,1);

  $cmd="";
  foreach my $c (keys %mapping) {
    my $mode=$mapping{$c}->[1];
    if ($mode=~/E/) {
      $mode =~ s/E//g;
      $cmd.="setchannelmode $c=$mode\n";
    }
    $cmd.="setchannelmode $c=$mapping{$c}->[1]\n";
  }
  addAlias("SET_RADIO_MODES","^setallmodes",$cmd ,1);

  $cmd="";
  foreach my $c (keys %mapping) {
    $cmd.="setchannelfreq $c=0\n";# or set random?
    if (defined $mapping{$c}->[4]) {
      $cmd.="setchannelfreq $mapping{$c}->[4]=0\n";
    }
  }
  addAlias("ZERO_RADIO_FREQS","^zeroallfreqs",$cmd,1);

  $cmd="";
  foreach my $c (keys %mapping) {
    next unless defined $mapping{$c}->[2];
    $cmd.="\@force me=setchannelfreq $c=[get(me/$mapping{$c}->[2])]\n";
    $cmd.="\@force me=setchannelfreq $mapping{$c}->[4]=[v($mapping{$c}->[0])]\n";
    my $mode=$mapping{$c}->[3];
    if ($mode=~/E/) {
      $mode =~ s/E//g;
      $cmd.="setchannelmode $c=$mode\n";
    }
    $cmd.="setchannelmode $mapping{$c}->[4]=$mapping{$c}->[3]\n";
  }
  addAlias("GO_RADIO_BACKUP","^setbackupfreqs",$cmd,1);

  $cmd ="\@force me=setchannelfreq a=[v($mapping{b}->[0])]\n";
  $cmd.="\@force me=setchannelmode a=$mapping{b}->[1]\n";
  addAlias("INIT_MECHWARRIOR","^initmw",$cmd,1);
  
}
