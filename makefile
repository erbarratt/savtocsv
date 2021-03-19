savtocsv: savtocsv.o savtocsvlib.o savtocsvcommon.o
		gcc -o savtocsv savtocsv.o savtocsvlib.o savtocsvcommon.o
savtocsv.o: savtocsv.c savtocsvlib.h
		gcc -c -g savtocsv.c
savtocsvlib.o: savtocsvlib.c savtocsvlib.h
		gcc -c -g savtocsvlib.c
savtocsvcommon.o: savtocsvcommon.c savtocsvcommon.h
		gcc -c -g savtocsvcommon.c
.PHONY : clean
clean :
		-rm *.o $(objects)