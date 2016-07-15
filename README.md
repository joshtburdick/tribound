
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

20160711, jtb - in bruteForce1.pdf, I originally wrote that
I wasn't sure whether the "naive" triangle-detecting circuit
with ${n \choose 3} + 1$ NAND gates could be improved on.
I thought that faster matrix multiplication algorithms
(such as Strassen's) would require calculating intermediate
numbers which are very large. However, as noted in CLR
(section FIXME), we can do the additions and multiplications
mod n+1 (when multiplying Boolean n-by-n matrices.) This will
beat the ${n \choose 3} + 1$ bound, eventually (as will
the faster matrix multiplication algorithms.)

Josh Burdick
jburdick@keyfitz.org

