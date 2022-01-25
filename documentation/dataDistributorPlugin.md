# Data Distributor Plugin

In most cases (i.e., without any filters applied) all PVA or CA channel 
updates are served to all client applications. This can become a problem 
when the network bandwidth is simply not large enough to send all updates to 
all clients continuously, or when data processing in client applications 
cannot keep up with channel updates. One example of this might be 
real time processing of Area Detector images.

The data distributor plugin enables distribution of channel data between
multiple client applications. The plugin considers two basic use cases
for a group of clients: 

- For simple parallel processing where client applications do not need
to share data all clients in a group receive n sequential updates 
in a round-robin fashion: client \#1 sees the first n updates, client \#2 the
second n updates, and so on. 

- For data analysis where several cooperating client applications must all
see the same data in order to process it the applications are grouped 
into sets, and each set of clients receives the same number of sequential 
updates. The first n updates are sent to all members of client set #1, the second n updates are sent to all members of client set #2, and so on.

## Requirements

This plugin is packaged with PvaPy version 4.1.0 or later. It relies on the 
pvDatabase plugin framework and requires epics base version > 7.0.6.1. 
With a help of the PvaPy mirror server, one can use it with PVA servers 
that do not use pvDatabase, or with servers built using older versions of 
epics. As its name indicates, the mirror server clones existing PVA or CA
channels and makes their data available on a mirrored PVA channel. For example,
the following command will create 'pvapy:image' channel as the mirror of the
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

The PV request object which triggers plugin instantiation is defined below:

```
"_[pydistributor=group:<group id>;set:<set_id>;trigger:<field_name>;updates:<n_updates>;mode:<update_mode>]"
```

The underscore character at the begining of the PV request object
indicates that the data distributor will be targeting entire PV structure.
The same PV request object format should work regardless of the language
in which a particular client application is written.

The plugin parameters are the following:

- group: this parameter indicates a group that client application belongs to
(default value: "default"); groups of clients are completely independent
of each other

- set: this parameter designates a client set that application belongs to 
within its group (default value: "default")

- trigger: this is the PV structure field that distinguishes 
different channel updates (default value: "timeStamp"); for example,
for area detector images one could use the "uniqueId" field of the NTND 
structure

- updates: this parameter configures how many sequential updates
a client (or a set of clients) will receive before the data distributor 
starts updating the next one (default value: "1")

- mode: this parameter configures how channel updates are to be
distributed between clients in a set:
  - one: update goes to one client per set
  - all: update goes to all clients in a set
  - default is "one" if client set id is not specified, and "all" if set 
    id is specified

The plugin obeys the following rules:

- Parameter names are case insensitive, but the string values
are not. For example, "group=abc" and "group=ABC" would indicate two
different groups of clients. 

- Updates for a set of clients are configured when the first client in
the set requests data. Configuration values (i.e., "trigger", 
"updates", and "mode"), passed in the PV request by the subsequent 
clients are ignored.

- A set is removed from the group once the last client in that
set disconnects. 

- A group is removed from the distributor plugin once all of its 
clients have disconnected.

- Different client groups are completely independent of each other.
In other words, channel updates sent to clients belonging to 
group A do not interfere with updates sent to clients
belonging to group B.

- The order in which clients and groups receive data is on a 
"first connected, first served basis".

- The current channel PV object is always distributed to a client on an 
initial connect. 

- Data distribution is dynamic with respect to the number of clients. 
As clients connect and disconnect, the data distribution in a group adjusts
accordingly. For example, with a group of clients configured to 
distribute one sequential update to each client, three clients would each be
receiving every third update; after client number four connects, all 
clients would start receiving every fourth update; if one of those then
disconnects, remaining three clients would again be receiving every third
update. 

## Examples

For all examples below we assume that PVA server is serving area detector
images on a channel pvapy:image (see, for example, PvaPy AD Simulator Server). 
All clients are started before the server itself. Initial (empty) object 
has unique id of 0.

