#!/usr/local/bin/perl

use Getopt::Std;
getopts('chspi:');

%no_xname = (RELATED=>1,RANGE=>1,RSVP=>1,XLICERRORTYPE=>1,XLICCOMPARETYPE=>1);

# Usually, open  param-c-types.txt
open(F,"$ARGV[0]") || die "Can't open  C parameter types file $ARGV[0]:$!";

# Write the file inline by copying everything before a demarcation
# line, and putting the generated data after the demarcation

if ($opt_i) {

  open(IN,$opt_i) || die "Can't open input file $opt_i";

  while(<IN>){

    if (/Do not edit/){
      last;
    }

    print;

  }    

  if($opt_p){
    print "# Everything below this line is machine generated. Do not edit. \n";
  } else {
    print "/* Everything below this line is machine generated. Do not edit. */\n";
  }

}



if (($opt_c or $opt_h) and !$opt_i) {

print <<EOM;
/* -*- Mode: C -*-
  ======================================================================
  FILE: icalderivedparameters.{c,h}
  CREATOR: eric 09 May 1999
  
  \044Id: mkderivedparameters.pl,v 1.1 1999/05/14 07:04:31 eric Exp eric \044
  \044Locker: eric \044
    
  (C) COPYRIGHT 1999 Eric Busboom 
  http://www.softwarestudio.org

  The contents of this file are subject to the Mozilla Public License
  Version 1.0 (the "License"); you may not use this file except in
  compliance with the License. You may obtain a copy of the License at
  http://www.mozilla.org/MPL/
 
  Software distributed under the License is distributed on an "AS IS"
  basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See
  the License for the specific language governing rights and
  limitations under the License.
 

 ======================================================================*/

/*
 * THIS FILE IS MACHINE GENERATED DO NOT EDIT
 */


EOM

}

if ($opt_p and !$opt_i){

print <<EOM;
# -*- Mode: Perl -*-
#  ======================================================================
#  \044Id:\044
#    
#  (C) COPYRIGHT 1999 Eric Busboom 
#  http://www.softwarestudio.org
#
#  The contents of this file are subject to the Mozilla Public License
#  Version 1.0 (the "License"); you may not use this file except in
#  compliance with the License. You may obtain a copy of the License at
#  http://www.mozilla.org/MPL/
# 
#  Software distributed under the License is distributed on an "AS IS"
#  basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See
#  the License for the specific language governing rights and
#  limitations under the License. 
#
#  The original author is Eric Busboom
#  The original code is derivedparams.h
#
#  ======================================================================*/

EOM

}


while(<F>){
  
  chop;
  my ($param,$type) = split(/\s{2,}/,$_);  
  
  my $ucf = join("",map {ucfirst(lc($_));}  split(/-/,$param));
  
  my $lc = lc($ucf);
  my $uc = uc($lc);
  
  my $lctype = lc($type);
  

  my $charorenum;
  my $set_code;
  my $pointer_check;
  my $new_pointer_check;
  my $new_pointer_check_v;
  my $xrange;

  if ($type=~/char/){
    $new_pointer_check = "icalerror_check_arg_rz( (v!=0),\"v\");"; 
    $new_pointer_check_v = "icalerror_check_arg_rv( (v!=0),\"v\");"; 
  }


  if ($type=~/char/ ) {

     $charorenum = "    icalerror_check_arg_rz( (param!=0), \"param\");\n    return ((struct icalparameter_impl*)param)->string;";
    
     $pointer_check = "icalerror_check_arg_rz( (v!=0),\"v\");";

     $set_code = "((struct icalparameter_impl*)param)->string = strdup(v);"

  } else {

    $xrange ="     if ( ((struct icalparameter_impl*)param)->string != 0){\n        return ICAL_${uc}_XNAME;\n        }\n" if !exists $no_xname{$uc};

    $charorenum=<<EOM;
    icalerror_check_arg( (param!=0), \"param\");
$xrange
    return ((struct icalparameter_impl*)param)->data.v_${lc};
EOM
     
     $pointer_check = "icalerror_check_arg( (v!=0),\"v\");";

     $set_code = "((struct icalparameter_impl*)param)->data.v_${lc} = v;";

    $print_code = "switch (impl->data.v_${lc}) {\ncase ICAL_${uc}_: {\n}\ncase ICAL_${uc}_XNAME: /* Fall Through */\n}\n";

   }
  
  
  
  if ($opt_c) {
    
  print <<EOM;
/* $param */
icalparameter* icalparameter_new_${lc}($type v)
{
   struct icalparameter_impl *impl;
   icalerror_clear_errno();
   $new_pointer_check
   impl = icalparameter_new_impl(ICAL_${uc}_PARAMETER);
   if (impl == 0) {
      return 0;
   }

   icalparameter_set_${lc}((icalparameter*) impl,v);
   if (icalerrno != ICAL_NO_ERROR) {
      icalparameter_free((icalparameter*) impl);
      return 0;
   }

   return (icalparameter*) impl;
}

${type} icalparameter_get_${lc}(icalparameter* param)
{
   icalerror_clear_errno();
$charorenum
}

void icalparameter_set_${lc}(icalparameter* param, ${type} v)
{
   $new_pointer_check_v
   icalerror_check_arg_rv( (param!=0), "param");
   icalerror_clear_errno();
   
   $set_code
}

EOM

  } elsif( $opt_h) {

  print <<EOM;
/* $param */
icalparameter* icalparameter_new_${lc}($type v);
${type} icalparameter_get_${lc}(icalparameter* value);
void icalparameter_set_${lc}(icalparameter* value, ${type} v);

EOM

} elsif ($opt_s) {

next if $type =~ /char/;

print<<EOM;
case ICAL_${uc}_PARAMETER:
{
  $print_code
}
EOM

}

  if ($opt_p) {
    
  print <<EOM;

# $param 

package Net::ICal::Parameter::${ucf};
\@ISA=qw(Net::ICal::Parameter);

sub new
{
   my \$self = [];
   my \$package = shift;
   my \$value = shift;

   bless \$self, \$package;

   my \$p;

   if (\$value) {
      \$p = Net::ICal::icalparameter_new_from_string(\$Net::ICal::ICAL_${uc}_PARAMETER,\$value);
   } else {
      \$p = Net::ICal::icalparameter_new(\$Net::ICal::ICAL_${uc}_PARAMETER);
   }

   \$self->[0] = \$p;

   return \$self;
}

sub get
{
   my \$self = shift;
   my \$impl = \$self->_impl();

   return Net::ICal::icalparameter_as_ical_string(\$impl);

}

sub set
{
   # This is hard to implement, so I've punted for now. 
   die "Set is not implemented";
}

EOM

}

}

if ($opt_h){

print <<EOM;
#endif /*ICALPARAMETER_H*/
EOM
}
