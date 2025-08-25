################################################################################
# SPDX-FileCopyrightText: 2000, Eric Busboom <eric@civicknowledge.com>
# SPDX-License-Identifier: LGPL-2.1-only OR MPL-2.0
################################################################################

sub read_values_file
{

  my $path = shift;
  my %h;
  my @SEEN_ENUMS;

  open(F, $path) || die "Can't open values file $path";

  my $line = 0;
  while (<F>) {
    $line++;

    chop;

    s/#.*$//g;
    s/\"//g;
    s/\r//g;

    next if !$_;

    @column = split(/,/, $_);

    my $value_name = $column[0];
    if (exists($h{$value_name})) {
      die "Previously defined value=$value_name line $line in $path";
    }
    my $enumConst = $column[1];
    if ($enumConst !~ /FIXME/) {
      if (grep(/^$enumConst$/, @SEEN)) {
        die "Reusing kindEnum=$enumConst line $line in $path";
      }
      push(@SEEN, $enumConst);
    }
    my $c_type_str = $column[2];
    my $c_autogen  = ($c_type_str =~ /\(a\)/);

    my $c_type = $c_type_str;
    $c_type =~ s/\(.\)//;

    my $python_type = $column[3];
    my $components  = $column[4];
    my $enum_values = $column[5];

    my @components;
    if ($components ne "unitary") {
      @components = split(/;/, $components);
    } else {
      @components = ();
    }

    my @enums;
    if ($enum_values) {
      @enums = split(/;/, $enum_values);

    } else {
      @enums = ();
    }

    $h{$value_name} = {
      C          => [$c_autogen, $c_type],
      kindEnum   => $enumConst,
      perl       => $perl_type,
      python     => $python_type,
      components => [@components],
      enums      => [@enums]
    };
  }

  return %h;
}

sub read_properties_file
{

  my $path = shift;
  my %h;
  my @SEEN;

  open(F, $path) || die "Can't open properties file $path";

  my $line = 0;
  while (<F>) {
    $line++;
    chop;

    s/#.*$//g;
    s/\"//g;
    s/\r//g;

    next if !$_;

    @column = split(/,/, $_);

    my $property_name = $column[0];
    if ($property_name && exists($h{$property_name})) {
      die "Previously defined property=$property_name line $line in $path";
    }
    my $enumConst = $column[1];
    if ($enumConst !~ /FIXME/) {
      if (grep(/^$enumConst$/, @SEEN)) {
        die "Reusing kindEnum=$enumConst line $line in $path";
      }
      push(@SEEN, $enumConst);
    }
    my $lic_value     = $column[2];
    my $default_value = $column[3];
    my $flags         = $column[4];

    my @flags;
    if ($flags ne "") {
      @flags = split(/;/, $flags);
    } else {
      @flags = ();
    }

    $h{$property_name} = {
      lic_value     => $lic_value,
      kindEnum      => $enumConst,
      default_value => $default_value,
      flags         => [@flags]
    };
  }

  return %h;
}

sub read_parameters_file
{

  my $path = shift;
  my %h;
  my @SEEN;

  open(F, $path) || die "Can't open parameters file $path";

  my $line = 0;
  while (<F>) {
    $line++;

    chop;

    s/#.*$//g;
    s/\"//g;
    s/\r//g;

    next if !$_;

    @column = split(/\,/, $_);

    my $parameter_name = $column[0];
    if (exists($h{$parameter_name})) {
      die "Previously defined parameter=$parameter_name line $line in $path";
    }
    my $enumConst = $column[1];
    if ($enumConst !~ /FIXME/) {
      if (grep(/^$enumConst$/, @SEEN)) {
        die "Reusing kindEnum=$enumConst line $line in $path";
      }
      push(@SEEN, $enumConst);
    }
    my $data_type   = $column[2];
    my $enum_string = $column[3];
    my $flags       = $column[4];

    my @enums;
    if ($enum_string) {
      @enums = split(/;/, $enum_string);
    }

    my @flags;
    if ($flags ne "") {
      @flags = split(/;/, $flags);
    } else {
      @flags = ();
    }

    $h{$parameter_name} = {
      C        => $data_type,
      kindEnum => $enumConst,
      enums    => [@enums],
      flags    => [@flags]
    };
  }

  close(F);

  return %h;
}

1;
