#=============================================================================
#
# This package is free software and is provided "as is" without express
# or implied warranty. It may be used, redistributed and/or modified
# under the same terms as perl itself. (Either the Artistic License or
# the GPL.)
#
#=============================================================================

=head1 NAME

Net::ICal::Duration -- represent a length of time

=head1 SYNOPSIS

  use Net::ICal;
  $d = Net::ICal::Duration->new("P3DT6H15M10S");
  $d = Net::ICal::Duration->new(3600); # 1 hour in seconds

=head1 DESCRIPTION

I<Duration> Represents a length of time, such a 3 days, 30 seconds or
7 weeks. You would use this for representing an abstract block of
time; "I want to have a 1-hour meeting sometime." If you want a
calendar- and timezone-specific block of time, see Net::ICal::Period.

=cut

#=============================================================================

package Net::ICal::Duration;
use strict;
use Carp;


=head1 METHODS

=head2 new

Create a new I<Duration> from:

=over 4

=item * A string in RFC2445 duration format

=item * An integer representing a number of seconds

=cut

sub new {
  my ($class, $value) = @_;
  my $seconds = 0;

  if ($value =~ /^[-+]?\d+$/) {
    $seconds = int $value;
  } else {
    $seconds = _string_to_seconds($value);
  }

  return bless \$seconds, $class;
}


# Converts a RFC2445 DURATION format string to an integer number of
# seconds.

sub _string_to_seconds {
  my $str = shift;
  my $seconds = 0;

  if ($str =~ /^([-+])?P(\d+)W$/) {
    $seconds += $2 * 7*24*60*60;
    $seconds *= -1 if defined $1 && $1 eq '-';
  }
  elsif ($str =~ m{
                   ^([-+])?P(?=\d|T)
                   (?:(\d+)D)?
                   (?:T(?=\d)(?:(\d+)H)?(?:(\d+)M)?(?:(\d+)S)?)?$
                  }x) {
    # Minutes should be present (maybe zero) if hours and seconds
    carp "Bad time format: $str" if defined $3 && defined $5 && ! defined $4;
    $seconds += $2 * 24*60*60 if $2;
    $seconds += $3 * 60*60    if $3;
    $seconds += $4 * 60       if $4;
    $seconds += $5            if $5;
    $seconds *= -1            if defined $1 && $1 eq '-';
  }
  else {
    carp "Invalid duration: $str";
  }

  return $seconds;
}



=head2 clone()

Return a new copy of the duration.

=cut

sub clone {
  my $self = shift;
  my $class = ref ($self);
  return $class->new($$self);
}



=head2 is_valid()

Determine if this is a valid duration (given criteria TBD). 

=cut

# XXX implement this
sub is_valid { }



=head2 as_int()

Return the length of the duration as seconds. 

=cut

sub as_int { ${$_[0]} }



=head2 as_ical_value()

Return the duration in an RFC2445 format string.

=cut

sub as_ical_value {
  my $self = shift;

  my $n       = abs $$self;
  my $days    = int $n / (60*60*24);
  my $hours   = $n / (60*60) % 24;
  my $minutes = $n / 60 % 60;
  my $seconds = $n % 60;

  return ($$self < 0 ? "-" : "") . "P" .
         ($days ? "${days}D" : "") .
         ($hours || $minutes || $seconds ? "T" : "") .
         ($hours ? "${hours}H" : "") .
         ($minutes || $hours && $seconds ? "${minutes}M" : "") .
         ($seconds ? "${seconds}S" : "");
}



=head2 add($duration)

Return a new duration that is the sum of this and $duration. Does not
modify this object.

=cut

sub add {
  my ($self, $duration) = @_;

  my $class = ref ($self);
  return $class->new($$self + $$duration);
}


=head2 subtract($duration)

Return a new duration that is the difference between this and
$duration. Does not modify this object.

=cut

sub subtract {
  my ($self, $duration) = @_;

  my $class = ref ($self);
  return $class->new($$self - $$duration);
}


1;
