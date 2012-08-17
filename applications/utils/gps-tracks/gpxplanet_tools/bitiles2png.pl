#!/usr/bin/perl

# Create tiles out of processed bitiles.
# Written by Ilya Zverev, licensed WTFPL.

use strict;
use GD;
use Bit::Vector;
use File::Path 2.07 qw(make_path);
use File::Basename;
use Getopt::Long;
use Fcntl qw( O_RDONLY O_RDWR O_CREAT O_BINARY );

my $source_dir;
my $dest_dir;
my $delete_original;
my $zoom;
my $colour_str = '0,0,0';
my $uptozoom;
my $make_empty_tiles;
my $help;
my $verbose;

GetOptions('h|help' => \$help,
           'v|verbose' => \$verbose,
           'i|input=s' => \$source_dir,
           'o|output=s' => \$dest_dir,
           'z|zoom=i' => \$zoom,
           'u|zoom0=i' => \$uptozoom,
           'd|delorig' => \$delete_original,
           'e|empty' => \$make_empty_tiles,
           'c|colour=s' => \$colour_str,
           ) || usage();

if( $help ) {
    usage();
}

usage("Please specify input directory with -i") unless defined($source_dir);
die "Source directory not found" unless -d $source_dir;
usage("Please specify source zoom level with -z") unless defined($zoom);

$dest_dir = $source_dir unless defined($dest_dir);
$uptozoom = $zoom unless defined($uptozoom) && $uptozoom <= $zoom;

my @colour = split(",", $colour_str);
die "Colour should be three comma-separated numbers: r,g,b" unless $#colour == 2;

my $xmin;
my $ymin;
my $xmax;
my $ymax;

process_zoom($zoom);

if( $uptozoom < $zoom && $xmin <= $xmax && $ymin <= $ymax ) {
    for my $zoomd (1 .. $zoom-$uptozoom) {
        my $newzoom = $zoom - $zoomd;
        generate_lowzoom($newzoom);
        clean_bitiles($newzoom + 1) if $delete_original || $newzoom + 1 < $zoom;
        process_zoom($newzoom);
    }
}
clean_bitiles($uptozoom) if $delete_original || $uptozoom < $zoom;

sub generate_lowzoom {
    my $zoom = shift;
    my $z = $zoom + 1;

    print STDERR "Generating zoom $zoom bitiles" if $verbose;
    for( my $x = $xmin - $xmin%2; $x <= $xmax; $x += 2 ) {
        print STDERR '.' if $verbose;
        make_path("$source_dir/$zoom/".($x/2));
        for( my $y = $ymin - $ymin%2; $y <= $ymax; $y += 2 ) {
            quadtile(
                "$source_dir/$z/$x/$y.bitile",
                "$source_dir/$z/".($x+1)."/$y.bitile",
                "$source_dir/$z/$x/".($y+1).".bitile",
                "$source_dir/$z/".($x+1)."/".($y+1).".bitile",
                "$source_dir/$zoom/".($x/2)."/".($y/2).".bitile"
            );
        }
    }
    print STDERR "Done\n" if $verbose;
}

sub quadtile {
    # (x,y) (x+1,y) (x,y+1) (x+1,y+1) (x/2,y/2)
    my ($b1file, $b2file, $b3file, $b4file, $result) = @_;
    return if !-r $b1file && !-r $b2file && !-r $b3file && !-r $b4file;

    my @btiles = (read_bit_vector($b1file), read_bit_vector($b2file),
        read_bit_vector($b3file), read_bit_vector($b4file));
    my $res = Bit::Vector->new(65536);

    for my $y (0..255) {
        for my $x (0..255) {
            my $b = ($y >> 7) * 2 + ($x >> 7);
            my $bx = ($x * 2) % 256;
            my $by = ($y * 2) % 256;
            $res->Bit_On($y * 256 + $x )
                if $btiles[$b]->bit_test($by * 256 + $bx)
                || $btiles[$b]->bit_test($by * 256 + $bx + 1)
                || $btiles[$b]->bit_test(($by + 1) * 256 + $bx)
                || $btiles[$b]->bit_test(($by + 1) * 256 + $bx + 1);
        }
    }

    sysopen(BITILE, $result, O_RDWR | O_CREAT | O_BINARY);
    syswrite(BITILE, $res->Block_Read());
    close BITILE;
}

