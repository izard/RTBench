Multicore real time system are normally running multithreaded code, which does not use a lot of communication between 
cores.

When this communication happens, it is usually organized as message passing across shared memory.
This u-benchmark demonstrates message passing between 3 threads pinned to 3 cores. The metric is CPU cycles
spent for single iteration, which involves sending, receiving and processing 6 messages between cores.
