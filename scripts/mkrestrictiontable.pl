#!/usr/bin/env perl
################################################################################
# SPDX-FileCopyrightText: 2000, Eric Busboom <eric@civicknowledge.com>
#
# SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
#
#
#
################################################################################

use Getopt::Std;
getopts('i:');

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

    my ($method, $targetcomp, $prop, $subcomp, $restr, $sub) = split(/,/, $_);

    next if !$method;

    if (!$sub) {
      $sub = "NULL";
    } else {
      $sub = "icalrestriction_" . $sub;
    }
    $restr =~ s/\s+$//;

    if ($prop eq "NONE") {
      $prop = "NO";
    }
    if ($subcomp eq "NONE") {
      $subcomp = "NO";
    }

    my @value = ($restr, $sub);
    $prop_restr{$method}{$targetcomp}{$prop}{$subcomp} = \@value;
  }

  # Build the restriction table
  print "static const icalrestriction_record icalrestriction_records[] = {\n";

  for $method ( sort keys %prop_restr ) {
    for $targetcomp ( sort keys %{ $prop_restr{$method} } ) {
      for $prop ( sort keys %{ $prop_restr{$method}{$targetcomp} } ) {
        for $subcomp ( sort keys %{ $prop_restr{$method}{$targetcomp}{$prop} } ) {
          my ($restr, $sub) = @{$prop_restr{$method}{$targetcomp}{$prop}{$subcomp}};
          print(
"    \{ICAL_METHOD_${method}, ICAL_${targetcomp}_COMPONENT, ICAL_${prop}_PROPERTY, ICAL_${subcomp}_COMPONENT, ICAL_RESTRICTION_${restr}, $sub},\n"
          );
        }
      }
    }
}

  # Print the terminating line
  print
    "    {ICAL_METHOD_NONE, ICAL_NO_COMPONENT, ICAL_NO_PROPERTY, ICAL_NO_COMPONENT, ICAL_RESTRICTION_NONE, NULL}\n";

  print "};\n";

}
