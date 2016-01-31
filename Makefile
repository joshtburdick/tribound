
all: bruteForce1.pdf

CXXFLAGS = -O6 -Wall -g -D_GLIBCXX_DEBUG

bruteForce1.pdf : bruteForce1.tex runtimes.csv
	pdflatex bruteForce1.tex
	bibtex bruteForce1.aux
	pdflatex bruteForce1.tex

runtimes.csv : testCases.csv writeFormula1 runTests
	./summarize.pl > runtimes.csv

writeFormula1: writeFormula1.cc
	$(CXX) $(CXXFLAGS) -o writeFormula1 writeFormula1.cc 

.PHONY: runTests
runTests:
	./runTests.pl

clean:
	rm -rf *.o core *~ *.bak writeFormula1

