# TSParser

## Usage

    tsparse [options] file
    
    Options:
        -h              help
        -t              print packets with pts
        -e              print only packets with EBP marker
        -r              print only packets with random access indentifier
        -p <p1,p2...>   print only packets with given pids. Can be a comma separated list of pids
        -i <i1,i2...>   Inspect packets. Range or comma separated list of packet numbers
        -s              Print only payload start packets
        -x              Print extra info, adaptation field and PES header
        -X              Print packet payload in hex

## Todo

- Handle pts as a <chrono> to ease printout and calculations
- Multithreaded read file and print. Use <condition_variable> 
- Group packets that belong to same frame.
- Print all packets in this payload (all packets with this pid util next payload start)
- Dump -d should dump payload (ES) of what is printed. Can be used to extract all packets for one pid or payload for one frame (jpeg-pic)
- Replace raw loops with std::algorithms
- Add unit test with catch framework

- Improve option parsing with error handling and required file argument.
- Calculate the length of the file.
- Indicate I-frame. and Dump I-frame data as a jpeg
- Read data from multicast address in addition to file.
- Payload extract. Class that sums the payloads from a bunch of (all) packets. Return a list of payloads.
  Payloads can then be used to get I-frame info and extract jpeg.

- -p without pid should print a list of pids in transport stream
- Display config to store what should be printed (for -x -xx, -p, -j and -i)
- -x and -xx or -X that prints extra information, the adaptation field and the pes-header. And -xx (or -X) prints the payload in hex.
- -j dump I-frame as jpeg image
- -s for summary or statistics that prints number of packets and a list of pids.
- Or a hide statistics if statistics is to be printed always.
- write file of one pid to an mp4-file.


## Bugs
- Summary printout for this

    $ tsparse -p 8191  ~/vcp-origin/tvb/flash-1.ts
