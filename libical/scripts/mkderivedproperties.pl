#!/usr/local/bin/perl

require "readvaluesfile.pl";

use Getopt::Std;
getopts('chspmi:');

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

# ARG 1 is value-types.txt
%valuemap  = read_values_file($ARGV[1]);


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

    print "/* Everything below this line is machine generated. Do not edit. */\n";


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

  my $type = $valuemap{$value}->{C}->[1];
  
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
icalproperty* icalproperty_new_${lc}($type v) {
   struct icalproperty_impl *impl = icalproperty_new_impl(ICAL_${uc}_PROPERTY);   $pointer_check
   icalproperty_set_${lc}((icalproperty*)impl,v);
   return (icalproperty*)impl;
}
icalproperty* icalproperty_vanew_${lc}($type v, ...){
   va_list args;
   struct icalproperty_impl *impl = icalproperty_new_impl(ICAL_${uc}_PROPERTY);   $pointer_check
   icalproperty_set_${lc}((icalproperty*)impl,v);
   va_start(args,v);
   icalproperty_add_parameters(impl, args);
   va_end(args);
   return (icalproperty*)impl;
}
void icalproperty_set_${lc}(icalproperty* prop, $type v){
    icalvalue *value;
    $set_pointer_check
    icalerror_check_arg_rv( (prop!=0),"prop");
    value = icalvalue_new_${lcvalue}(v);
    icalproperty_set_value(prop,value);
}
$type icalproperty_get_${lc}(icalproperty* prop){
    icalvalue *value;
    icalerror_check_arg( (prop!=0),"prop");
    value = icalproperty_get_value(prop);
    return icalvalue_get_${lcvalue}(value);
}
EOM


  } elsif ($opt_h) { # Generate C Header file
 print "\
/* $prop */\
icalproperty* icalproperty_new_${lc}($type v);\
icalproperty* icalproperty_vanew_${lc}($type v, ...);\
void icalproperty_set_${lc}(icalproperty* prop, $type v);\
$type icalproperty_get_${lc}(icalproperty* prop);";
  
} 


} # This brace terminates the main loop



if ($opt_h){

print "\n\n#endif /*ICALPROPERTY_H*/"
}

