# Data Distributor Plugin

In most cases (i.e., without any filters applied) all PVA or CA channel 
updates are served to all consumer applications. This model breaks down when
the network bandwidth is simply not large enough to send all updates to all 
consumers continuously, or when data processing in consumer applications cannot 
keep up with channel updates.

The data distributor plugin enables distribution of channel data among 
multiple groups of consumer applications, as well as within a group of 
consumers. Channel updates are distributed to one group of consumers at a time.
Once all consumers in a group receive updates, the distributor plugin will 
start updating next group of consumers, moving from group to group in a 
round-robin fashion.

## Requirements

This plugin is packaged with PvaPy version 4.1.0 or later, and requires 
epics base version > 7.0.6.1. One can use it with PVA servers that do not use 
pvDatabase, or with servers built using older versions of epics, with a
help of the PvaPy mirror server. The mirror server clones existing PVA or CA
channels and makes their data available on a mirrored channel. For example,
the following command will create 'pvapy:image' channel as a mirror of the
original area detector '13SIM1:Pva1:Image' channel:

```
$ pvapy-mirror-server --channel-map="(pvapy:image,13SIM1:Pva1:Image,PVA)"
```

Alternatively, the mirror server class can also be used directly from
python in the following fashion:

```
>>> import pvaccess as pva
>>> s = pva.PvaMirrorServer()
>>> s.addMirrorRecord('pvapy:image', '13SIM1:Pva1:Image', pva.PVA)
```

## Usage

The PV request string which triggers plugin instantiation is defined below:

```
"_[pydistributor=uniqueField:<field_name>;groupId:<group_id>;nUpdatesPerConsumer:<n_updates>;updateMode:<update_mode>;distributorId:<distributorId>]"
```

The underscore character at the begining of the PV request object
indicates that the data distributor will be targeting entire PV structure.
The same PV request string format should work regardless of the language
in which a particular consumer application is written.

The plugin parameters are the following:

- uniqueField: this is the PV structure field that distinguishes 
different channel updates (default value: "timeStamp"); for example,
for area detector images one could use the "uniqueId" field of the NTND 
structure

- groupId: this parameter designates group that consumer belongs to 
(default value: "default")

- nUpdatesPerConsumer: this parameter configures how many sequential updates
a consumer will receive before the data distributor starts updating next 
consumer (default value: "1")

- updateMode: this parameter configures how channel updates are to be
distributed between consumers in a group (default is "0" if group id is not
specified, and "1" if group id is specified); valid modes are:
  - 0: update goes to one consumer per group
  - 1: update goes to all consumers in a group

- distributorId: consumer applications may configure multiple distributors
(e.g., one per channel in a PVA server), and this parameter designates
distributor id from which consumer application wants to receive updates
(default value: "default"); distributor instances are completely independent
and have separate sets of consumer groups

The plugin obeys the following rules:

- Parameter names are case insensitive, but the string values
are not. For example, "groupId=abc" and "groupId=ABC" would indicate two
different groups of consumers. 

- Updates for a group of consumers are configured when the first consumer in a 
group requests data. Configuration values passed in the PV request by
the subsequent consumers are ignored.

- A group is removed from the distributor once the last consumer in that
group disconnects. 

- The current channel PV object is always distributed to a consumer on an 
initial connect. 

- The order in which consumers and groups receive data is on a 
"first connected, first served basis".

- As consumers connect and disconnect, the data distribution adjusts
accordingly. For example, with a single consumer group configured
to give one sequential update per consumer, three consumers would each be
receiving every third update; after consumer number four connects, all 
consumers would start receiving every fourth update; if one of those then
disconnects, remaining three consumers would again be receiving every third
update.

## Examples

For all examples below we assume that PVA server is serving area detector
images on a channel pvapy:image (see, for example, PvaPy AD Simulator Server). 
All consumers are started before the server itself. Initial (empty) object 
has unique id of 0.

### Example 1

This example show behavior of three consumers that belong to the same (default)
group. Data are distributed in a single consumer per group mode. Note that all
consumers received current object on initial connection, and every third
object afterward:

