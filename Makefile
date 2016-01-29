
all: bruteForce1.pdf

CXXFLAGS = -O6 -Wall -g -D_GLIBCXX_DEBUG

bruteForce1.pdf : bruteForce1.tex runtimes.csv
	pdflatex bruteForce1.tex

runtimes.csv : testCases.csv runTests
	./summarize.pl > runtimes.csv


writeFormula1: writeFormula1.cc
	$(CXX) $(CXXFLAGS) -o writeFormula1 writeFormula1.cc 


.PHONY: runTests
runTests:


clean:
	rm -rf *.o core *~ *.bak writeFormula1

