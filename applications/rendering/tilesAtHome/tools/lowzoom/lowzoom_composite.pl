#!/usr/bin/perl
use strict;
use LWP::Simple;
use Image::Magick;
use GD qw(:DEFAULT :cmp);
#------------------------------------------------------------------------------------
# LowZoom.pl
# Generates low-zoom map tiles, by downloading high-zoom map tiles, and merging them
# together. 
#
# Part of the OpenStreetMap tiles@home project
#
# Copyright 2007, Oliver White.
# Copying license: GNU general public license, v2 or later
#-----------------------------------------------------------------------------------

$|=1;

## nicked from tahconfig.pm from main t@h. 
## FIXME: use the actual module instead.
my %Config;
open(my $fp,"<lowzoom.conf") || die("Can't open \"lowzoom.conf\" ($!)\n");
while(my $Line = <$fp>){
    $Line =~ s/#.*$//; # Comments
    $Line =~ s/\s*$//; # Trailing whitespace
    if($Line =~ m{
        ^
        \s*
        ([A-Za-z0-9._-]+) # Keyword: just one single word no spaces
        \s*            # Optional whitespace
        =              # Equals
        \s*            # Optional whitespace
        (.*)           # Value
        }x){

# Store config options in a hash array
        $Config{$1} = $2;
        print "Found $1 ($2)\n" if(0); # debug option
    }
}
close $fp;

# Option: Where to move tiles, so that they get uploaded by another program
my $uploadDir = $Config{UploadDir};

die "can't find upload directory \"$uploadDir\"" unless (-d $uploadDir);

# Command-line arguments
my $X = shift();
my $Y = shift();
my $Z = shift();
my $MaxZ = shift() || 8;
my $OutputLayer = shift() || "tile";
my $BaseLayer = shift() || "tile";
my $CaptionLayer = shift();
my $Options = shift();

# Check the command-line arguments, and display usage information
my $Usage = "Usage: lowzoom.pl x y z baseZoom outputLayer [baseLayer] [captionLayer] [keep]\n  x,y,z - the tile at the top of the tileset to be generated\n  baseZoom - the zoom level to download tiles from\n  outputLayer - which layer to produce lowzoom tiles for (tile (default) or maplint)\n  baseLayer - which layer to use as a base layer (tile (default) or base) \n  captionLayer - layer to composite over base layer (none (default) or captions)\n  'keep' - don't move tiles to an upload area afterwards\n\nOther options (URLs, upload staging area) are part of the script - change them in source code\n\nNote: For zoom level 8-12 use lowzoom.pl x y z 12 tile captionless caption\n      For zoom level 0-7 use lowzoom.pl x y z 8 tile tile\n";
if(($MaxZ > 12)
  || ($MaxZ <= $Z)
  || ($Z < 0) || (!defined($Z))
  || ($MaxZ > 17)
  || ($X < 0) || (!defined($X))
  || ($Y < 0) || (!defined($Y))
  || ($X >= 2 ** $Z)
  || ($Y >= 2 ** $Z)
  ){
  die($Usage);
}

# Timestamp to assign to generated tiles
my $Timestamp = time();

# What we intend to do
my $Status = new status; 
$Status->area($BaseLayer,$X,$Y,$Z,$MaxZ);

#open oceantiles.dat and leave it open if UseOceantilesDat is on
my $oceantiles;
if ($Config{UseOceantilesDat}) {
    open($oceantiles, "<", "../../oceantiles_12.dat") or die("../../oceantiles-z12.dat not found");
}

#If UseLatestTxt, store info from latest.txt here
my %notBlanks ;
if ($Config{UseLatestTxt}) {
    parseLatestTxt();
}

#open a file for the suspicious tiles
my $suspiciousTiles;
if ($Config{WriteSuspicious}){
    open($suspiciousTiles, ">>", "suspicious_tiles.txt");
}

#create a BlackTile to detect tiles generated by a buggy inkscape
my $BlackTileImage = new GD::Image(256,256,1);
my $BlackTileBackground = $BlackTileImage->colorAllocate(0,0,0);
$BlackTileImage->fill(127,127,$BlackTileBackground);

# Create the requested tile
lowZoom($X,$Y,$Z, $MaxZ, $Status);

# Move all low-zoom tiles to upload directory
moveTiles(tempdir(), $uploadDir, $MaxZ) if($Options ne "keep");

