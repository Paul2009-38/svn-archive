# 
#
# checktouch.pl by gary68
#
# this program checks an osm file for crossing ways which don't share a common node at the intersection and are on the same layer
#
#
# Copyright (C) 2008, Gerhard Schwanz
#
# This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the 
# Free Software Foundation; either version 3 of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
# FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along with this program; if not, see <http://www.gnu.org/licenses/>
#
#
# example definition file:
# (IMPORTANT: don't enter a tag in both sections!)
#
#<XML>
#  <k="check" v="highway:motorway">
#  <k="check" v="highway:motorway_link">
#  <k="check" v="highway:trunk">
#  <k="check" v="highway:trunk_link">
#  <k="against" v="highway:primary">
#  <k="against" v="highway:primary_link">
#  <k="against" v="highway:secondary">
#  <k="against" v="highway:tertiary">
#  <k="against" v="junction:roundabout">
#</XML>
#
# Version 1.0 -002
# - layer grep changed
# Version 003
# - check layer values
# Version 1.1
# - stat
#
# Version 1.2
# - stat 2
#
# Version 1.3
# - ignore way starts and ends that are connected
#
# Version 1.4
# - faster parameters
# - iFrame
# 
# Version 2.0
# - quad trees
#
# Version 2.1
# - sort output by lon
# 

use strict ;
use warnings ;

use List::Util qw[min max] ;
use OSM::osm 5.0 ;
use OSM::QuadTree ;
use File::stat;
use Time::localtime;

my $program = "checktouch.pl" ;
my $usage = $program . " def.xml file.osm out.htm out.gpx" ;
my $version = "2.1" ;

my $maxDist = 0.002 ; # in km 
my $maxDist2 = 0.001 ; 

my $wayId ; my $wayId1 ; my $wayId2 ;
my $wayUser ; my @wayNodes ; my @wayTags ;
my $nodeId ; my $nodeId2 ;
my $nodeUser ; my $nodeLat ; my $nodeLon ; my @nodeTags ;
my $aRef1 ; my $aRef2 ;
my $wayCount = 0 ;
my $againstCount = 0 ;
my $checkWayCount = 0 ;
my $againstWayCount = 0 ;
my $invalidWays ;

my $qt ;

my @check ;
my @against ;
my @checkWays ;
my @againstWays ;

my $time0 = time() ; my $time1 ; my $timeA ;
my $i ;
my $key ;
my $num ;
my $tag1 ; my $tag2 ;
my $progress ;
my $potential ;
my $checksDone ;

my $html ;
my $def ;
my $gpx ;
my $osmName ;
my $htmlName ;
my $defName ;
my $gpxName ;

my %wayNodesHash ;
my @neededNodes ;
my %lon ; my %lat ;
my %xMax ; my %xMin ; my %yMax ; my %yMin ; 
my %wayCategory ;
my %wayHash ;
my %noExit ;
my %layer ;
my %wayCount ;	# number ways using this node

my $touches = 0 ;
my %touchingsHash ;

###############
# get parameter
###############
$defName = shift||'';
if (!$defName)
{
	die (print $usage, "\n");
}

$osmName = shift||'';
if (!$osmName)
{
	die (print $usage, "\n");
}

$htmlName = shift||'';
if (!$htmlName)
{
	die (print $usage, "\n");
}

$gpxName = shift||'';
if (!$gpxName)
{
	$gpxName = $htmlName ;
	$gpxName =~ s/htm/gpx/ ;
}

print "\n$program $version for file $osmName\n\n" ;


##################
# read definitions
##################

print "read definitions file $defName...\n" ;
open ($def, , "<", $defName) or die "definition file $defName not found" ;

