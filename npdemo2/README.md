# NP Project2 Demo Script (beta)

# Important information:

1. This is a sample demo scipt. It is just a sample, the final one might be different.

2. The test cases in this directory are from 2016, the ones used for this year will be different.

3. Please make your servers fit the requirements:

   - yourNpServer takes in 1 argument, which is the port number.

   `./yourNpServer 12345`

   - There will be a user_pipe/ directory inside the working directory. For concurrent connection-oriented paradigm with shared memory, user pipes (you can use regular files for user pipes) should be saved inside yourWorkingDir/user_pipe/

   - The initial working directory structure:

```
    work_dir
        |-bin
        |-test.html
        |-user_pipe
```

4. We recommand you use telnet during delopment.

   - Assume you run your server on nplinux2 listening to port 12345
   - Run the following telnet commnad on any computer

```
    bash$ telnet nplinux2.cs.nctu.edu.tw 12345
    Trying 140.113.235.217...
    Connected to nplinux2.cs.nctu.edu.tw.
    Escape character is '^]'.
    ****************************************
    ** Welcome to the information server. **
    ****************************************
    *** User '(no name)' entered from CGILAB/511. ***
    %
```

5. Please be aware that the line sent from telnet and our delayclient might end with "\r\n".

6. The output order of the command "ls" might be different, we will try to avoid this situation during demo.

7. To make it easier to demo, you can print IP/Port as CGILAB/511 for the command "who". However, you still need to know how to retrieve the ip and port of each client.

---

## This directory contains the following

### Server

- server.sh

  - Usage.sh: `./server.sh <server_path> <port>`
  - server.sh does the following:
    1. Contruct the working directory (work_template and work_dir)
    2. Compile the commands (noop, removetag...) and place them into bin/ inside the working directory
    3. Copy cat and ls into bin/ inside the working directory
    4. Run the your server inside work_dir

- zombie.sh:

  - Check if your server left any zombie processes in the system
  - Run this script right before you shut down your server

- shm.sh:

  - Check if your server left shared memory in the system
  - Run this script after you shut down your server

- src/

  - Contains source code of commands (noop, removetag...) and test.html

### Client

- There are two directories inside client: single_client & multi_client

  - Use single_client to test the first server (npserver_simple)
  - Use multi_client to test the other two servers (npserver_single_proc & npserver_multi_proc)

- demo.sh:

  - Usage: `./demo.sh <server ip> <port>`
  - demo.sh does the following:
    1. Test all testcases using delayclient
    2. Use diff to compare the files inside output/ and answer/
    3. Show the result of demo

* delayclient.c

  - You can compile this code using makefile
  - Usage: `./delayclient <server ip> <port> <testfile>`
  - The delayclient reads a specific format of input, please reder to the files under test_case/ for example
  - The delayclient acts as many clients, it will read the testfile and send commands using differenct sockets, protending that there are many clients logging onto to the server.

* compare.sh

  - Usage: `./compare.sh [n]`
  - Compare.sh will run vimdiff on the n'th answer and your output

* test_case/

  - Contains test cases

* answer/

  - Contains answers

## Scenario

### On nplinux2

- Assume there are two directories, np_project2_demo_sample and 0756000_np_project2 inside a private directroy on nplinux2.
- My npservers (3 servers) are inside 0756000_np_project2 directory
- I am now inside np_project2_demo_sample/server/

```
bash$ ./server.sh ../../0756000_np_project2/npserver_single_proc 12345
======= Your server is running =======
```

- After all demo finishes, before shutting down your server, use zombie.sh to test zombie processes

```
./zombie.sh
Good Job!
```

- Shut down your server, then user shm.sh to see if you left shared memory in the system.

```
./shm.sh
Good Job!
```

### On another computer

- Assume you also cloned np_project2_demo_sample onto this computer
- I am now inside np_project2_demo_sameple/client/multi_client/

1. Testing one testcase and save the output in myOutput1.txt

```
   ./delayclient nplinux2.cs.nctu.edu.tw 12345 test_case/1.txt > myOutput1.txt
```

2. Testing all testcases

```
   ./demo.sh nplinux2.cs.nctu.edu.tw 12345
   ===== Test case 1 =====
   Your answer is correct
   ===== Test case 2 =====
   Your answer is correct
   ===== Test case 3 =====
   Your answer is correct
   ===== Test case 4 =====
   Your answer is correct
   ===== Test case 5 =====
   Your answer is correct
   ===== Test case 6 =====
   Your answer is correct
   ===== Test case 7 =====
   Your answer is wrong
   ======= Summary =======
   [Correct]: 1 2 3 4 5 6
   [ Wrong ]: 7
```
