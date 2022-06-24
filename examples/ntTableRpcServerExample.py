#!/usr/bin/env python

import pvaccess as pva
import time

class NtTableServer(pva.RpcServer):
    def __init__(self):
        pva.RpcServer.__init__(self)
        self.registerService('ntt', self.handle_request)

    def create_control_reply(self, status, message):
        table = pva.NtTable([pva.BOOLEAN, pva.STRING])
        table.setLabels(['status', 'message'])
        table.setColumn(0, [status])
        table.setColumn(1, [message])
        print(table)
        return table

    def create_error_reply(self, message):
        return self.create_control_reply(False, message)

    def create_success_reply(self, message):
        return self.create_control_reply(True, message)

    def handle_request(self, request):
        print('Got request: %s' % request)
        try:
            if 'status' in request and request['status'] == 0:
                return self.create_success_reply('Yey....')
            return self.create_error_reply('Oh no....')
        except Exception as ex:
            print(ex)
            raise


if __name__ == '__main__':
    s = NtTableServer()
    s.listen(60)

