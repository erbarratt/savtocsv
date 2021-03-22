# savtocsv
SPSS .sav to short or long format csv file

<h2>Install via compile savtocsv:</h2>

Installation has so far been tested on Debian (+Raspbian) and Redhat (+CentOs 7).

Download the repository to your chosen directory then run the make command. This will read the makefile where gcc compile standards have been set. The makefile will create several .o files to combine into an executable, then remove the .o files.

<pre># sudo curl -OL https://github.com/erbarratt/savtocsv/archive/refs/tags/savtocsv.1.5.4.tar.gz</pre>
<pre># sudo tar -xvf savtocsv.1.5.4.tar.gz</pre>
<pre># cd savtocsv-savtocsv.1.5.4</pre>
<pre># sudo make</pre>
<pre># sudo chmod 755 savtocsv</pre>

When finished, an executable called savtocsv will have been created. To cleanup:

<pre># sudo rm *.c</pre>
<pre># sudo rm *.h</pre>
<pre># sudo rm makefile</pre>
<pre># sudo rm *.txt</pre>

Verify installation using the version option:

<pre># ./savtocsv -v</pre>

Should output similar to:

<pre>savtocsv version version 1.4.1 2021-03-22</pre>

<h2>Usage</h2>

To call the program in it's current directory, use:

<pre># sudo ./savtocsv [options] [arguments]</pre>

sudo is needed for being able to write files correctly.

<h3>Help</h3>

The help option is exclusive - the help output will only run if -h is the sole option passed. Help lists all available options.

<pre># ./savtocsv -h</pre>

<h3>Version</h3>

The version option is exclusive - the version output will only run if -v is the sole option passed.

<pre># ./savtocsv -v</pre>

<h3>Input file*</h3>

Use -f to define the input .sav file.

*This option is required

<pre># sudo ./savtocsv -f filename.sav</pre>

<h3>Output file prefix</h3>

Use -o to define the prefix for any output csv's. The program will append x.csv, where x is the number of the current csv file being written to, determined by the line limit option. Defaults to "out" (therefor out1.csv, out2.csv...).

<pre># sudo ./savtocsv  -f filename.sav -o prefix</pre>

<h3>CSV Line limit</h3>

Use -l to define how many rows should make up each output csv. Defaults to 1000000 (1 million). If output rows less, then only one file is generated. Program will check to see if this is a valid integer greater than 0.

<pre># sudo ./savtocsv  -f filename.sav -l 500000</pre>

<h3>Silent mode</h3>

Use -s to disable all output to stdout.

<pre># sudo ./savtocsv  -f filename.sav -s</pre>

<h3>Debug mode</h3>

Use -d to enable Debug mode, which will output all information read by the program from the .sav file. Debug mode will only output if not in silent mode.

<pre># sudo ./savtocsv  -f filename.sav -d</pre>

<h3>Flat csv</h3>

Use -F to output csv files as flat table style. Default is long format csv (many to many).

<pre># sudo ./savtocsv  -f filename.sav -F</pre>

<h3>Row index</h3>

Use -R to output the row index as the first field in each line of the csv.

<pre># sudo ./savtocsv  -f filename.sav -R</pre>

<h3>Full example</h3>

Set filename, output prefix and line limit, in silent mode, as a flat style csv including row indexes.

<pre># sudo ./savtocsv  -f filename.sav -o prefix -l 5000000 -sFR</pre>
