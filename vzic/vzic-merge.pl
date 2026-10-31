#!/usr/bin/perl -w
use Getopt::Long;
use Cwd            qw(abs_path);
use File::Basename qw( fileparse );
use File::Path     qw( make_path );
use File::Spec;

#
# Vzic - a program to convert Olson timezone database files into VZTIMEZONE
# files compatible with the iCalendar specification (RFC2445).
#
# SPDX-FileCopyrightText: 2000-2001 Ximian, Inc.
# SPDX-FileCopyrightText: 2003, Damon Chaplin <damon@ximian.com>
#
# SPDX-License-Identifier: GPL-2.0-or-later
#
# Author: Damon Chaplin <damon@gnome.org>
#

#
# This merges in a new set of VTIMEZONE files with the 'master' set. It only
# updates the files in the master set if the VTIMEZONE component has really
# been changes. Note that the TZID normally includes the date the VTIMEZONE
# file was generated on, so we have to ignore this when comparing the files.
#

# Set these to the toplevel directories of the 2 sets of VTIMEZONE files.
$MASTER_ZONEINFO_DIR = $ENV{'VZIC_ZONEINFO_MASTER'};
$NEW_ZONEINFO_DIR    = $ENV{'VZIC_ZONEINFO_NEW'};

# Set this to 1 if you have version numbers in the TZID like libical.
$LIBICAL_VERSIONING = 1;

# Set this to 0 for dry-runs, and 1 to actually update.
$DO_UPDATES = 1;

sub usage
{
  say STDERR
"Usage: $0 --master-zoneinfo-dir=<path_to_master_zoneinfo> --new_zoneinfo_dir=<path_to_new_zone_info>";
  exit 1;
}

GetOptions(
  'master-zoneinfo-dir=s' => \$MASTER_ZONEINFO_DIR,
  'new-zoneinfo-dir=s'    => \$NEW_ZONEINFO_DIR,
) or die 'Invalid command-line arguments';

if (!defined($MASTER_ZONEINFO_DIR)) {
  say STDERR "You forgot to pass a master-zoneinfo-dir command line option\n";
  usage();
}
if (!defined($NEW_ZONEINFO_DIR)) {
  say STDERR "You forgot to pass a new-zoneinfo-dir command line option\n";
  usage();
}
$MASTER_ZONEINFO_DIR = abs_path($MASTER_ZONEINFO_DIR);
$NEW_ZONEINFO_DIR    = abs_path($NEW_ZONEINFO_DIR);

# Save this so we can restore it later.
$input_record_separator = $/;

chdir $NEW_ZONEINFO_DIR
  || die "Can't cd to $NEW_ZONEINFO_DIR";

foreach $new_file (`find -name "*.ics"`) {

  # Get rid of './' at start and whitespace at end.
  $new_file =~ s/^\.\///;
  $new_file =~ s/\s+$//;

  #    print "File: $new_file\n";

  open(NEWZONEFILE, "$new_file")
    || die "Can't open file: $NEW_ZONEINFO_DIR/$new_file";
  undef $/;
  $new_contents = <NEWZONEFILE>;
  $/            = $input_record_separator;
  close(NEWZONEFILE);

  $master_file = $MASTER_ZONEINFO_DIR . "/$new_file";

  #    print "Master File: $master_file\n";

  $copy_to_master = 0;

  if (-l $new_file) {

    # This is a symlink, copy it in any case.

    if (!-e $master_file) {
      print "Creating new symlinkg $new_file...\n";
      $copy_to_master = 1;
    } elsif (-l $master_file) {
      my $src = readlink($master_file);
      my $dst = readlink($new_file);

      if ($src eq $dst) {
        $copy_to_master = 0;

        # print "Symlink $new_file unchanged: $src.\n"
      } else {
        print "Updating symlink $new_file to $src...\n";
        $copy_to_master = 1;
      }

    } else {
      print "Changing $new_file to symlink...\n";
      $copy_to_master = 1;
    }

  } elsif (-l $master_file) {
    $copy_to_master = 1;
    print "Changing $new_file from symlink to real file...\n";

  } elsif (-e $master_file) {

    # If the ics file exists in the master copy we have to compare them,
    # otherwise we can just copy the new file into the master directory.
    open(MASTERZONEFILE, "$master_file")
      || die "Can't open file: $master_file";
    undef $/;
    $master_contents = <MASTERZONEFILE>;
    $/               = $input_record_separator;
    close(MASTERZONEFILE);

    $new_contents_copy = $new_contents;

    # Strip the TZID from both contents.
    $new_contents_copy =~ s/^TZID:\S+\r?$//m;
    $new_tzid = $&;
    $master_contents =~ s/^TZID:\S+\r?$//m;
    $master_tzid = $&;

    # Strip LAST-MODIFIED
    $new_contents_copy =~ s/^LAST-MODIFIED:\S+\r?$//m;
    $master_contents   =~ s/^LAST-MODIFIED:\S+\r?$//m;

    # print "Matched: $master_tzid\n";

    if ($new_contents_copy ne $master_contents) {
      print "$new_file has changed. Updating...\n";
      $copy_to_master = 1;

      if ($LIBICAL_VERSIONING) {

        # We bump the version number in the new file.
        $master_tzid =~ m%_(\d+)/%;
        $version_num = $1;

        #		print "Version: $version_num\n";

        $version_num++;
        $new_tzid =~ s%_(\d+)/%_$version_num/%;

        #		print "New TZID: $new_tzid\n";
        $new_contents =~ s/^TZID:\S+$/$new_tzid/m;
      }
    }

  } else {
    print "$new_file doesn't exist in master directory. Copying...\n";
    $copy_to_master = 1;
  }

  if ($copy_to_master) {
    if ($DO_UPDATES) {

      my ($filename, $directories) = fileparse $master_file;
      if (!-d $directories) {
        make_path $directories or die "Failed to create path: $directories";
      }

      if (-e $master_file) {

        # delete old file so we don't have issues with symlinks
        unlink($master_file);
      }

      if (-l $new_file) {
        my $dst = readlink($new_file) || die "Can't read symlink $new_file.";

        print "Creating symlink to $dst in $master_file.\n";
        symlink $dst, $master_file || die "Can't create symlink $new_file.";
      } else {
        open(MASTERZONEFILE, ">$master_file")
          || die "Can't create file: $master_file";
        print MASTERZONEFILE $new_contents;
        close(MASTERZONEFILE);
      }
    }
  }

}

chdir $MASTER_ZONEINFO_DIR
  || die "Can't cd to $MASTER_ZONEINFO_DIR";

foreach $old_file (`find -name "*.ics"`) {

  # Get rid of './' at start and whitespace at end.
  $old_file =~ s/^\.\///;
  $old_file =~ s/\s+$//;

  $new_file = $NEW_ZONEINFO_DIR . "/$old_file";

  # If the ics file exists in the master copy we have to compare them,
  # otherwise we can just copy the new file into the master directory.
  if (!-e $new_file) {
    print "$old_file exists in the master but not the new zoneinfo. Deleting...\n";
    unlink($old_file);
  }
}
