#!/usr/bin/env perl
################################################################################
# SPDX-FileCopyrightText: 2000, Eric Busboom <eric@civicknowledge.com>
# SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
################################################################################
require "readvaluesfile.pl";

use Getopt::Std;
getopts('chvi:');

#Options
# c -> generate c code file
# h -> generate header file
# v -> generate VCARD values
# i -> .c/.h "in" file (template)

my $ucprefix = "ICAL";
if ($opt_v) {
    $ucprefix = "VCARD"
}
my $lcprefix = lc($ucprefix);

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

    print "static const struct ${lcprefix}property_map property_map[$count] = {\n";

    foreach $prop (@props) {

      next if !$prop;

      next if $prop eq 'NO' or $prop eq 'ANY';

      my ($uc, $lc, $lcvalue, $ucvalue, $type, @comp_types) = fudge_data($prop);
      my $defvalue = $propmap{$prop}->{'default_value'};
      $defvalue =~ s/-//g;

      my @flags = @{$propmap{$prop}->{'flags'}};

      print "    { ${ucprefix}_${uc}_PROPERTY, \"$prop\",\n";
      print "      ${ucprefix}_${ucvalue}_VALUE, ${ucprefix}_${defvalue}_VALUE,\n";
      print "      { ";

      if (@comp_types) {
          foreach $comp (@comp_types) {
              $comp =~ s/-//g;
              print "${ucprefix}_${comp}_VALUE, ";
          }
      } elsif ($defvalue ne "NO") {
          print "${ucprefix}_${defvalue}_VALUE, ";
      } else {
          print "${ucprefix}_${ucvalue}_VALUE, ";
      }

      print "${ucprefix}_NO_VALUE }, ";

      if (@flags) {
          my $sep = "\n      ";
          foreach $flag (@flags) {
              $flag =~ s/-//g;
              $flag  = uc($flag);
              print "${sep}${ucprefix}_PROPERTY_${flag}";
              $sep = " | ";
          }
      } else {
          print "0";
      }

      print " },\n";
    }

    $prop = "NO";

    my ($uc, $lc, $lcvalue, $ucvalue, $type) = fudge_data($prop);

    print "    { ${ucprefix}_${uc}_PROPERTY, \"\",";
    print "      ${ucprefix}_NO_VALUE, ${ucprefix}_NO_VALUE,\n";
    print "      { ${ucprefix}_NO_VALUE }, 0 }\n}";
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

          $e =~ /([a-zA-Z0-9\-\._]+)=?([0-9]+)?/;
          $e = $1;
          if ($2) {
            $idx = $2;
          } else {
            $idx++;
          }

          my $uce = join("", map {uc(lc($_));} split(/[\-\.]/, $e));

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
                "    {${ucprefix}_${ucv}_PROPERTY,${ucprefix}_${ucv}_NONE, \"\" }, /*$saveidx*/\n";
            }
          }

          # Place each property into a hash based on the index specified in value-types.csv
          # The lines are printed so they're in the same order as the indices
          $lines{$idx} = "    {${ucprefix}_${ucv}_PROPERTY,${ucprefix}_${ucv}_${uce}, \"$str\" }, /*$idx*/\n";
          $saveidx = $idx;
          $count++;
        }
      }
    }

    $bigcount++;

    print "static const struct ${lcprefix}property_enum_map enum_map[$bigcount] = {\n";
    foreach $line (sort keys %lines) {
      print $lines{$line};
    }
    print "    {${ucprefix}_NO_PROPERTY, 0, \"\"}\n};\n\n";

  }

  if ($opt_h) {

    # Create the property enumerations list
    my $enumConst = $propmap{'ANY'}->{"kindEnum"};
    print "typedef enum ${lcprefix}property_kind {\n    ${ucprefix}_ANY_PROPERTY = " . $enumConst . ",\n";
    foreach $prop (sort keys %propmap) {

      next if !$prop;

      next if $prop eq 'NO' or $prop eq 'ANY';

      my ($uc, $lc, $lcvalue, $ucvalue, $type) = fudge_data($prop);

      $enumConst = $propmap{$prop}->{"kindEnum"};

      print "    ${ucprefix}_${uc}_PROPERTY = " . $enumConst . ",\n";

    }
    $enumConst = $propmap{'NO'}->{"kindEnum"};
    print "    ${ucprefix}_NO_PROPERTY = " . $enumConst . "\n} ${lcprefix}property_kind;\n";

  }

  foreach $prop (sort keys %propmap) {

    next if !$prop;

    next if $prop eq 'NO' or $prop eq 'ANY'
        or $prop eq 'BEGIN' or $prop eq 'END';

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
${lcprefix}property *${lcprefix}property_vanew_${lc}($type v, ...)
{
    va_list args;
    struct ${lcprefix}property_impl *impl;
$pointer_check
    impl = ${lcprefix}property_new_impl(${ucprefix}_${uc}_PROPERTY);
    ${lcprefix}property_set_${lc}((${lcprefix}property*)impl, v);
    va_start(args, v);
    ${lcprefix}property_add_parameters(impl, args);
    va_end(args);
    return (${lcprefix}property*)impl;
}
EOM
      }
      print <<EOM;

/* $prop */
${lcprefix}property *${lcprefix}property_new_${lc}($type v)
{
    struct ${lcprefix}property_impl *impl;
$pointer_check
    impl = ${lcprefix}property_new_impl(${ucprefix}_${uc}_PROPERTY);
    ${lcprefix}property_set_${lc}((${lcprefix}property*)impl, v);
    return (${lcprefix}property*)impl;
}

void ${lcprefix}property_set_${lc}(${lcprefix}property *prop, $type v)
{$set_pointer_check
    icalerror_check_arg_rv((prop != 0), "prop");
    ${lcprefix}property_set_value(prop, ${lcprefix}value_new_${lcvalue}(v));
}

EOM

      # Dirk Theisen pointed out, exdate needs to match TZID parameters in EXDATE
      if ($lc eq "exdate") {
        print <<EOM;
$type ${lcprefix}property_get_${lc}(const ${lcprefix}property *prop)
{
    icalerror_check_arg((prop != 0), "prop");
    return ${lcprefix}property_get_datetime_with_component((${lcprefix}property *)prop, NULL);
}

EOM
      } else {
        print <<EOM;
$type ${lcprefix}property_get_${lc}(const ${lcprefix}property *prop)
{
    icalerror_check_arg((prop != 0), "prop");
    return ${lcprefix}value_get_${lcvalue}(${lcprefix}property_get_value(prop));
}

EOM
      }
    } elsif ($opt_h) {    # Generate C Header file

      print "\
/* $prop */\
LIBICAL_${ucprefix}_EXPORT ${lcprefix}property *${lcprefix}property_new_${lc}($type v);\
LIBICAL_${ucprefix}_EXPORT void ${lcprefix}property_set_${lc}(${lcprefix}property *prop, $type v);\
LIBICAL_${ucprefix}_EXPORT $type ${lcprefix}property_get_${lc}(const ${lcprefix}property *prop);";

      if ($include_vanew) {
        print "\nLIBICAL_${ucprefix}_EXPORT LIBICAL_SENTINEL ${lcprefix}property *${lcprefix}property_vanew_${lc}($type v, ...);\n";
      }

    }

  }    # This brace terminates the main loop

  if ($opt_h) {

    print "\n#endif /*${ucprefix}PROPERTY_H*/\n";
  }

}
