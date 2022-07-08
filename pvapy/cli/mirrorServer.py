#!/usr/bin/env python

import argparse
import time
import numpy as np
import pvaccess as pva

__version__ = pva.__version__

def main():
    parser = argparse.ArgumentParser(description='PvaPy Mirror Server')
    parser.add_argument('-v', '--version', action='version', version='%(prog)s {version}'.format(version=__version__))
    parser.add_argument('-cm', '--channel-map', dest='channel_map', default=None, help='Channel map specification given as a comma-separated list of tuples of the form (<mirror_channel>,<source_channel>[,source_provider][,source_queue_size]); if specified, source provider must be either "pva" or "ca" (default: pva), and source queue size must be >= 0 (default: 0)')
    parser.add_argument('-rt', '--runtime', type=float, dest='runtime', default=0, help='Server runtime in seconds; values <=0 indicate infinite runtime (default: infinite)')
    parser.add_argument('-rp', '--report-period', type=float, dest='report_period', default=0, help='Statistics report period for all channels in seconds; values <=0 indicate no reporting (default: 0)')

    args, unparsed = parser.parse_known_args()
    if len(unparsed) > 0:
        print('Unrecognized argument(s): %s' % ' '.join(unparsed))
        exit(1)

    if args.channel_map is None:
        print('Channel map is not specified.')
        exit(1)

    # Cleanup channel map entries
    # They are expected to be of the form
    # (mirror_channel, source_channel, source_provider_type, source_queue_size)
    tuples = args.channel_map.split(')')
    map_entries = []
    provider_type_map = {'pva' : pva.PVA, 'ca' : pva.CA}
    channel_list = []
    for t in tuples:
        e = t.replace(',(', '').replace('(', '')
        # Eliminate empty strings
        if not e:
            continue
        me = e.split(',')
        channel_list.append(me[0])
        if len(me) > 4:
           print('Invalid channel map entry: %s' % me)
           exit(1)
        elif len(me) == 3:
            # Add default queue size
            me.append('0')
        elif len(me) == 2:
            # Add default source provider type and queue size
            me.append('pva')
            me.append('0')
        me[2] = provider_type_map.get(me[2].lower())
        if me[2] is None:
            print('Invalid source provider type specified for entry: {}'.format(e))
            exit(1)
        me[3] = int(me[3])
        if me[3] < 0:
            print('Invalid source queue size specified for entry: {}'.format(e))
            exit(1)
        map_entries.append(me)
        
    server = pva.PvaMirrorServer()
    server.start()
    start_time = time.time()
    last_report_time = start_time
    for (cName,sName,sProviderType,sqSize) in map_entries:
        print('Adding mirror channel {} using source {} (provider type: {}; queue size: {})'.format(cName, sName, sProviderType, sqSize))
        server.addMirrorRecord(cName,sName,sProviderType,sqSize)

    print('Started mirror server @ %.3f' % start_time)
    sleep_time = 1
    while True:
        try:
            now = time.time()
            if args.runtime > 0:
                runtime = now - start_time
                if runtime > args.runtime:
                    print('Server is exiting after %s seconds' % runtime)
                    break
            if args.report_period > 0 and now-last_report_time > args.report_period:
                last_report_time = now
                print()
                for c in channel_list:
                    print('Channel %s @ %.3f: %s' % (c, now, server.getMirrorRecordCounters(c)))
            time.sleep(sleep_time)
        except KeyboardInterrupt as e:
            print('Keyboard interrupt received, exiting.')
            break
    server.stop()
    print('\nFinal Channel Statistics:')
    now = time.time()
    for c in channel_list:
        print('Channel %s @ %.3f: %s' % (c, now, server.getMirrorRecordCounters(c)))
    
if __name__ == '__main__':
    main()
