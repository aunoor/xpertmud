package XMBattleCore;
# Object used for receiving data, iow the Parser Interface
# Usage:
# my $battlecore=new XMBattleCore;
# addTrigger("HUDINFO","^HUD",sub { $battlecore->parse(@_); return undef },1);
#
# This object is a singleton, so multiple calls to new will return the
# same object.
#
# To destroy this object, simply drop all references to it, INCLUDING the Trigger.

use strict;
use vars qw/@ISA/;
use warnings;
use diagnostics;

@ISA = qw(XMPlugin);

my $globalXMBattleCore=undef;

sub new {
  my ($this,$title)=@_;
  my $class = ref($this) || $this;

  if (defined $globalXMBattleCore) {
    return $globalXMBattleCore;
  }

  # SUPER::new??
  my $self=new XMPlugin("xmud_battletech","BattleCoreWidget",$title);
  return undef unless defined $self;
  bless $self,$class;
  $globalXMBattleCore=$self;

  # Really???
  XM::weaken($globalXMBattleCore);

  return $self;
}

sub function {
  my ($self,$id,$args)=@_;
  if ($id==0) { # SEND
    XM::sendText($args);
    return "";
  } elsif ($id==1) {
    return $self->onMechinfoChange($args);
  } elsif ($id==2) {
    return $self->onLogInfo($args);
  } else {
    return SUPER::function(@_);
  }
}

# callback for every mechinfo change...
sub onMechinfoChange {
    my ($self,$mi)=@_;
    if (exists $self->{onMechinfoChange} and
	ref $self->{onMechinfoChange} eq "CODE") {
	return $self->{onMechinfoChange}->($self,$mi);
    } else {
	return undef;
    }
}
sub onLogInfo {
    my ($self,$log)=@_;
    if (exists $self->{onLogInfo} and
	ref $self->{onLogInfo} eq "CODE") {
	return $self->{onLogInfo}->($self,$log);
    } else {
	return undef;
    }
}


# Called for every HUDINFO Line...
sub parse {
  my ($self,$line)=@_;
  if (defined $line) {
    chomp $line;
    chomp $line;
    $self->call(0,$line);
  } 
}

sub parseText {
  my ($self,$line)=@_;
  if (defined $line) {
    $line=XM::ansiToRaw($line);
    chomp $line;
    chomp $line;
    $self->call(10,$line);
  } 
}


sub setTacticalInterval {
  my ($self,$inv)=@_;
  $self->call(1,$inv || 0);
}

sub setStatusInterval {
  my ($self,$inv)=@_;
  $self->call(2,$inv || 0);
}

sub getOwnID {
  return $_[0]->call(3,"");
}

sub getContacts {
  my $self=shift;
  if (wantarray) {
    return split /\|/, $self->call(4,"");
  } else {
    return $self->call(4,"");
  }
}

sub setContactsInterval {
  my ($self,$inv)=@_;
  $self->call(5,$inv || 0);
}

#    ID: mechid, ID of the unit
#    AC: arc, weapon arc the unit is in
#    SE: sensors, sensors that see the unit
#    UT: unit type character
#    MN: string, mechname of unit
#    X, Y, Z: coordinates of unit
#    RN: range, range to unit
#    BR: degree, bearing to unit
#    SP: speed, speed of unit
#    VS: speed, vertical speed of unit
#    HD: degree, heading of unit
#    JH: degree, jump heading, or '-' if not jumping
#    RTC: range, range from unit to X,Y center
#    BTC: degree, bearing from unit to X,Y center
#    TN: integer, unit weight in tons
#    HT: heatmeasure, unit's apparent heat (overheat)
#    FL: unit status string
#    AG: Age in secs since last LOS contact (0 == LOS maintained)

sub getContact {
  my ($self,$id)=@_;
  if (wantarray) {
    my @vars=qw/ID AC SE UT MN X Y Z RN BR SP VS HD JH RTC BTC TN HT FL AG/;
    my $result = $self->call(6,$id || "");
    if(defined $result) {
      return map {(shift @vars)=>$_} split /\|/, $result;
    } else {
      return ();
    }
  } else {
    return $self->call(6,$id || "");
  }
}


sub setContact {
  my ($self,$id,$info) =@_;
  if (defined $info) {
    $self->call(7,$id || "",$info);
  } else {
    $self->deleteContact($id);
  }
}

sub deleteContact {
  my ($self,$id)=@_;
  $self->call(8,$id || "");
}

sub setWeaponsInterval {
  my ($self,$inv)=@_;
  $self->call(9,$inv || 0);
}

sub startSecondary {
  my ($self)=@_;
  $self->call(11,1);
}

# overload show? keep this invisible?
# sub show() {}


1;
