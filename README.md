# Area detector Kafka interface

An EPICS areaDetector plugin which sends areaDetector data serialised using flatbuffers to a Kafka broker. The plugin is in a state which should make it useful (ignoring unknown bugs). Several suggestions on improvements are listed last in this document however.

## Background and requirements

### Kafka
Apache Kafka is an open-source platform for handling streaming data using or more data brokers in order to maximize throughput and reliability. More information on Apache Kafka can be found at [the website of that project.](https://kafka.apache.org/intro)

For communicating with the Kafka broker, the C++ version of `librdkafka` is used. The source code for this library can be downloaded from [https://github.com/edenhill/librdkafka](https://github.com/edenhill/librdkafka).

### Flatbuffers
For serializing the areaDetector (NDArray) data, [Google FlatBuffers](https://github.com/google/flatbuffers) is used. Serializing data using FlatBuffers is fast with a relatively small memory overhead while being easier to use than C-structs.

To simplify building of this project, tha flatbuffers source code has been included in this repository. Read the file *flatbuffers_LICENSE.txt* for the flatbuffers license.

### JSONcpp
`librdkafka` produces statistics messages in JSON and these are parsed using `jsoncpp` ([https://github.com/open-source-parsers/jsoncpp](https://github.com/open-source-parsers/jsoncpp)). To simplify building of this project, the `jsoncpp` source code has been included in this project. The license of this library can be found in the file *jsoncpp_LICENSE.txt*.

## Compiling and running the example
The steps shown here worked on the development machine but has been tested nowhere else.

1. Copy the _ADPluginKafka_ directory to your _$(EPICS_MODULES_PATH)/areaDetector_ directory.
2. Set the variable `SIMDET` in the file _ADPluginKafka/iocs/ADPluginKafkaIOC/configure/RELEASE_ to point to the location of _ADSimDetector_ in your EPICS installation.
3. Modify the file _/ADPluginKafka/ADPluginKafkaApp/src/Makefile_ such that `RDKAFKA` points to the directory where librdkafka is installed (usually something like `/usr/local/`).
4. Modify the files _Makefile_ and _start_epics_ in the _ADPluginKafka/iocs/ADPluginKafkaIOC/iocBoot/iocADPluginKafka_ directory to reflect the current platform.
5. Change (`cd`) to the _ADPluginKafka_ directory and compile by running `make`.
6. Modify the _ADPluginKafka/iocs/ADPluginKafkaIOC/iocBoot/iocADPluginKafka/st.cmd_ file to use the address of your Kafka broker.
7. Run `sh start_epics` from that directory.

## Process variables (PV:s)
This plugin provides a few extra process variables (PV) besides the ones provided through inheritance from `NDPluginDriver`. The plugin also modifies one process variable inherited from `NDPluginDriver` directly. The new PV are listed below. Note that the PV names are prefixed by `$(P)$(R)`.

PV | Type | Default value | Description
---|---|---|---
SourceName, SourceName_RBV | `string` | n/a |The name of the data source in the flatbuffers produced by this plugin. Can not be an empty string.
ReconnectFlush, ReconnectFlush_RBV | `bool` (0 or 1) | `false` | Tells the plugin if it should attempt to flush the current message buffer before re-doing a connection. Disabled by default as this is ablocking call.
ReconnectFlushTime, ReconnectFlushTime_RBV | `int` | `500` [ms] | The (maximum) amount of time in ms to wait (block) for a flush to finish if _ReconnectFlush_ is set to `true`.
KafkaBufferSize, KafkaBufferSize_RBV | `int` | `500000` [kb] | The maximum kafka message buffer size in kb. Note that this setting has a higher priority than _KafkaMaxQueueSize_. Changing this value will trigger a disconnect and re-connect of the Kafka connection.
KafkaMaxMessageSize, KafkaMaxMessageSize_RBV | `int` | `10000000` [b]| The maximum accepted message size (of individual flatbuffer messages) in bytes. Changing this value will trigger a disconnect and re-connect of the Kafka connection.
UnsentPackets_RBV | `int` | n/a | The number of (flatbuffer) messages lost/dropped due to connection issues with the Kafka broker. Note that we will only start dropping (permanently loosing) messages when the message buffer is full.
ConnectionStatus_RBV | `enum` | n/a | The current Kafka connection status of the plugin. Can take the values "Connected" (0), "Connecting" (1), "Disconnected" (2), "Error" (3).
ConnectionMessage_RBV | `string` | n/a | The current connection status in the form of a string. For easier debugging of connection issues.
KafkaTopic, KafkaTopic_RBV | `string` | n/a | The Kafka topic to which the flatbufffer messages are to be transmitted. Can not be an empty string.
KafkaBrokerAddress, KafkaBrokerAddress_RBV | `string` | n/a | The address (and port) to a Kafka broker. Note that if you have more than one broker in your cluster, the address provided here might not be the one that is ultimately used. Changing this value will trigger a disconnect and re-connect of the Kafka connection.
KafkaStatsIntervalTime, KafkaStatsIntervalTime_RBV | `int` | `500` [ms] | How often the Kafka connection status PVs are updated in ms. Changing this value will trigger a disconnect and re-connect of the Kafka connection.
KafkaMaxQueueSize, KafkaMaxQueueSize_RBV | `int` | 200 | Maximum number of messages in the buffer of messages to be sent to Kafka. Note that this setting has a lower priority than _KafkaBufferSize_. Changing this value will trigger a disconnect and re-connect of the Kafka connection.





## Unit tests
The repository contains a directory with code for unit tests of the two projects. Do note that the build system of the unit tests (specifically the CMake file) will most likely require some modification to work on your system. Due to differences in EPICS installations, the CMake file has only been tested on the development machine (running MacOSX).

The unit tests use GTest/GMock (and librdkafka of course). These dependencies are provided using [conan](https://conan.io/). To compile and run the unit tests (assuming the CMake code has been suitably modified for your system), execute the following command line arguments.

```
cd ad-kafka-interface
mkdir build
cd build
conan install ../ --build=outdated
cmake ..
make
./bin/unit_tests
```

