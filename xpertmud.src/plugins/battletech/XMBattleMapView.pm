package XMBattleMapView;
use strict;
use vars qw/@ISA/;
use warnings;
use diagnostics;

@ISA = qw(XMPlugin);

sub new {
  my ($this,$title,$style)=@_;
  my $class = ref($this) || $this;

  $style="Heavy" if !defined $style;
  # SUPER::new??
  my $self=new XMPlugin("xmud_battletech","BattleMapView",$title,$style);
  return undef unless defined $self;
  bless $self,$class;

  return $self;
}

sub function {
  my ($self,$id,$args)=@_;

  if ($id==0) {
    # Tile Leftclicked
    if ($self->hexClicked($args)) {
      return 1;
    }
  } elsif ($id==1) {
    # Mech leftclicked
    if ($self->mechClicked($args)) {
      return 1;
    }
  } else {
    return SUPER::function(@_);
  }
  return 0;
}


sub mechClicked {
  my ($self,$mechid)=@_;
  if (exists $self->{mechClicked} and ref $self->{mechClicked} eq "CODE") {
    my $func=$self->{mechClicked};
    return &$func($self,$mechid);
  }
  return undef;
}
sub hexClicked {
  my ($self,$hex)=@_;
  if ($hex=~/^(\d+)\s+(\d+)\s+(\d+)$/ ) {
    if (exists $self->{hexClicked} and ref $self->{hexClicked} eq "CODE") {
      my $func=$self->{hexClicked};
      return &$func($self,$1,$2);
    }
  }
  return undef;
}

sub centerMech {
  my ($self,$mech)=@_;
  $self->call(0,$mech);
}
sub scroll {
  my ($self,$dx,$dy)=@_;
  $self->call(1,"$dy, $dy");
}
sub setZoom {
  my ($self,$zoom)=@_;
  $self->call(2,$zoom);
}
sub showQuickbar {
  $_[0]->call(3,"");
}
sub hideQuickbar {
  $_[0]->call(4,"");
}

sub saveImage {
#  my ($self,$filename,$x1,$y1,$x2,$y2)=@_;
  my $self=shift;
  $self->call(5,join(',',@_));
}

sub switchStyle {
  $_[0]->call(6,$_[1]);
}

1;
