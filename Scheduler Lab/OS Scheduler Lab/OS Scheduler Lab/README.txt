To build the executable:
$ make

to run the executable:
$ ./sched [-v] [-s<schedspec>] inputfile randfile

Note: When you are specifying the scheduler type with a quantum input, 
do not leave a space between the scheduler type and the quantum. 

For example this is a valid:
	$ ./sched -v -sR2 inputfile randfile

This is not valid:
	$ ./sched -v -sR 2 inputfile randfile
	
	