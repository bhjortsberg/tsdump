# TSParser



## Todo:

- A Printer class that takes a filter as a parameter and outputs data nicely formated and filtered.
- Parse arguments with error handling. Specify filters from command line.
- PTS in wall clock time format
- Calculate the length of the file.
- Group packets that belong to same frame.
- Indicate I-frame. and Dump I-frame data as a jpeg
- Read data from multicast address instead of file.

- filter() should return bool wheather it should be filtered or not
- TSReport should have a function that prints the line. Should be called if filter() return false and prints whole line
- The print line function should be simplified compared to how filter is implemented. Just add with what info is found  - pid pts ptsStr ebp rai

- -p without pid should print a list of pids in transport stream
- Display config to store what should be printed (for -x -xx, -p, -j and -i)
- -x and -xx or -X that prints extra information, the adaption field and the pts-header. And -xx (or -X) prints the payload in hex.
- Single packet print on packet number i(inspect).
- -j dump I-frame as jpeg image
- -s for summary or statistics that prints number of packets and a list of pids.
- Or a hide statistics if statistics is to be printed always.