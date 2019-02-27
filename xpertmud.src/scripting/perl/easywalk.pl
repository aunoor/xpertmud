use Data::Dumper;
use IO::File;
use vars qw($walkPaths $walkNodes $walkInvNodes $walkWin
	    $mudLocation);

$walkNodes = {};
$walkInvNodes = {};
$walkPaths = {};


delAlias("AlWalkTravel");
addAlias("AlWalkTravel",'^_(\w+)$', \&walkAliasTravel,0);

delAlias("AlWalkAddNode");
addAlias("AlWalkAddNode",'^addNode (\w+)', \&walkAliasAddNode, 1);

delAlias("AlWalkDelNode");
addAlias("AlWalkDelNode",'^delNode$', \&walkAliasDelNode, 1);

delAlias("AlWalkAddPath");
addAlias("AlWalkAddPath",'^addPath (\w+) (.*)', \&walkAliasAddPath, 1);

delAlias("AlWalkDelPath");
addAlias("AlWalkDelPath",'^delPath (\w+)', \&walkAliasDelPath, 1);

delAlias("AlWalkPrintNodes");
addAlias("AlWalkPrintNodes",'^listNodes(?:\s+(.*))?$', \&walkAliasListNodes, 1);
delAlias("AlWalkPrintDests");
addAlias("AlWalkPrintDests",'^listDests$', \&walkAliasListDests, 1);


sub walkAliasTravel {
    my($e) = @_;
    if($e =~ /^_(\w+)$/) {
	walkTravel($walkInvNodes->{$mudLocation},$1, 100);
#	walkTravel(,$1);
    }
}


#Travel from origin to dest
#maxwalk is the maximum nodes we can travel, to avoid server overload in case of bug!
sub walkTravel {
    my($origin, $dest, $maxwalk)=@_;
    my $r = shift;
    $maxwalk--;
    if($maxwalk <= 0){prs("MaxWalk value reached!\n"); return;}

    if(!$walkInvNodes->{$mudLocation}) {prs("Present location is not a node! ($mudLocation)\n"); return;}
    if(!$walkNodes->{$dest}) {prs("Destination is not a node!\n"); return;}

    my $path = $walkPaths->{"$origin" . "_$dest"};
    if(!$path) {
	prs("Unknown Path! ($origin" . "_$dest). Finding.\n");

	my $knownPaths = walkFindPaths($origin, $dest, $maxwalk);
#	walkPrintFoundPaths($knownPaths);
	$path = walkGetShortPath($knownPaths);
	$walkWin->print("Short is:" . join(";", @$path) . "\n");
    }
    my $lastnode = $origin;
    if($path) {
	foreach my $t(@$path) {
	    if($t =~ /_(\w+)$/) {
#		$debugWin->print("Doing $lastnode" . "_$1\n");
		walkTravel($lastnode, $1, $maxwalk);
		$lastnode = $1;
	    } else {
#		$debugWin->print("Doing $t\n");
		&$onTextEntered($t);
	    }
	}
    }

}

sub walkAliasListNodes {
    my($e) = @_;
    if($e =~ /^listNodes(?:\s+(.*))?$/) {
	walkListNodes($1);
    }
}

sub walkListNodes {
    my($regexp) = @_;
    $walkWin->print("Nodes listing:\n");
    foreach(sort(keys %$walkNodes)) {
	if(!$regexp || $_ =~ /$regexp/) {
	    $walkWin->print("\t$_\n");
	}
    }
}


sub walkAliasListDests {
    my($e) = @_;
    if($e =~ /^listDests$/) {
	if($walkInvNodes->{$mudLocation}) {
	    walkListDests($walkInvNodes->{$mudLocation});
	} else {
	    $walkWin->print("You are not in a known location\n");
	}
    }
}

sub walkListDests {
    my($loc) = @_;
    my ($info, $dests);
    $info = $walkNodes->{$loc};
    $dests = @$info[1];
    $walkWin->print("Destinations from $loc:\n");
    foreach(keys %$dests) {
	$walkWin->print("\t$_\n");
    }
}

