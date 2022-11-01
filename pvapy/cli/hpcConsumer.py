#!/usr/bin/env python

import argparse
import pvaccess as pva
from ..hpc.dataConsumerController import DataConsumerController
from ..hpc.mpDataConsumerController import MpDataConsumerController

__version__ = pva.__version__

def main():
    parser = argparse.ArgumentParser(description='PvaPy HPC Consumer utility. It can be used for receiving and processing data using specified implementation of the data processor interface.')
    parser.add_argument('-v', '--version', action='version', version='%(prog)s {version}'.format(version=__version__))
    parser.add_argument('-id', '--consumer-id', dest='consumer_id', type=int, default=1, help='Consumer id (default: 1). If spawning multiple consumers, this option will be interpreted as the first consumer id; for each subsequent consumer id will be increased by 1. Note that consumer id is used for naming various PVA channels, so care must be taken when multiple consumer processes are running independently of each other.')
    parser.add_argument('-nc', '--n-consumers', type=int, dest='n_consumers', default=1, help='Number of consumers to instantiate (default: 1). If > 1, multiprocessing module will be used for receiving and processing data in separate processes.')
    parser.add_argument('-cid', '--consumer-id-list', dest='consumer_id_list', default=None, help='Comma-separated list of consumer IDs (default: None). This option can also be specified as "range(<firstId>,<lastId+1>[,<idStep>)". If this option is used, values given for <consumerId> and <nConsumers> options will be ignored.')
    parser.add_argument('-ic', '--input-channel', dest='input_channel', required=True, help='Input PV channel name. The "*" character will be replaced with <consumerId> formatted using <idFormatSpec> specification.')
    parser.add_argument('-ipt', '--input-provider-type', dest='input_provider_type', default='pva', help='Input PV channel provider type, it must be either "pva" or "ca" (default: pva).')
    parser.add_argument('-oc', '--output-channel', dest='output_channel', default=None, help='Output PVA channel name (default: None). If specified, this channel can be used for publishing processing results. The value of "_" indicates that the output channel name will be set to "pvapy:consumer:<consumerId>:output", while the "*" character will be replaced with <consumerId> formatted using <idFormatSpec> specification.')
    parser.add_argument('-sc', '--status-channel', dest='status_channel', default=None, help='Status PVA channel name (default: None). If specified, this channel will provide consumer status. The value of "_" indicates that the status channel name will be set to "pvapy:consumer:<consumerId>:status", while the "*" character will be replaced with <consumerId> formatted using <idFormatSpec> specification.')
    parser.add_argument('-cc', '--control-channel', dest='control_channel', default=None, help='Control channel name (default: None). If specified, this channel can be used to control consumer configuration and processing. The value of "_" indicates that the control channel name will be set to "pvapy:consumer:<consumerId>:control", while the "*" character will be replaced with <consumerId> formatted using <idFormatSpec> specification. The control channel object has two strings: command and args. The only allowed values for the command string are: "configure", "reset_stats", "get_stats" and "stop". The configure command is used to allow for runtime configuration changes; in this case the keyword arguments string should be in json format to allow data consumer to convert it into python dictionary that contains new configuration. For example, sending configuration dictionary via pvput command might look like this: pvput input_channel:consumer:2:control \'{"command" : "configure", "args" : "{\\"x\\":100}"}\'. Note that system parameters that can be modified at runtime are the following: "monitorQueueSize" (only if client monitor queue has been configured at the start), "skipInitialUpdates" (affects processing behavior after resetting stats), and "objectIdOffset" (may be used to adjust offset if consumers have been added or removed from processing). The reset_stats command will cause consumer to reset its statistics data, the get_stats will force statistics data update, and the stop command will result in consumer process exiting; for all of these commands args string is not needed.')
    parser.add_argument('-ifs', '--id-format-spec', dest='id_format_spec', default=None, help='Specification to be used for consumer id when forming input, output, status and control channel names (default: None).')
    parser.add_argument('-sqs', '--server-queue-size', type=int, dest='server_queue_size', default=0, help='Server queue size (default: 0); this setting will increase memory usage on the server side, but may help prevent missed PV updates.')
    parser.add_argument('-mqs', '--monitor-queue-size', type=int, dest='monitor_queue_size', default=-1, help='PVA channel monitor (client) queue size (default: -1); if < 0, PV updates will be processed immediately without copying them into PvObjectQueue; if >= 0, PvObjectQueue will be used for receving PV updates (value of zero indicates infinite queue size).')
    parser.add_argument('-ao', '--accumulate-objects', type=int, dest='accumulate_objects', default=-1, help='Number of objects to accumulate in the PVA channel monitor (client) queue before they can be processed (default: -1); if <= 0 the processing happens regarding of the current monitor queue length. This option is ignored unless monitor (client) queue size is set (i.e., >= 0). Note that after accumulation timeout, all objects in the queue will be processed.')
    parser.add_argument('-at', '--accumulation-timeout', type=float, dest='accumulation_timeout', default=1, help='Time period since last received item after which objects in the PVA channel monitor (client) queue will be processed regardless of the current queue length (default: 1 second). This option is ignored unless monitor (client) queue size is set (i.e, >= 0) and if number of accumulated objects is not set (i.e., <= 0).')
    parser.add_argument('-pf', '--processor-file', dest='processor_file', default=None, help='Full path to the python file containing user processor class. If this option is not used, the processor class should be specified using "<modulePath>.<className>" notation.')
    parser.add_argument('-pc', '--processor-class', dest='processor_class', default=None, help='Name of the class located in the user processor file that will be processing PV updates. Alternatively, if processor file is not given, the processor class should be specified using the "<modulePath>.<className>" notation. The class should be initialized with a dictionary and must implement the "process(self, pv)" method.')
    parser.add_argument('-pa', '--processor-args', dest='processor_args', default=None, help='JSON-formatted string that can be converted into dictionary and used for initializing user processor object.')
    parser.add_argument('-of', '--oid-field', dest='oid_field', default='uniqueId', help='PV update id field used for calculating data processor statistics (default: uniqueId).')
    parser.add_argument('-oo', '--oid-offset', type=int, dest='oid_offset', default=0, help='This parameter determines by how much object id should change between the two PV updates, and is used for determining the number of missed PV updates (default: 0). This parameter should be modified only if data distributor plugin will be distributing data between multiple clients, in which case it should be set to "(<nConsumers>-1)*<nUpdates>+1" for a single client set, or to "(<nSets>-1)*<nUpdates>+1" for multiple client sets. Values <= 0 will be replaced with the appropriate value depending on the number of client sets specified. Note that this relies on using the same value for the --n-distributor-sets when multiple instances of this command are running separately.')
    parser.add_argument('-fr', '--field-request', dest='field_request', default='', help='PV field request string (default: None). This parameter can be used to request only a subset of the data available in the input channel. The system will automatically append object id field to the specified request string. Note that this parameter is ignored when data distributor is used.')
    parser.add_argument('-siu', '--skip-initial-updates', type=int, dest='skip_initial_updates', default=1, help='Number of initial PV updates that should not be processed (default: 1).')
    parser.add_argument('-dpn', '--distributor-plugin-name', dest='distributor_plugin_name', default='pydistributor', help='Distributor plugin name (default: pydistributor).')
    parser.add_argument('-dg', '--distributor-group', dest='distributor_group', default=None, help='Distributor client group that application belongs to (default: None). This parameter should be used only if data distributor plugin will be distributing data between multiple clients. Note that different distributor groups are completely independent of each other.')
    parser.add_argument('-ds', '--distributor-set', dest='distributor_set', default=None, help='Distributor client set that application belongs to within its group (default: None). This parameter should be used only if data distributor plugin will be distributing data between multiple clients. Note that all clients belonging to the same set receive the same PV updates. If set id is not specified (i.e., if a group does not have multiple sets of clients), a PV update will be distributed to only one client.')
    parser.add_argument('-dt', '--distributor-trigger', dest='distributor_trigger', default=None, help='PV structure field that data distributor uses to distinguish different channel updates (default: None). This parameter should be used only if data distributor plugin will be distributing data between multiple clients. In case of, for example, area detector applications, the "uniqueId" field would be a good choice for distinguishing between the different frames.')
    parser.add_argument('-du', '--distributor-updates', dest='distributor_updates', default=None, help='Number of sequential PV channel updates that a client (or a set of clients) will receive (default: None). This parameter should be used only if data distributor plugin will be distributing data between multiple clients.')
    parser.add_argument('-nds', '--n-distributor-sets', type=int, dest='n_distributor_sets', default=1, help='Number of distributor client sets (default: 1). This setting is used to determine appropriate value for the processor object id offset in case where multiple instances of this command are running separately for different client sets. If distributor client set is not specified, this setting is ignored.')
    parser.add_argument('-mc', '--metadata-channels', dest='metadata_channels', default=None, help='Comma-separated list of metadata channels specified in the form "protocol:\\<channelName>", where protocol can be either "ca" or "pva". If channel name is specified without a protocol, "ca" is assumed.')
    parser.add_argument('-rt', '--runtime', type=float, dest='runtime', default=0, help='Server runtime in seconds; values <=0 indicate infinite runtime (default: infinite).')
    parser.add_argument('-rp', '--report-period', type=float, dest='report_period', default=0, help='Statistics report period for all consumers in seconds; values <=0 indicate no reporting (default: 0).')
    parser.add_argument('-rs', '--report-stats', dest='report_stats', default='all', help='Comma-separated list of statistics subsets that should be reported (default: all); possible values: monitor, queue, processor, user, all.')
    parser.add_argument('-ll', '--log-level', dest='log_level', help='Log level; possible values: debug, info, warning, error, critical. If not provided, there will be no log output.')
    parser.add_argument('-lf', '--log-file', dest='log_file', help='Log file.')
    parser.add_argument('-dc', '--disable-curses', dest='disable_curses', default=False, action='store_true', help='Disable curses library screen handling. This is enabled by default, except when logging into standard output is turned on.')

    args, unparsed = parser.parse_known_args()
    if len(unparsed) > 0:
        print('Unrecognized argument(s): {}'.format(' '.join(unparsed)))
        exit(1)

    nConsumers = args.n_consumers
    consumerId = args.consumer_id
    consumerIdList = None
    if args.consumer_id_list:
        consumerIdList = DataConsumerController.generateIdList(args.consumer_id_list)
        consumerId = consumerIdList[0]
        nConsumers = len(consumerIdList)
    if nConsumers == 1:
        ControllerClass = DataConsumerController
    else:
        ControllerClass = MpDataConsumerController

    controller = ControllerClass(
        args.input_channel, 
        outputChannel=args.output_channel,
        statusChannel=args.status_channel,
        controlChannel=args.control_channel,
        idFormatSpec=args.id_format_spec,
        processorFile=args.processor_file,
        processorClass=args.processor_class,
        processorArgs=args.processor_args,
        objectIdField=args.oid_field,
        objectIdOffset=args.oid_offset,
        fieldRequest=args.field_request,
        skipInitialUpdates=args.skip_initial_updates,
        reportStatsList=args.report_stats,
        logLevel=args.log_level,
        logFile=args.log_file,
        disableCurses=args.disable_curses,
        consumerId=consumerId,
        nConsumers=nConsumers,
        consumerIdList=consumerIdList,
        inputProviderType=args.input_provider_type,
        serverQueueSize=args.server_queue_size,
        monitorQueueSize=args.monitor_queue_size,
        accumulateObjects=args.accumulate_objects,
        accumulationTimeout=args.accumulation_timeout,
        distributorPluginName=args.distributor_plugin_name,
        distributorGroup=args.distributor_group,
        distributorSet=args.distributor_set,
        distributorTrigger=args.distributor_trigger,
        distributorUpdates=args.distributor_updates,
        nDistributorSets=args.n_distributor_sets,
        metadataChannels=args.metadata_channels
    )
    controller.run(args.runtime, args.report_period)

if __name__ == '__main__':
    main()
