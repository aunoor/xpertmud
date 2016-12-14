package XMHTML;
use strict;
use vars qw/@ISA/;
use warnings;

@ISA=qw(XMPlugin);

sub new {
  my ($this,$title)=@_;
  my $class = ref($this) || $this;

  # SUPER::new??
  my $self=new XMPlugin("xmud_html","XMHtml",$title);
  return undef unless defined $self;
  bless $self,$class;

  return $self;
}

# Method 1:
# call begin(url) once (url is not opened, but used for relative links)
# call write one or several times
# call end when you're done.
sub begin {
  my ($self,$url)=@_;
  $url="xmud://" unless defined $url;
  return $self->call(0,"$url");
}
sub write {
  my ($self,$html)=@_;
  return $self->call(1,"$html");
}
sub end {
  my ($self)=@_;
  return $self->call(2,"");
}

# Method 2:
# open a http:// or file:// url directly
sub open {
  my ($self,$url)=@_;
  return $self->call(3,"$url");
}

# Misc stuff
# set zoom level (in percent)
sub zoom {
  my ($self,$zoom)=@_;
  return $self->call(4,$zoom);
}

#execute some javascript call
# js "this" value is set to toplevel document. 
sub javascript {
  my ($self,$script)=@_;
  return $self->call(5,"$script");
}


# callbacks
sub function {
  my ($self,$id,$args)=@_;

  if ($id==0) {
      return $self->onCaptionChange($args);
  } elsif ($id==1) {
      return $self->onOpenUrl($args);
  } else {
    return SUPER::function(@_);
  }
}

#overload this function or supply a subref in $html->{onCaptionChange}
sub onCaptionChange {
    my ($self,$caption)=@_;
    if (exists $self->{onCaptionChange} and
	ref $self->{onCaptionChange} eq "CODE") {
	return $self->{onCaptionChange}->($self,$caption);
    } else {
	if (defined $caption) {
	    return $self->setTitle($caption);
	}
	return undef;
    }
}

#overload this function or supply a subref in $html->{onCaptionChange}
sub onOpenUrl {
    my ($self,$url)=@_;
    if (exists $self->{onOpenUrl} and
	ref $self->{onOpenUrl} eq "CODE") {
	return $self->{onOpenUrl}->($self,$url);
    } else {
	if (defined $url and ($url=~/^http/ or $url=~/^file/)) {
	    return $self->open($url);
	}
    }
}

