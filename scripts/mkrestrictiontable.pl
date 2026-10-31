#!/usr/bin/env perl
################################################################################
# SPDX-FileCopyrightText: 2000, Eric Busboom <eric@civicknowledge.com>
# SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
################################################################################

use Getopt::Std;
getopts('vi:');

#Options
# v -> generate VCARD restrictions
# i -> .c "in" file (template)

my $ucprefix = "ICAL";
my $uckey    = "METHOD";
if ($opt_v) {
  $ucprefix = "VCARD";
  $uckey    = "VERSION";
}
my $lcprefix = lc($ucprefix);

# the argument should be the path to the restriction datafile, usually
# design-data/restrictions.csv
open(F, "$ARGV[0]") || die "Can't open restriction data file $ARGV[0]:$!";

# Write the file inline by copying everything before a demarcation
# line, and putting the generated data after the demarcation

if ($opt_i) {

  open(IN, $opt_i) || die "Can't open input file $opt_i";

  while (<IN>) {

    if (/<insert_code_here>/) {
      insert_code();
    }

    if (/Do not edit/) {
      last;
    }

    print;

  }

  close IN;
}

sub insert_code
{
  # First parse the restrictions file and make a hash of the restrictions
  my @prop_restr = ();
  my @comp_restr = ();

  while (<F>) {

    chop;

    s/\#.*$//;

    # $key is either iCalendar METHOD or vCard VERSION
    my ($key, $targetcomp, $prop, $subcomp, $restr, $sub) = split(/,/, $_);

    next if !$key;

    if (!$sub) {
      $sub = "NULL";
    } else {
      $sub = "${lcprefix}restriction_" . $sub;
    }
    $restr =~ s/\s+$//;

    if ($prop eq "NONE") {
      $prop = "NO";
    }
    if ($subcomp eq "NONE") {
      $subcomp = "NO";
    }

    my @value = ($restr, $sub);
    $prop_restr{$key}{$targetcomp}{$prop}{$subcomp} = \@value;
  }

  # Build the restriction table
  print "static const ${lcprefix}restriction_record ${lcprefix}restriction_records[] = {\n";

  for $key (sort keys %prop_restr) {
    for $targetcomp (sort keys %{$prop_restr{$key}}) {
      for $prop (sort keys %{$prop_restr{$key}{$targetcomp}}) {
        for $subcomp (sort keys %{$prop_restr{$key}{$targetcomp}{$prop}}) {
          my ($restr, $sub) = @{$prop_restr{$key}{$targetcomp}{$prop}{$subcomp}};
          print(
"    \{${ucprefix}_${uckey}_${key}, ${ucprefix}_${targetcomp}_COMPONENT, ${ucprefix}_${prop}_PROPERTY, ${ucprefix}_${subcomp}_COMPONENT, ${ucprefix}_RESTRICTION_${restr}, $sub},\n"
          );
        }
      }
    }
  }

  # Print the terminating line
  print
"    {${ucprefix}_${uckey}_NONE, ${ucprefix}_NO_COMPONENT, ${ucprefix}_NO_PROPERTY, ${ucprefix}_NO_COMPONENT, ${ucprefix}_RESTRICTION_NONE, NULL}\n";

  print "};\n";

}
