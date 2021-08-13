# Access Security Using PvaPy

When the records are created, appropriate AS structures are initialized, 
using provided AS group and AS level, and those structures are then used when 
record is accessed to verify whether it is write-able or not for a given client.

The following example demonstrates how things work.

## Example

Start server and create record with AS, put it into DEFAULT group with level 1:

```sh
$ python
>>> from pvaccess import *
>>> s = PvaServer()
>>> print(s.isAsActive())
False
>>> pv = PvObject({'x' : INT}, {'x':0})
>>> s.addRecordWithAs('x', pv, 1, 'DEFAULT', None)
```

At this point record is fully write-able. From the second terminal, one can 
do this:

```sh
>>> c = Channel('x')
>>> print(c.get())
structure 
    int x 0

>>> pv = PvObject({'x' : INT}, {'x':1})
>>> c.put(pv)
>>> print(c.get())
structure 
    int x 1
```

Enable access security with a file like this:


```sh
$ cat /local/sveseli/EPICS/acl.conf
UAG(uag) {user1,user2}
HAG(hag) {host1,host2}
ASG(DEFAULT) {
    RULE(0,READ)
    RULE(0,WRITE) 
    RULE(1,READ)
    RULE(1,WRITE) {
        UAG(uag)
        HAG(hag)
    }
}
```

In python:

```sh
>>> s.initAs('/local/sveseli/EPICS/acl.conf', '')

    At this point, the record we created does not allow writes any longer:

>>> c = Channel('x')
>>> print(c.get())
structure 
    int x 1

>>> pv = PvObject({'x' : INT}, {'x':2})
>>> c.put(pv)
Traceback (most recent call last):
  File "", line 1, in 
pvaccess.pvaccess.PvaException: channel x PvaClientPut::put Channel put is not allowed
>>> print(c.get())
structure 
    int x 1
>>>
```

If we modify the above file to include client's machine and username, re-run initAs() method, the record can be written again using that same client.

