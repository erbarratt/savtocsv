# savtocsv
SPSS .sav to long format csv file (short format to come)

<h3>Install via compile savtocsv:</h3>

Installation has so far been tested on Debian (+Raspbian) and Redhat (+CentOs 7).

Download the repository to your chosen directory then run the make command. This will read the makefile where gcc compile standards have been set. The makefile will create several .o files to combine into an executable, then remove the .o files.

<pre># sudo make</pre>

When finished, an executable called savtocsv will have been created. To cleanup:

<pre># sudo rm -f *.c</pre>
<pre># sudo rm -f *.h</pre>
<pre># sudo rm -f makefile</pre>

