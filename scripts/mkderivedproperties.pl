#!/usr/bin/env perl
################################################################################
# (C) COPYRIGHT 2000, Eric Busboom <eric@softwarestudio.org>
#     http://www.softwarestudio.org
#
# This library is free software; you can redistribute it and/or modify
# it under the terms of either:
#
#   The LGPL as published by the Free Software Foundation, version
#   2.1, available at: http://www.gnu.org/licenses/lgpl-2.1.txt
#
# Or:
#
#   The Mozilla Public License Version 2.0. You may obtain a copy of
#   the License at http://www.mozilla.org/MPL/
################################################################################
require "readvaluesfile.pl";

use Getopt::Std;
getopts('chspmi:');

# ARG 0 is properties.csv
%propmap = read_properties_file($ARGV[0]);

# ARG 1 is value-types.txt
%valuemap = read_values_file($ARGV[1]);

$include_vanew = 1;

# Write the file inline by copying everything before a demarcation
# line, and putting the generated data after the demarcation

if ($opt_i) {

  open(IN, $opt_i) || die "Can't open input file $opt_i";

  while (<IN>) {

    if (/<insert_code_here>/) {
      insert_code();
    } else {
      print;
    }

  }

}

sub fudge_data
{
  my $prop = shift;

  my $value = $propmap{$prop}->{'lic_value'};

  if (!$value) {
    die "Can't find value for property \"$prop\"\n";
  }
  my $ucf = join("", map {ucfirst(lc($_));} split(/-/, $prop));
  my $lc  = lc($ucf);
  my $uc  = uc($lc);

  my $ucfvalue = join("", map {ucfirst(lc($_));} split(/-/, $value));
  my $lcvalue  = lc($ucfvalue);
  my $ucvalue  = uc($lcvalue);

  my $type = $valuemap{$value}->{C}->[1];
  $type =~ s/char\*/char \*/;

  my @comp_types = @{$valuemap{$value}->{'components'}};
  return ($uc, $lc, $lcvalue, $ucvalue, $type, @comp_types);

}

