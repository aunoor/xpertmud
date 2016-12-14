package XMBattleWeapons;
use strict;
use vars qw/@ISA/;
use warnings;
use diagnostics;

@ISA = qw(XMPlugin);

sub new {
  my ($this,$title)=@_;
  my $class = ref($this) || $this;

  # SUPER::new??
  my $self=new XMPlugin("xmud_battletech","BattleWeaponsWidget",$title);
  return undef unless defined $self;
  bless $self,$class;

  return $self;
}

sub function {
  my ($self,$id,$args)=@_;

#  if ($id==0) {
#    return "";
#  } else {
    return SUPER::function(@_);
#  }
}


1;
