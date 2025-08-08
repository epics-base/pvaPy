# Streaming Framework Throughput Tests

## Input/Output Modes and Firewalls

By default, consumers receive input data by establishing PVA monitor
on the input channel, and they publish their output via their PVA server
instance. However, in some cases it may be advantageous to publish 
processing output by directly updating channels hosted by external
PVA or RPC servers. For example, if data is produced at site A that
allows incoming TCP connections, and it is
processed at site B which only allows outgoing connections, on site B we might
use PVA monitors to receive input and either PVA or RPC clients to
publish processing output back to servers listening on site A.

The framework offers the following combinations of PRODUCER (OUTPUT) => CONSUMER (INPUT) modes:
* PVAS (server) => PVA (monitor)
* PVA (client) => PVAS (server)
* RPC (client) => RPC (server)
* EJFAT (publisher) => EJFAT (receiver)

The above modes are specified via the '--input-mode' and '--output-mode'
arguments for the pvapy-hpc-consumer command, with 'pva' being the
default input mode and 'pvas' the default output mode. Keep in mind that
the client-based output modes will be less performant than the
default server-based mode.

The ESnet JLab FPGA Accelerated Transport (EJFAT) mode
requires [E2SAR library](https://github.com/JeffersonLab/E2SAR)
to be installed. This mode can be configured by using
the '--input-args' and '--output-args' configuration dictionary similar
to how the user data processor is configured. Some of the most important
configuration parameters are the following:
* port: starting UDP port (publisher, receiver)
* addProcessorIdToPort: add processor (consumer) id to the starting port
  number (publisher, receiver); this flag should be set to 1 when 
  multiple consumers are spawned
* bufferCacheSize: number of message buffers that can be cached
  (publisher, receiver); for the data publisher, if buffer cache size is <=
  1, synchronous publishing mode will be enabled (i.e., PvObject will be
  published as soon as it is serialized, rather than it being added to
  the E2SAR send queue); for the data receiver, this buffer is used to
  hold received PvObjects that are waiting to be deserialized)
* nReceivingThreads: number of receiving threads (receiver)
* useCP: use control plane (publisher, receiver); this flag must be set
  to 0 when EJFAT load balancer is not used (e.g., for direct
  publisher/receiver connections)
* withLBHeader: with load balancer header (receiver); this flag must be
  set to 1 when EJFAT load balancer is not used (e.g., for direct
  publisher/receiver connections)
* rcvSocketBufSize: receive socket buffer size (receiver)
* sndSocketBufSize: send socket buffer size (publisher)
* numSendSockets: number of send sockets (publisher)
* mtu: maximum transmission unit in bytes (publisher)

For the EPICS-based input and output modes, one can use the following environment variables for controlling client and server behavior:
* EPICS_PVA_BROADCAST_PORT: for controlling UDP-based channel search on the client side
* EPICS_PVA_NAME_SERVERS: for enabling TCP-based channel search on the client side
* EPICS_PVAS_SERVER_PORT: for controlling server TCP port

## Throughput Tests

All tests described in this section have been performed on a single machine using the loopback device. With everything else being the same, the same tests performed on multiple machines would be affected by the network bandwidth and
configuration.

In order to asses how much data can be pushed through the framework we
ran a series of tests using the [base AD image processor](../pvapy/hpc/adImageProcessor.py)
that does not manipulate image and hence does not generate any additional
load on the test machine. For each test we ran AD image simulator and two sets of
connected consumers.

Server queue size varied according to the test image size, and was typically set to
less than 1 second worth of framews.
Whenever we used multiple consumers (N_CONSUMERS > 1) data distributor was
turned on using the '--distributor-updates 1' option. For a single consumer
this option was left out.

A given test was deemed successful if no frames were 
missed during the 60 second server runtime. Results for the maximum
simulated detector rate that image consumers were able to sustain 
without missing any frames are shown below.

### PVAS => PVA,PVAS => PVA,PVAS

Terminal 1 (stage 1 consumers):

```sh
$ pvapy-hpc-consumer \
    --input-channel pvapy:image \
    --control-channel stage1:*:control \
    --status-channel stage1:*:status \
    --output-channel stage1:*:output \
    --output-mode pvas \
    --processor-class pvapy.hpc.adImageProcessor.AdImageProcessor \
    --report-period 10 \
    --server-queue-size SERVER_QUEUE_SIZE \
    --n-consumers N_CONSUMERS \
    [--distributor-updates 1]
```

Terminal 2 (stage 2 consumers):

```sh
$ pvapy-hpc-consumer \
    --input-channel stage1:*:output \
    --input-mode pva \
    --control-channel stage2:*:control \
    --status-channel stage2:*:status \
    --output-channel stage2:*:output \
    --processor-class pvapy.hpc.adImageProcessor.AdImageProcessor \
    --report-period 10 \
    --skip-initial-updates 0 \
    --server-queue-size SERVER_QUEUE_SIZE \
    --n-consumers N_CONSUMERS \
    [--oid-offset N_CONSUMERS]
```

Terminal 3 (AD sim server):

```sh
$ pvapy-ad-sim-server \
    -cn pvapy:image -nf 100 -dt uint8 -rt 60 \
    -nx FRAME_SIZE -ny FRAME_SIZE -fps FRAME_RATE -rp FRAME_RATE
```

### PVAS => PVA,RPC => RPC,PVAS

Terminal 1 (stage 1 consumers):

```sh
$ pvapy-hpc-consumer \
    --input-channel pvapy:image \
    --control-channel stage1:*:control \
    --status-channel stage1:*:status \
    --output-channel stage1:*:output \
    --output-mode rpc \
    --processor-class pvapy.hpc.adImageProcessor.AdImageProcessor \
    --report-period 10 \
    --server-queue-size SERVER_QUEUE_SIZE \
    --n-consumers N_CONSUMERS \
    [--distributor-updates 1]
```

