#!/usr/local/bin/perl

use Getopt::Std;
getopts('chsypmi:');

# ARG 0 is prop-to-value
open(PV,"$ARGV[0]") || die "Can't open prop to value  file $ARGV[0]:$!";


while (<PV>){

  chop;
  my @v = split(/\s+/,$_);  
  
  my $prop = shift @v;
  my $value = shift @v;
  my $comment = join(" ",@v);
  
  $propmap{$prop} = $value;
}

close PV;

# ARG 1 is value-c-types.txt
open(F,"$ARGV[1]") || die "Can't open  C parameter types file $ARGV[1 ]:$!";

while (<F>){

  chop;
  my @v = split(/\t+/,$_);  
  
  my $value = shift @v;
  my $mode = shift @v;
  my $type = shift @v;
  my $comment = join(" ",@v);

 $valuemap{$value} = $type;

}

close F;

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

  
if ( ($opt_h or $opt_s) and !$opt_i ){

print <<EOM;
/* -*- Mode: C -*-
  ======================================================================
  FILE: icalderivedproperties.{c,h}
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
 * THIS FILE IS MACHINE GENERATED. DO NOT EDIT
 */


EOM

}


if ($opt_p and !$opt_i){

print <<EOM;

EOM

}

foreach $prop (keys %propmap) {

  next if !$prop;

  my $value = $propmap{$prop};

  if (!$value){
    die "Can't find value for property \"$prop\"\n";
  }

  my $ucf = join("",map {ucfirst(lc($_));}  split(/-/,$prop));
  my $lc = lc($ucf);
  my $uc = uc($lc);

  my $ucfvalue = join("",map {ucfirst(lc($_));}  split(/-/,$value));
  my $lcvalue = lc($ucfvalue);
  my $ucvalue = uc($lcvalue);

  my $type = $valuemap{$value};
  
  my $pointer_check;
  if ($type =~ /\*/){
    $pointer_check = "icalerror_check_arg_rz( (v!=0),\"v\");\n" if $type =~ /\*/;
  } elsif ( $type eq "void" ){
    $pointer_check = "icalerror_check_arg_rv( (v!=0),\"v\");\n" if $type =~ /\*/;

  }    

  my $set_pointer_check = "icalerror_check_arg_rv( (v!=0),\"v\");\n" if $type =~ /\*/;

  if($opt_c) { # Generate C source
 print<<EOM;

/* $prop */

icalproperty* icalproperty_new_${lc}($type v)
{
   struct icalproperty_impl *impl = icalproperty_new_impl(ICAL_${uc}_PROPERTY);  
   $pointer_check

   icalproperty_set_${lc}((icalproperty*)impl,v);

   return (icalproperty*)impl;
}

icalproperty* icalproperty_vanew_${lc}($type v, ...)
{
   va_list args;
   struct icalproperty_impl *impl = icalproperty_new_impl(ICAL_${uc}_PROPERTY);  
   $pointer_check

   icalproperty_set_${lc}((icalproperty*)impl,v);

   va_start(args,v);
   icalproperty_add_parameters(impl, args);
   va_end(args);

   return (icalproperty*)impl;
}
 
void icalproperty_set_${lc}(icalproperty* prop, $type v)
{
    icalvalue *value;
   
    $set_pointer_check
    icalerror_check_arg_rv( (prop!=0),"prop");

    value = icalvalue_new_${lcvalue}(v);

    icalproperty_set_value(prop,value);

}

$type icalproperty_get_${lc}(icalproperty* prop)
{
    icalvalue *value;
    icalerror_check_arg( (prop!=0),"prop");

    value = icalproperty_get_value(prop);

    return icalvalue_get_${lcvalue}(value);
}
EOM


  } elsif ($opt_h) { # Generate C Header file
 print<<EOM;

/* $prop */
icalproperty* icalproperty_new_${lc}($type v);
icalproperty* icalproperty_vanew_${lc}($type v, ...);
void icalproperty_set_${lc}(icalproperty* prop, $type v);
$type icalproperty_get_${lc}(icalproperty* prop);
EOM
  
} elsif ($opt_s) { # Generate case in a switch statement

print <<EOM;
case ICAL_${uc}_PROPERTY:
EOM


} elsif ($opt_p) { # Generate perl code

print <<EOM;

package Net::ICal::Property::${ucf}; 
use Net::ICal::Property;
\@ISA=qw(Net::ICal::Property);
sub new
{
   my \$package = shift;

   my \$p = Net::ICal::icalproperty_new(\$Net::ICal::ICAL_${uc}_PROPERTY);
   my \$self = Net::ICal::Property::new_from_ref(\$p);

   \$self->_add_elements(\\\@_);

   return bless \$self, \$package;
}

sub set_value
{
   my \$self = shift;
   my \$v = shift;

   my \$impl = \$self->_impl();


   if ( ref \$v && Net::ICal::icalvalue_isa_value(\$v->_impl())){
      Net::ICal::icalproperty_set_value(\$impl,\$v->_impl);
   } else { 
      my \$value = Net::ICal::icalvalue_new_from_string(\$Net::ICal::ICAL_${ucvalue}_VALUE,\$v);
      die if !\$impl;
      Net::ICal::icalproperty_set_value(\$impl,\$value) unless !\$value;
   }

}

sub get_value
{
   my \$self = shift;
   my \$impl = \$self->_impl();   

   if (defined \$impl){
     my \$value = Net::ICal::icalproperty_get_value(\$impl);
     return "" if !\$value;
     return  Net::ICal::icalvalue_as_ical_string(\$value);
   } else {
      return "";
   }
}

EOM

} elsif ($opt_m) { # Generate a map

print "\'${uc}\' => \'Net::ICal::Property::${ucf},\'\n";

}

} # This brace terminates the main loop



# Add things to the end of the output.

if ($opt_p)
{
  print "1;\n";
}

if ($opt_h){

print <<EOM;
#endif /*ICALPROPERTY_H*/
EOM
}

