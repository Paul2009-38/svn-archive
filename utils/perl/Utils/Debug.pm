##################################################################
package Utils::Debug;
##################################################################

use Exporter;
@ISA = qw( Exporter );
use vars qw(@ISA @EXPORT @EXPORT_OK %EXPORT_TAGS $VERSION);
@EXPORT = qw( $DEBUG $VERBOSE
	      mem_info
	      mem_usage
	      print_time
	      time_estimate
	      );

our $DEBUG   = 0;
our $VERBOSE = 0;

use strict;
use warnings;

use IO::File;
use Utils::Math;


# print the time elapsed since starting
# starting_time is the first argument
sub print_time($){
    my $start_time = shift;
    my $time_diff = time()-$start_time;
    if ( $time_diff > 1 ) {
	printf STDERR " in %.0f sec", $time_diff;
    }
    printf STDERR "\n";
}

my $mem_statistics={};

# get memory usage from /proc Filesystem
sub mem_usage(;$){
    my $type = shift||'';
    my $proc_file = "/proc/$$/statm";
    my $msg = '';
    if ( -r $proc_file ) {
	my $fh = IO::File->new("<$proc_file");
	my $statm = $fh->getline()
	    or return "";
	$fh->close();
	chomp $statm;
	my @statm = split(/\s+/,$statm);
	return unless @statm;
	my $vsz = ($statm[0]*4)/1024;
	my $rss = ($statm[1]*4)/1024;
	#      printf STDERR " PID: $$ ";
	$mem_statistics->{"max vsz"} = max($mem_statistics->{"max vsz"},$vsz) if $vsz;
	$mem_statistics->{"max rss"} = max($mem_statistics->{"max rss"},$rss) if $rss;
	return $rss if $type eq "rss";
	return $vsz if $type eq "vsz";
	return $mem_statistics->{"max rss"} if $type eq "max rss";
	return $mem_statistics->{"max vsz"} if $type eq "max vsz";
	$msg .= sprintf( "MEM: %.0f(%.0f) MB ",$vsz,mem_info("MemTotal"));
	#$msg .= sprintf( "RSS: %.0f MB ",$rss);
	#$msg .= mem_info();
    }
    return $msg;
}


# get memory usage from /proc Filesystem
sub mem_info(;$){
    my $type = shift||'';
    my $proc_file = "/proc/meminfo";
    my $msg = '';
    if ( -r $proc_file ) {
	my $fh = IO::File->new("<$proc_file");
	my $mem={};
	while ( my $line = $fh->getline() ) {
	    my ($k,$v)=split(/\:\s*/,$line);
	    $v =~ s/ kB//;
	    $mem->{$k}=$v/1024;
	}
	$fh->close();
	if ( $type ) {
	    return $mem->{$type};
	} else {
	    $msg .= "Mem ";
	    $msg .= sprintf( "free: %.0f MB ",$mem->{MemFree});
	    $msg .= sprintf( "total: %.0f MB ",$mem->{MemTotal});
	}
    }
    return $msg;
}


# returns a time estimation for the rest of the process
sub time_estimate($$$){
    my $start_time = shift; # Time the process was started
    my $elem_no    = shift||1; # The number of the current element
    my $elem_max   = shift; # the maximum number of possible elements

    my $time_diff=time()-$start_time;
    my $time_estimated= $time_diff/$elem_no*$elem_max;
    my $unit="min";
    my $factor=60;
    my $digits=0;
    if ( $time_estimated >7200 ) {
	$unit="h";
	$factor=60*60;
	$digits=2;
    }
    if ( $time_estimated <4*60 ) {
	$unit="sec";
	$factor=1;
	$digits=0;
    }
    my $msg = sprintf( " %.${digits}f(%.${digits}f)%s",
		       $time_diff/$factor,$time_estimated/$factor,$unit);
    if ( $DEBUG >2 || $VERBOSE>4 ) {
	$msg  .= " since start: $time_diff sec".
	    sprintf(" (estimate: %.2f sec)",$time_estimated).
	    " element $elem_no($elem_max) ";
    }
    if ( $DEBUG >2 || $VERBOSE>2 ) {
	$msg .= sprintf(" %.2f%% ",100*$elem_no/$elem_max);
    }
    if ( $DEBUG >2 || $VERBOSE>2 ) {
	$msg .= sprintf(" %d(%d) ",$elem_no,$elem_max);
    }
    return $msg;
}

1;
