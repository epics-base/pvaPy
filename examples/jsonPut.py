#!/usr/bin/env python

from pvaccess import *

provider = ProviderType.PVA
result = raw_input("do you want provider ca: ")
if result=="yes": provider = ProviderType.CA
while True:
    name = raw_input("channel name: ")
    chan = Channel(name,provider)
    request = raw_input("request: ")
    args = list()
    while True:
        arg = raw_input("next arg or continue: ")
        if arg == "" :  break
        args.append(arg)
    try:
        chan.parsePut(args,request,True)
        print(chan.get(request))
    except Exception as ex:
        print(ex)
    request = raw_input("enter exit or continue: ")
    if request=="exit" : break

