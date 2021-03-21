savtocsv: savtocsv.o savtocsvlib.o savtocsvcommon.o
		gcc -o savtocsv savtocsv.o savtocsvlib.o savtocsvcommon.o -O3 -std=gnu90;
		-rm *.o $(objects)
savtocsv.o: savtocsv.c savtocsvlib.h
		gcc -c -g savtocsv.c -std=gnu90;
savtocsvlib.o: savtocsvlib.c savtocsvlib.h
		gcc -c -g savtocsvlib.c -std=gnu90;
savtocsvcommon.o: savtocsvcommon.c savtocsvcommon.h
		gcc -c -g savtocsvcommon.c -std=gnu90;