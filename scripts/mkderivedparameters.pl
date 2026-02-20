#!/usr/bin/env perl
################################################################################
# SPDX-FileCopyrightText: 2000, Eric Busboom <eric@civicknowledge.com>
# SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
################################################################################

require "readvaluesfile.pl";

use Getopt::Std;
getopts('chspi:v');

#Options
# c -> generate c code file
# h -> generate header file
# p -> generate perl code
# v -> generate VCARD parameters
# i -> .c/.h "in" file (template)

my $ucprefix = "ICAL";
if ($opt_v) {
  $ucprefix = "VCARD";
}
my $lcprefix = lc($ucprefix);

%no_xname = (RELATED => 1, RANGE => 1, RSVP => 1, XLICERRORTYPE => 1, XLICCOMPARETYPE => 1);

%params = read_parameters_file($ARGV[0]);

# Write the file inline by copying everything before a demarcation
# line, and putting the generated data after the demarcation

if ($opt_i) {

  open(IN, $opt_i) || die "Can't open input file $opt_i";

  while (<IN>) {
    if (/<insert_code_here>/) {
      $autogenMsg = "of section of machine generated code (mkderivedparameters.pl). Do not edit.";
      if ($opt_p) {
        $startComment = "#";
        $endComment   = "";
      } else {
        $startComment = "/*";
        $endComment   = " */";
      }
      print $startComment. " START " . $autogenMsg . $endComment . "\n\n";

      insert_code();

      print $startComment. " END   " . $autogenMsg . $endComment . "\n\n";
    } else {
      print;
    }

  }

}

