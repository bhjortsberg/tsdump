# TSParser

## Usage

    tsparse -f <file> [options]
        -h              help
        -t              print packets with pts
        -e              print only packets with EBP marker
        -r              print only packets with random access indentifier
        -p <p1,p2...>   print only packets with given pids. Can be a comma separated list of pids

## Todo

- Improve option parsing with error handling and required file argument.
- Calculate the length of the file.
- Group packets that belong to same frame.
- Indicate I-frame. and Dump I-frame data as a jpeg
- Read data from multicast address in addition to file.
- Payload extract. Class that sums the payloads from a bunch of (all) packets. Return a list of payloads.
  Payloads can then be used to get I-frame info and extract jpeg.

- -p without pid should print a list of pids in transport stream
- Display config to store what should be printed (for -x -xx, -p, -j and -i)
- -x and -xx or -X that prints extra information, the adaption field and the pes-header. And -xx (or -X) prints the payload in hex.
- -j dump I-frame as jpeg image
- -s for summary or statistics that prints number of packets and a list of pids.
- Or a hide statistics if statistics is to be printed always.
- list or range of packets to print
- add reference to previos packet to TSPacket. The previous packet with same pid. A step to collect payloads/print frame
- Multithreaded read file and print. Use <condition_variable> 

