# Data Distributor Plugin

In most cases (i.e., without any filters applied) all PVA or CA channel 
updates are served to all client applications. This model breaks down when
the network bandwidth is simply not large enough to send all updates to all 
clients continuously, or when data processing in client applications cannot 
keep up with channel updates.

The data distributor plugin enables distribution of channel data among 
multiple groups of client applications, as well as within a group of clients.
Channel updates are distributed to one group of clients at a time. Once
all clients in a group receive updates, the distributor plugin will start
updating next group of clients, moving from group to group in a 
round-robin fashion.

## Usage

The PV request string which triggers plugin instantiation is defined below:

```
"_[pydistributor=distinguishingField:<field_name>;groupId:<group_id>;nUpdatesPerConsumer:<n_updates>;updateMode:<update_mode>;distributorId:<distributorId>]"
```

The underscore character at the begining of the PV request object
indicates that the data distributor will be targeting entire PV structure.
The plugin parameters are the following:

- distinguishingField: this is the PV structure field that distinguishes 
different channel updates (default value: "timeStamp"); for example,
for area detector images one could use the "uniqueId" field of the NTND 
structure

- groupId: this parameter designates group that client belongs to 
(default value: "default")

- nUpdatesPerConsumer: this parameter configures how many sequential updates
a client will receive before the data distributor starts updating next client
(default value: "1")

- updateMode: this parameter configures how channel updates are to be
distributed between clients in a group (default: "0"); valid modes are:
  - 0: update goes to one consumer per group
  - 1: update goes to all consumers in a group

- distributorId: client applications may configure multiple distributors
(e.g., one per channel in a PVA server), and this parameter designates
distributor id from which client application wants to receive updates
(default value: "default")

The plugin obeys the following rules:

- Parameter names are case insensitive, but the string values
are not. For example, "groupId=abc" and "groupId=ABC" would indicate two
different groups of clients. 

- Updates for a group of clients are configured when the first client in a 
group requests data. Configuration values passed in the PV request by
the subsequent clients are ignored.

- A group is removed from the distributor once the last client in that
group disconnects. 

- The current channel PV object is always distributed to a client on an 
initial connect.

- The order in which clients and groups receive data is on a 
"first connected, first served basis".

## Examples

For all examples below we assume that PVA server is serving area detector
images on a channel pvap:image. All clients are started before the server
itself.


### Example 1

This example show behavior of three clients that belong to the same (default)
group. Data are distributed in a single client per group mode. Note that all
clients received current object on initial connection, and every third
object afterward:

Client 1:
```
$ pvget -m -r _[pydistributor=distinguishingField:uniqueId] pvapy:image  | grep uniqueId
    int uniqueId 0
    int uniqueId 0
    int uniqueId 3
    int uniqueId 6
    int uniqueId 9
```

Client 2:
```
$ pvget -m -r _[pydistributor=distinguishingField:uniqueId] pvapy:image  | grep uniqueId
    int uniqueId 0
    int uniqueId 1
    int uniqueId 4
    int uniqueId 7
    int uniqueId 10
```

Client 3:
```
$ pvget -m -r _[pydistributor=distinguishingField:uniqueId] pvapy:image  | grep uniqueId 
    int uniqueId 0
    int uniqueId 2
    int uniqueId 5
    int uniqueId 8
    int uniqueId 11
```

### Example 2

In this example we have two groups of two clients: group G1 receives 2 sequential updates in the "update single client per group" mode, while group G2 receives
3 sequential updates in the same mode. Group G1 connected first, and hence
it is the first to receive data.

In this case the first client in the group G1 receives two sequential
updates (0,1), the first client in the group G2 receives three sequential 
updates (2,3,4), the second client in the group G1 receives updates (5,6), 
the second client in the group G2 receives updates (7,8,9), and so on. 
This illustrates how the plugin distributes data between groups in 
a round-robin fashion, and to clients in a group depending on the group 
configuration. 

Client 1/Group G1:
```
$ pvget -m -r "_[pydistributor=groupId:G1;distinguishingField:uniqueId;nUpdatesPerConsumer:2]" pvapy:image  | grep uniqueId
    int uniqueId 0
    int uniqueId 0
    int uniqueId 1
    int uniqueId 10
    int uniqueId 11
    int uniqueId 20
    int uniqueId 21
```

Client 1/Group G2:
```
$ pvget -m -r "_[pydistributor=groupId:G2;distinguishingField:uniqueId;nUpdatesPerConsumer:3]" pvapy:image  | grep uniqueId
    int uniqueId 0
    int uniqueId 2
    int uniqueId 3
    int uniqueId 4
    int uniqueId 12
    int uniqueId 13
    int uniqueId 14
    int uniqueId 22
    int uniqueId 23
    int uniqueId 24
```

Client 2/Group G1:
```
$ pvget -m -r "_[pydistributor=groupId:G1;distinguishingField:uniqueId;nUpdatesPerConsumer:2]" pvapy:image  | grep uniqueId
    int uniqueId 0
    int uniqueId 5
    int uniqueId 6
    int uniqueId 15
    int uniqueId 16
    int uniqueId 25
    int uniqueId 26
```

Client 2/Group G2:
```
$ pvget -m -r "_[pydistributor=groupId:G2;distinguishingField:uniqueId;nUpdatesPerConsumer:3]" pvapy:image  | grep uniqueId
    int uniqueId 0
    int uniqueId 7
    int uniqueId 8
    int uniqueId 9
    int uniqueId 17
    int uniqueId 18
    int uniqueId 19
    int uniqueId 27
    int uniqueId 28
    int uniqueId 29
```

### Example 3

In this example we have two groups of two clients: group G1 receives two
sequential updates in the "update single client per group" mode, while
group G2 receives three sequential updates in the "update all clients per 
group" mode. Group G2 connected first, and hence it is the first to receive
data.

In this case both clients in the group G2 receive three sequential updates
(0,1,2), the first client in the group G1 receives two sequential updates
(3,4), both clients in the group G2 receive updates (5,6,7)
the second client in the group G1 receives updates (8,9), etc.

Client 1 and Client 2/Group G2:
```
$ pvget -m -r "_[pydistributor=groupId:G2;distinguishingField:uniqueId;nUpdatesPerConsumer:3;updateMode:1]" pvapy:image  | grep uniqueId
    int uniqueId 0
    int uniqueId 0
    int uniqueId 1
    int uniqueId 2
    int uniqueId 5
    int uniqueId 6
    int uniqueId 7
    int uniqueId 10
    int uniqueId 11
    int uniqueId 12
```

Client 1/Group G1:
```
$ pvget -m -r "_[pydistributor=groupId:G1;distinguishingField:uniqueId;nUpdatesPerConsumer:2]" pvapy:image  | grep uniqueId
    int uniqueId 0
    int uniqueId 3
    int uniqueId 4
    int uniqueId 13
    int uniqueId 14
    int uniqueId 23
    int uniqueId 24
```

Client 2/Group G1:
```
(daq) bluegill2> pvget -m -r "_[pydistributor=groupId:G1;distinguishingField:uniqueId;nUpdatesPerConsumer:2]" pvapy:image  | grep uniqueId
    int uniqueId 0
    int uniqueId 8
    int uniqueId 9
    int uniqueId 18
    int uniqueId 19
    int uniqueId 28
    int uniqueId 29
```

