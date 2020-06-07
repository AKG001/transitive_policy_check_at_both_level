#RWFM model class
#
#
#
#

#RWFM subject class
class Subject:
    def __init__(self, sub_id_index, owner, readers, writers):
	self.sub_id_index = sub_id_index
	self.owner = owner
	self.readers = readers
	self.writers = writers

#RWFM object class
class Object:
    def __init__(self, obj_id_index, owner, readers, writers):
	self.obj_id_index = obj_id_index
	self.owner = owner
	self.readers = readers
	self.writers = writers

#General RWFM model class
class RWFM:
    def __init__(self):
	self.num_subjects = 0
	self.num_objects = 0
	self.subjects = {}
	self.objects = {}

    #Add a new RWFM subject
    def add_a_subject(self, sub_id_index, owner, readers, writers):
	reader_encode = 0
	writer_encode = 0
	for r in readers:
	    reader_encode = reader_encode | 1 << r
	for w in writers:
	    writer_encode = writer_encode | 1 << w

	new_subject = Subject(sub_id_index, owner, reader_encode, writer_encode)
	self.subjects[sub_id_index] = new_subject
	self.num_subjects = self.num_subjects + 1

    #Print all RWFM subjects
    def print_all_subjects(self):
	for subject_id, value in self.subjects.items():
	    print (str(subject_id) + ":" + str(value.__dict__))

    #Add a new RWFM object
    def add_an_object(self, obj_id_index, owner, readers, writers):
	reader_encode = 0
	writer_encode = 0
	for r in readers:
	    reader_encode = reader_encode | 1 << r
	for w in writers:
	    writer_encode = writer_encode | 1 << w

	new_object = Object(obj_id_index, owner, reader_encode, writer_encode)
	self.objects[obj_id_index] = new_object
	self.num_objects = self.num_objects + 1

    #Print all RWFM objects
    def print_all_objects(self):
	for object_id, value in self.objects.items():
	    print (str(object_id) + ":" + str(value.__dict__))

    def write_labels_to_file(self):
	f = open("rwfm_labels", "w+")
	f.write(str(self.num_subjects) + "\n")

	for subject_id, value in self.subjects.items():
	    f.write(str(subject_id) + " " +  str(value.owner) + " " + str(value.readers) + " " + str(value.writers) + "\n")
	f.write(str(self.num_objects) + "\n")

	for object_id, value in self.objects.items():
	    f.write(str(object_id) + " " + str(value.owner) + " " + str(value.readers) + " " + str(value.writers) + "\n")
	f.close()
