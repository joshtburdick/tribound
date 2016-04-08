
This is a sketch of C++ code which writes out formulas
(in DIMACS format) for finding NAND-gate circuits
which detect triangles in _tiny_ graphs.

It depends on the bc2cnf tool, from the BC package,
available online at

http://users.ics.aalto.fi/tjunttil/circuits/

A very small number of instances were tested using minisat,
available from

http://minisat.se

See bruteForce1.pdf for a description of the strategy, and
a tiny number of results.

Josh Burdick
jburdick@keyfitz.org

