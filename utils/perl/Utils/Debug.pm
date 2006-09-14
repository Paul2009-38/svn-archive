##################################################################
package Utils::Debug;
##################################################################

use Exporter;
@ISA = qw( Exporter );
use vars qw(@ISA @EXPORT @EXPORT_OK %EXPORT_TAGS $VERSION);
@EXPORT = qw( $DEBUG $VERBOSE
	      mem_usage
	      time_estimate
	      print_time
	      );

our $DEBUG   = 0;
our $VERBOSE = 0;

use strict;
use warnings;


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

# get memory usage from /proc Filesystem
sub mem_usage(;$){
    my $type = shift||'';
    my $proc_file = "/proc/$$/statm";
    my $msg = '';
    if ( -r $proc_file ) {
	my $statm = `cat $proc_file`;
	chomp $statm;
	my @statm = split(/\s+/,$statm);
	my $vsz = ($statm[0]*4)/1024;
	my $rss = ($statm[1]*4)/1024;
	#      printf STDERR " PID: $$ ";
	return $rss if $type eq "rss";
	return $vsz if $type eq "vsz";
	$msg .= sprintf( "VSZ: %.0f MB ",$vsz);
	$msg .= sprintf( "RSS: %.0f MB",$rss);
    }
    return $msg;
}


# returns a time estimation for the rest of the process
sub time_estimate($$$){
    my $start_time = shift; # Time the process was started
    my $elem_no    = shift; # The number of the current element
    my $elem_max   = shift; # the maximum number of possible elements

    my $time_diff=time()-$start_time;
    my $time_estimated= $time_diff*$elem_no/$elem_max;
    my $msg = sprintf( " %.0f(%.0f) minutes",$time_diff/60,$time_estimated/60);
    return $msg;
}

1;
