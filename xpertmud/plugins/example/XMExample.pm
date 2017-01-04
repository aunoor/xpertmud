package XMExample;
use strict;
use vars qw/@ISA/;
use warnings;
use diagnostics;

@ISA=qw(XMPlugin);

sub new {
  my ($this,$title,$text)=@_;
  my $class = ref($this) || $this;

  # SUPER::new??
  my $self=new XMPlugin("xmud_example","XmudExample",$title);
  return undef unless defined $self;
  bless $self,$class;

  $self->setButtonText($text) if defined $text;
  return $self;
}

sub setButtonText {
  my ($self,$text)=@_;
  return $self->call(1,$text);
}

sub getButtonText {
  my ($self)=@_;
  return $self->call(0, "");
}

sub resetText {
  my ($self)=@_;
  return $self->call(2, "");
}

sub function {
  my ($self,$id,$args)=@_;

  if ($id==0) {
    return "ButtonPress reached perl";
  } else {
    return SUPER::function(@_);
  }
}
