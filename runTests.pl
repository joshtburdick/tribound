#!/usr/bin/perl -w
# Runs some tests; this will probably require some tweaking.
# Assumes bc2cnf and minisat are in $PATH; if not, either 
# move them there, or alter the script.

use strict;

mkdir "minisatOutput";

open IN, "testCases.csv" || die;
$_ = <IN>;         # skip header
while (<IN>) {
  chomp;
  my @a = split /,/;

  my $f = "minisatOutput/bound_" . (join "_", @a) . ".txt";

  # only run cases which haven't been run yet
  if (! -e $f) {
    print "[writing $f]\n";
    system "./writeFormula1 " . (join " ", @a) . " | bc2cnf | minisat > $f";
  }
}
close IN;