# Status message, saying what we did
$Status->final();

# Recursively create (including any downloads necessary) a tile
sub lowZoom {
  my ($X,$Y,$Z,$MaxZ,$Status) = @_;
  
  # Get tiles
  if($Z >= $MaxZ){
        downloadtile($X,$Y,$Z,$BaseLayer);
  }
  else{
    # Recursively get/create the 4 subtiles
    lowZoom($X*2,$Y*2,$Z+1,$MaxZ, $Status);
    lowZoom($X*2+1,$Y*2,$Z+1,$MaxZ, $Status);
    lowZoom($X*2,$Y*2+1,$Z+1,$MaxZ, $Status);
    lowZoom($X*2+1,$Y*2+1,$Z+1,$MaxZ, $Status);
  
    # Create the tile from those subtiles
    supertile($X,$Y,$Z,$OutputLayer,$BaseLayer,$CaptionLayer);
  }
}
# Download a tile from the tileserver
sub downloadtile {
  my ($X,$Y,$Z,$BaseLayer) = @_;
    my $f2 = localfile($X,$Y,$Z,$BaseLayer);
    my $key = sprintf("%s,%s,%s",$X,$Y,$Z);

    if(($Z < 13) || (($Z == 12) && ($notBlanks{$key} > 0))){ ## FIXME: this check doesn't make sense
        
        my $f1 = remotefile($X,$Y,$Z,$BaseLayer);

        mirror($f1,$f2);

        my $Size = -s $f2;
        
        my $Image = newFromPng GD::Image($f2, 1);
        if (not($Image->compare($BlackTileImage) & GD_CMP_IMAGE)) {  ## FIXME: this only makes sense if z=12
            unlink $f2;
            if (askOceantiles($X,$Y) eq "land" ) {
                link("land.png", $f2);
            } else {
                link("./sea.png", $f2);
            }
            if ($Config{WriteSuspicious}) {
                print $suspiciousTiles "$X $Y : downloaded tile is a black tile, probably a inkscape bug \n";
            }
            $Status->downloadCount($BaseLayer,$X,$Y,$Z,$Size);

            return;
        }

        if ($Config{UseOceantilesDat}) {
            if ($Z eq 12) {
                if (($Size == 103) && (askOceantiles($X,$Y) eq "land")) {
                    if ($Config{WriteSuspicious}) {
                        print $suspiciousTiles "$X $Y : downloaded tile is sea, oceantiles says land \n";
                    }
                    if ($Config{BlankSource} eq "oceantiles") {
                        unlink $f2;
                        link("../../emptyland.png", $f2);
                    }
                }elsif (($Size == 179) && (askOceantiles($X,$Y) eq "sea")) {
                    if ($Config{WriteSuspicious}) {
                        print $suspiciousTiles "$X $Y : downloaded tile is land, oceantiles says sea \n";
                    }
                    if ($Config{BlankSource} eq "oceantiles") {
                        unlink $f2;
                        link("../../emptysea.png", $f2);
                    }
                }
            }
        }
        $Status->downloadCount($BaseLayer,$X,$Y,$Z,$Size);
        
        return;
    }  
  
    unlink $f2;

    if (askOceantiles($X,$Y) eq "land" ) {
            link("land.png", $f2);
    }else {
            link("./sea.png", $f2);
    }

    my $Size = 0;
  $Status->downloadCount($BaseLayer,$X,$Y,$Z,$Size);
    
}

