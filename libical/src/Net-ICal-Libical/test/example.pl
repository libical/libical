#!/usr/bin/perl
#
# Example of usig Net::iCal
#
#

$compstr = get_string_from_somewhere();

$comp = Net::ICal::Component($compstr);

@errors = $comp->errors();

if($#errors){
  print "There were parsing errors\n";
  foreach $e (@errors){
    # Suppose imaginary error object
    print "$e->type $edebug_text\n";
  }

  exit;
}

# If the component is a request, and it is during lunch in my
# timezone, then counterpropose that it be moved to 1:00

if($c->method() eq 'REQUEST' &&
   $c->span()->start()->hour() >= 12 &&
    $c->span()->start()->hour() < 13 ){
     # Not another damn lunch meeting! Create a new copy of the 
     # request, but change the span to after lunch. 

     my $counter = $c->clone;
     
     $counter->span($c->span());
     $counter->span->start()->hour(13);

     # The new span should have the same duration as the old one
     $counter->span->duration($c->span()->duration());

     $counter->comment("I'd like to meet after lunch");

     $counter->method('COUNTER');

     send_counter_back_to_net($counter);

}

