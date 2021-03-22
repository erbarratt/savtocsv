savtocsv: savtocsv.o savtocsvlib.o savtocsvcommon.o
		gcc -Wall -o savtocsv savtocsv.o savtocsvlib.o savtocsvcommon.o -O3 -std=gnu90;
		-rm *.o $(objects)
savtocsv.o: savtocsv.c savtocsvlib.h
		gcc -Wall -c -g savtocsv.c -O3 -std=gnu90;
savtocsvlib.o: savtocsvlib.c savtocsvlib.h
		gcc -Wall -c -g savtocsvlib.c -O3 -std=gnu90;
savtocsvcommon.o: savtocsvcommon.c savtocsvcommon.h
		gcc -Wall -c -g savtocsvcommon.c -O3 -std=gnu90;