# Delete blank subtiles of a blank tile
# When we notice that a tile is blank because all its subtiles are blank,
# because of the fallback mechanism in the server we can delete those
# subtiles. However, to avoid the fallback on the server having to work too
# hard, we ensure that there are still real blank tiles every few zoom
# levels.
sub deleteBlankSubtiles
{
  my($X,$Y,$Z,$OutputLayer) = @_;
    if (($Z == 7 ) && ($Options ne "keep")){
        for my $x (0,1)
        {
            for my $y (0,1)
            {
                my $f = localfile(2*$X + $x, 2*$Y + $y, $Z+1, $OutputLayer);
                unlink $f;
            }
        }
    };
  return if $Z <= 9;  # Not lowzoom's problem
  # This keeps real blank tiles at zooms 3,6 and 9
  return if ($Z+1)%3 == 0;
  
  for my $x (0,1)
  {
    for my $y (0,1)
    {
      my $f = localfile(2*$X + $x, 2*$Y + $y, $Z+1, $OutputLayer);
      # Unlink prior to creating, file may be a hard link
      unlink $f;
      open my $fh, ">", $f;  # Make zero byte file, the marker for the server to delete the tile
    }
  }
}
# Create a supertile, by merging together 4 local image files, and creating a new local file
sub supertile {
  my ($X,$Y,$Z,$OutputLayer,$BaseLayer,$CaptionLayer) = @_;
  my $CaptionFile;

  # Load captions
  if ($CaptionLayer ne undef) {
    my $f2 = localfile($X,$Y,$Z,$CaptionLayer);
    my $f1 = remotefile($X,$Y,$Z,$CaptionLayer);
    mirror($f1,$f2);
    $CaptionFile = readLocalImage($X,$Y,$Z,$CaptionLayer);
  }
  
  # Load the subimages
  my $AA = readLocalImage($X*2,$Y*2,$Z+1,$BaseLayer);
  my $BA = readLocalImage($X*2+1,$Y*2,$Z+1,$BaseLayer);
  my $AB = readLocalImage($X*2,$Y*2+1,$Z+1,$BaseLayer);
  my $BB = readLocalImage($X*2+1,$Y*2+1,$Z+1,$BaseLayer);
  

    # BaseFile is a file containing a tile without captions.
    # OutputFile is a file that is a merge of a base tile and a captions tile if there is one.
    my $BaseFile = localfile($X,$Y,$Z,$BaseLayer);
    my $OutputFile = localfile($X,$Y,$Z,$OutputLayer);

    # Always delete files first. The use of hardlinks means we might accedently overwrite other files.
    unlink($BaseFile);
    unlink($OutputFile);

#    print "generating $OutputFile \n";

    if ($AA == undef) { $AA = Image::Magick->new; }
    if ($AB == undef) { $AB = Image::Magick->new; }
    if ($BA == undef) { $BA = Image::Magick->new; }
    if ($BB == undef) { $BB = Image::Magick->new; }

    # all images the same size? 
    if(($AA->Get('filesize') == 103 )  && ($AA->Get('filesize') == $BA->Get('filesize')) && ($BA->Get('filesize') == $AB->Get('filesize')) && ( $AB->Get('filesize') == $BB->Get('filesize')) ) 
    {#if its a "404 sea" or a "sea.png" and all 4 sizes are the same, make one 69 bytes sea of it
            my $SeaFilename = "../../emptysea.png"; 
            link($SeaFilename,$BaseFile);
            deleteBlankSubtiles($X,$Y,$Z,$OutputLayer);
            return;
    }
    elsif(($AA->Get('filesize') == 179 ) && ($AA->Get('filesize') == $BA->Get('filesize')) && ($BA->Get('filesize') == $AB->Get('filesize')) && ( $AB->Get('filesize') == $BB->Get('filesize')) ) 
    {#if its a "blank land" or a "land.png" and all 4 sizes are the same, make one 69 bytes land of it
            my $LandFilename = "../../emptyland.png"; 
            link($LandFilename,$BaseFile);
            deleteBlankSubtiles($X,$Y,$Z,$OutputLayer);
            return;
    }
    else{
        my $Image = Image::Magick->new;

        # Create the supertile
        $Image->Set(size=>'512x512');
        $Image->ReadImage('xc:white');

        # Copy the subimages into the 4 quadrants
        foreach my $x (0, 1)
        {
                foreach my $y (0, 1)
                {
                        next unless (($Z < 9) || (($x == 0) && ($y == 0)));
                        $Image->Composite(image => $AA, 
                                        geometry => sprintf("512x512+%d+%d", $x, $y),
                                        compose => "darken") if ($AA);

                        $Image->Composite(image => $BA, 
                                        geometry => sprintf("512x512+%d+%d", $x + 256, $y),
                                        compose => "darken") if ($BA);

                        $Image->Composite(image => $AB, 
                                        geometry => sprintf("512x512+%d+%d", $x, $y + 256),
                                        compose => "darken") if ($AB);

                        $Image->Composite(image => $BB, 
                                        geometry => sprintf("512x512+%d+%d", $x + 256, $y + 256),
                                        compose => "darken") if ($BB);
                }
        }


        $Image->Scale(width => "256", height => "256");
        $Image->Set(type=>"Palette");
        $Image->Set(quality => 90);  # compress image
        $Image->Write($BaseFile);
        utime $Timestamp, $Timestamp, $BaseFile;

        # Overlay the captions onto the tiled image and then write it
      	$Image->Composite(image => $CaptionFile);
      	$Image->Write($OutputFile);
        utime $Timestamp, $Timestamp, $OutputFile;

        undef $Image; ## Destroy the ImageMagick object to save Memory
    }

     #remove tiles which will not be uploaded
    if (($Z == 11 ) && ($Options ne "keep")){
        for my $x (0,1)
        {
            for my $y (0,1)
            {
                my $f = localfile(2*$X + $x, 2*$Y + $y, $Z+1, $OutputLayer);
                unlink $f;
            }
        }
    };

}

