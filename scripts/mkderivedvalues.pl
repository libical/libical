#!/usr/bin/perl
################################################################################
# SPDX-FileCopyrightText: 2000, Eric Busboom <eric@civicknowledge.com>
# SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
################################################################################

use lib '.';

require 'readvaluesfile.pl';

use Getopt::Std;
getopts('chvi:');

#Options
# c -> generate c code file
# h -> generate header file
# v -> generate VCARD values
# i -> .c/.h "in" file (template)

my $ucprefix = "ICAL";
if ($opt_v) {
  $ucprefix = "VCARD";
}
my $lcprefix = lc($ucprefix);

# Open with value-types.txt

my %h = read_values_file($ARGV[0]);

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

  if ($opt_h) {
    print "#endif /*${ucprefix}VALUE_H*/\n";
  }

}

sub insert_code
{
  # Map type names to the value in the icalvalue_impl data union */

  %union_map = (
    BOOLEAN        => 'int',
    CALADDRESS     => 'string',
    DATE           => 'time',
    DATEANDORTIME  => 'time',
    DATETIME       => 'time',
    DATETIMEDATE   => 'time',
    DATETIMEPERIOD => 'period',
    DURATION       => 'duration',
    INTEGER        => 'int',
    LANGUAGETAG    => 'string',
    TEXT           => 'string',
    STRUCTURED     => 'structured',
    TEXTLIST       => 'textlist',
    TIME           => 'time',
    TIMESTAMP      => 'time',
    UID            => 'string',
    URI            => 'string',
    UTCOFFSET      => 'int',
    QUERY          => 'string',
    XMLREFERENCE   => 'string',
    X              => 'string'
  );

  if ($opt_h) {

    # First print out the value enumerations
    $idx = $h{'ANY'}->{"kindEnum"};
    print "typedef enum ${lcprefix}value_kind {\n";
    print "   ${ucprefix}_ANY_VALUE=$idx,\n";

    foreach $value (sort keys %h) {

      next if !$value;

      next if $value eq 'NO' or $value eq 'ANY';

      my $ucv = join("", map {uc(lc($_));} split(/-/, $value));

      $idx = $h{$value}->{"kindEnum"};

      print "    ${ucprefix}_${ucv}_VALUE=$idx,\n";
    }

    $idx = $h{'NO'}->{"kindEnum"};
    print "   ${ucprefix}_NO_VALUE=$idx\n} ${lcprefix}value_kind;\n\n";

    # Now create enumerations for property values
    $lastidx = $idx = 10000;

    print "#define ${ucprefix}PROPERTY_FIRST_ENUM $idx\n\n";

    foreach $value (sort keys %h) {

      next if !$value;

      next if $value eq 'NO' or $value eq 'ANY';

      my $ucv   = join("", map {uc(lc($_));} split(/-/, $value));
      my @enums = @{$h{$value}->{'enums'}};

      if (@enums) {

        my ($c_autogen, $c_type) = @{$h{$value}->{'C'}};
        print "typedef $c_type {\n";
        my $first = 1;

        foreach $e (@enums) {
          if (!$first) {
            print ",\n";
          } else {
            $first = 0;
          }

          $e =~ /([a-zA-Z0-9\-\._]+)=?([0-9]+)?/;
          $e = $1;
          if ($2) {
            $idx = $2;
          } else {
            $idx++;
          }
          if ($idx > $lastidx) {
            $lastidx = $idx;
          }

          my $uce = join("", map {uc(lc($_));} split(/[\-\.]/, $e));

          print "    ${ucprefix}_${ucv}_${uce} = $idx";
        }

        $c_type =~ s/enum //;

        print "\n} $c_type;\n\n";
      }
    }

    $lastidx++;
    print "#define ${ucprefix}PROPERTY_LAST_ENUM $lastidx\n";
  }

  if ($opt_c) {

    # print out the value to string map

    my $count     = scalar(keys %h) + 1;
    my $map_count = $count - 2;
    print "static const struct ${lcprefix}value_kind_map value_map[$map_count]={\n";

    foreach $value (sort keys %h) {

      next if $value eq 'NO' or $value eq 'ANY';

      my $ucv = join("", map {uc(lc($_));} split(/-/, $value));

      print "    {${ucprefix}_${ucv}_VALUE,\"$value\"},\n";
    }

    print "    {${ucprefix}_NO_VALUE,\"\"}\n};";

  }

  foreach $value (sort keys %h) {

    next if $value eq 'ANY';

    my $autogen = $h{$value}->{C}->[0];
    my $type    = $h{$value}->{C}->[1];
    $type =~ s/char\*/char \*/;
    $type =~ s/array\*/array \*/;

    my $ucf = join("", map {ucfirst(lc($_));} split(/-/, $value));

    my $lc = lc($ucf);
    my $uc = uc($lc);

    my $pointer_check    = "    icalerror_check_arg_rz((v != 0), \"v\");\n" if $type =~ /\*/;
    my $pointer_check_rv = "    icalerror_check_arg_rv((v != 0), \"v\");\n" if $type =~ /\*/;

    my $assign;

    if ($type =~ /char/) {
      $assign =
"icalmemory_strdup(v);\n\n    if (impl->data.v_string == 0) {\n        errno = ENOMEM;\n    }\n";
    } else {
      $assign = "v;";
    }

    my $union_data;

    if (@{$h{$value}->{'enums'}}) {
      $union_data = 'enum';

    } elsif (exists $union_map{$uc}) {
      $union_data = $union_map{$uc};
    } else {
      $union_data = $lc;
    }

    if ($opt_c && $autogen) {

      print "\
${lcprefix}value *${lcprefix}value_new_${lc}($type v)\
{\
    struct ${lcprefix}value_impl *impl;\
$pointer_check\
    impl = ${lcprefix}value_new_impl(${ucprefix}_${uc}_VALUE);\
    ${lcprefix}value_set_${lc}((${lcprefix}value *)impl, v);\
    return (${lcprefix}value*)impl;\
}\
\
void ${lcprefix}value_set_${lc}(${lcprefix}value *value, $type v)\
{\
    struct ${lcprefix}value_impl *impl;\
    icalerror_check_arg_rv((value != 0), \"value\");\
$pointer_check_rv\
    icalerror_check_value_type(value, ${ucprefix}_${uc}_VALUE);\
    impl = (struct ${lcprefix}value_impl *)value;\n";

      if ($union_data eq 'string') {

        print
"    if (impl->data.v_${union_data} != 0) {\n        icalmemory_free_buffer((void *)impl->data.v_${union_data});\n    }\n";

      } elsif ($union_data eq 'textlist') {
        print "    ${lcprefix}strarray_free(impl->data.v_textlist);\n";
      }

      $castStr = "";
      if ($union_data eq 'enum') {$castStr = "(int)";}
      print "\
    impl->data.v_$union_data = $castStr$assign\
    ${lcprefix}value_reset_kind(impl);\n}\n\n";

      print "$type\ ${lcprefix}value_get_${lc}(const ${lcprefix}value *value)\n{\n";
      $retString = "";
      if ($union_data eq 'string' or $union_data eq 'textlist') {
        print "    icalerror_check_arg_rz((value != 0), \"value\");\n";
      } else {
        print "    icalerror_check_arg((value != 0), \"value\");\n";
        if ($union_data eq 'enum') {
          print "    if (!value) {\n        return ${ucprefix}_${uc}_NONE;\n    }\n";
          $retString = "(${type})";
        } elsif ($union_data eq 'int') {
          print "    if (!value) {\n        return 0;\n    }\n";
        } elsif ($union_data eq 'float') {
          print "    if (!value) {\n        return 0.0;\n     }\n";
        } elsif ($union_data eq 'time') {
          if ($opt_v) {
            print "    if (!value) {\n        return vcardtime_null_datetime();\n    }\n";
          } else {
            print "    if (!value) {\n        return icaltime_null_time();\n    }\n";
          }
        } elsif ($union_data eq 'duration') {
          print "    if (!value) {\n        return icaldurationtype_null_duration();\n    }\n";
        } elsif ($union_data eq 'period') {
          print "    if (!value) {\n        return icalperiodtype_null_period();\n    }\n";
        } elsif ($union_data eq 'requeststatus') {
          print "    if (!value) {\n        return icalreqstattype_from_string(\"0.0\");\n    }\n";
        }
      }
      print "    icalerror_check_value_type(value, ${ucprefix}_${uc}_VALUE);\
    return ${retString}(((struct ${lcprefix}value_impl *)value)->data.v_${union_data});\n}\n";

    } elsif ($opt_h && $autogen) {

      print "\n/* $value */\
LIBICAL_${ucprefix}_EXPORT ${lcprefix}value *${lcprefix}value_new_${lc}($type v);\
LIBICAL_${ucprefix}_EXPORT $type ${lcprefix}value_get_${lc}(const ${lcprefix}value *value);\
LIBICAL_${ucprefix}_EXPORT void ${lcprefix}value_set_${lc}(${lcprefix}value *value, ${type} v);\n";
    }

  }

}
