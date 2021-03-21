savtocsv: savtocsv.o savtocsvlib.o savtocsvcommon.o
		gcc -o savtocsv savtocsv.o savtocsvlib.o savtocsvcommon.o -O3 -std=c99
savtocsv.o: savtocsv.c savtocsvlib.h
		gcc -c -g savtocsv.c -std=c99
savtocsvlib.o: savtocsvlib.c savtocsvlib.h
		gcc -c -g savtocsvlib.c -std=c99
savtocsvcommon.o: savtocsvcommon.c savtocsvcommon.h
		gcc -c -g savtocsvcommon.c -std=c99
.PHONY : clean
clean :
		-rm *.o $(objects)