# Open a PNG file, and return it as a Magick image (or 0 if not found)
sub readLocalImage
{
    my ($X,$Y,$Z,$Layer) = @_;
    my $Filename; 
    $Filename = localfile($X,$Y,$Z,$Layer); 
    if (!-f $Filename)
    {
        return undef;
    }
    my $Image = new Image::Magick;
    if (my $err = $Image->Read($Filename))
    {
        print STDERR "$err\n";
        return undef;
    }
        if ($Image->Get('filesize') == 69) 
        {
            # do not return 1x1 pixel images since we might have to put them into a lower zoom
            @$Image=();
            if (my $err = $Image->Read("sea.png"))
            {
                    print STDERR "$err\n";
                    return undef;
            }
        }
        if ($Image->Get('filesize') == 67) 
        {
            # do not return 1x1 pixel images since we might have to put them into a lower zoom
            @$Image=();
            if (my $err = $Image->Read("land.png"))
            {
                    print "$err\n";
                    return undef;
            }
        }
    return($Image);
}

# Take any tiles that were created (as opposed to downloaded), and move them to
# an area ready for upload.
# + Delete any tiles that were downloaded
sub moveTiles {
  my ($from, $to, $MaxZ) = @_;
  opendir(my $dp, $from) || die($!);
  while(my $file = readdir($dp)){
    if(($file =~ /^${OutputLayer}_(\d+)_(\d+)_(\d+)\.png$/o) || ($file =~ /^${BaseLayer}_(\d+)_(\d+)_(\d+)\.png$/o)){
      my ($Z,$X,$Y) = ($1,$2,$3);
      my $f1 = "$from/$file";
      my $f2 = "$to/$file";
      if($Z < $MaxZ){
        # Rename can fail if the target is on a different filesystem
        rename($f1, $f2) or system("mv",$f1,$f2);
      }
      else{
        unlink $f1;
      }
    }
  }  
  close $dp;
}

# takes x and y coordinates and returns if the corresponding tile 
# should be sea or land 
sub askOceantiles {

    my ($X, $Y) = @_;

    my $tileoffset = ($Y * (2**12)) + $X;

    if ($Config{UseOceantilesDat}) {
        seek $oceantiles, int($tileoffset / 4), 0;  
        my $buffer;
        read $oceantiles, $buffer, 1;
        $buffer = substr( $buffer."\0", 0, 1 );
        $buffer = unpack "B*", $buffer;
        my $str = substr( $buffer, 2*($tileoffset % 4), 2 );

#        print("lookup handler finds: $str\n") ;
        if ($str eq "10") {    return "sea"; };
        if ($str eq "01") {     return "land"; };
        if ($str eq "11") {    return "land"; };

        return "unknown";

        # $str eq "00" => unknown (not yet checked)
        # $str eq "01" => known land
        # $str eq "10" => known sea
        # $str eq "11" => known edge tile
    }
    else
    {
        print "need UseOceantilesDat config option to proceed for tile $X $Y 12\n";
        exit (1);
    }
}