sub walkAliasAddPath {
    my($e) = @_;
    if($e =~ /^addPath (\w+) (.*)/) {
	walkAddPath($1,$2);
    }
}

sub walkAddPath {
    my($dest, $path) = @_;
    if(!$walkInvNodes->{$mudLocation}) {prs("Present location is not a node! ($mudLocation)\n"); return;}
    if(!$walkNodes->{$dest}) {prs("Unknown destination node! ($dest)\n"); return;}
    my @l = split(";" , $path);
#    $walkPaths->{"" . "_$dest"} = \@l;
    $walkPaths->{$walkInvNodes->{$mudLocation} . "_$dest"} = \@l;

    my $h = $walkNodes->{$walkInvNodes->{$mudLocation}}[1];
    $h->{$dest} = 1;
    prs("Added path " . "_$dest\n");

}

sub walkAliasDelPath {
    my($e) = @_;
    if($e =~ /^delPath (\w+)/) {
	walkDelPath($walkInvNodes->{$mudLocation}, $1);
    }
}

sub walkDelPath {
    my($origin, $dest) = @_;
    if(!$origin) {prs("Unknown origin node! ($mudLocation)\n"); return;}
    if(!$walkNodes->{$dest}) {prs("Unknown destination node! ($dest)\n"); return;}

    if($walkPaths->{"$origin" . "_$dest"}) {
	delete $walkPaths->{"$origin" . "_$dest"};
	my $h = $walkNodes->{$origin}[1];
	delete $h->{$dest};
	prs("Removed path $origin" . "_$dest\n");
    } else {
	prs("Unknown path $origin" . "_$dest\n");
    }

}

sub walkAliasAddNode {
    my($name) = @_;
    if($name =~ /^addNode (\w+)/) {
	$name = $1;
    }
    walkAddNode($name);
}


sub walkAddNode {
    my($name) = @_;
    if($walkNodes->{$name}) { prs("Node $name already exists!\n"); return;}
    if($walkInvNodes->{$mudLocation}) { prs("Room short description conflit!\n"); return;}
    $walkNodes->{$name} = [$mudLocation, {}];
    $walkInvNodes->{$mudLocation} = $name;
    prs("Added node $name\n");
}


sub walkAliasDelNode {
    walkDelNode();
}

sub walkDelNode {
    my($name) = @_;
    my $onodes = $walkNodes->{$name}[1];
    my $loc = $walkNodes->{$name}[0];
    foreach $onode(keys %$onodes) {
	walkDelPath($name, $onode);
	my $oonodes = $walkNodes->{$onode}[1];
	if($oonodes->{$name}) {
	    walkDelPath($onode,$name);
	}
    }
    delete $walkInvNodes->{$loc};
    delete $walkNodes->{$name};
    prs("Removed node $name\n");
}


sub walkDumpData {
    my($fname) = @_;
    my $f;
    $Data::Dumper::Varname="conf";
    open $f, ">$fname";
    print {$f} Dumper([$walkNodes, $walkInvNodes, $walkPaths]);
    close($f);
}

sub walkDumpDataDebug {
    my $f;
    $Data::Dumper::Varname="conf";
    $walkWin->print(Dumper([$walkNodes, $walkInvNodes, $walkPaths]));
}

sub walkReadData {
    my($fname) = @_;
    my $conf = do($fname);
    $walkNodes = @$conf[0];
    $walkInvNodes = @$conf[1];
    $walkPaths = @$conf[2];
}

sub walkPrintDest {
    my $h = $walkNodes->{$walkInvNodes->{$mudLocation}}[1];
    my @l = keys %$h;
    $walkWin->print($walkInvNodes->{$mudLocation} . " Destinations:\n");
    foreach(@l) { $walkWin->print("\t$_\n");}
}

