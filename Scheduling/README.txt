  WORKPLACE ORGANIZATION:
mkdir ../work
cd ../work
cp ../ftp/* .
gzip -d sched.tgz
tar -xvf sched.tar
rm sched.tar


  READING:
less/vi README.tjk
netscape/lynx install_unix.html
netscape/lynx user_guide.html


  COMPILE:
javac -nowarn *.java


  RUN:
java Scheduling scheduling.conf


  FILES:
input file name:   scheduling.conf
output file name:  Summary-Processes
log file name:     Summary-Processes


--[ YOUR TASK ]-------------------------------------------------------

Create a configuration file in which all processes run an average
of 2000 milliseconds with a standard deviation of zero, and which
are blocked for input or output every 500 milliseconds. Run the
simulation for 10000 milliseconds with 2 processes. Examine the
two output files. Try again for 5 processes. Try again for 10
processes. Explain what's happening.

----------------------------------------------------------------------

The Configuration File

   The configuration file (scheduling.conf) is used to specify various
   parameters for the simulation, including:
     * the number of processes,
     * the mean runtime for a process,
     * the standard deviation in runtime for a process,
     * for each process, how long the process runs before it blocks for  
       input or output, and
     * how long the simulation should run.
       
  Configuration File Options
  
   There are a number of options which can be specified in the
   configuration file. These are summarized in the table below.
   
   Keyword Values Description
   numprocess n The number of processes to create for the simulation.
   meandev n The average length of time in milliseconds that a process
   should execute before terminating.
   standdev n The number of standard deviations from the average length
   of time a process should execute before terminating.
   process n The amount of time in milliseconds that the process should
   execute before blocking for input or output. There should be a
   separate process directive for each process specified by the
   numprocess directive.
   runtime n The maximum amount of time the simulation should run in
   milliseconds.

----------------------------------------------------------------------
  
  Sample Configuration File
  
   The "scheduling.conf" configuration file looks like this:
   
// # of Process
numprocess 3

// mean deivation
meandev 1100

// standard deviation
standdev 510

// process    # I/O blocking
process 100
process 500
process 30

// duration of the simulation in milliseconds
runtime 5000


----------------------------------------------------------------------
  
The Summary-Results File

   The Summary-Results file contains a summary report describing the
   simulation and includes one line of summary information for each
   process. The fields and columns in the report are described in the
   following table.
   
   Field Description
   Scheduling Type: The type of the scheduling algorithm used. The value
   displayed is "hard coded" in the SchedulingAlgorithm.java file.
   Scheduling Name: The name of the scheduling algorithm used. The value
   displayed is "hard coded" in the SchedulingAlgorithm.java file.
   Simulation Run Time: The number of milliseconds that the simulation
   ran. This may be less than or equal to the total amount of time
   specified by the "runtime" configuration parameter.
   Mean: The average amount of runtime for the processes as specified by
   the "meandev" configuration parameter.
   Standard Deviation: The standard deviation from the average amount of
   runtime for the processes as specified by the "standdev" configuration
   parameter.
   Process # The process number assigned to the process by the simulator.
   The process number is between 0 and n-1, where n is the number
   specified by the "numprocess" configuration parameter.
   CPU Time The randomly generated total runtime for the process in
   milliseconds. This is determined by the "meandev" and "standdev"
   parameters in the configuration file.
   IO Blocking The amount of time the process runs before it blocks for
   input or output. This is specified for each process by a "process"
   directive in the configuration file.
   CPU Completed The amount of runtime in milliseconds completed for the
   process. Note that this may be less than the CPU Time for the process
   if the simulator runs out of time as specified by the "runtime"
   configuration parameter.
   CPU Blocked The number of times the process blocked for input or
   output during the simulation.
   
  Sample Summary-Results File
  
   The output "Summary-Results" file looks something like this:
   
Scheduling Type: Batch (Nonpreemptive)
Scheduling Name: First-Come First-Served
Simulation Run Time: 2750  
Mean: 1100
Standard Deviation: 510
Process #       CPU Time        IO Blocking     CPU Completed   CPU Blocked
0               1372 (ms)       100 (ms)        1372 (ms)       13 times
1               689 (ms)        500 (ms)        689 (ms)        1 times
2               689 (ms)        30 (ms)         689 (ms)        22 times

----------------------------------------------------------------------
  
The Summary-Processes File

   The Summary-Processes file contains a log of the actions taken by the
   scheduling algorithm as it considers each process in the scheduling
   queue.
   
   Each line in the log file is of the following form:
   
Process: process-number process-status... (cpu-time block-time accumulated-time
 accumulated-time)

   The fields in the line are described in the table below.
   
   Field Description
   process-number The process number assigned to the process by the
   simulator. This is a number between 0 and n-1, where n is the value
   specified for the "numprocess" configuration parameter.
   process-status The status of the process at this point in time. If
   "registered" then the process is under consideration by the scheduling
   algorithm. If "I/O blocked", then the scheduling algorithm has noticed
   that the process is blocked for input or output. If "completed", then
   the scheduling algorithm has noticed that the process has met or
   exceeded its allocated execution time.
   cpu-time The total amount of run time allowed for this process. This 
   number is randomly generated for the process based on the "meandev"
   and "standdev" values specified in the configuration file.
   block-time The amount of time in milliseconds to execute before
   blocking process. This number is specified for the process by the
   "process" directive in the configuration file.
   accumulated-time The total amount of time process has executed in
   milliseconds. (This number appears twice in the log file; one should 
   be removed).
   
  Sample Summary-Processes File
  
   The output "Summary-Processes" file looks something like this:
   
Process: 0 registered... (1372 100 0 0)
Process: 0 I/O blocked... (1372 100 100 100)
Process: 1 registered... (689 500 0 0)
Process: 1 I/O blocked... (689 500 500 500)
Process: 0 registered... (1372 100 100 100)
Process: 0 I/O blocked... (1372 100 200 200)
Process: 1 registered... (689 500 500 500)
Process: 1 completed... (689 500 689 689)
Process: 0 registered... (1372 100 200 200)
Process: 0 I/O blocked... (1372 100 300 300)
Process: 2 registered... (689 30 0 0)
Process: 2 I/O blocked... (689 30 30 30)

