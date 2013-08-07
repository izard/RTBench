Linear code is a u-benchmark designed to mimic a backend of a typical
IEC61131 compiler. Those backends are not emitting the code well-tuned for 
target architecture, and some are actively using "threaded code" approach.

The benchmark builds big in-memory binary and jumps to it. The main point of the binary
is to test I-cache performance on bloated code. Metric - CPU cycles per function call