Terminal 2 (stage 2 consumers):

```sh
$ pvapy-hpc-consumer \
    --input-channel stage1:*:output \
    --input-mode rpc \
    --control-channel stage2:*:control \
    --status-channel stage2:*:status \
    --output-channel stage2:*:output \
    --processor-class pvapy.hpc.adImageProcessor.AdImageProcessor \
    --report-period 10 \
    --skip-initial-updates 0 \
    --n-consumers N_CONSUMERS \
    [--oid-offset N_CONSUMERS]
```

Terminal 3 (AD sim server):

```sh
$ pvapy-ad-sim-server \
    -cn pvapy:image -nf 100 -dt uint8 -rt 60 \
    -nx FRAME_SIZE -ny FRAME_SIZE -fps FRAME_RATE -rp FRAME_RATE
```

### PVAS => PVA,PVA => PVAS,PVAS

Terminal 1 (stage 1 consumers):

```sh
$ pvapy-hpc-consumer \
    --input-channel pvapy:image \
    --control-channel stage1:*:control \
    --status-channel stage1:*:status \
    --output-channel stage1:*:output \
    --output-mode pva \
    --processor-class pvapy.hpc.adImageProcessor.AdImageProcessor \
    --report-period 10 \
    --server-queue-size SERVER_QUEUE_SIZE \
    --n-consumers N_CONSUMERS \
    [--distributor-updates 1]
```

Terminal 2 (stage 2 consumers):

```sh
$ pvapy-hpc-consumer \
    --input-channel stage1:*:output \
    --input-mode pvas \
    --control-channel stage2:*:control \
    --status-channel stage2:*:status \
    --output-channel stage2:*:output \
    --processor-class pvapy.hpc.adImageProcessor.AdImageProcessor \
    --report-period 10 \
    --skip-initial-updates 0 \
    --n-consumers N_CONSUMERS \
    [--oid-offset N_CONSUMERS]
```

Terminal 3 (AD sim server):

```sh
$ pvapy-ad-sim-server \
    -cn pvapy:image -nf 100 -dt uint8 -rt 60 \
    -nx FRAME_SIZE -ny FRAME_SIZE -fps FRAME_RATE -rp FRAME_RATE
```

### PVAS => PVA,EJFAT => EJFAT,PVAS

Terminal 1 (stage 1 consumers):

```sh
$ pvapy-hpc-consumer \
    --input-channel pvapy:image \
    --control-channel stage1:*:control \
    --status-channel stage1:*:status \
    --output-channel 'ejfat://useless@192.168.100.1:9876/lb/1?sync=192.168.0.1:12345&data=127.0.0.1:10000'\
    --output-mode ejfat \
    --output-args='{"useCP" : 0, "addProcessorIdToPort" : 1}' \
    --processor-class pvapy.hpc.adImageProcessor.AdImageProcessor \
    --report-period 10 \
    --server-queue-size SERVER_QUEUE_SIZE \
    --n-consumers N_CONSUMERS \
    [--distributor-updates 1]
```

Terminal 2 (stage 2 consumers):

```sh
$ pvapy-hpc-consumer \
    --input-channel 'ejfat://useless@192.168.100.1:9876/lb/1?sync=192.168.0.1:12345&data=0.0.0.0' \
    --input-mode ejfat \
    --input-args='{"port" : 10000, "nReceivingThreads" : 1, "useCP" : 0, "withLBHeader" : 1, "ipAddress" : "127.0.0.1", "addProcessorIdToPort" : 1}' \
    --control-channel stage2:*:control \
    --status-channel stage2:*:status \
    --output-channel stage2:*:output \
    --processor-class pvapy.hpc.adImageProcessor.AdImageProcessor \
    --report-period 10 \
    --skip-initial-updates 0 \
    --n-consumers N_CONSUMERS \
    [--oid-offset N_CONSUMERS]
```

Terminal 3 (AD sim server):

```sh
$ pvapy-ad-sim-server \
    -cn pvapy:image -nf 100 -dt uint8 -rt 60 \
    -nx FRAME_SIZE -ny FRAME_SIZE -fps FRAME_RATE -rp FRAME_RATE
```

#### Test Date: 08/2025

Environment:

* PvaPy Version: 5.6.0
* Python Version: 3.11
* Test Machine: 64-bit linux, 96 logical cores @ 2.8 GHz (Intel Xeon Gold 6342 CPU with hyperthreading enabled), 2 TB RAM

Results:

* Image size: 4096 x 4096 (uint8, 16.78 MB); Server queue size: 100

| Consumers | Frames/<br>second  | Frames/second/<br>consumer | Frames/<br>minute | Data rate/<br>consumer | Total data rate |
| ---:      | ---:           | ---:                       | ---:          | ---:                   | ---:            |
|<td colspan=6>PVAS => PVA,PVAS => PVA,PVAS</td>
|        1  |      170       |     170                    |    10200      | 2.85 GBps              |    2.85 GBps    |
|       8  |      720        |      90                    |    43200      | 1.51 GBps              |   12.08 GBps    |

* Image size: 512 x 512 (uint8, 0.26 MB); Server queue size: 1000

| Consumers | Frames/<br>second  | Frames/second/<br>consumer | Frames/<br>minute | Data rate/<br>consumer | Total data rate |
| ---:      | ---:           | ---:                       | ---:          | ---:                   | ---:            |
|        1  |      7600      |     7600                   |   456000      |      1.99 GBps         |    1.99 GBps    |
|        4  |     23200      |     5800                   |  1392000      |      1.52 GBps         |    6.08 GBps    |