sub walkFindPathR {
    my($origin, $dest, $knownNodes, $knownPaths, $pathTillHere, $depth) = @_;
    my($path, $exits, $r);
    
    if($depth == 0) { return;}
    $depth--;
    $knownNodes->{$origin} = 1;
    $exits = $walkNodes->{$origin}[1];
    foreach $ex(keys %$exits) {
	push @$pathTillHere, "_" . $ex;
	if($ex eq $dest) {
	    my $foundPath;
	    push @$foundPath, @$pathTillHere;
	    push @$knownPaths, $foundPath;
	} elsif(!$knownNodes->{$ex}) { # new node
	    walkFindPathR($ex, $dest, $knownNodes, $knownPaths, $pathTillHere,$depth);
	}
	pop @$pathTillHere;
    }

    $knownNodes->{$origin} = 0;
}

sub walkFindPaths {
    my($origin, $dest, $depth) = @_;
    my $knownPaths = [];
    walkFindPathR($origin, $dest, {}, $knownPaths, [], $depth);
#    printFoundPaths($knownPaths);
#    my $s = walkGetShortPath($knownPaths);
#    prs("short2: " . join(";" , @$s) . "\n");
    return $knownPaths;
}

sub walkFindPathsPrint {
    my($origin, $dest, $depth) = @_;
    my $knownPaths = [];
    walkFindPathR($origin, $dest, {}, $knownPaths, [], $depth);
    printFoundPaths($knownPaths);
    my $s = walkGetShortPath($knownPaths);
    prs("short: " . join(";" , @$s) . "\n");
}

sub walkGetShortPath {
    my($knownPaths) = @_;

 #   prs(scalar(@$knownPaths) . " paths.\n");
    my $s = 9999;
    my $i = 0;
    my $l;
    foreach(@$knownPaths) {
	$l = scalar(@$_);
	if ($l < $s) {$s = $l; $short = $i;};
	$i++;
    }
    return @$knownPaths[$short];
}

sub walkPrintFoundPaths {
    my($knownPaths) = @_;
    prs("Found Paths:\n");
    foreach(@$knownPaths) {
	prs("Found: " . join(";", @$_) . "\n");
    }
}

###################### WINDOW ###############################

sub createWalkWin {
    $walkWin = new XMTextBufferWindow(); # it's still hidden
    $walkWin->resizeChars(40, 10);
    $walkWin->setTitle('Walk - $walkWin');
    $walkWin->move(603, 362);
    $walkWin->show();
}

sub destroyWalkWin {
    undef($walkWin);
}


####################33 MAIN ##################################3
#createWalkWin();
$walkWin = statusWindow();

__END__

TODO:
    - support to pather (script that records commands??)
    - short path finder
    - paths weight and type
    - Non node ending paths (something i miss)
    - nodes area location 
    - add path name (besides destination - null destinies)


Requisits:

    You must define some triggers that mantain the variable $mudLocation. This can be the room short location (easy to filter in some muds) or any other thing that may distinguish the actual room from the rest. This string will distinguish this room from any other and should be unique. Note that this is not the name you will later use to design the room.

How to use it:
    All the output of the script goes to $walkWin, by default this is the statusWindow.

    The room name (pex A) you will use next, is a freely  choosen name. It should be simple because it's the one we will have to remember later to travel to that room.

If you have problems remember to check if $mudLocation is correctly updated
before typing the commands.

    Go to a room (we will name it A) and add it as a node (addNode A).
    Go to other room (B) and add it as a node (addNode B)
    Make sure $mudLocation is updated (critical here, and since my mudlocation is only updated on enter or look room, i need to look here to update it since it was not a node when i first entered it)
    add the path to room A (addPath A n;s;e;e;e;n;)
    return no Room A by the using the new path (_A)
    add the path to room B (addPath B s;w;w;w;n;s)
    Now go to another room (C), add it and the paths from and to A (as you did in A-B)
    You can now travel from C to B without adding a new path for it. (just type _B in room C).
    It's possible to make composed paths from C to B. From C (addPath B _A;_B) but this may screw the find short path algorithm (tough it might be usefull when you have long paths and want to be sure you follow a choosen one). 
    Commands separated by a ; will be sent independently via $onTextEntered with the exception of _* commands wich are processed first (due to being dependent on $mudLocation wich is not known untill we actually reach there!).
    You can use the listNodes <regexp> command to list Nodes and the listDests command to list the destinations from the current room
    The delPath and delNode commands are also avaiable to maintain walk graph.
