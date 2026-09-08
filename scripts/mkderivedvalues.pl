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

# Return which ${lcprefix}value_impl union member a value type reads from, as a UNION_* tag.
sub value_union_type
{
  my $value = shift;

  my $uc = uc(join("", map {lc($_);} split(/-/, $value)));

  return 'X'    if $opt_v and $uc eq 'X';
  return 'ENUM' if @{$h{$value}->{'enums'}};

  my $union_data = exists $union_map{$uc} ? $union_map{$uc} : lc($uc);

  return exists $union_type_map{$union_data} ? $union_type_map{$union_data} : 'NONE';
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

  # Map an icalvalue_impl union member name to its ${lcprefix}value_union tag

  %union_type_map = (
    'enum'       => 'ENUM',
    'float'      => 'FLOAT',
    'geo'        => 'GEO',
    'int'        => 'INT',
    'string'     => 'STRING',
    'structured' => 'STRUCTURED',
    'textlist'   => 'TEXTLIST',
    'time'       => 'TIME'
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

    # vCard only: emit the value_union enum and the kind -> union lookup the accessors call
    if ($opt_v) {
      print "\n\n";
      print "typedef enum ${lcprefix}value_union {\n";
      print "    ${ucprefix}_UNION_NONE = 0,\n";
      foreach $ut (sort values %union_type_map) {
        print "    ${ucprefix}_UNION_${ut},\n";
      }
      print "    ${ucprefix}_UNION_X\n} ${lcprefix}value_union;\n\n";

      print
"static ${lcprefix}value_union ${lcprefix}value_kind_to_union(${lcprefix}value_kind kind)\n{\n    switch (kind) {\n";

      # No 'default' label: the switch must stay exhaustive over
      # ${lcprefix}value_kind so that -Wswitch catches a newly added value type.
      foreach $value (sort keys %h) {

        my $ucv        = join("", map {uc(lc($_));} split(/-/, $value));
        my $union_type = value_union_type($value);

        print "    case ${ucprefix}_${ucv}_VALUE:\n";
        print "        return ${ucprefix}_UNION_${union_type};\n";
      }

      print "    }\n\n    return ${ucprefix}_UNION_NONE;\n}\n";
    }
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

    my $union_type = value_union_type($value);

    # Fail the build rather than emit accessors whose type check would always pass
    if ($opt_v and $autogen and $union_type eq 'NONE') {
      die "No ${lcprefix}value_impl union member is registered for value type " .
        "$value (union data '$union_data'): the generated accessors would " .
        "carry a vacuous type check.  Add '$union_data' to %union_type_map " .
        "(and the member to struct ${lcprefix}value_impl), or mark $value as " .
        "(m) in $ARGV[0] and write its accessors by hand.\n";
    }

    # Zero value a getter returns for a NULL or wrongly-typed argument
    my $null_ret     = "0";
    my $has_null_ret = 1;

    if ($union_data eq 'enum') {
      $null_ret = "${ucprefix}_${uc}_NONE";
    } elsif ($union_data eq 'int') {
      $null_ret = "0";
    } elsif ($union_data eq 'float') {
      $null_ret = "0.0";
    } elsif ($union_data eq 'time') {
      $null_ret = $opt_v ? "vcardtime_null_datetime()" : "icaltime_null_time()";
    } elsif ($union_data eq 'duration') {
      $null_ret = "icaldurationtype_null_duration()";
    } elsif ($union_data eq 'period') {
      $null_ret = "icalperiodtype_null_period()";
    } elsif ($union_data eq 'requeststatus') {
      $null_ret = "icalreqstattype_from_string(\"0.0\")";
    } else {
      $has_null_ret = 0;
    }

    # Likewise for a non-pointer getter with no zero value to return
    if ($opt_v and $autogen and !$has_null_ret and $type !~ /\*/) {
      die "No zero value is registered for value type $value (union data " .
        "'$union_data'): the generated getter would return the literal 0 for " .
        "its non-pointer return type '$type'.  Add a zero value for " .
        "'$union_data' above, or mark $value as (m) in $ARGV[0] and write " .
        "its accessors by hand.\n";
    }

    # Setter guard: vCard compares union tags, iCal keeps the exact-kind assertion
    my $set_kind_check =
      $opt_v
      ? "    if (${lcprefix}value_kind_to_union(value->kind) != ${ucprefix}_UNION_${union_type}) {\n        icalerror_set_errno(ICAL_BADARG_ERROR);\n        return;\n    }"
      : "    icalerror_check_value_type(value, ${ucprefix}_${uc}_VALUE);";

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
$set_kind_check\
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
      if ($union_data eq 'enum') {
        $retString = "(${type})";
      }

      # vCard getters return the zero value on a kind mismatch; iCal aborts via icalerror
      if ($opt_v) {
        print
"    if (!value) {\
        icalerror_set_errno(ICAL_BADARG_ERROR);\
        return $null_ret;\
    }\
    if (${lcprefix}value_kind_to_union(value->kind) != ${ucprefix}_UNION_${union_type}) {\
        return $null_ret;\
    }\n";
      } else {
        if ($union_data eq 'string' or $union_data eq 'textlist') {
          print "    icalerror_check_arg_rz((value != 0), \"value\");\n";
        } else {
          print "    icalerror_check_arg((value != 0), \"value\");\n";
          print "    if (!value) {\n        return $null_ret;\n    }\n" if $has_null_ret;
        }
        print "    icalerror_check_value_type(value, ${ucprefix}_${uc}_VALUE);\n";
      }
      print
"    return ${retString}(((struct ${lcprefix}value_impl *)value)->data.v_${union_data});\n}\n";

    } elsif ($opt_h && $autogen) {

      print "\n/* $value */\
LIBICAL_${ucprefix}_EXPORT ${lcprefix}value *${lcprefix}value_new_${lc}($type v);\
LIBICAL_${ucprefix}_EXPORT $type ${lcprefix}value_get_${lc}(const ${lcprefix}value *value);\
LIBICAL_${ucprefix}_EXPORT void ${lcprefix}value_set_${lc}(${lcprefix}value *value, ${type} v);\n";
    }

  }

}