sub process_zoom {
    my $zoom = shift;
    print STDERR "Generating PNG images for zoom $zoom" if $verbose;

    $xmin = 10**6;
    $ymin = 10**6;
    $xmax = -10**6;
    $ymax = -10**6;

    my $dh;
    if( !opendir($dh, "$source_dir/$zoom") ) {
        print STDERR "Fail: cannot open $source_dir/$zoom\n";
        return;
    }
    my @xlist = grep { /^\d+$/ && -d "$source_dir/$zoom/$_" } readdir($dh);
    closedir($dh);
    if( $#xlist < 0 ) {
        print STDERR "No tiles there\n";
        return;
    }

    for my $x (@xlist) {
        my $folder = "$source_dir/$zoom/$x";
        opendir($dh, $folder) || next;
        my @ylist = grep { /^\d+\.bitile$/ && -r "$folder/$_" } readdir($dh);
        closedir($dh);

        print STDERR "." if $verbose;
        make_path("$dest_dir/$zoom/$x");

        for my $y (@ylist) {
            $y =~ /^(\d+)/;
            my $yt = $1;

            my $vec = read_bit_vector("$folder/$y");
            my $tile = vector2png($vec);
            open PIC, ">$dest_dir/$zoom/$x/$yt.png";
            binmode PIC;
            print PIC $tile->png();
            close PIC;

            $xmin = $x if $x < $xmin;
            $xmax = $x if $x > $xmax;
            $ymin = $yt if $yt < $ymin;
            $ymax = $yt if $yt > $ymax;
        }
    }
    if( $make_empty_tiles ) {
        print STDERR "Empty tiles..." if $verbose;
        generate_empty_tiles($zoom);
    }
    print STDERR "Done\n" if $verbose;
}

sub read_bit_vector {
    my $filename = shift;
    my $vec = Bit::Vector->new(65536);
    if( sysopen(BITILE, $filename, O_RDONLY | O_BINARY) ) {
        if( sysread(BITILE, my $read, 8192) == 8192 ) {
            $vec->Block_Store($read);
        }
        close BITILE;
    }
    return $vec;
}

sub vector2png {
    my $vec = shift;
    my $tile = GD::Image->new(256, 256);
    my $transp = $tile->colorAllocate(253,254,255);
    $tile->transparent($transp);
    $tile->filledRectangle(0,0,255,255,$transp);
    my $col = $tile->colorAllocate($colour[0], $colour[1], $colour[2]);

    for my $yy (0..255) {
        for my $xx (0..255) {
            if( $vec->bit_test($yy * 256 + $xx) ) {
                $tile->setPixel($xx, $yy, $col);
            }
        }
    }
    return $tile;
}

sub generate_empty_tiles {
    my $z = shift;

    my $img = GD::Image->new(256, 256);
    my $white = $img->colorAllocate(255, 255, 255);
    $img->transparent($white);
    $img->filledRectangle(0,0,255,255,$white);
    my $empty_tile = $img->png();

    for my $x ($xmin .. $xmax) {
        for my $y ($ymin .. $ymax) {
            my $filename = "$dest_dir/$z/$x/$y.png";
            if( !-f $filename ) {
                open PIC, ">$filename";
                binmode PIC;
                print PIC $empty_tile;
                close PIC;
            }
        }
    }
}

sub clean_bitiles {
    my $zoom = shift;

    print STDERR "Removing zoom $zoom bitiles\n" if $verbose;
    opendir(my $dh, "$source_dir/$zoom") || return;
    my @xlist = grep { /^\d+$/ && -d "$source_dir/$zoom/$_" } readdir($dh);
    closedir($dh);
    return if $#xlist < 0;

    for my $x (@xlist) {
        my $folder = "$source_dir/$zoom/$x";
        opendir($dh, $folder) || next;
        my @ylist = grep { /^\d+\.bitile$/ && -r "$folder/$_" } readdir($dh);
        closedir($dh);

        for my $y (@ylist) {
            $y =~ /^(\d+)/;
            my $yt = $1;
            unlink "$folder/$y" if -f "$dest_dir/$zoom/$x/$yt.png";
        }
        rmdir $folder;
    }
    rmdir "$source_dir/$zoom";
}

sub usage {
    my ($msg) = @_;
    print STDERR "$msg\n\n" if defined($msg);

    my $prog = basename($0);
    print STDERR << "EOF";
This script traverses input directory and creates PNG tiles from all
bitiles found (created with gpx2bitiles.pl).

usage: $prog [-h] [-v] -i source [-o target] -z zoom [-u zoom] [-d] [-e] [-c colour]

 -h        : print ths help message and exit.
 -i source : directory with bitiles.
 -o target : directory to store PNG tiles (by default equal to source).
 -z zoom   : bitiles zoom level.
 -u zoom   : generate tiles up to that zoom level (e.g. 0).
 -d        : delete processed bitiles.
 -e        : generate empty tiles where there's no bitiles.
 -c colour : colour of GPS points (three comma-separated numbers: r,g,b
             default is black).
 -v        : display progress.

EOF
    exit;
}