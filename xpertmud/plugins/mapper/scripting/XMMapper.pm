package XMMapper;
use strict;
use vars qw/@ISA/;
use warnings;
use diagnostics;
#use Switch;

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

sub trigger {
  my ($self,$iline)=@_;
  $self->call(0, $iline);
}

sub function {
  my ($self, $id, $args) = @_;

  if ($id==0) {
    XM::addTrigger("XMMAPPER_TRIGGER","^.*\$", sub {
          my $line = shift;
          my $sline = XM::ansiToRaw($line);
          $self->trigger($sline);
          return $line }
        ,0,1);
         return "Trigger enabled";
      }
    elsif($id==1) {
      XM::deleteTrigger("XMMAPPER_TRIGGER");
      return "Trigger deleted";
    }
    else {return SUPER::function(@_);}
}
