All kIFS-CAmkES examples are available at: kifs-camkes/projects/sel4-tutorials/tutorials

## To run a project:
1. Change the example folder name to hello-camkes-1
2. In kifs-camkes directory, run the following commands:

   a. mkdir <project_name>
   
   b. cd <project_name>
   
   c. ../init --plat pc99 --tut hello-camkes-1
   
   d. ninja
   
   e. ./simulate

Ids for components and interfaces can be found in <project_name>/rwfm_log file as follows: 

## Format for component:
(component object -> (component name, id, component type))
## Example:
(<camkes.ast.objects.Component object at 0x7fe8c8bd2a50> -> ('client2', 2, <class 'camkes.ast.objects.Component'>))

## Format for interface:
(interface object -> (interface name, id, interface type, parent))
## Example:
(client1.h2 -> ('h2', 6, <class 'camkes.ast.objects.Uses'>, 'client1'))

Here is a brief overview of the examples: 
1. hello-camkes-1-rpc-call: Demonstrates RPC Call from Client 1 to Echo to Client 2.
2. hello-camkes-1-rpc: Demonstrates RPC from Client 1 to Echo to Client 2. 
3. hello-camkes-1-timing-rpc-call: To calculate the number of RPC Calls in 100 seconds. (Use kifs-camkes/timing.sh to run it, after changing the example name to hello-camkes-1)
4. hello-camkes-1-timing-rpc: To calculate the number of RPC in 100 seconds. (Use kifs-camkes/timing.sh to run it, after changing the example name to hello-camkes-1)
5. hello-camkes-1-paper-leak: Demonstrates that RWFM helps in preventing the paper leak from TA to students.