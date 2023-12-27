CFLAGS = -O3 -Wall -std=c++11
CFLAGS = -g -Wall -std=c++11

nonogram: nonogram.o main.o
	g++ -o $@ nonogram.o main.o $(CFLAGS)
#	strip $@

main.o: main.cpp nonogram.h
	g++ -c main.cpp $(CFLAGS)

nonogram.o: nonogram.cpp nonogram.h
	g++ -c nonogram.cpp $(CFLAGS)

# json:
# 	cd jsonGen; g++ -o jgen jsonGen.cpp $(CFLAGS)
# 	ln -sf jsonGen/jgen .

clean:
	rm -f nonogram *.o