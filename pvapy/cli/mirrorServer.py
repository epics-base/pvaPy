#!/usr/bin/env python

'''
PVA Mirror Server.
'''

import sys
import argparse
import time
import pvaccess as pva

__version__ = pva.__version__

def main():
    parser = argparse.ArgumentParser(description='PvaPy Mirror Server')
    parser.add_argument('-v', '--version', action='version', version=f'%(prog)s {__version__}')
    parser.add_argument('-cm', '--channel-map', dest='channel_map', default=None, help='Channel map specification given as a comma-separated list of tuples of the form (<mirror_channel>,<source_channel>[,source_provider][,source_queue_size[,n_source_monitors,source_field_request_descriptor]]]); if specified, source provider must be either "pva" or "ca" (default: pva), and source queue size must be >= 0 (default: 0); specifying number of source monitors and source request descriptor is typically used with the data distributor plugin, which must be supported by the source PVA server (example request descriptor: "_[pydistributor=updates:1;group:mirror;trigger:uniqueId]").')
    parser.add_argument('-rt', '--runtime', type=float, dest='runtime', default=0, help='Server runtime in seconds; values <=0 indicate infinite runtime (default: infinite)')
    parser.add_argument('-rp', '--report-period', type=float, dest='report_period', default=0, help='Statistics report period for all channels in seconds; values <=0 indicate no reporting (default: 0)')

    args, unparsed = parser.parse_known_args()
    if len(unparsed) > 0:
        print(f'Unrecognized argument(s): {" ".join(unparsed)}')
        sys.exit(1)

    if args.channel_map is None:
        print('Channel map is not specified.')
        sys.exit(1)

    # Cleanup channel map entries
    # They are expected to be of the form
    # (mirror_channel, source_channel, source_provider_type, source_queue_size)
    tuples = args.channel_map.split(')')
    mapEntries = []
    providerTypeMap = {'pva' : pva.PVA, 'ca' : pva.CA}
    channelList = []
    for t in tuples:
        e = t.replace(',(', '').replace('(', '')
        # Eliminate empty strings
        if not e:
            continue
        me = e.split(',')
        channelList.append(me[0])
        entryLength = len(me)
        if entryLength > 6:
            print(f'Invalid channel map entry: {e}')
            sys.exit(1)
        if entryLength <= 2:
            # Add default source provider type
            me.append('pva')
        if entryLength <= 3:
            # Add default queue size
            me.append('0')
        if entryLength <= 4:
            # Add default number of monitors
            me.append('1')
        if entryLength <= 5:
            # Add default request descriptor
            me.append('')
        me[2] = providerTypeMap.get(me[2].lower())
        if me[2] is None:
            print(f'Invalid source provider type specified for entry: {e}')
            sys.exit(1)
        me[3] = int(me[3])
        if me[3] < 0:
            print(f'Invalid source queue size specified for entry: {e}')
            sys.exit(1)
        me[4] = int(me[4])
        if me[4] < 1:
            print(f'Invalid number of source monitors specified for entry: {e}')
            sys.exit(1)
        mapEntries.append(me)

    server = pva.PvaMirrorServer()
    server.start()
    startTime = time.time()
    lastReportTime = startTime
    for (cName,sName,sProviderType,sqSize,nsMonitors,sRequestDescriptor) in mapEntries:
        print(f'Adding mirror channel {cName} using source {sName} (provider type: {sProviderType}; queue size: {sqSize}; number of monitors: {nsMonitors}; request descriptor: {sRequestDescriptor})')
        server.addMirrorRecord(cName,sName,sProviderType,sqSize,nsMonitors,sRequestDescriptor)

    print(f'Started mirror server @ {startTime:.3f}')
    sleepTime = 1
    while True:
        try:
            now = time.time()
            if args.runtime > 0:
                runtime = now - startTime
                if runtime > args.runtime:
                    print(f'Server is exiting after {runtime:.3f} seconds')
                    break
            if args.report_period > 0 and now-lastReportTime > args.report_period:
                lastReportTime = now
                print()
                for c in channelList:
                    print(f'Channel {c} @ {now:.3f}: {server.getMirrorRecordCounters(c)}')
            time.sleep(sleepTime)
        except KeyboardInterrupt:
            print('Keyboard interrupt received, exiting.')
            break
    server.stop()
    print('\nFinal Channel Statistics:')
    now = time.time()
    for c in channelList:
        print(f'Channel {c} @ {now:.3f}: {server.getMirrorRecordCounters(c)}')

if __name__ == '__main__':
    main()
