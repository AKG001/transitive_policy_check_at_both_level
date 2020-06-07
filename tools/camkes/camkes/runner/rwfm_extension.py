#   RWFM wrap arround code over seL4
#
#
#
#
#

from camkes.ast import Instance, Connection, Component, Uses, Provides
import ctypes
from camkes.runner.rwfmmodel import *

global_id = 1
component_list = {}
connections_list = {}
interfaces_list = {}
rwfm_from_end_interface_list = {}

seL4_as_owner = "0"

logging_file = "rwfm_log"
subject_initial_label_file = "rwfm_subject_initial_label"
object_initial_label_file = "rwfm_object_initial_label"
connection_initial_label_file = "rwfm_connection_initial_label"

#RWFM object
rwfm = RWFM()

def get_subject_list():
    return component_list

def get_object_list():
    return interfaces_list

def print_list (list_obj, list_name):
    file = open(logging_file, 'a+')
    file.write ("\n"+list_name+"\n\n")
    for key, value in list_obj.items():
	file.write ("(" + str(key) + " -> " + str(value) +")\n" )
    file.close()

def find_subjects_and_connections(assembly):
    global global_id

    #Finding and saving the component names which will act as subjects. Format for saving (Object->(Name, ID, Type))
    for name, obj in assembly.composition._mapping.items():
	if isinstance(obj, Instance) and name.encode("ascii")!='rwfm_monitor':
            component_list[obj.type] = (name.encode("ascii"), global_id, type(obj.type))
	    global_id = global_id + 1
    print_list(component_list, "component_list")
    
    #Finding and saving the connection names. Format for saving (Object->(Name, ID, Type))
    for name, obj in assembly.composition._mapping.items():
	if isinstance(obj, Connection) \
	   and obj._to_ends[0]._instance._name.encode("ascii")!='rwfm_monitor' \
	   and obj._from_ends[0]._instance._name.encode("ascii")!='rwfm_monitor':
	    connections_list[obj] = (name.encode("ascii"), global_id, type(obj))
	    global_id = global_id + 1
    print_list(connections_list, "connections_list")


def find_interfaces(assembly):
    global global_id
    for interfaces in assembly.composition.connections:
	for from_end in interfaces.from_ends:
	    if from_end._instance._name.encode("ascii") != 'rwfm_monitor' \
	       and from_end._parent._to_ends[0]._instance._name.encode("ascii") != 'rwfm_monitor':
	        interfaces_list[from_end] = (from_end.interface.name.encode("ascii"), global_id, type(from_end.interface), from_end.instance.name.encode("ascii"))
	        global_id = global_id + 1
	for to_end in interfaces.to_ends:
	    if to_end._instance._name.encode("ascii") != 'rwfm_monitor' \
	       and to_end._parent._from_ends[0]._instance._name.encode("ascii") != 'rwfm_monitor':
	        interfaces_list[to_end] = (to_end.interface.name.encode("ascii"), global_id, type(to_end.interface), to_end.instance.name.encode("ascii"))
	        global_id = global_id + 1
    print_list (interfaces_list, "interface_list")

def find_rwfm_from_end_interfaces(assembly):
    for interfaces in assembly.composition.connections:
	for from_end in interfaces.from_ends:
	    if from_end._parent._to_ends[0]._instance._name.encode("ascii") == 'rwfm_monitor':
	        rwfm_from_end_interface_list[from_end.instance.name.encode("ascii")] = from_end.interface.name.encode("ascii")
    print_list (rwfm_from_end_interface_list, "RWFM Monitor interface list")

#Function to generate the initial labels for components
def generate_subject_initial_labels():
    file = open (subject_initial_label_file, 'w+')

    #Create a reader list with all the components IDs.
    reader_list = []
    for r in component_list.values():
	reader_list.append(r[1])

    readers = '{'    
    for clients in component_list.values():
	readers = readers + " " + str(clients[1])
    readers = readers + " }"

    for clients in component_list.values():
	v = str(clients[1])

	#Adding RWFM subjects
	rwfm.add_a_subject(clients[1], clients[1], reader_list, [clients[1]])

	file.write(v + "  (" + v + "," + v + "," + readers + "," + v + ")\n")
    file.close()
    rwfm.print_all_subjects()

#Function to generate the initial labels for connections
def generate_connections_initial_labels():
    file = open(connection_initial_label_file, 'w+')
    for conn, value in connections_list.items():
	from_end = conn.from_end.instance.name.encode("ascii")
	to_end = conn.to_end.instance.name.encode("ascii")
	for client in component_list.values():
	    if (from_end == client[0]):
		from_end = client[1]
	    if (to_end == client[0]):
		to_end = client[1]

	#Adding the connections as RWFM objects
	rwfm.add_an_object(value[1], int(seL4_as_owner), [to_end], [from_end])

	file.write (str(value[1]) + " (" + str(value[1]) + "," + seL4_as_owner + ",{" + str(to_end) + "}" + ",{" + str(from_end) + "}" + ")\n")
    file.close()
    rwfm.print_all_objects()

#Function to generate the initial labels for the interfaces(connection ends)
def generate_object_initial_labels():
    file = open (object_initial_label_file, 'w+')

    for interface_obj, interface_value in interfaces_list.items():
	reader = []
	writer = []
	reader_id = []
	writer_id = []
	if interface_obj._parent._type._name.encode("ascii") == "seL4RPCCall":
            if interface_obj.end == "from":
                reader.append(interface_obj.parent.to_end.instance.name.encode("ascii"))
		reader.append(interface_obj.instance.name.encode("ascii"))
                writer.append(interface_obj.parent.to_end.instance.name.encode("ascii"))
                writer.append(interface_obj.instance.name.encode("ascii"))

            else:
                reader.append(interface_obj.instance.name.encode("ascii"))
		reader.append(interface_obj.parent.from_end.instance.name.encode("ascii"))
                writer.append(interface_obj.instance.name.encode("ascii"))
                writer.append(interface_obj.parent.from_end.instance.name.encode("ascii"))


	elif interface_obj._parent._type._name.encode("ascii") == "seL4RPC":
	    if interface_obj.end == "from":
	        reader.append(interface_obj.parent.to_end.instance.name.encode("ascii"))
	        writer.append(interface_obj.instance.name.encode("ascii"))
	    else:
	        reader.append(interface_obj.instance.name.encode("ascii"))
	        writer.append(interface_obj.parent.from_end.instance.name.encode("ascii"))

	for values in component_list.values():
	    if interface_value[3] == values[0]:
		owner = values[1]
	    if values[0] in reader:
		reader_id.append(values[1])
	    if values[0] in writer:
		writer_id.append(values[1])

	#Adding an object to RWFM object rwfm
	rwfm.add_an_object (interface_value[1], owner, reader_id, writer_id)

	file.write(str(interface_value[1]) + " (" + str(interface_value[1]) + "," + str(owner) + "," + str(reader_id) + "," + str(writer_id) + ")\n")
    file.close()

def generate_final_initial_labels():
    rwfm.write_labels_to_file()

def get_rwfm_object():
    return rwfm

def get_rwfm_from_end_interfaces_list():
    return rwfm_from_end_interface_list
