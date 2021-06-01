< envPaths
errlogInit(20000)

dbLoadDatabase("$(TOP)/dbd/ADPluginKafkaApp.dbd")
ADPluginKafkaApp_registerRecordDeviceDriver(pdbbase)

epicsEnvSet("PREFIX", "13SIM1:")
epicsEnvSet("SIMDET_PORT", "SIM1")
epicsEnvSet("K_PORT", "KAFKA1")
epicsEnvSet("XSIZE", "$(XSIZE=200)")
epicsEnvSet("YSIZE", "$(YSIZE=200)")
epicsEnvSet("QSIZE", "20")
epicsEnvSet("EPICS_DB_INCLUDE_PATH", "$(ADCORE)/db")

simDetectorConfig("$(SIMDET_PORT)", $(XSIZE), $(YSIZE), 1, 0, 0)
dbLoadRecords("$(SIMDET)/db/simDetector.template", "P=$(PREFIX), R=cam1:, PORT=$(SIMDET_PORT), ADDR=0, TIMEOUT=1")

KafkaPluginConfigure("$(K_PORT)", 3, 1, "$(SIMDET_PORT)", 0, -1, "localhost:9092", "sim_data_topic", "some_source")
dbLoadRecords("$(ADPLUGINKAFKA)/db/ADPluginKafka.template", "P=$(PREFIX),R=KFK:,PORT=$(K_PORT),ADDR=0,TIMEOUT=1,NDARRAY_PORT=$(SIMDET_PORT)")

iocInit()

dbpf $(PREFIX)KFK:EnableCallbacks Enable
dbpf $(PREFIX)cam1:AcquirePeriod 2
dbpf $(PREFIX)cam1:Acquire 1

#Always end with a new line
