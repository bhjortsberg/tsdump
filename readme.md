# TSParser

## Usage

    tsparse [options] file
    
    Options:
        -h              help
        -t              print packets with pts
        -e              print only packets with EBP marker
        -r              print only packets with random access indentifier
        -p <p1,p2...>   print only packets with given pids. Can be a comma separated list of pids
        -i <i1,i2...>   Inspect packets. Can be comma separated list of packet number
        -s              Print only payload start packets
        -x              Print extra info, adaption field and PES header
        -X              Print packet payload in hex

## Todo

- list or range of packets to print
- Handle pts as a <chrono> to ease printout and calculations
- Multithreaded read file and print. Use <condition_variable> 

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
- add reference to previos packet to TSPacket. The previous packet with same pid. A step to collect payloads/print frame
- Dump -d should dump payload of what is printed. Can be used to extract all packets for one pid or payload for one frame (jpeg-pic)
- Print all packets in this payload (all packets with this pid util next payload start)
- write file of one pid to an mp4-file.
