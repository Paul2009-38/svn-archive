# 
# PERL mapweaver module by gary68
#
#
#
#
# Copyright (C) 2011, Gerhard Schwanz
#
# This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the 
# Free Software Foundation; either version 3 of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
# FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along with this program; if not, see <http://www.gnu.org/licenses/>
#


package mwMisc ; 

use strict ;
use warnings ;

use Math::Trig;
use Math::Polygon ;
use List::Util qw[min max] ;

use mwConfig ;
use mwFile ;

use vars qw($VERSION @ISA @EXPORT @EXPORT_OK);

require Exporter ;

@ISA = qw ( Exporter AutoLoader ) ;

@EXPORT = qw (	getValue
		createLabel
		buildRings
		angleMapgen
		triangleNode
		intersection
		areaSize
		 ) ;



sub getValue {
	my ($key, $aRef) = @_ ;
	my $value = undef ;
	foreach my $kv (@$aRef) {
		if ($kv->[0] eq $key) { $value = $kv->[1]; }
	}
	return $value ;
}

sub createLabel {
#
# takes @tags and labelKey(s) from style file and creates labelTextTotal and array of labels for directory
# takes more keys in one string - using a separator. 
#
# § all listed keys will be searched for and values be concatenated
# # first of found keys will be used to select value
# "name§ref" will return all values if given
# "name#ref" will return name, if given. if no name is given, ref will be used. none given, no text
#
	my ($ref1, $styleLabelText, $lon, $lat) = @_ ;
	my @tags = @$ref1 ;
	my @keys ;
	my @labels = () ;
	my $labelTextTotal = "" ; 

	if (grep /!/, $styleLabelText) { # AND
		@keys = split ( /!/, $styleLabelText) ;
		# print "par found: $styleLabelText; @keys\n" ;
		for (my $i=0; $i<=$#keys; $i++) {
			if ($keys[$i] eq "_lat") { push @labels, $lat ; } 
			if ($keys[$i] eq "_lon") { push @labels, $lon ; } 
			foreach my $tag (@tags) {
				if ($tag->[0] eq $keys[$i]) {
					push @labels, $tag->[1] ;
				}
			}
		}
		$labelTextTotal = "" ;
		foreach my $label (@labels) { $labelTextTotal .= $label . " " ; }
	}
	else { # PRIO
		@keys = split ( /#/, $styleLabelText) ;
		my $i = 0 ; my $found = 0 ;
		while ( ($i<=$#keys) and ($found == 0) ) {
			if ($keys[$i] eq "_lat") { push @labels, $lat ; $found = 1 ; $labelTextTotal = $lat ; } 
			if ($keys[$i] eq "_lon") { push @labels, $lon ; $found = 1 ; $labelTextTotal = $lon ; } 
			foreach my $tag (@tags) {
				if ($tag->[0] eq $keys[$i]) {
					push @labels, $tag->[1] ;
					$labelTextTotal = $tag->[1] ;
					$found = 1 ;
				}
			}
			$i++ ;
		}		
	}
	return ( $labelTextTotal, \@labels) ;
}

sub buildRings {
#
# accepts ref to array of ways and option if unclosed rings shoulf be returned
# closeOpt == 1 returns only closed rings
#
# returns two refs to arrays of arrays: ways and nodes
#
	my ($ref, $closeOpt) = @_ ;
	my (@allWays) = @$ref ;
	my @ringWays = () ;
	my @ringNodes = () ;
	my $ringCount = 0 ;

	my ($memWayNodesRef, $memWayTagsRef) = mwFile::getWayPointers() ;

	# print "build rings for @allWays\n" ;
	if (cv('debug') eq "1" ) { print "BR: called.\n" ; }
	while ( scalar @allWays > 0) {
		# build new test ring
		my (@currentWays) = () ; my (@currentNodes) = () ;
		push @currentWays, $allWays[0] ;
		if (cv('debug') eq "1" ) { print "BR: initial way for next ring id= $allWays[0]\n" ; }
		push @currentNodes, @{$$memWayNodesRef{$allWays[0]}} ;
		my $startNode = $currentNodes[0] ;
		my $endNode = $currentNodes[-1] ;
		if (cv('debug') eq "1" ) { print "BR: initial start and end node $startNode $endNode\n" ; }
		my $closed = 0 ;
		shift @allWays ; # remove first element 
		if ($startNode == $endNode) {	$closed = 1 ; }

		my $success = 1 ;
		while ( ($closed == 0) and ( (scalar @allWays) > 0) and ($success == 1) ) {
			# try to find new way
			if (cv('debug') eq "1" ) { print "TRY TO FIND NEW WAY\n" ; }
			$success = 0 ;
			if (cv('debug') eq "1" ) { print "BR: actual start and end node $startNode $endNode\n" ; }
			my $i = 0 ;
			while ( ($i < (scalar @allWays) ) and ($success == 0) ) {
				if (cv('debug') eq "1" ) { print "BR: testing way $i = $allWays[$i]\n" ; }
				if (cv('debug') eq "1" ) { print "BR:   rev in front?\n" ; }
				if ( $$memWayNodesRef{$allWays[$i]}[0] == $startNode ) { 
					$success = 1 ;
					# reverse in front
					@currentWays = ($allWays[$i], @currentWays) ;
					@currentNodes = (reverse (@{$$memWayNodesRef{$allWays[$i]}}), @currentNodes) ;
					splice (@allWays, $i, 1) ;
				}
				if ($success ==0) {
					if (cv('debug') eq "1" ) { print "BR:   app at end?\n" ; }
					if ( $$memWayNodesRef{$allWays[$i]}[0] == $endNode)  { 
						$success = 1 ;
						# append at end
						@currentWays = (@currentWays, $allWays[$i]) ;
						@currentNodes = (@currentNodes, @{$$memWayNodesRef{$allWays[$i]}}) ;
						splice (@allWays, $i, 1) ;
					}
				}
				if ($success ==0) {
					if (cv('debug') eq "1" ) { print "BR:   app in front?\n" ; }
					if ( $$memWayNodesRef{$allWays[$i]}[-1] == $startNode) { 
						$success = 1 ;
						# append in front
						@currentWays = ($allWays[$i], @currentWays) ;
						@currentNodes = (@{$$memWayNodesRef{$allWays[$i]}}, @currentNodes) ;
						splice (@allWays, $i, 1) ;
					}
				}
				if ($success ==0) {
					if (cv('debug') eq "1" ) { print "BR:   rev at end?\n" ; }
					if ( $$memWayNodesRef{$allWays[$i]}[-1] == $endNode) { 
						$success = 1 ;
						# append reverse at the end
						@currentWays = (@currentWays, $allWays[$i]) ;
						@currentNodes = (@currentNodes, (reverse (@{$$memWayNodesRef{$allWays[$i]}}))) ;
						splice (@allWays, $i, 1) ;
					}
				}
				$i++ ;
			} # look for new way that fits

			$startNode = $currentNodes[0] ;
			$endNode = $currentNodes[-1] ;
			if ($startNode == $endNode) { 
				$closed = 1 ; 
				if (cv('debug') eq "1" ) { print "BR: ring now closed\n" ;} 
			}
		} # new ring 
		
		# examine ring and act
		if ( ($closed == 1) or ($closeOpt == 0) ) {
			# eliminate double nodes in @currentNodes
			my $found = 1 ;
			while ($found) {
				$found = 0 ;
				LABCN: for (my $i=0; $i<$#currentNodes; $i++) {
					if ($currentNodes[$i] == $currentNodes[$i+1]) {
						$found = 1 ;
						splice @currentNodes, $i, 1 ;
						last LABCN ;
					}
				}
			}
			# add data to return data
			@{$ringWays[$ringCount]} = @currentWays ;
			@{$ringNodes[$ringCount]} = @currentNodes ;
			$ringCount++ ;
		}
	} 
	return (\@ringWays, \@ringNodes) ;
}

sub angleMapgen {
#
# angle between lines/segments
#
	my ($g1x1) = shift ;
	my ($g1y1) = shift ;
	my ($g1x2) = shift ;
	my ($g1y2) = shift ;
	my ($g2x1) = shift ;
	my ($g2y1) = shift ;
	my ($g2x2) = shift ;
	my ($g2y2) = shift ;

	my $g1m ;
	if ( ($g1x2-$g1x1) != 0 )  {
		$g1m = ($g1y2-$g1y1)/($g1x2-$g1x1) ; # steigungen
	}
	else {
		$g1m = 999999999 ;
	}

	my $g2m ;
	if ( ($g2x2-$g2x1) != 0 ) {
		$g2m = ($g2y2-$g2y1)/($g2x2-$g2x1) ;
	}
	else {
		$g2m = 999999999 ;
	}

	if ($g1m == $g2m) {   # parallel
		return (0) ;
	}
	else {
		my $t1 = $g1m -$g2m ;
		my $t2 = 1 + $g1m * $g2m ;
		if ($t2 == 0) {
			return 90 ;
		}
		else {
			my $a = atan (abs ($t1/$t2)) / 3.141592654 * 180 ;
			return $a ;
		}
	}
} 

sub triangleNode {
#
# get segment of segment as coordinates
# from start or from end of segment
#
	# 0 = start
	# 1 = end
	my ($x1, $y1, $x2, $y2, $len, $startEnd) = @_ ;
	my ($c) = sqrt ( ($x2-$x1)**2 + ($y2-$y1)**2) ;
	my $percent = $len / $c ;

	my ($x, $y) ;
	if ($startEnd == 0 ) {	
		$x = $x1 + ($x2-$x1)*$percent ;
		$y = $y1 + ($y2-$y1)*$percent ;
	}
	else {
		$x = $x2 - ($x2-$x1)*$percent ;
		$y = $y2 - ($y2-$y1)*$percent ;
	}
	return ($x, $y) ;
}

sub intersection {
#
# returns intersection point of two lines, else (0,0)
#
	my ($g1x1) = shift ;
	my ($g1y1) = shift ;
	my ($g1x2) = shift ;
	my ($g1y2) = shift ;
	
	my ($g2x1) = shift ;
	my ($g2y1) = shift ;
	my ($g2x2) = shift ;
	my ($g2y2) = shift ;

	if (($g1x1 == $g2x1) and ($g1y1 == $g2y1)) { # p1 = p1 ?
		return ($g1x1, $g1y1) ;
	}
	if (($g1x1 == $g2x2) and ($g1y1 == $g2y2)) { # p1 = p2 ?
		return ($g1x1, $g1y1) ;
	}
	if (($g1x2 == $g2x1) and ($g1y2 == $g2y1)) { # p2 = p1 ?
		return ($g1x2, $g1y2) ;
	}

	if (($g1x2 == $g2x2) and ($g1y2 == $g2y2)) { # p2 = p1 ?
		return ($g1x2, $g1y2) ;
	}

	my $g1m ;
	if ( ($g1x2-$g1x1) != 0 )  {
		$g1m = ($g1y2-$g1y1)/($g1x2-$g1x1) ; # steigungen
	}
	else {
		$g1m = 999999 ;
	}

	my $g2m ;
	if ( ($g2x2-$g2x1) != 0 ) {
		$g2m = ($g2y2-$g2y1)/($g2x2-$g2x1) ;
	}
	else {
		$g2m = 999999 ;
	}

	if ($g1m == $g2m) {   # parallel
		return (0, 0) ;
	}

	my ($g1b) = $g1y1 - $g1m * $g1x1 ; # abschnitte
	my ($g2b) = $g2y1 - $g2m * $g2x1 ;

	my ($sx) = ($g2b-$g1b) / ($g1m-$g2m) ;             # schnittpunkt
	my ($sy) = ($g1m*$g2b - $g2m*$g1b) / ($g1m-$g2m);

	my ($g1xmax) = max ($g1x1, $g1x2) ;
	my ($g1xmin) = min ($g1x1, $g1x2) ;	
	my ($g1ymax) = max ($g1y1, $g1y2) ;	
	my ($g1ymin) = min ($g1y1, $g1y2) ;	

	my ($g2xmax) = max ($g2x1, $g2x2) ;
	my ($g2xmin) = min ($g2x1, $g2x2) ;	
	my ($g2ymax) = max ($g2y1, $g2y2) ;	
	my ($g2ymin) = min ($g2y1, $g2y2) ;	

	if 	(($sx >= $g1xmin) and
		($sx >= $g2xmin) and
		($sx <= $g1xmax) and
		($sx <= $g2xmax) and
		($sy >= $g1ymin) and
		($sy >= $g2ymin) and
		($sy <= $g1ymax) and
		($sy <= $g2ymax)) {
		return ($sx, $sy) ;
	}
	else {
		return (0, 0) ;
	}
} 


sub areaSize {
	my $ref = shift ; # nodes
	my @nodes = @$ref ;

	my ($lonRef, $latRef) = mwFile::getNodePointers() ;

	my @poly = () ;
	foreach my $node ( @nodes ) {
		my ($x, $y) = mwMap::convert ($$lonRef{$node}, $$latRef{$node}) ;
		push @poly, [$x, $y] ;
	}
	my ($p) = Math::Polygon->new(@poly) ;
	my $size = $p->area ;

	return $size ;
}

1 ;


