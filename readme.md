# TSParser

## Usage

    tsparse <source> [options]
    
      <source>                    Source where transport stream is read.
                                  Can be a file or multicast address on the
                                  form <ip>:<port>
      Options:
              -h                  help
              -t                  Print packets with pts
              -e                  Print packets with EBP markers
              -r                  Print packets with random access indicators
              -p <pid1,pid2...>   Print only packets with specified pids, comma separated
                                  list of pids. If no pids given, print a list of pids in
                                  transport stream
              -i <pkt1, pk2...>   Inspect (print) packet with given number, range or comma
                                  separated list of packets
              -s                  Print only payload start packets
              -x                  Print adaptation field and PES header
              -X                  Print packet payload in hex


## Todo

- Look over smart pointer usage, perhaps change to unique_ptr
- Implement search for sync-byte if not found at start

- Replace raw loops with std::algorithms
- Add unit test with catch framework

- rename to ints, inspects, bints, bjints, hints,  "inspect ts"
- Handle pts as a <chrono> to ease printout and calculations
- Group packets that belong to same frame.
- Print all packets in this payload (all packets with this pid util next payload start)
- Dump -d should dump payload (ES) of what is printed. Can be used to extract all packets for one pid or payload for one frame (jpeg-pic)

- Improve option parsing with error handling and required file argument.
- Calculate the length of the file.
- Indicate I-frame. and Dump I-frame data as a jpeg
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
