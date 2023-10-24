#!/usr/bin/env perl
#
# Script to fix some issues in Conda's Perl installation:
#   $installarchlib/Config_heavy.pl
#     * Remove --sysroot=... parameter from ccflags= and cppflags=
#   $installarchlib/CORE/config.h
#     * Comment out #define I_XLOCALE
#
# We have found these changes necessary to be able to build and/or run
# EPICS Base using the Conda versions of Perl 5.26.x
#
# NOTE: This script modifies the above files in-place, without making
# backups of them. If you don't trust my code, make your own backups
# first, or be prepared to reinstall Perl.

use strict;
use Config;
use File::Basename;

my $tool = basename($0);

my $archdir = $Config{installarchlib};
die "$tool: installarchlib='$archdir' doesn't exist\n"
    unless -d $archdir;

# $archdir/Config_heavy.pl
# 
my $ccflags = $Config{ccflags};
my $cppflags = $Config{cppflags};
if ($ccflags =~ m(--sysroot=) ||
    $cppflags =~ m(--sysroot=)) {

    sed("$archdir/Config_heavy.pl",
        sub { s( ?--sysroot=[^ ']+)( )g; });
}

# $archdir/CORE/config.h
#
sed("$archdir/CORE/config.h",
    sub { s[^#define\s+I_XLOCALE\s+/][/*$& ]; });
sed("$archdir/CORE/config.h",
    sub { s[^#define\s+HAS_NEWLOCALE\s+/][/*$& ]; })
    if $Config{archname} eq 'darwin-thread-multi-2level';

sub sed {
    my ($filename, $edsub) = @_;

    die "$tool: File $filename doesn't exist\n"
        unless -w $filename;
    open my $file, '+<', $filename or
        die "$tool: Can't open $filename: $!\n";
    my @text = <$file>;
    seek $file, 0, 0;
    &$edsub foreach @text;
    print $file @text;
    truncate $file, tell($file);
    close $file or
        die "$tool: Problem closing $filename: $!\n";
}    