sub insert_code
{

  # Write parameter enumerations and datatypes

  if ($opt_h) {
    my $enumConst = $params{'ANY'}->{"kindEnum"};
    print "typedef enum ${lcprefix}parameter_kind {\n    ${ucprefix}_ANY_PARAMETER = "
      . $enumConst . ",\n";
    $enumVal = 1;
    foreach $param (sort keys %params) {

      next if !$param;

      next if $param eq 'NO' or $param eq 'ANY';

      my $uc = join("", map {uc($_);} split(/-/, $param));

      $enumConst = $params{$param}->{"kindEnum"};

      print "    ${ucprefix}_${uc}_PARAMETER = " . $enumConst . ",\n";

    }
    $enumConst = $params{'NO'}->{"kindEnum"};
    print "    ${ucprefix}_NO_PARAMETER = " . $enumConst . "\n} ${lcprefix}parameter_kind;\n\n";

    # Now create enumerations for parameter values
    $lastidx = $idx = 20000;

    print "#define ${ucprefix}PARAMETER_FIRST_ENUM $idx\n\n";

    foreach $param (sort keys %params) {

      next if !$param;

      next if $param eq 'NO' or $param eq 'ANY';

      my $type  = $params{$param}->{"C"};
      my $ucv   = join("", map {uc(lc($_));} split(/-/, $param));
      my @enums = @{$params{$param}->{'enums'}};

      if (@enums) {

        print "typedef enum $type {\n";
        my $first = 1;

        foreach $e (@enums) {
          if (!$first) {
            print ",\n";
          } else {
            $first = 0;
          }

          $e =~ /([a-zA-Z0-9\-]+)=?([0-9]+)?/;
          $e = $1;
          if ($2) {
            $idx = $2;
          } else {
            $idx++;
          }
          if ($idx > $lastidx) {
            $lastidx = $idx;
          }

          my $uce = join("", map {uc(lc($_));} split(/-/, $e));

          print "    ${ucprefix}_${ucv}_${uce} = $idx";
        }
        $c_type =~ s/enum //;

        print "\n} $type;\n\n";
      }
    }

    $lastidx++;
    print "#define ${ucprefix}PARAMETER_LAST_ENUM $lastidx\n\n";

  }

  if ($opt_c) {

    # Create the ${lcprefix}parameter_value to ${lcprefix}value_kind conversion table
    my $count = 0;
    my $out;

    foreach $enum (@{$params{'VALUE'}->{'enums'}}) {
      $enum =~ /([a-zA-Z0-9\-]+)=?([0-9]+)?/;
      $enum = $1;

      next if $enum eq 'X'  or $enum eq 'NONE';
      next if $enum eq 'NO' or $enum eq 'ERROR';
      $uc = join("", map {uc(lc($_));} split(/-/, $enum));
      $out .= "    {${ucprefix}_VALUE_${uc}, ${ucprefix}_${uc}_VALUE},\n";
      $count++;
    }

    $count += 2;
    print "static const struct ${lcprefix}parameter_value_kind_map value_kind_map[$count] = {\n";
    print $out;
    print "    {${ucprefix}_VALUE_X, ${ucprefix}_X_VALUE},\n";
    print "    {${ucprefix}_VALUE_NONE, ${ucprefix}_NO_VALUE}\n};\n\n";

    #Create the parameter Name map

    $out   = "";
    $count = 0;
    foreach $param (sort keys %params) {

      next if !$param;

      next if $param eq 'NO' or $param eq 'ANY';

      my $lc = join("", map {lc($_);} split(/-/, $param));
      my $uc = join("", map {uc(lc($_));} split(/-/, $param));

      $count++;
      $out .= "    {${ucprefix}_${uc}_PARAMETER, \"$param\"";

      my $type = $params{$param}->{"C"};
      if ($type =~ /char/) {
        $out .= ", ${ucprefix}_TEXT_VALUE";

      } elsif ($type =~ /int/) {
        $out .= ", ${ucprefix}_INTEGER_VALUE";

      } elsif ($opt_v && $type =~ /vcardtimetype/) {
        $out .= ", ${ucprefix}_DATEANDORTIME_VALUE";

      } elsif ($opt_v && $type =~ /vcardstructured/) {
        $out .= ", ${ucprefix}_STRUCTURED_VALUE";

      } else {
        $out .= ", ${ucprefix}_X_VALUE";
      }

      my @flags = @{$params{$param}->{'flags'}};
      if (@flags) {
        my $sep = ", ";
        foreach $flag (@flags) {
          $flag =~ s/-//g;
          $flag = uc($flag);
          $out .= "${sep}${ucprefix}_PARAMETER_${flag}";
          $sep = " | ";
        }
      } else {
        $out .= ", 0";
      }

      $out .= "},\n";
    }
    $count += 1;
    print "static const struct ${lcprefix}parameter_kind_map parameter_map[$count] = {\n";
    print $out;
    print "    { ${ucprefix}_NO_PARAMETER, \"\", ${ucprefix}_NO_VALUE, 0}\n};\n\n";

    # Create the parameter value map
    $out   = "";
    $count = 0;
    foreach $param (sort keys %params) {

      next if !$param;

      next if $param eq 'NO' or $param eq 'ANY';

      my $type  = $params{$param}->{"C"};
      my $uc    = join("", map {uc(lc($_));} split(/-/, $param));
      my @enums = @{$params{$param}->{'enums'}};

      if (@enums) {

        foreach $e (@enums) {
          $e =~ /([a-zA-Z0-9\-]+)=?([0-9]+)?/;
          $e = $1;

          next if $e eq 'X' or $e eq 'NONE';

          my $uce = join("", map {uc(lc($_));} split(/-/, $e));

          $count++;
          $out .= "    {${ucprefix}_${uc}_PARAMETER,${ucprefix}_${uc}_${uce}, \"$e\"},\n";
        }

      }
    }

    $count += 3;
    print "static const struct ${lcprefix}parameter_map ${lcprefix}parameter_map[] = {\n";
    print "    {${ucprefix}_ANY_PARAMETER, 0, \"\"},\n";
    print $out;
    print "    {${ucprefix}_NO_PARAMETER, 0, \"\"}\n};\n\n";

  }

  foreach $param (sort keys %params) {

    next if $param eq 'NO' or $param eq 'ANY';

    my $type = $params{$param}->{'C'};

    my $ucf = join("", map {ucfirst(lc($_));} split(/-/, $param));

    my $lc = lc($ucf);
    my $uc = uc($lc);

    my $is_multivalued = 0;
    my @flags          = @{$params{$param}->{'flags'}};
    if (@flags) {
      foreach $flag (@flags) {
        $flag =~ s/-//g;
        $flag = uc($flag);
        if ($flag eq 'IS_MULTIVALUED') {
          $is_multivalued = 1;
          last;
        }
      }
    }

    my $apitype;
    my $charorenum;
    my $set_code;
    my $pointer_check;
    my $pointer_check_v;
    my $xrange;

    # Only relevant for multivalued
    my $singletype;
    my $elemtype;
    my $charorenum_nth;

    if ($type =~ /char/) {
      $type =~ s/char\*/char \*/;

      if ($is_multivalued) {
        $elemtype   = $type;
        $singletype = $type;
        $type       = "${lcprefix}strarray *";
        $apitype    = "${lcprefix}strarray";
        $charorenum =
          "    icalerror_check_arg_rz((param != 0), \"param\");\n    return param->values;";

        $charorenum_nth =
"   icalerror_check_arg_rz((param != 0), \"param\");\n    if (param->values && ${lcprefix}strarray_size(param->values)) {\n        return ${lcprefix}strarray_element_at(param->values, position);\n    } else {\n        return NULL;\n    }";

        $charorenum_values_add_v = "icalstrarray_add(values, v)";

        $set_code =
"if (param->values != NULL) {\n        ${lcprefix}strarray_free(param->values);\n    }\n    ((struct ${lcprefix}parameter_impl *)param)->values = v;";

      } else {
        $charorenum =
          "    icalerror_check_arg_rz((param != 0), \"param\");\n    return param->string;";

        $set_code =
"if (param->string != NULL) {\n        icalmemory_free_buffer((void *)param->string);\n    }\n    ((struct ${lcprefix}parameter_impl *)param)->string = icalmemory_strdup(v);";
      }

      $pointer_check   = "    icalerror_check_arg_rz((v != 0), \"v\");";
      $pointer_check_v = "\n    icalerror_check_arg_rv((v != 0), \"v\");";

    } elsif ($type =~ /int/) {

      $xrange =
        "    if (param != 0) {\n       return param->data;\n    } else {\n       return 0;\n    }";
      $charorenum = "    icalerror_check_arg((param != 0), \"param\");\n$xrange";

      $set_code = "((struct ${lcprefix}parameter_impl *)param)->data = v;";

    } elsif ($type =~ /vcardtimetype/) {

      $xrange =
"    if (param != 0) {\n       return param->date;\n    } else {\n       return vcardtime_null_datetime();\n    }";
      $charorenum = "    icalerror_check_arg((param != 0), \"param\");\n$xrange";

      $set_code = "((struct ${lcprefix}parameter_impl *)param)->date = v;";

    } elsif ($type =~ /struct icaldurationtype/) {

      $xrange =
"    if (param != 0) {\n       return param->duration;\n    } else {\n       return icaldurationtype_null_duration();\n    }";
      $charorenum = "    icalerror_check_arg((param != 0), \"param\");\n$xrange";

      $set_code = "((struct ${lcprefix}parameter_impl *)param)->duration = v;";

    } elsif ($type =~ /vcardstructuredtype/) {

      $type =~ s/vcardstructuredtype\*/vcardstructuredtype \*/;
      $charorenum =
        "    icalerror_check_arg_rz((param != 0), \"param\");\n    return param->structured;";

      $set_code =
"if (param->structured != NULL) {\n        vcardstructured_unref(param->structured);\n    }\n    vcardstructured_ref(v);\n    ((struct ${lcprefix}parameter_impl *)param)->structured = v;";

    } else {

      if ($is_multivalued) {
        $elemtype   = $type;
        $singletype = "${lcprefix}enumarray_element *";
        $type       = "${lcprefix}enumarray *";
        $apitype    = "${lcprefix}enumarray";
        $charorenum =
          "    icalerror_check_arg_rz((param != 0), \"param\");\n    return param->values;";

        $charorenum_nth =
"   icalerror_check_arg((param != 0), \"param\");\n    if (param && param->values && ${lcprefix}enumarray_size(param->values)) {\n        const $singletype v = ${lcprefix}enumarray_element_at(param->values, position);\n        return (${elemtype})v->val;\n    } else {\n        return ${ucprefix}_${uc}_NONE;\n    }";

        $charorenum_values_add_v =
          "icalenumarray_element _elem = { (int)v, NULL };\n    icalenumarray_add(values, &_elem)";

        $set_code =
"if (param->values != NULL) {\n        ${lcprefix}enumarray_free(param->values);\n    }\n    ((struct ${lcprefix}parameter_impl *)param)->values = v;";

      } else {
        $xrange = "    if (param->string != 0) {\n        return ${ucprefix}_${uc}_X;\n    }\n"
          if !exists $no_xname{$uc};

        $charorenum =
"    icalerror_check_arg((param != 0), \"param\");\n    if (!param) {\n        return ${ucprefix}_${uc}_NONE;\n    }\n$xrange\n    return ($type)(param->data);";

        $pointer_check =
"    icalerror_check_arg_rz(v >= ${ucprefix}_${uc}_X, \"v\");\n    icalerror_check_arg_rz(v < ${ucprefix}_${uc}_NONE, \"v\");";

        $pointer_check_v =
"\n    icalerror_check_arg_rv(v >= ${ucprefix}_${uc}_X, \"v\");\n    icalerror_check_arg_rv(v < ${ucprefix}_${uc}_NONE, \"v\");";

        $set_code = "((struct ${lcprefix}parameter_impl *)param)->data = (int)v;";
      }
    }

    my $newfnsuffix;
    if ($is_multivalued) {
      $newfnsuffix = "_list";
    }

    if ($opt_c) {

      print <<EOM;

/* $param */
${lcprefix}parameter *${lcprefix}parameter_new_${lc}${newfnsuffix}($type v)
{
    struct ${lcprefix}parameter_impl *impl;
    icalerror_clear_errno();
$pointer_check
    impl = ${lcprefix}parameter_new_impl(${ucprefix}_${uc}_PARAMETER);
    if (impl == 0) {
       return 0;
    }

    ${lcprefix}parameter_set_${lc}((${lcprefix}parameter *)impl, v);
    if (icalerrno != ICAL_NO_ERROR) {
       ${lcprefix}parameter_free((${lcprefix}parameter *)impl);
       return 0;
    }

    return (${lcprefix}parameter *)impl;
}

${type} ${lcprefix}parameter_get_${lc}(const ${lcprefix}parameter *param)
{
    icalerror_clear_errno();
$charorenum
}

void ${lcprefix}parameter_set_${lc}(${lcprefix}parameter *param, ${type} v)
{$pointer_check_v
    icalerror_check_arg_rv((param != 0), "param");
    icalerror_clear_errno();

    $set_code
}
EOM

      if ($is_multivalued) {
        print <<EOM;

${lcprefix}parameter * ${lcprefix}parameter_new_${lc}($elemtype v)
{
    ${apitype} *values = ${apitype}_new(1);
    $charorenum_values_add_v;
    return ${lcprefix}parameter_new_${lc}${newfnsuffix}(values);
}

size_t ${lcprefix}parameter_get_${lc}_size(${lcprefix}parameter *param)
{
    icalerror_check_arg_rz((param != 0), "param");
    if (param->values) {
        return param->values->num_elements;
    }
    return 0;
}

${elemtype} ${lcprefix}parameter_get_${lc}_nth(${lcprefix}parameter *param, size_t position)
{
 $charorenum_nth
}

void ${lcprefix}parameter_add_${lc}(${lcprefix}parameter *param, ${singletype} v)
{${apitype} **values;
    $pointer_check_v
    icalerror_check_arg_rv((param != 0), "param");
    icalerror_clear_errno();

    values = &((struct ${lcprefix}parameter_impl *)param)->values;
    if (*values == 0) {
        *values = ${apitype}_new(5);
    }
    ${apitype}_add(*values, v);
}

void ${lcprefix}parameter_remove_${lc}(${lcprefix}parameter *param, ${singletype} v)
{${apitype} *values;
    $pointer_check_v
    icalerror_check_arg_rv((param != 0), "param");
    icalerror_clear_errno();

    values = ((struct ${lcprefix}parameter_impl *)param)->values;
    if (values != 0) {
        ${apitype}_remove(values, v);
    }
}

EOM
      }

    } elsif ($opt_h) {

      print <<EOM;

/* $param */
LIBICAL_${ucprefix}_EXPORT ${lcprefix}parameter * ${lcprefix}parameter_new_${lc}${newfnsuffix}($type v);
LIBICAL_${ucprefix}_EXPORT ${type} ${lcprefix}parameter_get_${lc}(const ${lcprefix}parameter *value);
LIBICAL_${ucprefix}_EXPORT void ${lcprefix}parameter_set_${lc}(${lcprefix}parameter *value, ${type} v);
EOM

      if ($is_multivalued) {
        print <<EOM;
LIBICAL_${ucprefix}_EXPORT ${lcprefix}parameter * ${lcprefix}parameter_new_${lc}($elemtype v);
LIBICAL_${ucprefix}_EXPORT size_t ${lcprefix}parameter_get_${lc}_size(${lcprefix}parameter *param);
LIBICAL_${ucprefix}_EXPORT ${elemtype} ${lcprefix}parameter_get_${lc}_nth(${lcprefix}parameter *param, size_t position);
LIBICAL_${ucprefix}_EXPORT void ${lcprefix}parameter_add_${lc}(${lcprefix}parameter *value, ${singletype} v);
LIBICAL_${ucprefix}_EXPORT void ${lcprefix}parameter_remove_${lc}(${lcprefix}parameter *value, ${singletype} v);
EOM
      }
    }

    if ($opt_p) {

      print <<EOM;

# $param

package Net::${Lcprefix}::Parameter::${ucf};
\@ISA=qw(Net::${Lcprefix}::Parameter);

sub new
{
   my \$self = [];
   my \$package = shift;
   my \$value = shift;

   bless \$self, \$package;

   my \$p;

   if (\$value) {
      \$p = Net::${Lcprefix}::${lcprefix}parameter_new_from_string(\$Net::${Lcprefix}::${ucprefix}_${uc}_PARAMETER,\$value);
   } else {
      \$p = Net::${Lcprefix}::${lcprefix}parameter_new(\$Net::${Lcprefix}::${ucprefix}_${uc}_PARAMETER);
   }

   \$self->[0] = \$p;

   return \$self;
}

sub get
{
   my \$self = shift;
   my \$impl = \$self->_impl();

   return Net::${Lcprefix}::${lcprefix}parameter_as_${lcprefix}_string(\$impl);

}

sub set
{
   # This is hard to implement, so I've punted for now.
   die "Set is not implemented";
}

EOM

    }

  }

  if ($opt_h) {

    print <<EOM;
#endif /*${ucprefix}PARAMETER_H*/

EOM
  }

}
