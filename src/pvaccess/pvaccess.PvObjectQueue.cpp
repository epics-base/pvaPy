// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include "boost/python/class.hpp"
#include "boost/python/self.hpp"
#include "boost/python/operators.hpp"
#include "pvapy.environment.h"
#include "PvObjectQueue.h"

using namespace boost::python;


//
// PvObjectQueue class
//
void wrapPvObjectQueue()
{

class_<PvObjectQueue>("PvObjectQueue", 
    "PvObjectQueue is a class that can be used for receiving channel updates.\n\n"
    "**PvObjectQueue([maxLength])**\n\n"
    "\t:Parameter: *maxLength* (int) - (optional) maximum queue length; if not provided, queue length will be unlimited\n\n"
    "\tExample:\n\n"
    "\t::\n\n"
    "\t\tpvq = PvObjectQueue(10000)\n\n"
    "\n\n", 
    init<>())

    .def(init<int>(args("maxLength")))

    .def("__len__",
        static_cast<unsigned int(PvObjectQueue::*)()>(&PvObjectQueue::size),
        "Retrieves queue size.\n\n"
        "::\n\n"
        "    size = len(pvq)\n\n")

    .def("get",
        static_cast<PvObject(PvObjectQueue::*)()>(&PvObjectQueue::get),
        "Retrieves PvObject from the queue.\n\n"
        ":Returns: PvObject from the queue\n\n"
        ":Raises: *QueueEmpty* - when the queue is empty\n\n"
        "::\n\n"
        "    pv = pvq.get()\n\n")

    .def("get",
        static_cast<PvObject(PvObjectQueue::*)(double)>(&PvObjectQueue::get),
        "Retrieves PvObject from the queue with wait if the queue is empty.\n\n"
        ":Parameter: *timeout* (float) - amount of time to wait for a new PvObject if queue is empty\n\n"
        ":Returns: PvObject from the queue\n\n"
        ":Raises: *QueueEmpty* - when the queue is empty after the specified timeout\n\n"
        "::\n\n"
        "    pv = pvq.get(10)\n\n")

    .def("put",
        static_cast<void(PvObjectQueue::*)(const PvObject&)>(&PvObjectQueue::put),
        args("pvObject"),
        "Puts PvObject into the queue.\n\n"
        ":Parameter: *pvObject* (PvObject) - PV object that will be pushed into the queue\n\n"
        ":Raises: *QueueFull* - when the queue is full\n\n"
        "::\n\n"
        "    pvq.put(PvInt(1))\n\n")

    .def("put",
        static_cast<void(PvObjectQueue::*)(const PvObject&,double)>(&PvObjectQueue::put),
        args("pvObject", "timeout"),
        "Puts PvObject into the queue with wait if the queue is full.\n\n"
        ":Parameter: *pvObject* (PvObject) - PV object that will be pushed into the queue\n\n"
        ":Parameter: *timeout* (float) - amount of time to wait if the queue is full\n\n"
        ":Raises: *QueueFull* - when the queue is full after the specified timeout\n\n"
        "::\n\n"
        "    pvq.put(PvInt(1), 10)\n\n")

    .def("waitForPut",
        static_cast<void(PvObjectQueue::*)(double)>(&PvObjectQueue::waitForPut),
        args("timeout"),
        "Waits until the new PvObject is pushed into the queue.\n\n"
        "::\n\n"
        "    pvq.waitForPut(1.0)\n\n")

    .def("waitForGet",
        static_cast<void(PvObjectQueue::*)(double)>(&PvObjectQueue::waitForGet),
        args("timeout"),
        "Waits until the new PvObject is retrieved from the queue.\n\n"
        "::\n\n"
        "    pvq.waitForGet(1.0)\n\n")

    .def("cancelWaitForPut",
        static_cast<void(PvObjectQueue::*)()>(&PvObjectQueue::cancelWaitForPut),
        "Cancels wait on queue put.\n\n"
        "::\n\n"
        "    pvq.cancelWaitForPut()\n\n")

    .def("cancelWaitForGet",
        static_cast<void(PvObjectQueue::*)()>(&PvObjectQueue::cancelWaitForGet),
        "Cancels wait on queue get.\n\n"
        "::\n\n"
        "    pvq.cancelWaitForGet()\n\n")

    .def("clear",
        static_cast<void(PvObjectQueue::*)()>(&PvObjectQueue::clear),
        "Clear queue.\n\n"
        "::\n\n"
        "    pvq.clear()\n\n")

    .def("resetCounters",
        static_cast<void(PvObjectQueue::*)()>(&PvObjectQueue::resetCounters),
        "Reset all statistics counters to zero.\n\n"
        "::\n\n"
        "    pvq.resetCounters()\n\n")

    .def("getCounters",
        static_cast<dict(PvObjectQueue::*)()>(&PvObjectQueue::getCounters),
        "Retrieve dictionary with all statistics counters, which include number of PvObjects accepted (pushed into the queue), rejected (not pushed into the queue) and retrieved (popped from the queue). The dictionary might also contain user defined counters, or other system counters, such as the number of PVA chanel monitor overruns.\n\n"
        ":Returns: dictionary containing available statistics counters\n\n"
        "::\n\n"
        "    counterDict = pvq.getCounters()\n\n")

    .def("setCounter",
        static_cast<void(PvObjectQueue::*)(const std::string&, unsigned int)>(&PvObjectQueue::setCounter),
        args("key", "value"),
        "Sets value for the statistics counter identified with a given key. Note that setting system managed counters (nReceived, nRejected, nDelivered, nQueued) will not work.\n\n"
        ":Parameter: *key* (str) - counter key\n\n"
        ":Parameter: *value* (int) - counter value (should be >= 0)\n\n"
        "::\n\n"
        "    pvq.setCounter('myCnt', 1)\n\n")

    .def("addToCounter",
        static_cast<void(PvObjectQueue::*)(const std::string&, unsigned int)>(&PvObjectQueue::addToCounter),
        args("key", "value"),
        "Adds value to the statistics counter identified with a given key. Note that manipulating system managed counters (nReceived, nRejected, nDelivered, nQueued) will not work.\n\n"
        ":Parameter: *key* (str) - counter key\n\n"
        ":Parameter: *value* (int) - counter value (should be >= 0)\n\n"
        "::\n\n"
        "    pvq.addToCounter('myCnt', 1)\n\n")

    .add_property("maxLength", &PvObjectQueue::getMaxLength, &PvObjectQueue::setMaxLength)

;

} // wrapPvObjectQueue()

