#!/usr/bin/perl -w
# Summarizes results.

use strict;

print "num. vertices,num. gates,num. random cases,wires sorted?,satisfiable?,CPU time\n";
foreach my $f (<minisatOutput/*.txt>) {
  my($skip, $n, $g, $random_cases, $wires_sorted) = split /_/, $f;
  my $sat = "-";
  my $t = "";

  $wires_sorted = $wires_sorted eq "1" ? "yes" : "no";

  open IN, "<$f" || die;
  while(<IN>) {
    chomp;
    if (/^SATISFIABLE/) {
      $sat = "yes";
    }
    if (/^UNSATISFIABLE/) {
      $sat = "no";
    }
    if (/CPU time.*: (.*)/) {
      $t = $1;
    }
  }

  close IN;

  print join ",", ($n, $g, $random_cases, $wires_sorted, $sat, $t);
  print "\n";
}


