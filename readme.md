# TSDump

A command line tool to inspect MPEG-TS transport streams. Will print every
packet in the transport stream and allows user to specify filtering options for
inspecting different levels of details. Able to parse MPEG-TS from a file or
from a multicast stream.

## Usage

    tsdump <source> [options]

      <source>                    Source where transport stream is read.
                                  Can be a file or multicast address on the
                                  form <ip>:<port>
      Options:
              -h                  help
              -t                  Print packets with pts
              -e                  Print packets with EBP markers
              -r                  Print packets with random access indicators
              -p <pid1,pid2...>   Print only packets with specified pids, comma separated
                                  list of pids
              --pids              Print a list of pids in the transport stream
              -i <pkt1, pk2...>   Inspect (print) packet with given number, range or comma
                                  separated list of packets
              -s                  Print only payload start packets
              -w <file>           Write data for printed packets to <file>
              -x                  Print adaptation field and PES header
              -X                  Print packet payload in hex


The tool is interrupted using `ctrl-c`. When reading a file the tool exits when
entire file is read (or interrupted with `ctrl-c`).

## Examples

### List all packets in a file

    $ tsdump mpeg.ts

### List all pids in the stream

    $ tsdump mpeg.ts --pids

### List packets with specified pid and having random access indicator

    $ tsdump mpeg.ts -p 101 -r

### Show entire packet in binary form for packets with EBP markers

    $ tsdump mpeg.ts -e -X

### List packets from a multicast stream that have pts

    $ tsdump 224.1.1.1:1111 -t


### Write packets from a multicast stream with specific pid to a file

    $ tsdump 224.1.1.1:1111 -p 101 -w pid_101.ts
    $ ls -l