while (my $line = <$def>) {
	#print "read line: ", $line, "\n" ;
	my ($k)   = ($line =~ /^\s*<k=[\'\"]([:\w\s\d]+)[\'\"]/); # get key
	my ($v) = ($line =~ /^.+v=[\'\"]([:\w\s\d]+)[\'\"]/);       # get value
	
	if ($k and defined ($v)) {
		#print "key: ", $k, "\n" ;
		#print "val: ", $v, "\n" ;

		if ($k eq "check") {
			push @check, $v ;
		}
		if ($k eq "against") {
			push @against, $v ;
		}
	}
}

close ($def) ;


print "Check ways: " ;
foreach (@check) { print $_, " " ;} print "\n" ;
print "Against: " ;
foreach (@against) { print $_, " " ;} print "\n\n" ;



######################
# skip all nodes first
######################
openOsmFile ($osmName) ;
print "pass1: skipping nodes...\n" ;
skipNodes () ;


#############################
# identify check/against ways
#############################
print "pass1: identify check ways...\n" ;
($wayId, $wayUser, $aRef1, $aRef2) = getWay () ;
if ($wayId != -1) {
	@wayNodes = @$aRef1 ;
	@wayTags = @$aRef2 ;
}
while ($wayId != -1) {	
	$wayCount++ ;
	if (scalar (@wayNodes) >= 2) {


		my $found = 0 ;
		my $layerTemp = "0" ;
		foreach $tag1 (@wayTags) {
			if (grep (/layer:/, $tag1)) { $layerTemp = $tag1 ; $layerTemp =~ s/layer:// ; }
			foreach $tag2 (@against) {
				if ($tag1 eq $tag2) { $found = 1 ; }
			}
		}

		my $correctLayer = 0 ;
		foreach (-5..5) { 
			if ($layerTemp eq $_) { $correctLayer = 1 ;} 
		}
		if ( ! $correctLayer ) {
			print "incorrect layer tag \"$layerTemp\" - will be set to 0.\n" ;
			$layerTemp = 0 ;
		}

		if ($found) {
			$againstWayCount++ ;
			push @againstWays, $wayId ;
			@{$wayNodesHash{$wayId}} = @wayNodes ;
			push @neededNodes, @wayNodes ;
			$layer{$wayId} = $layerTemp ;
			$wayCategory{$wayId} = 2 ;
			foreach my $node (@wayNodes) { $wayCount{$node}++ ; }
		}

		$found = 0 ;
		foreach $tag1 (@wayTags) {
			foreach $tag2 (@check) {
				if ($tag1 eq $tag2) { $found = 1 ; }
			}
		}
		if ($found)  { 
			push @checkWays, $wayId ; 
			$checkWayCount++ ;
			@{$wayNodesHash{$wayId}} = @wayNodes ;
			push @neededNodes, @wayNodes ;
			$layer{$wayId} = $layerTemp ;
			$wayCategory{$wayId} = 1 ;
			foreach my $node (@wayNodes) { $wayCount{$node}++ ; }
		}
	}
	else {
		#print "invalid way (one node only): ", $wayId, "\n" ;
		$invalidWays++ ;
	}

	# next way
	($wayId, $wayUser, $aRef1, $aRef2) = getWay () ;
	if ($wayId != -1) {
		@wayNodes = @$aRef1 ;
		@wayTags = @$aRef2 ;
	}
}

closeOsmFile () ;

print "number total ways: $wayCount\n" ;
print "number invalid ways (1 node only): $invalidWays\n" ;
print "number check ways: $checkWayCount\n" ;
print "number against ways: $againstWayCount\n" ;



######################
# get node information
######################
print "pass2: get node information...\n" ;
openOsmFile ($osmName) ;

my $minLon = 999 ;
my $maxLon = -999 ;
my $minLat = 999 ;
my $maxLat = -999 ;


@neededNodes = sort { $a <=> $b } @neededNodes ;

($nodeId, $nodeLon, $nodeLat, $nodeUser, $aRef1) = getNode () ;
if ($nodeId != -1) {
	@nodeTags = @$aRef1 ;
}

while ($nodeId != -1) {
	my $needed = 0 ;

	$needed = binSearch ($nodeId, \@neededNodes ) ;

	if ($needed >= 0) { 
		$lon{$nodeId} = $nodeLon ; 
		$lat{$nodeId} = $nodeLat ; 

		# noExit
		$noExit{$nodeId} = 0 ;
		foreach (@nodeTags) {
			if (grep /noexit:yes/, $_) {
				$noExit{$nodeId} = 1 ;
			} 
		}
	}

	if ($nodeLon > $maxLon) { $maxLon = $nodeLon ; }
	if ($nodeLon < $minLon) { $minLon = $nodeLon ; }
	if ($nodeLat > $maxLat) { $maxLat = $nodeLat ; }
	if ($nodeLat < $minLat) { $minLat = $nodeLat ; }

	# next
	($nodeId, $nodeLon, $nodeLat, $nodeUser, $aRef1) = getNode () ;
	if ($nodeId != -1) {
		@nodeTags = @$aRef1 ;
	}
}

closeOsmFile () ;

$qt = OSM::QuadTree->new (	-xmin => $minLon, 
				-xmax => $maxLon, 
				-ymin => $minLat, 
				-ymax => $maxLat, 
				-depth => 8) ;


##########################
# init areas for checkWays
##########################
print "init areas for checkways...\n" ;

###############
# init way hash
###############
foreach $wayId (@checkWays) {

	($xMin{$wayId}, $xMax{$wayId}, $yMin{$wayId}, $yMax{$wayId}) = getArea ( @{$wayNodesHash{$wayId}} );

	$qt->add ($wayId, $xMin{$wayId}, $yMin{$wayId}, $xMax{$wayId}, $yMax{$wayId}) ;
}


###############################
# check for nearly touches
###############################
print "check for nearly touching elements...\n" ;

$progress = 0 ;
$timeA = time() ;

push @againstWays, @checkWays ;
my $total = scalar (@againstWays) ;

$potential = $total * scalar (@checkWays) ;

foreach $wayId1 (@againstWays) {
	$progress++ ;
	if ( ($progress % 1000) == 0 ) {
		printProgress ($program, $osmName, $timeA, $total, $progress) ;
	}

	# create temp array according to hash
	my ($aXMin, $aXMax, $aYMin, $aYMax) = getArea ( @{$wayNodesHash{$wayId1}} );
	my $ref = $qt->getEnclosedObjects ($aXMin, $aYMin, $aXMax, $aYMax) ;
	my @temp = @$ref ;

	foreach $wayId2 (@temp) {
		# check for overlapping "way areas"
		if ( (checkOverlap ($aXMin, $aYMin, $aXMax, $aYMax, $xMin{$wayId2}, $yMin{$wayId2}, $xMax{$wayId2}, $yMax{$wayId2})) and ($layer{$wayId1} == $layer{$wayId2}) ) {
			if ( $wayId1 == $wayId2 ) {
				# don't do anything because same way!
			}
			else {
				$checksDone++ ;
				for ($b=0; $b<$#{$wayNodesHash{$wayId2}}; $b++) {
					# check start id1
					if ( ($noExit{$wayNodesHash{$wayId1}[0]} == 0) and ($wayCount{$wayNodesHash{$wayId1}[0]} == 1 ) ) {
						if ( ($wayNodesHash{$wayId1}[0] != $wayNodesHash{$wayId2}[$b]) and ($wayNodesHash{$wayId1}[0] != $wayNodesHash{$wayId2}[$b+1]) ) {
							my ($d1) = shortestDistance ($lon{$wayNodesHash{$wayId2}[$b]},
											$lat{$wayNodesHash{$wayId2}[$b]},
											$lon{$wayNodesHash{$wayId2}[$b+1]},
											$lat{$wayNodesHash{$wayId2}[$b+1]},
											$lon{$wayNodesHash{$wayId1}[0]}, 
											$lat{$wayNodesHash{$wayId1}[0]} ) ;
							if ($d1 < $maxDist) {
								$touches++ ;
								@{$touchingsHash{$touches}} = ($lon{$wayNodesHash{$wayId1}[0]}, $lat{$wayNodesHash{$wayId1}[0]}, $wayId1, $wayId2, $d1) ;
							}
						}
					}

					# check end id1	
					if ( ($noExit{$wayNodesHash{$wayId1}[-1]} == 0) and ($wayCount{$wayNodesHash{$wayId1}[-1]} == 1 ) ) {
						if ( ($wayNodesHash{$wayId1}[-1] != $wayNodesHash{$wayId2}[$b]) and ($wayNodesHash{$wayId1}[-1] != $wayNodesHash{$wayId2}[$b+1]) ) {
							my ($d1) = shortestDistance ($lon{$wayNodesHash{$wayId2}[$b]},
											$lat{$wayNodesHash{$wayId2}[$b]},
											$lon{$wayNodesHash{$wayId2}[$b+1]},
											$lat{$wayNodesHash{$wayId2}[$b+1]},
											$lon{$wayNodesHash{$wayId1}[-1]}, 
											$lat{$wayNodesHash{$wayId1}[-1]} ) ;
							if ($d1 < $maxDist) {
								$touches++ ;
								@{$touchingsHash{$touches}} = ($lon{$wayNodesHash{$wayId1}[-1]}, $lat{$wayNodesHash{$wayId1}[-1]}, $wayId1, $wayId2, $d1) ;
							}
						}
					}
				} # for
			} # categories
		} # overlap
	} 
}

print "potential checks: $potential\n" ;
print "checks actually done: $checksDone\n" ;
my $percent = $checksDone / $potential * 100 ;
printf "work: %2.3f percent\n", $percent ;
print "touches found: $touches\n" ;

$time1 = time () ;


##################
# PRINT HTML INFOS
##################
print "\nwrite HTML tables and GPX file...\n" ;

open ($html, ">", $htmlName) || die ("Can't open html output file") ;
open ($gpx, ">", $gpxName) || die ("Can't open gpx output file") ;


printHTMLiFrameHeader ($html, "Touch Check by Gary68") ;
printGPXHeader ($gpx) ;

print $html "<H1>Touch Check by Gary68</H1>\n" ;
print $html "<p>Version ", $version, "</p>\n" ;
print $html "<H2>Statistics</H2>\n" ;
print $html "<p>", stringFileInfo ($osmName), "<br>\n" ;
print $html "number ways total: $wayCount<br>\n" ;
print $html "number invalid ways (1 node only): $invalidWays<br>\n" ;
print $html "number check ways: $checkWayCount<br>\n" ;
print $html "number against ways: $againstWayCount</p>\n" ;

print $html "<p>Check ways: " ;
foreach (@check) { print $html $_, " " ;} print $html "</p>\n" ;
print $html "<p>Against: " ;
foreach (@against) { print $html $_, " " ;} print $html "</p>\n" ;


print $html "<H2>Touches found</H2>\n" ;
print $html "<p>At the given location a node of one way nearly hits another way. Potentially there is a connection missing." ;
print $html "<table border=\"1\">\n";
print $html "<tr>\n" ;
print $html "<th>Line</th>\n" ;
print $html "<th>WayId1</th>\n" ;
print $html "<th>WayId2</th>\n" ;
print $html "<th>Distance</th>\n" ;
print $html "<th>OSM</th>\n" ;
print $html "<th>OSB</th>\n" ;
print $html "<th>JOSM</th>\n" ;
print $html "<th>Pic</th>\n" ;
print $html "</tr>\n" ;
$i = 0 ;

my @sorted = () ;
foreach $key (keys %touchingsHash) {
	my ($x, $y, $id1, $id2, $dist) = @{$touchingsHash{$key}} ;
	push @sorted, [$key, $x] ;
}

@sorted = sort { $a->[1] <=> $b->[1]} @sorted ;

foreach my $s (@sorted) {

	my $key ;	
	$key = $s->[0] ;

	my ($x, $y, $id1, $id2, $dist) = @{$touchingsHash{$key}} ;
	#print "HTML $x, $y, $id1, $id2\n" ;
	$i++ ;
	$dist = $dist * 1000 ; # in meters

	# HTML
	print $html "<tr>\n" ;
	print $html "<td>", $i , "</td>\n" ;
	print $html "<td>", historyLink ("way", $id1) , "</td>\n" ;
	print $html "<td>", historyLink ("way", $id2) , "</td>\n" ;
	if ($dist < ($maxDist2*1000)) {
		printf $html "<td><strong>~ %2.1f m</strong></td>\n", $dist ;
	}
	else {
		printf $html "<td>~ %2.1f m</td>\n", $dist ;
	}
	print $html "<td>", osmLink ($x, $y, 16) , "</td>\n" ;
	print $html "<td>", osbLink ($x, $y, 16) , "</td>\n" ;
	print $html "<td>", josmLinkSelectWays ($x, $y, 0.005, $id1, $id2), "</td>\n" ;
	print $html "<td>", picLinkOsmarender ($x, $y, 16), "</td>\n" ;
	print $html "</tr>\n" ;

	# GPX
	my $text = "ChkTouch - " . $id1 . "/" . $id2 . " one way nearly hits another" ;
	printGPXWaypoint ($gpx, $x, $y, $text) ;
}
print $html "</table>\n" ;
print $html "<p>$i lines total</p>\n" ;



########
# FINISH
########
print $html "<p>", stringTimeSpent ($time1-$time0), "</p>\n" ;
printHTMLFoot ($html) ;
printGPXFoot ($gpx) ;

close ($html) ;
close ($gpx) ;

statistics ( ctime(stat($osmName)->mtime),  $program,  $defName, $osmName,  $checkWayCount,  $i) ;

print "\n$program finished after ", stringTimeSpent ($time1-$time0), "\n\n" ;


sub statistics {
	my ($date, $program, $def, $area, $total, $errors) = @_ ;
	my $statfile ; my ($statfileName) = "statistics.csv" ;

	if (grep /\.bz2/, $area) { $area =~ s/\.bz2// ; }
	if (grep /\.osm/, $area) { $area =~ s/\.osm// ; }
	my ($area2) = ($area =~ /.+\/([\w\-]+)$/ ) ;

	if (grep /\.xml/, $def) { $def =~ s/\.xml// ; }
	my ($def2) = ($def =~ /([\w\d\_]+)$/ ) ;

	my ($success) = open ($statfile, "<", $statfileName) ;

	if ($success) {
		print "statfile found. writing stats...\n" ;
		close $statfile ;
		open $statfile, ">>", $statfileName ;
		printf $statfile "%02d.%02d.%4d;", localtime->mday(), localtime->mon()+1, localtime->year() + 1900 ;
		printf $statfile "%02d/%02d/%4d;", localtime->mon()+1, localtime->mday(), localtime->year() + 1900 ;
		print $statfile $date, ";" ;
		print $statfile $program, ";" ;
		print $statfile $def2, ";" ;
		print $statfile $area2, ";" ;
		print $statfile $total, ";" ;
		print $statfile $errors ;
		print $statfile "\n" ;
		close $statfile ;
	}
	return ;
}
sub getArea {
	my @nodes = @_ ;

	my $minLon = 999 ;
	my $maxLon = -999 ;
	my $minLat = 999 ;
	my $maxLat = -999 ;


	foreach my $node (@nodes) {
		if ($lon{$node} > $maxLon) { $maxLon = $lon{$node} ; }
		if ($lon{$node} < $minLon) { $minLon = $lon{$node} ; }
		if ($lat{$node} > $maxLat) { $maxLat = $lat{$node} ; }
		if ($lat{$node} < $minLat) { $minLat = $lat{$node} ; }
	}	
	return ($minLon, $maxLon, $minLat, $maxLat) ;
}

