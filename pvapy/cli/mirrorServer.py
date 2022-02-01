#!/usr/bin/env python

import argparse
import time
import numpy as np
import pvaccess as pva

__version__ = pva.__version__

def main():
    parser = argparse.ArgumentParser(description='PvaPy Mirror Server')
    parser.add_argument('--channel-map', '-cm', dest='channel_map', default=None, help='Channel map specification given as a comma-separated list of tuples of the form (<mirror_channel>,<source_channel>[,source_provider]); if specified, source provider must be either "pva" or "ca" (default: pva)')
    parser.add_argument('--runtime', '-rt', type=float, dest='runtime', default=0, help='Server runtime in seconds; values <=0 indicate infinite runtime (default: infinite)')
    parser.add_argument('-v', '--version', action='version', version='%(prog)s {version}'.format(version=__version__))

    args, unparsed = parser.parse_known_args()
    if len(unparsed) > 0:
        print('Unrecognized argument(s): %s' % ' '.join(unparsed))
        exit(1)

    if args.channel_map is None:
        print('Channel map is not specified.')
        exit(1)

    # Cleanup channel map entries
    tuples = args.channel_map.split(')')
    map_entries = []
    provider_type_map = {'pva' : pva.PVA, 'ca' : pva.CA}
    for t in tuples:
        e = t.replace(',(', '').replace('(', '')
        # Eliminate empty strings
        if not e:
            continue
        me = e.split(',')
        if len(me) > 3:
           print('Invalid channel map entry: %s' % me)
           exit(1)
        elif len(me) == 2:
            # Add default source provider type.
            me.append('pva')
        me[-1] = provider_type_map.get(me[-1].lower())
        if me[-1] is None:
            print('Invalid provider type specified for entry: %s' % e)
            exit(1)
        map_entries.append(me)
        
    server = pva.PvaMirrorServer()
    server.start()
    start_time = time.time()
    for (cName,sName,sProviderType) in map_entries:
        print('Adding mirror channel %s using source %s (%s)' % (cName, sName, sProviderType))
        server.addMirrorRecord(cName,sName,sProviderType)

    print('Started mirror server @ %.3f' % start_time)
    while True:
        try:
            sleep_time = 10
            if args.runtime > 0:
                now = time.time()
                runtime = now - start_time
                if runtime > args.runtime:
                    print('Server is exiting after %s seconds' % runtime)
                    break
                else:
                    if runtime + sleep_time > args.runtime:
                        sleep_time = 1
            time.sleep(sleep_time)
        except KeyboardInterrupt as e:
            print('Keyboard interrupt received, exiting.')
            break
    server.stop()
    
if __name__ == '__main__':
    main()