#parses the latest.txt file and stores info about tiles
sub parseLatestTxt {

 open (fh,"<","latest_12.txt");

printf("searching for zoom %d tiles where X is between %d and %d \n",$MaxZ,$X*2**($MaxZ-$Z),($X+1)*2**($MaxZ-$Z));
printf("searching for zoom %d tiles where Y is between %d and %d \n",$MaxZ,$Y*2**($MaxZ-$Z),($Y+1)*2**($MaxZ-$Z));
my $notBlankCount = 0;
my $newCount = 0;
my @tileinfo;
my $key;
while (my $line = <fh>){
    if ( $line =~ /^[0-9]*\,[0-9]*,[0-9]*,/ ) {
        @tileinfo = split(/,/,$line);
        if ($Z <= $tileinfo[2] && $tileinfo[2] <= $MaxZ) {
            if ($X*2**($MaxZ-$Z) <= $tileinfo[0] && $tileinfo[0] <= ($X+1)*2**($MaxZ-$Z)) {
                if ($Y*2**($MaxZ-$Z) <= $tileinfo[1] && $tileinfo[1] <= ($Y+1)*2**($MaxZ-$Z)) {
                    $key = sprintf("%s,%s,%s",$tileinfo[0],$tileinfo[1],$tileinfo[2]);
                    if ($tileinfo[5] > (time()-3600*1)) {
                        $newCount++;
                        $notBlanks{$key} = 2;
                    }else{
                        $notBlanks{$key} = 1;
                    }
                    $notBlankCount++;
                }
            }
        }
    }
}

close(fh);

printf("latest.txt parsed, found %i notBlanks (%i new)\n",$notBlankCount,$newCount);

}

# Option: filename for our temporary map tiles
# (note: this should match whatever is expected by the upload scripts)
sub localfile {
  my ($X,$Y,$Z,$Layer) = @_;
  return sprintf("%s/%s_%d_%d_%d.png", tempdir(), $Layer,$Z,$X,$Y);
}
# Option: URL for downloading tiles
sub remotefile {
  my ($X,$Y,$Z,$Layer) = @_;
  return sprintf("http://tah.openstreetmap.org/Tiles/%s.php/%d/%d/%d.png", $Layer,$Z,$X,$Y);
}
# Option: what to use as temporary storage for tiles
sub tempdir {
  return("temp");
}

package status;
use Time::HiRes qw(time); # Comment-this out if you want, it's not important
sub new {
  my $self  = {};
  $self->{DONE} = 0;
  $self->{TODO} = 1;
  $self->{SIZE} = 0;
  bless($self);
  return $self;
}
sub downloadCount(){
  my $self = shift();
  $self->{LAYER} = shift();
  $self->{LAST_X} = shift();
  $self->{LAST_Y} = shift();
  $self->{LAST_Z} = shift();
  $self->{LAST_SIZE} = shift();
  $self->{DONE}++;
  $self->{SIZE} += $self->{LAST_SIZE};
  $self->{PERCENT} = $self->{TODO} ? (100 * ($self->{DONE} / $self->{TODO})) : 0;
  $self->display();
}
sub area(){
  my $self = shift();
  $self->{LAYER}=shift();
  $self->{X} = shift();
  $self->{Y} = shift();
  $self->{Z} = shift();
  $self->{MAX_Z} = shift();
  $self->{RANGE_Z} = $self->{MAX_Z} - $self->{Z};
  $self->{TODO} = 4 ** $self->{RANGE_Z};
  $self->display();
  $self->{START_T} = time();
}
sub update(){
  my $self = shift();
  $self->{T} = time();
  $self->{DT} = $self->{T} - $self->{START_T};
  $self->{EXPECT_T} = $self->{DONE} ? ($self->{TODO} * $self->{DT} / $self->{DONE}) : 0;
  $self->{EXPECT_FINISH} = $self->{START_T} + $self->{EXPECT_T};
  $self->{REMAIN_T} = $self->{EXPECT_T} - $self->{EXPECT_DT};
}
sub display(){
  my $self = shift();
  $self->update();
  
  printf( "\rJob %s(%d,%d,%d): %03.1f%% done, %1.1f min (%d,%d,%d = %1.1f KB)", 
    $self->{LAYER},
    $self->{X},
    $self->{Y},
    $self->{Z},
    $self->{PERCENT}, 
    $self->{REMAIN_T} / 60,
    $self->{LAST_X},
    $self->{LAST_Y},
    $self->{LAST_Z},
    $self->{LAST_SIZE}/1024
    );
}
sub final(){
  my $self = shift();
  $self->{END_T} = time();
  printf("Done, %d downloads, %1.1fKB total, took %1.0f seconds\n",
    $self->{DONE},
    $self->{SIZE} / 1024,
    $self->{DT});
}


