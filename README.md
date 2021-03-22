# savtocsv
SPSS .sav to long format csv file (short format to come)

<h2>Install via compile savtocsv:</h2>

Installation has so far been tested on Debian (+Raspbian) and Redhat (+CentOs 7).

Download the repository to your chosen directory then run the make command. This will read the makefile where gcc compile standards have been set. The makefile will create several .o files to combine into an executable, then remove the .o files.

<pre># sudo curl -OL https://github.com/erbarratt/savtocsv/archive/refs/tags/savtocsv.1.2.tar.gz</pre>
<pre># sudo tar -xvf savtocsv.1.2.tar.gz</pre>
<pre># cd savtocsv-savtocsv.1.2</pre>
<pre># sudo make</pre>
<pre># sudo chmod 755 savtocsv</pre>

When finished, an executable called savtocsv will have been created. To cleanup:

<pre># sudo rm *.c</pre>
<pre># sudo rm *.h</pre>
<pre># sudo rm makefile</pre>
<pre># sudo rm *.txt</pre>

<h2>Usage</h2>

To call the program in it's current directory, use:

<pre>./savtocsv -[options]</pre>

<h3>Version</h3>

The version option is exclusive - the version output will only run if -v is the sole option passed.

<pre>./savtocsv -v</pre>

<h3>Input file</h3>

Use -f to define the input .sav file.

<pre>./savtocsv -f filename.sav</pre>

<h3>Output file prefix</h3>

Use -o to define the prefix for any output csv's. The program will append x.csv, where x is the number of the current csv file being written to, determined by the line limit option. Defaults to "out" (therefor out1.csv, out2.csv...).

<pre>./savtocsv  -f filename.sav -o prefix</pre>

<h3>CSV Line limit</h3>

Use -l to define how many rows should make up each output csv. Defaults to 1000000 (1 million). If output rows less, then only one file is generated. Program will check to see if this is a valid integer greater than 0.

<pre>./savtocsv  -f filename.sav -l 500000</pre>

<h3>Silent mode</h3>

Use -s to disable all output to stdout.

<pre>./savtocsv  -f filename.sav -s</pre>

<h3>Debug mode</h3>

Use -d to enable Debug mode, which will output all information read by the program from the .sav file. Debug mode will only output if not in silent mode.

<pre>./savtocsv  -f filename.sav -d</pre>

<h3>Full example</h3>

Set filename, output prefix and line limit, in silent mode.

<pre>./savtocsv  -f filename.sav -o prefix -l 5000000 -s</pre>
