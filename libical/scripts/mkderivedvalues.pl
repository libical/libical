#!/usr/bin/perl 


use Getopt::Std;
getopts('chspi:');

#Options
# c -> generate c code file
# h-> generate header file   
# s -> generate switch statement

# Open with value-c-types.txt


open(F,"$ARGV[0]") || die "Can't open values data file $ARGV[0]:$!";


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


if (($opt_c || $opt_h) and !$opt_i) {
print <<EOM;
/* -*- Mode: C -*-
  ======================================================================
  FILE: icalderivedvalues.{c,h}
  CREATOR: eric 09 May 1999
  
  \044Id:\044
    
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
#  The original code is derivedvalues.h
#
#  ======================================================================*/

EOM

}

# Map type names to the value in the icalvalue_impl data union */

%union_map = (
	      BOOLEAN => 'int',
	      CALADDRESS=>'string',
	      DATE=>'time',
	      DATETIME=>'time',
	      DATETIMEDATE=>'time',
	      DATETIMEPERIOD=>'period',
	      DURATION=>'duration',
	      INTEGER=>'int',
	      TEXT=>'string',
	      URI=>'string',
	      UTCOFFSET=>'int',
	      QUERY=>'string',
	      BINARY=>'string'
	     );

while(<F>)
{
  
  chop;
  my @v = split(/\t+/,$_);  
  
  my $value = shift @v;
  my $mode = shift @v;
  my $type = shift @v;
  my $comment = join(" ",@v);
  
  my $ucf = join("",map {ucfirst(lc($_));}  split(/-/,$value));
  
  my $lc = lc($ucf);
  my $uc = uc($lc);

  my $pointer_check = "icalerror_check_arg_rz( (v!=0),\"v\");\n" if $type =~ /\*/;
  my $pointer_check_rv = "icalerror_check_arg_rv( (v!=0),\"v\");\n" if $type =~ /\*/;

  my $assign;

  if ($type =~ /char/){
    $assign = "strdup(v);\n\n    if (impl->data.v_string == 0){\n      errno = ENOMEM;\n    }\n";
  } else {
    $assign = "v;";
  }

  my $union_data;
  
  if (exists $union_map{$uc} ){
    $union_data=$union_map{$uc};
  } else {
    $union_data = $lc;
  }

  if ($opt_c && $mode eq "std") {
print <<EOM;

icalvalue*
icalvalue_new_${lc} ($type v)
{
   struct icalvalue_impl* impl = icalvalue_new_impl(ICAL_${uc}_VALUE);
 
   $pointer_check
   icalvalue_set_${lc}((icalvalue*)impl,v);

   return (icalvalue*)impl;
}

void
icalvalue_set_${lc}(icalvalue* value, $type v)
{
    struct icalvalue_impl* impl; 
    
    icalerror_check_arg_rv( (value!=0),"value");
    $pointer_check_rv
    icalerror_check_value_type(value, ICAL_${uc}_VALUE);

    impl = (struct icalvalue_impl*)value;
EOM

if( ${union_data} eq 'string'){
print"    if(impl->data.v_${union_data}!=0) {free((void*)impl->data.v_${union_data});}\n";
}

print <<EOM;

    impl->data.v_${union_data} = $assign
}

$type
icalvalue_get_${lc}(icalvalue* value)
{
    icalerror_check_arg( (value!=0),"value");
    icalerror_check_value_type(value, ICAL_${uc}_VALUE);
  
    return ((struct icalvalue_impl*)value)->data.v_${union_data};
}

EOM

} elsif($opt_h) {

  print <<EOM;
/* $value $comment */
icalvalue* icalvalue_new_${lc}($type v);
$type icalvalue_get_${lc}(icalvalue* value);
void icalvalue_set_${lc}(icalvalue* value, ${type} v);

EOM

} elsif ($opt_s) {

if ( $ud{$union_data}++ == 0) {

print<<EOM;
const char* icalvalue_${union_data}_as_ical_string(icalvalue* value) {

    $type data;
    char temp[1024];
    char *str;
    icalerror_check_airg( (value!=0),"value");
    data = ((struct icalvalue_impl*)value)->data.v_${union_data}

    str = icalmemory_strdup(temp);

    return str;
}

EOM

}
}  elsif ($opt_p) { # Generate perl code

print <<EOM;

package Net::ICal::Value::${ucf}; 
use Net::ICal::Value;
\@ISA=qw(Net::ICal::Value);
sub new
{
   my \$self = [];
   my \$package = shift;
   my \$value = shift;

   bless \$self, \$package;

   my \$p;

   if (\$value){
      \$p = Net::ICal::icalvalue_new_from_string(\$Net::ICal::ICAL_${uc}_VALUE,\$value);
   } else {
      \$p = Net::ICal::icalvalue_new(\$Net::ICal::ICAL_${uc}_VALUE);
   }

   \$self->[0] = \$p;

   return \$self;
}

sub set
{
   my \$self = shift;
   my \$v = shift;

   my \$impl = \$self->_impl();

   if (\$v) {
      my \$new_value = Net::ICal::icalvalue_new_from_string(\$Net::ICal::ICAL_${uc}_VALUE,\$v);
      if (\$new_value){
         Net::ICal::icalvalue_free(\$self->[0]);
          \$self->[0] = \$new_value;
      }

   }

}

sub get
{
   my \$self = shift;
   my \$impl = \$self->[0];   

   if (defined \$impl){

     return  Net::ICal::icalvalue_as_ical_string(\$impl);

   }
}

EOM
}



}

if ($opt_p)
{
  print "1;\n";
}


if ($opt_h){

print <<EOM;
#endif /*ICALVALUE_H*/
EOM
}


__END__

