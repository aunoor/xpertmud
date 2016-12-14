package XMBattleContacts;
use strict;
use vars qw/@ISA/;
use warnings;
use diagnostics;

@ISA = qw(XMPlugin);

sub new {
  my ($this,$title)=@_;
  my $class = ref($this) || $this;

  # SUPER::new??
  my $self=new XMPlugin("xmud_battletech","BattleContactWidget",$title);
  return undef unless defined $self;
  $self->{CONCALLER}="con ";
  bless $self,$class;

  return $self;
}

sub setConCaller {
  $_[0]->{CONCALLER}=$_[1];
}

sub function {
  my ($self,$id,$args)=@_;

  if ($id==0) {
    # Contact caller
    ::wrap_onTextEntered($self->{CONCALLER}.$args);
    return "";
  } else {
    return SUPER::function(@_);
  }
}


1;
