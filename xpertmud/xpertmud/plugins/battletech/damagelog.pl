use strict;
use warnings;
no warnings 'redefine';

my %inflictedDamage;
my %takenDamage;
my $lastWeapon="unknown";
my %inflictedWeapons;

sub damageText {
	my $txt='Damage Log:%r%cb%cuDamage inflicted:%cn%r%r';
	my $shots=0;
	my $damage=0;
	foreach my $loc (keys %inflictedDamage) {
		$txt .= sprintf('%20s: %4d damage in %4d shots.%%r',
				$loc,
				$inflictedDamage{$loc}->[1],
				$inflictedDamage{$loc}->[0]);
		$shots+=$inflictedDamage{$loc}->[0];
		$damage+=$inflictedDamage{$loc}->[1];
	}
 	$txt.=sprintf('%%ch%20s: %4d damage in %4d shots.%%cn%%r',
		"Total",$damage,$shots);
		
 	$txt.='%r%cb%cuWeapons hit:%cn%r%r';
	foreach my $loc (keys %inflictedWeapons) {
		$txt .= sprintf('%20s: %4d damage in %4d shots.%%r',
				$loc,
				$inflictedWeapons{$loc}->[1],
				$inflictedWeapons{$loc}->[0]);
	}
	
	$shots=0;
	$damage=0;	
	$txt.='%r%cb%cuDamage taken:%cn%r%r';
	foreach my $loc (keys %takenDamage) {
		$txt .= sprintf('%20s: %4d damage in %4d shots.%%r',
				$loc,
				$takenDamage{$loc}->[1],
				$takenDamage{$loc}->[0]);
		$shots+=$takenDamage{$loc}->[0];
		$damage+=$takenDamage{$loc}->[1];
	
	}
 	$txt.=sprintf('%%ch%20s: %4d damage in %4d shots.%%r%%cn',
		"Total",$damage,$shots);

	$txt=~s/ (?= )/ \%/g;
	return $txt;
}

delAlias(qr/^DAMAGE_/);
addAlias("DAMAGE_RESET",qr/^damagereset\s*$/,sub {
	%inflictedDamage=();
	%inflictedWeapons=();
	%takenDamage=();
	statusWindow->print(ansi('%cbDamage log reset%cn%r'));
	return undef;
	},1);
	
addAlias("DAMAGE_PRINT",qr/^damageprint\s*$/,sub {
	statusWindow->print(ansi(damageText));
	return undef;
	},1);

delTrigger(qr/^DAMAGE_/);
addTrigger("DAMAGE_DEAL",qr/You hit for (\d+) points of damage in the ([\w (]+[\w)])/, sub { 
        my $w=$2;
        chomp $w;
	$inflictedDamage{$w}->[0]++;
	$inflictedDamage{$w}->[1]+=$1;
	$inflictedWeapons{$lastWeapon}->[0]++;
	$inflictedWeapons{$lastWeapon}->[1]+=$1;
	return shift;
	});

addTrigger("DAMAGE_TAKE",qr/You have been hit for (\d+) points of damage in the ([\w (]+[\w)])/, sub {
	my $p=$1;
	my $w=$2;
        chomp $w;
	unless ($w=~/transfer/) {
		$takenDamage{$w}->[0]++;
		$takenDamage{$w}->[1]+=$p;
	}
	return shift;
	});

addTrigger("DAMAGE_WEAPON",qr/You fire ([\w\-\/]+) at/, sub {
	$lastWeapon=$1;
	delDelayed(qr/^DAMAGE_WEAPON_RESET$/);
	addDelayed("DAMAGE_WEAPON_RESET",3,sub { $lastWeapon="unknown"; });
	return shift;
});

addTrigger("DAMAGE_WEAPON_KICK",qr/You try and kick/, sub {
	$lastWeapon="Kick";	
	delDelayed(qr/^DAMAGE_WEAPON_RESET$/);
	addDelayed("DAMAGE_WEAPON_RESET",3,sub { $lastWeapon="unknown"; });
	return shift;
});

addTrigger("DAMAGE_WEAPON_PUNCH",qr/You try to punch/, sub {
	$lastWeapon="Punch";	
	delDelayed(qr/^DAMAGE_WEAPON_RESET$/);
	addDelayed("DAMAGE_WEAPON_RESET",3,sub { $lastWeapon="unknown"; });
	return shift;
});

addTrigger("DAMAGE_WEAPON_AXE",qr/You try to swing your axe/, sub {
	$lastWeapon="Axe";	
	delDelayed(qr/^DAMAGE_WEAPON_RESET$/);
	addDelayed("DAMAGE_WEAPON_RESET",3,sub { $lastWeapon="unknown"; });
	return shift;
});


addAlias("DAMAGE_INSERT",qr/\@DAMAGE\@/,sub {
	my $t=damageText;
	my $l=shift;
	$l=~ s/\@DAMAGE\@/$t/;
	return $l;
});
