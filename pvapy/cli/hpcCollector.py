#!/usr/bin/env python
'''
Data collector command line interface.
'''

import sys
import argparse
import pvaccess as pva
from ..hpc.dataCollectorController import DataCollectorController

__version__ = pva.__version__

def main():
    ''' CLI method. '''
    parser = argparse.ArgumentParser(description='PvaPy HPC Collector utility. It can be used for receiving data from a set of producer processes, and processing this data using a specified implementation of the data processor interface.')
    parser.add_argument('-v', '--version', action='version', version=f'%(prog)s {__version__}')
    parser.add_argument('-id', '--collector-id', dest='collector_id', type=int, default=1, help='Collector id (default: 1). This may be used for naming various PVA channels, so care must be taken when multiple collector processes are running independently of each other.')
    parser.add_argument('-pid', '--producer-id-list', dest='producer_id_list', default='1,2', help='Comma-separated list of producer IDs (default: 1,2). This option can also be specified as "range(<firstId>,<lastId+1>[,<idStep>)".')
    parser.add_argument('-ic', '--input-channel', dest='input_channel', required=True, help='Input PV channel name. The "*" character will be replaced with <producerId> formatted using <idFormatSpec> specification.')
    parser.add_argument('-oc', '--output-channel', dest='output_channel', default=None, help='Output PVA channel name (default: None). If specified, this channel can be used for publishing processing results. The value of "_" indicates that the output channel name will be set to "pvapy:collector:<collectorId>:output", while the "*" character will be replaced with <collectorId> formatted using <idFormatSpec> specification.')
    parser.add_argument('-sc', '--status-channel', dest='status_channel', default=None, help='Status PVA channel name (default: None). If specified, this channel will provide collector status. The value of "_" indicates that the status channel name will be set to "pvapy:collector:<collectorId>:status", while the "*" character will be replaced with <collectorId> formatted using <idFormatSpec> specification.')
    parser.add_argument('-cc', '--control-channel', dest='control_channel', default=None, help='Control channel name (default: None). If specified, this channel can be used to control collector configuration and processing. The value of "_" indicates that the control channel name will be set to "pvapy:collector:<collectorId>:control", while the "*" character will be replaced with <collectorId> formatted using <idFormatSpec> specification. The control channel object has two strings: command and args. The only allowed values for the command string are: "configure", "reset_stats", "get_stats" and "stop". The configure command is used to allow for runtime configuration changes; in this case the keyword arguments string should be in json format to allow data collector to convert it into python dictionary that contains new configuration. For example, sending configuration dictionary via pvput command might look like this: pvput pvapy:collector:1:control \'{"command" : "configure", "args" : "{\\"x\\":100}"}\'. Note that system parameters that can be modified at runtime are the following: "collectorCacheSize", "monitorQueueSize" (only if client monitor queues have been configured at the start), "skipInitialUpdates" (affects processing behavior after resetting stats), and "objectIdOffset". The reset_stats command will cause collector to reset its statistics data, the get_stats will force statistics data update, and the stop command will result in collector process exiting; for all of these commands args string is not needed.')
    parser.add_argument('-ifs', '--id-format-spec', dest='id_format_spec', default=None, help='Specification to be used for producer or collector id when forming input, output, status and control channel names (default: None).')
    parser.add_argument('-sqs', '--server-queue-size', type=int, dest='server_queue_size', default=0, help='Server queue size (default: 0); this setting will increase memory usage on the server side, but may help prevent missed PV updates.')
    parser.add_argument('-mqs', '--monitor-queue-size', type=int, dest='monitor_queue_size', default=-1, help='PVA channel monitor (client) queue size (default: -1); if < 0, PV updates will be processed immediately without copying them into PvObjectQueue; if >= 0, PvObjectQueue will be used for receving PV updates (value of zero indicates infinite queue size).')
    parser.add_argument('-ccs', '--collector-cache-size', type=int, dest='collector_cache_size', default=-1, help='Collector cache size (default: -1). Collector puts all received PV updates into its cache; once the cache is full, PV updates are sorted by the objectIdField value, removed from the cache and further processed. If specified cache size is negative, or smaller than the minimum allowed value (nProducers), this option will be ignored.')
    parser.add_argument('-pf', '--processor-file', dest='processor_file', default=None, help='Full path to the python file containing user processor class. If this option is not used, the processor class should be specified using "<modulePath>.<className>" notation.')
    parser.add_argument('-pc', '--processor-class', dest='processor_class', default=None, help='Name of the class located in the user processor file that will be processing PV updates. Alternatively, if processor file is not given, the processor class should be specified using the "<modulePath>.<className>" notation. The class should be initialized with a dictionary and must implement the "process(self, pv)" method.')
    parser.add_argument('-pa', '--processor-args', dest='processor_args', default=None, help='JSON-formatted string that can be converted into dictionary and used for initializing user processor object.')
    parser.add_argument('-of', '--oid-field', dest='oid_field', default='uniqueId', help='PV update id field used for calculating data processor statistics (default: uniqueId).')
    parser.add_argument('-oo', '--oid-offset', type=int, dest='oid_offset', default=1, help='This parameter determines by how much object id should change between the two PV updates, and is used for determining the number of missed PV updates (default: 1).')
    parser.add_argument('-fr', '--field-request', dest='field_request', default='', help='PV field request string (default: None). This parameter can be used to request only a subset of the data available in the input channel. The system will automatically append object id field to the specified request string.')
    parser.add_argument('-siu', '--skip-initial-updates', type=int, dest='skip_initial_updates', default=1, help='Number of initial PV updates that should not be processed (default: 1).')
    parser.add_argument('-mc', '--metadata-channels', dest='metadata_channels', default=None, help='Comma-separated list of metadata channels specified in the form "protocol:\\<channelName>", where protocol can be either "ca" or "pva". If channel name is specified without a protocol, "ca" is assumed.')
    parser.add_argument('-rt', '--runtime', type=float, dest='runtime', default=0, help='Server runtime in seconds; values <=0 indicate infinite runtime (default: infinite).')
    parser.add_argument('-rp', '--report-period', type=float, dest='report_period', default=0, help='Statistics report period for the collector in seconds; values <=0 indicate no reporting (default: 0).')
    parser.add_argument('-rs', '--report-stats', dest='report_stats', default='all', help='Comma-separated list of statistics subsets that should be reported (default: all); possible values: monitor, queue, processor, user, all.')
    parser.add_argument('-ll', '--log-level', dest='log_level', help='Log level; possible values: debug, info, warning, error, critical. If not provided, there will be no log output.')
    parser.add_argument('-lf', '--log-file', dest='log_file', help='Log file.')
    parser.add_argument('-dc', '--disable-curses', dest='disable_curses', default=False, action='store_true', help='Disable curses library screen handling. This is enabled by default, except when logging into standard output is turned on.')

    args, unparsed = parser.parse_known_args()
    if len(unparsed) > 0:
        print(f'Unrecognized argument(s): {" ".join(unparsed)}')
        sys.exit(1)

    controller = DataCollectorController(
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
        collectorId=args.collector_id,
        producerIdList=args.producer_id_list,
        serverQueueSize=args.server_queue_size,
        monitorQueueSize=args.monitor_queue_size,
        collectorCacheSize=args.collector_cache_size,
        metadataChannels=args.metadata_channels
    )
    controller.run(args.runtime, args.report_period)

if __name__ == '__main__':
    main()
