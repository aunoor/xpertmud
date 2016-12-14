use strict;

my $window=new XMTextWindow("Snake");
my @snake=([2,2]);
my $length=5;
my $dx=1;
my $dy=0.5;
my $firstRun=1;
sub moveSnake {
  if ($firstRun) {
    $window->resizeChars(20,10);
    $window->show;
    $firstRun=0;
  }
  my @pos=$window->getCursorPos;
  my ($x,$y) = @{$snake[0]};
  $x+=$dx;
  $y+=$dy;
  my $oldchar=$window->getCharAt($x,$y);
  $window->setCursor($x,$y);
  if ($oldchar eq "" or $oldchar eq " ") {
    $window->setFGColor(MAGENTA);
    $window->print("*");
  } elsif ($oldchar eq "*") {
    $window->setFGColor(RED);
    $window->print("@");
    $length-- if $length>3;
  } else {
    $window->setFGColor(BLUE);
    $window->print("#");
    $length++;
  }
  $dx=1 if $x <= 0;
  $dy=0.5 if $y <= 0;
  $dx=-1 if $x >= $window->getColumns-1;
  $dy=-0.5 if $y >= $window->getLines-1;
  unshift @snake,[$x,$y];
  while (@snake > $length) {
    my ($x,$y)=@{pop @snake};
    $window->setCursor($x,$y);
    $window->print(" ");
  }

  if (rand()<0.05) {
    my $x=int(rand()*$window->getColumns);
    my $y=int(rand()*$window->getLines);
    $window->setCursor($x,$y);
    $window->setFGColor(GREEN);
    $window->print("O");
  }

  if ($x>0 and $x<$window->getColumns-1 and rand()<0.01) {
    $dx=-$dx;
  }
  if ($y>0 and $y<$window->getLines-1 and rand()<0.01) {
    $dy=-$dy;
  }
  $window->resetAttributes;
  $window->setCursor(@pos);
}

$window->registerMouseDownEvent(sub {
				  my ($win,$x,$y)=@_;
				  $win->setCursor($x,$y);
				  $win->setFGColor(WHITE);
				  $win->print("<");
				});
$window->registerMouseDragEvent(sub {
				  my ($win,$x,$y)=@_;
				  $win->setCursor($x,$y);
				  $win->setFGColor(YELLOW);
				  $win->print("O");
				});
$window->registerMouseUpEvent(sub {
				  my ($win,$x,$y)=@_;
				  $win->setCursor($x,$y);
				  $win->setFGColor(CYAN);
				  $win->print(">");
				});

$window->enableMouseEvents;

addTimer("SNAKE_Timer",0.20, \&moveSnake);
addAlias("SNAKE_Stop","^stop\$",sub {
	   delTimer("SNAKE_Timer");
	   delAlias("SNAKE_Stop");
	   undef $window;
	   return "";
	 });


1;
