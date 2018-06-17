# limesdr_toolbox
The limesdr_toolbox is a set of the tools to play with the LimeSDR / LimeSDR-Mini. No external dependancy is need other than official module (libLimeSuite.so for unix-like operating system, see https://github.com/MyriadRF/LimeSuite)

The toolbox contains the following tools:
 - limesdr_dump : dump I/Q signals on arbitrary frequencies
 - limesdr_send : send generated or dumped I/Q signals
 - limesdr_forward : relay of both digital and analog signals : forward input I/Q signals of a RX channel (inputs frequencies) to a TX channel (output frequencies).
 - limesdr_stopchannel : stop all RX/TX channels (debug purpose)

# limesdr_dump
limesdr_dump allows to dump I/Q signals on arbitrary frequencies with 16-bits resolution (highest precision of the hardware).
```
$ ./limesdr_dump
Usage: ./limesdr_dump <OPTIONS>
  -f <FREQUENCY>
  -b <BANDWIDTH_CALIBRATING> (default: 200e3)
  -s <SAMPLE_RATE> (default: 2e6)
  -g <GAIN_NORMALIZED> (default: 1)
  -l <BUFFER_SIZE>  (default: 1024*1024)
  -d <DEVICE_INDEX> (default: 0)
  -c <CHANNEL_INDEX> (default: 0)
  -a <ANTENNA> (LNAL | LNAH | LNAW) (default: LNAW)
  -o <OUTPUT_FILENAME> (default: stdout)
```
Most of the parameters have a default value. Only the central frequency and optionally the output capture file need to be set. Ctrl-c command can used to stop the capture.

E.g. dumping of station radio :
```
$ ./limesdr_dump -f 100.1e6 -o out.dat
```

The output capture file contains a set of 16-bits I/Q signals.


# limesdr_send
limesdr_send allows to send any I/Q signals.
```
$ ./limesdr_send
Usage: limesdr_send <OPTIONS>
  -f <FREQUENCY>
  -b <BANDWIDTH_CALIBRATING> (default: 200e3)
  -s <SAMPLE_RATE> (default: 2e6)
  -g <GAIN_NORMALIZED> (default: 1)
  -l <BUFFER_SIZE> (default: 1024*1024)
  -p <POSTPONE_EMITTING_SEC> (default: 3)
  -d <DEVICE_INDEX> (default: 0)
  -c <CHANNEL_INDEX> (default: 0)
  -a <ANTENNA> (BAND1 | BAND2) (default: BAND1)
  -i <INPUT_FILENAME> (default: stdin)
```

E.g. send of previous caputured I/Q signals
```
$ ./limesdr_send -f 100.1e6 -i out.dat
```

Because we are handling I/Q signals from Intermediate Frequencies (IF), we can output I/Q signals on frequencies different than input frequencies
```
$ ./limesdr_dump -f 100.1e6 -o out.dat
$ ./limesdr_send -f 234.5e6 -i out.dat
```

# limesdr_forward
limesdr_forward allows to forward I/Q signals captured around input frequencies on others frequencies on a real time. This is like a kind of streaming but on arbitrary radio frequencies, and can be use as a relay of both digital and analog signals.
```
$ ./limesdr_forward
Usage: ./limesdr_forward <OPTIONS>
  -f <INPUT_FREQUENCY>
  -F <OUTPUT_FREQUENCY>
  -b <BANDWIDTH_CALIBRATING> (default: 200e3)
  -s <SAMPLE_RATE> (default: 2e6)
  -g <INPUT_GAIN_NORMALIZED> (default: unused)
  -G <OUTPUT_GAIN_NORMALIZED> (default: 1)
  -l <BUFFER_SIZE> (default: 1024*1024)
  -d <DEVICE_INDEX> (default: 0)
  -c <INPUT_CHANNEL_INDEX> (default: 0)
  -C <OUTPUT_CHANNEL_INDEX> (default: 0)
  -a <INPUT_ANTENNA> (LNAL | LNAH | LNAW) (default: LNAW)
  -A <OUTPUT_ANTENNA> (BAND1 | BAND2) (default: BAND1)
```

E.g. forwarding of a radio station
```
$ ./limesdr_forward -f 100.1e6 -F 234.5e6
```

# limesdr_stopchannel
limesdr_stopchannel allows to stop all channels on RX/TX.
```
$ ./limesdr_stopchannel
```