Consumer 1:
```
$ pvget -m -r _[pydistributor=uniqueField:uniqueId] pvapy:image  | grep uniqueId
    int uniqueId 0
    int uniqueId 1
    int uniqueId 4
    int uniqueId 7
    int uniqueId 10
```

Consumer 2:
```
$ pvget -m -r _[pydistributor=uniqueField:uniqueId] pvapy:image  | grep uniqueId
    int uniqueId 0
    int uniqueId 2
    int uniqueId 5
    int uniqueId 8
    int uniqueId 11
```

Consumer 3:
```
$ pvget -m -r _[pydistributor=uniqueField:uniqueId] pvapy:image  | grep uniqueId 
    int uniqueId 0
    int uniqueId 3
    int uniqueId 6
    int uniqueId 9
    int uniqueId 12
```

### Example 2

In this example we have two groups of two consumers: group G1 receives two, and
group G2 three sequential updates. By default, the update mode is set to the 
"update all consumers per group". 

Group G2 connected first, and hence it is the first to receive data. All 
consumers from group G2 receive updates (1,2,3), all consumers from group G1
receive updates (4,5), group G2 receives updates (6,7,8), etc.

Consumer 1 and Consumer 2/Group G2:
```
$ (daq) bluegill2> pvget -m -r "_[pydistributor=groupId:G2;uniqueField:uniqueId;nUpdatesPerConsumer:3]" pvapy:image  | grep uniqueId
    int uniqueId 0
    int uniqueId 1
    int uniqueId 2
    int uniqueId 3
    int uniqueId 6
    int uniqueId 7
    int uniqueId 8
    int uniqueId 11
    int uniqueId 12
    int uniqueId 13
```

Consumer 1 and Consumer 2/Group G1:
```
$ pvget -m -r "_[pydistributor=groupId:G1;uniqueField:uniqueId;nUpdatesPerConsumer:2]" pvapy:image  | grep uniqueId
    int uniqueId 0
    int uniqueId 4
    int uniqueId 5
    int uniqueId 9
    int uniqueId 10
    int uniqueId 14
    int uniqueId 15
```

### Example 3

This example illustrates mixed update mode in which we have two groups of 
two consumers: the group G1 receives two sequential updates in the 
"update single consumer per group" mode, while the group G2 receives 
three sequential updates in the "update all consumers per 
group" mode (the default mode when group id parameters are present in the
PV request string). 

Group G2 connected first, and hence it is the first to receive data.
In this case both consumers in the group G2 receive three sequential updates
(1,2,3), the first consumer in the group G1 receives two sequential updates
(4,5), both consumers in the group G2 receive updates (6,7,8)
the second consumer in the group G1 receives updates (9,10), etc.

Consumer 1 and Consumer 2/Group G2:
```
$ pvget -m -r "_[pydistributor=groupId:G2;uniqueField:uniqueId;nUpdatesPerConsumer:3]" pvapy:image  | grep uniqueId
    int uniqueId 0
    int uniqueId 1
    int uniqueId 2
    int uniqueId 3
    int uniqueId 6
    int uniqueId 7
    int uniqueId 8
    int uniqueId 11
    int uniqueId 12
    int uniqueId 13
    int uniqueId 16
    int uniqueId 17
    int uniqueId 18
```

Consumer 1/Group G1:
```
$ pvget -m -r "_[pydistributor=groupId:G1;uniqueField:uniqueId;nUpdatesPerConsumer:2;updateMode=0]" pvapy:image  | grep uniqueId

    int uniqueId 0
    int uniqueId 4
    int uniqueId 5
    int uniqueId 14
    int uniqueId 15
    int uniqueId 24
    int uniqueId 25
```

Consumer 2/Group G1:
```
$ pvget -m -r "_[pydistributor=groupId:G1;uniqueField:uniqueId;nUpdatesPerConsumer:2;updateMode=0]" pvapy:image  | grep uniqueId
    int uniqueId 0
    int uniqueId 9
    int uniqueId 10
    int uniqueId 19
    int uniqueId 20
```

