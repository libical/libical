#!/usr/bin/perl
# -*- Mode: perl -*-
#======================================================================
# FILE: Property.pm
# CREATOR: eric 1 Mar 01
#
# DESCRIPTION:
#   
#
#  $Id: Property.pm,v 1.1 2001-03-02 19:50:31 ebusboom Exp $
#  $Locker:  $
#
# (C) COPYRIGHT 2000, Eric Busboom, eric@softwarestudio.org
#
# This package is free software and is provided "as is" without express
# or implied warranty.  It may be used, redistributed and/or modified
# under the same terms as perl itself. ( Either the Artistic License or the
# GPL. ) 
#
#
#======================================================================

use Net::ICal::Libical::Property;


package Net::ICal::Libical::Property;

sub new { 

  my $class = shift;
  my $arg = shift;
  my $self = {};

  if(ref($arg) == HASH){
  
    $self->{'ref'} = $arg->{'ref'};

  } else {
        $kind = Net::ICal::Libical::icalproperty_string_to_kind($arg);
	$self->{'ref'} = Net::ICal::Libical::icalproperty_new($kind);
  }

  die if !$self->{'ref'};
  
  bless $self, $class;
}


sub DESTROY {
  my $self = shift;

  my $r = $self->{'ref'};

  if($r && !Net::ICal::Libical::icalproperty_get_parent($r)){
    Net::ICal::Libical::icalproperty_free($self->{'ref'});
  }
}

sub name {
  my $self = shift;

  die if !$self->{'ref'};

  $str = Net::ICal::Libical::icalproperty_as_ical_string($self->{'ref'});
	
  $str =~ /^([A-Z\-]+)\n/;

  return $1;

}

#Get/Set the internal reference to the libical icalproperty """
sub prop_ref {
  my $self = shift;
  my $p_r = shift;

  if($p_r){
    $self->{'ref'} = $p_r;
  }

  return $self->{'ref'};
  
}


#Get/set the RFC2445 representation of the value. Dict value 'value'
sub value {
  my $self = shift;
  my $v = shift;
  my $kind = shift;
  
  if($v){
            
    if ($kind) {
      $self->{'VALUE'} = $kind;
      $vt = $kind;
    }
    elsif ($self->{'VALUE'}) { 
      $vt = $self->{'VALUE'};
    }
    else {
      $vt = 'NO'; # Use the kind of the existing value
    }      
     

    Net::ICal::Libical::icalproperty_set_value_from_string($self->{'ref'},$v,$vt);
      
  }
      
  return Net::ICal::Libical::icalproperty_get_value_as_string($self->{'ref'});

}


# Get a named parameter
sub get_parameter{
  my $self  = shift;
  my $key = shift;

  $key = uc($key);
  my $ref = $self->{'ref'};

  my $str = Net::ICal::Libical::icalproperty_get_parameter_as_string($ref,$key);

  if($str eq 'NULL') {
    return undef;
  }

  return $str  

}


# Set the value of the named parameter
sub set_parameter{
  my $self  = shift;
  my $key = shift;
  my $value = shift;

  $key = uc($key);
  my $ref = $self->{'ref'};

  my $str = Net::ICal::Libical::icalproperty_set_parameter_from_string($ref,$key,$value);


  return $self->get_parameter($self);

}


sub as_ical_string {
  my $self = shift;
  my $str = Net::ICal::Libical::icalproperty_as_ical_string($self->{'ref'});

  $str =~ s/\r//g;
  $str =~ s/\n\s?//g;

  return $str;
}



1;
