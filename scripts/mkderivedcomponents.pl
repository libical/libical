#!/usr/bin/env perl
################################################################################
# SPDX-FileCopyrightText: 2000, Eric Busboom <eric@civicknowledge.com>
# SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
################################################################################

use Getopt::Std;
getopts('chspi:');

# ARG 0 is components.txt
open(PV, "$ARGV[0]") || die "Can't open components  file $ARGV[0]:$!";

my @components;

while (<PV>) {

  s/#.*//;

  chop;

  push(@components, $_);

}

close PV;

# Write the file inline by copying everything before a demarcation
# line, and putting the generated data after the demarcation

if ($opt_i) {

  open(IN, $opt_i) || die "Can't open input file \"$opt_i\"";

  while (<IN>) {

    if (/Do not edit/) {
      last;
    }

    print;

  }

  if ($opt_i) {
    print "# Everything below this line is machine generated. Do not edit. \n";
  } else {
    print "/* Everything below this line is machine generated. Do not edit. */\n";
  }

}

if ($opt_c or $opt_h and !$opt_i) {

  print <<EOM;
/* -*- Mode: C -*-
  ======================================================================
  FILE: icalderivedproperties.{c,h}
  CREATOR: eric 09 May 1999

  \044Id:\044

  SPDX-FileCopyrightText: 1999 Eric Busboom <eric@civicknowledge.com>
  SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
 ======================================================================*/

/*
 * THIS FILE IS MACHINE GENERATED DO NOT EDIT
 */

#include <stdarg.h> /* for va_arg functions */

EOM

}

if ($opt_p and !$opt_i) {

  print <<EOM;

EOM

}

foreach $comp (@components) {

  next if !$comp;

  my $ucf = join("", map {ucfirst(lc($_));} split(/-/, $comp));
  my $lc  = lc($ucf);
  my $uc  = uc($lc);

  if ($opt_c) {    # Make C source
    print <<EOM;

/* $comp */

icalcomponent* icalcomponent_new_${lc}()
{
   return (icalcomponent*)icalcomponent_new_impl(ICAL_${uc}_COMPONENT);
}

icalcomponent* icalcomponent_vanew_${lc}(...)
{
   va_list args;
   struct icalcomponent_impl *impl = icalcomponent_new_impl(ICAL_${uc}_component);

   va_start(args,v);
   icalcomponent_add_properties(impl, args);
   va_end(args);

   return (icalcomponent*)impl;
}

EOM

  } elsif ($opt_h) {    # Make a C header
    print <<EOM;

/* $comp */
icalcomponent* icalcomponent_new_${lc}();
icalcomponent* icalcomponent_vanew_${lc}(...);
EOM

  } elsif ($opt_s) {    # Make something for a switch statement

    print <<EOM;
case ICAL_${uc}_PROPERTY:
EOM

  } elsif ($opt_p) {    # make perl source

    print <<EOM;

# $comp
package Net::ICal::Component::${ucf};
\@ISA=qw(Net::ICal::Component);

sub new
{
   my \$package = shift;
   my \$c = Net::ICal::icalcomponent_new(\$Net::ICal::ICAL_${uc}_COMPONENT);

   my \$self = Net::ICal::Component::new_from_ref(\$c);
   Net::ICal::Component::_add_elements(\$self,\\\@_);

   # Self is blessed in new_from_ref

   return \$self;

}
EOM

  }

}
