#include <iostream>
#include <string>
#include <stdio.h>

/* Required by for routine */
#include <sys/types.h>
#include <unistd.h>

#include <stdlib.h>   /* Declaration for exit() */

using namespace std;

int globalVariable = 2;

int main()
{
  string sIdentifier;
  int    iStackVariable = 20;

  pid_t   pID = fork();
  pid_t   pID2 = 9;

  if (pID == 0)                // child
  {
    // Code only executed by child process

    pID2 = fork();
    if (pID2 == 0) {
      cout << "third child\n";
      
    } 

    sIdentifier = "Child Process: ";
    globalVariable++;
    iStackVariable++;
  
  }
  else if (pID < 0)            // failed to fork
  {
    cerr << "Failed to fork" << endl;
    exit(1);
    // Throw exception
  }
  else                                   // parent
  {
    // Code only executed by parent process
    sIdentifier = "Parent Process:";
    sleep(3);
  }

  // Code executed by both parent and child.

  cout << sIdentifier;
  cout << " Global variable: " << globalVariable;
  cout << " Stack variable: "  << iStackVariable;
  cout << " pID2 " << pID << " " << pID2 << endl;
  exit(0);
  }
              