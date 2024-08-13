# nanorq
nanorq is a compact, performant implementation of the raptorq fountain code capable of reaching multi-gigabit speeds on a single core.

nanorq provides flexible I/O handling, wrappers are provided for memory buffers, mmap (zero copy) and streams. Additional abstractions can be implemented without interacting with the decoder logic.

THIS COPY WAS DONE AT 12/08/2024 from official repository at https://github.com/sleepybishop/nanorq

# Use cases
- firmware deployment / software updates
- video streaming
- large data transfers across high latency links
- data carousel broadcast

## Notes
  Default build is configured for AVX, adjust Makefile as needed for other archs
