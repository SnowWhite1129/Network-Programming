#include <tuple>
#include <string>
#include <vector>
#include <sstream>
#include <fcntl.h>
#include <iomanip>
#include <cstdlib>
#include <cstring>
#include <signal.h>
#include <unistd.h>
#include <iostream>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#define MAXPIPE 5000
#define MAXPROC 30000
#define COLOR_RED "\x1b[31m"
#define COLOR_RESET "\x1b[0m"
#define VERSION_NCOMMAND 0
#define VERSION_NLINE 1

const int VERSION = VERSION_NLINE;

using namespace std;
extern char ** environ;
enum class IOTYPE {STD, FILETYPE, PIPE};

vector<pid_t> pidTable;

void childHandler(int signo) {
  int status;
  while (waitpid(-1, &status, WNOHANG) > 0);
}
void printError(string str) {
  cerr << COLOR_RED <<  "[ERROR] " << COLOR_RESET << str << endl;
}
string getStr(IOTYPE ioType) {
  switch(ioType) {
    case(IOTYPE::STD):
      return "STD";
    case(IOTYPE::FILETYPE):
      return "FILE";
    case (IOTYPE::PIPE):
      return "PIPE";
  }
  return "";
}
class Command {
  public:
    friend ostream& operator<<(ostream& out, const Command& Cmd);
    int setOut(IOTYPE ioType) {
      if (outType != IOTYPE::STD)
        return -1;
      outType = ioType;
      return 0;
    }
    int setIn(IOTYPE ioType) {
      if (inType != IOTYPE::STD)
        return -1;
      inType = ioType;
      return 0;
    }
    int setErr(IOTYPE ioType) {
      if (errType != IOTYPE::STD)
        return -1;
      errType = ioType;
      return 0;
    }
    void parse(string cmd) {
      stringstream ss(cmd);
      string arg;
      while (ss >> arg)
        args.push_back(arg);
    }
    IOTYPE getInType() const{
      return inType;
    }
    IOTYPE getOutType() const{
      return outType;
    }
    IOTYPE getErrType() const{
      return errType;
    }
    int getOutPipeN() const{
      return outPipeN;
    }
    void setOutPipeN(int n) {
      outPipeN = n;
    }
    int getErrPipeN() const{
      return errPipeN;
    }
    void setErrPipeN(int n) {
      errPipeN = n;
    }
    void setFile(string name) {
      fileName = name;
    }
    string getFile() const{
      return fileName;
    }
    string getCmdName() const{
      return args[0];
    }
    vector<string> getCmdArgs() const{
      return args;
    }
  private:
    vector<string> args;
    IOTYPE inType = IOTYPE::STD;
    IOTYPE outType = IOTYPE::STD;
    IOTYPE errType = IOTYPE::STD;
    int outPipeN = 1;
    int errPipeN = 1;
    string fileName = "";
};
ostream& operator<<(ostream& out, const Command& Cmd) {

  for (size_t i = 0; i < Cmd.args.size(); i++) {
    cout << Cmd.args[i] << " ";
  }
  cout << endl;
  cout << "INTYPE:" << getStr(Cmd.getInType()) << endl;
  cout << "OUTTYPE:" << getStr(Cmd.getOutType());
  if (Cmd.getOutType() == IOTYPE::FILETYPE) {
    cout << " " << Cmd.getFile();
  }
  if (Cmd.getOutType() == IOTYPE::PIPE) {
    cout << " " << Cmd.getOutPipeN();
  }
  cout << endl;
  cout << "ERRTYPE:" << getStr(Cmd.getErrType());
  if (Cmd.getErrType() == IOTYPE::PIPE) {
    cout << " " << Cmd.getErrPipeN() << endl;
  } else {
    cout << endl;
  }
  return out;
}
class Table {
  public: 
    Table () {
      for (int i = 0; i < MAXPROC; i++) {
        _IOType[i][IN] = IOTYPE::STD;
        _IOType[i][OUT] = IOTYPE::STD;
        _IOType[i][ERR] = IOTYPE::STD;
        _pipeID[i][IN] = -1;
        _pipeID[i][OUT] = -1;
        _pipeID[i][ERR] = -1;
      }
    }
    void dump(int id) {
      if (id > MAXPROC) {
        printError("ID too large");
        exit(EXIT_FAILURE);
      }
      cout << setw(5) << "pid" << setw(5) << "IN" << setw(5) << "ID" << setw(5) << "OUT" << setw(5) << "ID" << setw(5) << "ERR" << setw(5) << "ID" << endl;
      cout << "-------------------------" << endl;
      for (int i = id % MAXPROC, count = 0; count < 10; i = (i + 1) % MAXPROC, count++) {
        if (i == tbPtr)
          cout << COLOR_RED << flush;
        cout << setw(5) << i << flush;
        cout << setw(5) << getStr(_IOType[i][IN]) << flush;
        cout << setw(5) << _pipeID[i][IN] << flush;
        cout << setw(5) << getStr(_IOType[i][OUT]) << flush;
        cout << setw(5) << _pipeID[i][OUT] << flush;
        cout << setw(5) << getStr(_IOType[i][ERR]) << flush;
        cout << setw(5) << _pipeID[i][ERR] << flush;

        cout << COLOR_RESET << endl;
      }
      cout << "-------------------------" << endl;
    }
    void setInPipe(int offset, int pipeID) {
      _pipeID[(tbPtr + offset) % MAXPROC][IN] = pipeID;
      _IOType[(tbPtr + offset) % MAXPROC][IN] = IOTYPE::PIPE;
    }
    void setOutPipe(int offset, int pipeID) {
      _pipeID[(tbPtr + offset) % MAXPROC][OUT] = pipeID;
      _IOType[(tbPtr + offset) % MAXPROC][OUT] = IOTYPE::PIPE;
    }
    void setErrPipe(int offset, int pipeID) {
      _pipeID[(tbPtr + offset) % MAXPROC][ERR] = pipeID;
      _IOType[(tbPtr + offset) % MAXPROC][ERR] = IOTYPE::PIPE;
    }
    void next() {
      _IOType[tbPtr][IN] = IOTYPE::STD;
      _IOType[tbPtr][OUT] = IOTYPE::STD;
      _IOType[tbPtr][ERR] = IOTYPE::STD;
      tbPtr  = (tbPtr + 1) % MAXPROC;
    }
    int getPtr() {
      return tbPtr;
    }
    IOTYPE getInType(int offset = 0) {
      int i = (tbPtr + offset) % MAXPROC;
      return _IOType[i][IN];
    }
    IOTYPE getOutType(int offset = 0) {
      int i = (tbPtr + offset) % MAXPROC;
      return _IOType[i][OUT];
    }
    IOTYPE getErrType(int offset = 0) {
      int i = (tbPtr + offset) % MAXPROC;
      return _IOType[i][ERR];
    }
    int getInPipeID(int offset = 0) {
      int i = (tbPtr + offset) % MAXPROC;
      return _pipeID[i][IN];
    }
    int getOutPipeID(int offset = 0) {
      int i = (tbPtr + offset) % MAXPROC;
      return _pipeID[i][OUT];
    }
    int getErrPipeID(int offset = 0) {
      int i = (tbPtr + offset) % MAXPROC;
      return _pipeID[i][ERR];
    }
  private:
    int _pipeID[MAXPROC][3];
    IOTYPE _IOType[MAXPROC][3];
    int tbPtr = 0;
    const int IN = 0, OUT = 1, ERR = 2;
};
class Shell {
  public:
    friend class Table;
    Shell() {
      while(environ[0]) {
        string env(environ[0]);
        size_t found = env.find_first_of("=");
        env = env.substr(0, found);
        unsetenv(env.c_str());
      }
      setenv("PATH", "bin:.", 1);
    }
    vector<string> parseCmd(string cmd) {
      vector<string> args;
      stringstream ss(cmd);
      string arg;
      while (ss >> arg)
        args.push_back(arg);
      return args;
    }
    void parseLine(string line) {
      for (size_t found = line.find("\r"); found != string::npos; found = line.find("\r"))
        line[found] = ' ';
      size_t begin = 0, end = 0;
      while (begin = line.find_first_not_of(" |!", end), begin != string::npos) {
        Command Cmd;
        string cmd;
        end = line.find_first_of("|!", begin);
        cmd = line.substr(begin, end - begin);
        if (end != string::npos) { // not the last cmd in the line
          int pipeN = 1;
          size_t pipePosition = end;
          if (isdigit(line[end + 1])) {
            size_t split;
            split = line.find_first_of(" \n", end + 1);
            pipeN = atoi(line.substr(end + 1, split - (end + 1)).c_str());
            end += split - end;
          }
          if (line[pipePosition] == '!') {
            Cmd.setOut(IOTYPE::PIPE);
            Cmd.setErr(IOTYPE::PIPE);
            Cmd.setOutPipeN(pipeN);
            Cmd.setErrPipeN(pipeN);
          } else if (line[pipePosition] == '|') {
            Cmd.setOut(IOTYPE::PIPE);
            Cmd.setOutPipeN(pipeN);
          }

        }
        string outputFile = "";
        size_t redirect = cmd.find(">");
        if (redirect != string::npos) {
          if (Cmd.setOut(IOTYPE::FILETYPE) != 0) {
            printError("Output duplication");
            return;
          }
          size_t beginRedir = cmd.find_first_not_of(" ", redirect + 1);
          size_t endRedir = cmd.find_first_of(" ", beginRedir);
          outputFile = cmd.substr(beginRedir, endRedir - beginRedir);
          Cmd.setFile(outputFile);
          cmd = cmd.substr(0, redirect);
        }
        Cmd.parse(cmd);
        CmdList.push_back(Cmd);
      }
      if (VERSION == VERSION_NLINE) {
        for (size_t i = 0; i < CmdList.size(); i++) {
          if (i < CmdList.size() - 1 && 
              (CmdList[i].getOutPipeN() > 1 || CmdList[i].getErrPipeN() > 1)) {
            printError("Invalid Pipe");
            CmdList.clear();
          }
        }
      }
    }
    void execute() {
      for (size_t index = 0; index < CmdList.size(); index++) {
        Table *tb1 = &table, *tb2 = &table;
        int offset = 0;
        if (VERSION == VERSION_NLINE) {
          if (index == 0) tb1 = &lineTable;
          if (index == CmdList.size() -1) tb2 = &lineTable;
          if (tb1 != tb2) offset = -1;
        }

        Command Cmd = CmdList[index];
        if (Cmd.getCmdName() == "exit") {
          exit(0);
        } else if (Cmd.getCmdName() == "setenv") {
          if (tb1->getInType() != IOTYPE::STD) {
            printError("Someone piped to setenv");
            int pipeID = tb1->getInPipeID();
            closePipe(pipeID);
          }
          vector<string> args = Cmd.getCmdArgs();
          if (args.size() > 2) {
            setenv(args[1].c_str(), args[2].c_str(), 1);
          } else {
            printError(args[0] + " missing argument");
          }
          tb1->next();
        } else if (Cmd.getCmdName() == "printenv") {
          if (tb1->getInType() != IOTYPE::STD) {
            printError("Someone piped to printenv");
          }
          vector<string> args = Cmd.getCmdArgs();
          if (args.size() > 1) {
            char* env = getenv(args[1].c_str());
            if (env != NULL)
              cout << env << endl;
          } else {
            printError(args[0] + " missing argument");
          }
          tb1->next(); 
        } else {
          int inFD = 0, outFD = 1, errFD = 2;
          if (tb1->getInType() != IOTYPE::STD) {
            int pipeID = tb1->getInPipeID();
            inFD = pipeFD[pipeID][0];
          } 
          if (Cmd.getOutType() == IOTYPE::FILETYPE) {
            string fileName = Cmd.getFile();
            outFD = creat(fileName.c_str(), 0644);
          } else if (Cmd.getOutType() == IOTYPE::PIPE) {
            int pipeN = Cmd.getOutPipeN();
            int pipeID;
            if (tb2->getInType(pipeN + offset) == IOTYPE::STD) {
              pipeID = openPipe();
              outFD = pipeFD[pipeID][1];
              tb1->setOutPipe(0, pipeID);
              tb2->setInPipe(pipeN + offset, pipeID);
            } else if(tb2->getInType(pipeN + offset) == IOTYPE::PIPE) {
              pipeID = tb2->getInPipeID(pipeN + offset);
              outFD = pipeFD[pipeID][1];
              tb1->setOutPipe(0, pipeID);
            }
          }
          if (Cmd.getErrType() == IOTYPE::PIPE) {
            int pipeN = Cmd.getErrPipeN();
            int pipeID;
            if (tb2->getInType(pipeN + offset) == IOTYPE::STD) {
              pipeID = openPipe();
              errFD = pipeFD[pipeID][1];
              tb1->setErrPipe(0, pipeID);
              tb2->setInPipe(pipeN + offset, pipeID);
            } else if(tb2->getInType(pipeN + offset) == IOTYPE::PIPE) {
              pipeID = tb2->getInPipeID(pipeN + offset);
              errFD = pipeFD[pipeID][1];
              tb1->setErrPipe(0, pipeID);
            }
          }
          pid_t pid;
          while ((pid = fork()) < 0) {
            usleep(1000);
          }
          pidTable.push_back(pid);
          if (pid == 0) {
            signal(SIGCHLD, SIG_DFL);
            if (inFD != 0) {
              dup2(inFD, 0);
              close(inFD);
            }
            if (outFD == errFD) {
              dup2(outFD, 1);
              dup2(outFD, 2);
              close(outFD);
            }
            else if (outFD != 1) {
              dup2(outFD, 1);
              close(outFD);
            }
            else if (errFD != 2) {
              dup2(errFD, 2);
              close(errFD);
            }
            closeAllPipe();
            vector<char*> argv;
            vector<string> args = Cmd.getCmdArgs();
            for (size_t i = 0; i < args.size(); i++) {
              argv.push_back(const_cast<char*>(args[i].c_str()));
            }
            argv.push_back(0);
            //cout << Cmd.getCmdName() << endl;
            execvp(argv[0], argv.data());
            if (errno == 2) {
              cerr << "Unknown command: [" << argv[0] << "]." << endl;
            } else {
              cerr << strerror(errno) << endl;
            }
            exit(0);
          } else {
            if (tb1->getInType() == IOTYPE::PIPE) {
              int pipeID = tb1->getInPipeID();
              closePipe(pipeID);
            }
            if (tb1->getOutType() == IOTYPE::FILETYPE) {
              close(outFD);
            }
            tb1->next();
          }
        }
      }
      if (CmdList.size() > 0 && CmdList.back().getOutType() == IOTYPE::PIPE) {
        // don't wait for it
      } else {
        for (auto && pid : pidTable) {
          int status;
          waitpid(pid, &status, 0);
        }
      }
      pidTable.clear();
    }
    void handleLine(string line) {
      parseLine(line);
      execute();
      CmdList.clear();
    }
    int usedPipeCount() {
      int count = 0;
      for (int i = 0; i < MAXPIPE; i++) {
        if (pipeUsed[i] != 0) {
          count++;
        }
      }
      return count;
    }
    int vacantPipe() {
      for (int i = 0; i < MAXPIPE; i++ ) {
        if (pipeUsed[i] == 0)
          return i;
      }
      return -1;
    }
    int openPipe() {
      int vacant = vacantPipe();
      if (vacant < 0) {
        printError("No vacant pipe");
        exit(EXIT_FAILURE);
      }
      while (pipe(pipeFD[vacant]) < 0) {
        usleep(1000);
      }
      /*
      if (pipe(pipeFD[vacant]) == -1) {
        printError("Open pipe error");
        exit(EXIT_FAILURE);
      }
      */
      pipeUsed[vacant] = 1;
      return vacant;
    }
    void closePipe(int id) {
      close(pipeFD[id][0]);
      close(pipeFD[id][1]);
      pipeUsed[id] = 0;
    }
    void closeAllPipe() {
      for (int i = 0; i < MAXPIPE; ++i) {
        if (pipeUsed[i]) {
          close(pipeFD[i][0]);
          close(pipeFD[i][1]);
          pipeUsed[i] = 0;
        }
      }
    }
  private:
    vector<Command> CmdList;
    Table table;
    Table lineTable;
    int pipeFD[MAXPIPE][2] = {};
    int pipeUsed[MAXPIPE] = {};
};
int main() {
  signal(SIGCHLD, childHandler);
  string line;
  Shell shell;
  while(cout << "% " << flush, getline(cin, line)) {
    shell.handleLine(line);
  }
  return 0;
}