sub insert_code
{

  # Create the property map data
  if ($opt_c) {

    my @props = sort {$propmap{$a}->{"kindEnum"} <=> $propmap{$b}->{"kindEnum"}} keys %propmap;
    my $count = scalar(@props);

    print "static const struct icalproperty_map property_map[$count] = {\n";

    foreach $prop (@props) {

      next if !$prop;

      next if $prop eq 'NO' or $prop eq 'ANY';

      my ($uc, $lc, $lcvalue, $ucvalue, $type, @comp_types) = fudge_data($prop);
      my $defvalue = $propmap{$prop}->{'default_value'};
      $defvalue =~ s/-//g;

      my @flags = @{$propmap{$prop}->{'flags'}};

      print "    { ICAL_${uc}_PROPERTY, \"$prop\",\n";
      print "      ICAL_${ucvalue}_VALUE, ICAL_${defvalue}_VALUE,\n";
      print "      { ";

      if (@comp_types) {
          foreach $comp (@comp_types) {
              $comp =~ s/-//g;
              print "ICAL_${comp}_VALUE, ";
          }
      } elsif ($defvalue ne "NO") {
          print "ICAL_${defvalue}_VALUE, ";
      } else {
          print "ICAL_${ucvalue}_VALUE, ";
      }

      print "ICAL_NO_VALUE }, ";

      if (@flags) {
          my $sep = "\n      ";
          foreach $flag (@flags) {
              $flag =~ s/-//g;
              $flag  = uc($flag);
              print "${sep}ICAL_PROPERTY_${flag}";
              $sep = " | ";
          }
      } else {
          print "0";
      }

      print " },\n";
    }

    $prop = "NO";

    my ($uc, $lc, $lcvalue, $ucvalue, $type) = fudge_data($prop);

    print "    { ICAL_${uc}_PROPERTY, \"\",";
    print "      ICAL_NO_VALUE, ICAL_NO_VALUE,\n";
    print "      { ICAL_NO_VALUE }, 0 }\n}";
    print ";\n\n";

    $count    = 1;
    $bigcount = 0;
    my %lines;

    foreach $value (sort keys %valuemap) {

      next if !$value;
      next if $value eq 'NO' or $prop eq 'ANY';

      my $ucv = join("", map {uc(lc($_));} split(/-/, $value));
      my @enums = @{$valuemap{$value}->{'enums'}};

      if (@enums) {

        my ($c_autogen, $c_type) = @{$valuemap{$value}->{'C'}};

        foreach $e (@enums) {

          $e =~ /([a-zA-Z0-9\-]+)=?([0-9]+)?/;
          $e = $1;
          if ($2) {
            $idx = $2;
          } else {
            $idx++;
          }

          my $uce = join("", map {uc(lc($_));} split(/-/, $e));

          if ($e ne "X" and $e ne "NONE") {
            $str = $e;
          } else {
            $str = "";
          }
          if ($e eq "NONE") {
            $bigcount += 100;
          }

          # Create empty "future" properties so the hash math works.
          if ($e eq "NONE") {
            my ($tbd) = 1;
            $saveidx++;
            for (; $saveidx < $idx; $saveidx++, $tbd++) {
              $lines{$saveidx} =
                "    {ICAL_${ucv}_PROPERTY,ICAL_${ucv}_NONE, \"\" }, /*$saveidx*/\n";
            }
          }

          # Place each property into a hash based on the index specified in value-types.csv
          # The lines are printed so they're in the same order as the indices
          $lines{$idx} = "    {ICAL_${ucv}_PROPERTY,ICAL_${ucv}_${uce}, \"$str\" }, /*$idx*/\n";
          $saveidx = $idx;
          $count++;
        }
      }
    }

    $bigcount++;

    print "static const struct icalproperty_enum_map enum_map[$bigcount] = {\n";
    foreach $line (sort keys %lines) {
      print $lines{$line};
    }
    print "    {ICAL_NO_PROPERTY, 0, \"\"}\n};\n\n";

  }

  if ($opt_h) {

    # Create the property enumerations list
    my $enumConst = $propmap{'ANY'}->{"kindEnum"};
    print "typedef enum icalproperty_kind {\n    ICAL_ANY_PROPERTY = " . $enumConst . ",\n";
    foreach $prop (sort keys %propmap) {

      next if !$prop;

      next if $prop eq 'NO' or $prop eq 'ANY';

      my ($uc, $lc, $lcvalue, $ucvalue, $type) = fudge_data($prop);

      $enumConst = $propmap{$prop}->{"kindEnum"};

      print "    ICAL_${uc}_PROPERTY = " . $enumConst . ",\n";

    }
    $enumConst = $propmap{'NO'}->{"kindEnum"};
    print "    ICAL_NO_PROPERTY = " . $enumConst . "\n} icalproperty_kind;\n";

  }

  foreach $prop (sort keys %propmap) {

    next if !$prop;

    next if $prop eq 'NO' or $prop eq 'ANY';

    my ($uc, $lc, $lcvalue, $ucvalue, $type) = fudge_data($prop);

    my $pointer_check;
    if ($type =~ /\*/) {
      $pointer_check = "    icalerror_check_arg_rz((v != 0), \"v\");\n" if $type =~ /\*/;
    } elsif ($type eq "void") {
      $pointer_check = "    icalerror_check_arg_rv((v != 0), \"v\");\n" if $type =~ /\*/;

    }

    my $set_pointer_check = "\n    icalerror_check_arg_rv((v != 0), \"v\");" if $type =~ /\*/;

    if ($opt_c) {    # Generate C source

      if ($include_vanew) {
        print <<EOM;
icalproperty *icalproperty_vanew_${lc}($type v, ...)
{
    va_list args;
    struct icalproperty_impl *impl;
$pointer_check
    impl = icalproperty_new_impl(ICAL_${uc}_PROPERTY);
    icalproperty_set_${lc}((icalproperty*)impl, v);
    va_start(args, v);
    icalproperty_add_parameters(impl, args);
    va_end(args);
    return (icalproperty*)impl;
}
EOM
      }
      print <<EOM;

/* $prop */
icalproperty *icalproperty_new_${lc}($type v)
{
    struct icalproperty_impl *impl;
$pointer_check
    impl = icalproperty_new_impl(ICAL_${uc}_PROPERTY);
    icalproperty_set_${lc}((icalproperty*)impl, v);
    return (icalproperty*)impl;
}

void icalproperty_set_${lc}(icalproperty *prop, $type v)
{$set_pointer_check
    icalerror_check_arg_rv((prop != 0), "prop");
    icalproperty_set_value(prop, icalvalue_new_${lcvalue}(v));
}

EOM

      # Dirk Theisen pointed out, exdate needs to match TZID parameters in EXDATE
      if ($lc eq "exdate") {
        print <<EOM;
$type icalproperty_get_${lc}(const icalproperty *prop)
{
    icalerror_check_arg((prop != 0), "prop");
    return icalproperty_get_datetime_with_component((icalproperty *)prop, NULL);
}

EOM
      } else {
        print <<EOM;
$type icalproperty_get_${lc}(const icalproperty *prop)
{
    icalerror_check_arg((prop != 0), "prop");
    return icalvalue_get_${lcvalue}(icalproperty_get_value(prop));
}

EOM
      }
    } elsif ($opt_h) {    # Generate C Header file

      print "\
/* $prop */\
LIBICAL_ICAL_EXPORT icalproperty *icalproperty_new_${lc}($type v);\
LIBICAL_ICAL_EXPORT void icalproperty_set_${lc}(icalproperty *prop, $type v);\
LIBICAL_ICAL_EXPORT $type icalproperty_get_${lc}(const icalproperty *prop);";

      if ($include_vanew) {
        print "\nLIBICAL_ICAL_EXPORT icalproperty *icalproperty_vanew_${lc}($type v, ...);\n";
      }

    }

  }    # This brace terminates the main loop

  if ($opt_h) {

    print "\n#endif /*ICALPROPERTY_H*/\n";
  }

}
