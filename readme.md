# TSDump

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



