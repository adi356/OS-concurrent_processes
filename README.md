# 4760-concurrent_processes

### Purpose
The goal of this project is to beecome familiar with concurrent processing in Linux using shared memory. You will use multiple concurrent processes to write into a file at random times, solving the concurrency issues using the multiple concurrent processes algorithm (algorithm 4 in notes) to synchronize processes. Your job is to create the environment such that two processes cannot write into the file simultaneously and yet, every process gets its turn to write into the file.

### Task
Generate twenty processes using a master program, called **master**, and make them write into a file called **cstest** in their current working directory. Needless to say that all processes will use the same working directory. Each child process will be executed by an executable called **slave**. The message to be written into the file is: 

<div align="center"> `HH:MM:SS File modified by process number xx` </div>

where HH:MM:SS is the current system time, xx is the logical process number as specified by **master**. The value of xx is between 1 and 20. This implies that the child process will be run by the command:

<div align="center"> `slave xx` </div>

The critical resource is the file **cstest** which should be updated by a child under exclusive control access. This implies that each **slave** will have a criticial section that will control access to the file to write into it.

#### The main program _master_

Write **master** that runs up to _n_ **slave** processes at a time. Make sure the **n** never exceeds 20. Start **master** by typing the following command:

`master -t ss n`

where **ss** is the maximum time in seconds (default 100 seconds) after which the process should terminate itself if not completed.

Implement **master** as follows:

1. Check for the command line argument and output a usage message if the argument is not appropriate. If **n** is more than 20, issue a warning and limit **n** to 20. It will be a good idea to **#define** the maximum value of **n** or keep it as a configurable.
2. Allocate shared memory and initialize it appropriately.
3. Execute the **slave** processes and wait for all of them to terminate.
4. Start a time for a specified number of seconds (default: 100). If all children have not terminated by then, terminate the children.
5. Deallocate shared memory and terminate.

#### The Application Program (slave)

The **slave** just writes the message into the file inside the critical section. We want to have some log messages to see that the process is behaving appropriately and it does follow the guidance required for critical section.

If a process starts to execute code to enter the critical section, it must print a message to that effect in its own log file. I'll suggest naming the log file as **logfile.xx** where **xx** is the process number for the child, passed via the command line. It will be a good idea to include the time when that happens. Also, indicate the time in log file when the process actually enters and exits the critical section. Within the critical section, wait for a random number of seconds (in the range [1,3]) before you write into the file, and then, wait for another [1,3] seconds before leaving the critical section. For each child process, tweak the code so that the process requests and enters the critical section at most five times.

The code for each child process should use the following template:

```
for ( i = 0; i < 5; i++)
{
  execute code to enter critical section:
  sleep for random amount of time (between 1 and 3 seconds);
  critical_section();
  sleep for random amount of time (between 1 and 3 seconds);
  execute code to exit from critical section;
}
```
The critical section algorithm requires you to specify the number of processes in the system. I'll suggest specifying it as a contrast macro (use **#define**) in a header file called **config.h**. Please keep this number under 20. This is the maximum number of processes that can be forked. The number of processes actually forked will come from the number specified on command line when you start the program.

### Implementation

You will be required to create the specified number of separate **slave** processes from your **master**. That is, the **master** will just spawn the child processes and wait for them to finish. The **master** process also sets a timer at the start of computation to specified number of seconds. If computation has not finished b this time, the **master** kills all the **slave** processes and then exits. Make sure that you print the appropriate message(s).

**master** will also allocate shared memory for synchronization purposes. It will open and close a **logfile** but will not open **cstest**. **cstest** will be opened by the child process as it enters critical section (before the **sleep**) and closed as it exits.

In addition, **master** should also print a message when an interrupt signal (**^C**) is received. The child processes just ignor the interrupt signals (no messages on screen). Make sure that the processes handle multiple interrupts correctly. As a precaution, add this feature only after your program is well debugged.

The code for **master** and **slave** processes should be compiled separately and the executables be called **master** and **slave**.

Other points to remember: You are required to use **fork**, **exec** (or one of its variants), **wait**, and **exit** to manage multiple processes. Use **shmctl** suite of calls for shared memory allocation. Make sure that you never have more than 20 processes in the system at any time, even if I specify a larger number in the command line (issue a warning in such case).

### Invoking the solution

**master** should be invoked using the following command:

` master -t ss n `

**Termination Criteria:** There are several termination criteria. First, if all the **slaves** have finished, **master** should deallocate shared memory and terminate.
In addition, I expect the program to terminate after the specified amount of time as specified in **config.h**. This can be done using a timeout signal, at which point it should kill all currently running child processes and terminate. It should also catch the **ctrl-c** signal, free up shared memory and then terminate all children. No matter how it terminates, **master** should alos output the time of termination to the log file. For an example of a periodic timer interrupt, you can look at p318 in the text, in the program **periodicasterisk.c**.
