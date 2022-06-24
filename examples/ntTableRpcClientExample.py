#!/usr/bin/env python

import sys
import pvaccess as pva

if __name__ == '__main__':
    status = 1
    if len(sys.argv) > 1:
        status = int(sys.argv[1])
    rpc = pva.RpcClient('ntt')
    request = pva.PvObject({'status' : pva.INT},{'status' : status})
    print('Sending request: %s' % request)
    response = rpc.invoke(request)
    print('Got response: %s' % response)

