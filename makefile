.PHONY: all
all: savtocsv
savtocsv: savtocsv.o savtocsvlib.o savtocsvcommon.o
		gcc -Wextra -Wconversion -pedantic -o savtocsv savtocsv.o savtocsvlib.o savtocsvcommon.o -O3 -std=gnu99;
savtocsv.o: savtocsv.c savtocsvlib.h
		gcc -Wextra -Wconversion -pedantic -c -g savtocsv.c -O3 -std=gnu99;
savtocsvlib.o: savtocsvlib.c savtocsvlib.h
		gcc -Wextra -Wconversion -pedantic -c -g savtocsvlib.c -O3 -std=gnu99;
savtocsvcommon.o: savtocsvcommon.c savtocsvcommon.h
		gcc -Wextra -Wconversion -pedantic -c -g savtocsvcommon.c -O3 -std=gnu99;
clean :
		rm *.o