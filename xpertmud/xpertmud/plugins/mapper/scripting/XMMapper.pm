package XMMapper;
use strict;
use vars qw/@ISA/;
use warnings;
use diagnostics;

@ISA=qw(XMPlugin);

sub new {
  my ($this, $title, $text) = @_;
  my $class = ref($this) || $this;

  # SUPER::new??
  my $self=new XMPlugin("xmud_mapper", "XmudMapper", $title);
  return undef unless defined $self;
  bless $self,$class;

  return $self;
}

sub function {
  my ($self, $id, $args) = @_;

  if ($id == 0) {
    return "Function no 0";
  } else {
    return SUPER::function(@_);
  }
}