### Example 1

This example show behavior of three clients that belong to the same (default)
group. Each client receives one sequential update in a round-robin fashion. 
Note that all clients received current object on initial connection, 
and every third object afterward:

Client 1:
```
$ pvget -m -r _[pydistributor=trigger:uniqueId] pvapy:image | grep uniqueId
    int uniqueId 0
    int uniqueId 1
    int uniqueId 4
    int uniqueId 7
    int uniqueId 10
```

Client 2:
```
$ pvget -m -r _[pydistributor=trigger:uniqueId] pvapy:image | grep uniqueId
    int uniqueId 0
    int uniqueId 2
    int uniqueId 5
    int uniqueId 8
    int uniqueId 11
```

Client 3:
```
$ pvget -m -r _[pydistributor=trigger:uniqueId] pvapy:image | grep uniqueId 
    int uniqueId 0
    int uniqueId 3
    int uniqueId 6
    int uniqueId 9
    int uniqueId 12
```

### Example 2

In this example we have two sets of two clients, each client set receiving 
three sequential updates. Both clients from client set \#1 receive updates 
(1,2,3), both clients from client set \#2 receive updates (4,5,6), 
client set \#1 receives updates (7,8,9), and so on.

Client 1 and Client 2/Set 1:
```
$ pvget -m -r "_[pydistributor=set:S1;trigger:uniqueId;updates:3]" pvapy:image  | grep uniqueId
    int uniqueId 0
    int uniqueId 1
    int uniqueId 2
    int uniqueId 3
    int uniqueId 7
    int uniqueId 8
    int uniqueId 9
    int uniqueId 13
    int uniqueId 14
    int uniqueId 15
```

Client 3 and Client 4/Set 2:
```
$ pvget -m -r "_[pydistributor=set:S2;trigger:uniqueId;updates:3]" pvapy:image  | grep uniqueId
    int uniqueId 0
    int uniqueId 4
    int uniqueId 5
    int uniqueId 6
    int uniqueId 10
    int uniqueId 11
    int uniqueId 12
    int uniqueId 16
    int uniqueId 17
    int uniqueId 18
```

### Example 3

This example illustrates what happens when multiple independent groups of 
clients connect to the same channel. Group G1 has two clients belonging
to the same default set, and requesting one sequential update per client, while
Group G2 has two clients in the default set requesting three
sequential updates per client.

In this case the first client in group G1 receives updates
(1,3,5,...), while the second one receives updates (2,4,6,...). On the
other hand, the first client in group G2 receives updates
(1,2,3,7,8,9,...), while the second one receives updates (4,5,6,10,11,12,...).

Client 1/Group G1:
```
$ pvget -m -r "_[pydistributor=group:G1;trigger:uniqueId]" pvapy:image | grep uniqueId
    int uniqueId 0
    int uniqueId 1
    int uniqueId 3
    int uniqueId 5
    int uniqueId 7
    int uniqueId 9
```

Client 2/Group G1:
```
pvget -m -r "_[pydistributor=group:G1;trigger:uniqueId]" pvapy:image | grep uniqueId
    int uniqueId 0
    int uniqueId 2
    int uniqueId 4
    int uniqueId 6
    int uniqueId 8
```

Client 1/Group G2:
```
$ pvget -m -r "_[pydistributor=group:G2;trigger:uniqueId;updates:3]" pvapy:image | grep uniqueId
    int uniqueId 0
    int uniqueId 1
    int uniqueId 2
    int uniqueId 3
    int uniqueId 7
    int uniqueId 8
    int uniqueId 9
```

Client 2/Group G2:
```
$ pvget -m -r "_[pydistributor=group:D2;trigger:uniqueId;updates:3]" pvapy:image | grep uniqueId
    int uniqueId 0
    int uniqueId 4
    int uniqueId 5
    int uniqueId 6
    int uniqueId 10
    int uniqueId 11
    int uniqueId 12
```

The above shows that the two client groups do not interfere with each other.

