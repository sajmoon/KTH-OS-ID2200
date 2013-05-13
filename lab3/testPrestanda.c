#include <stdio.h>
#include <malloc.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include "sys/types.h"
#include "sys/sysinfo.h"
#include <string.h>
#include <stdlib.h>

#define LOOPCOUNTS 40000
#define defaultObjectSize 200

typedef struct {
    unsigned long size,resident,share,text,lib,data,dt;
} statm_t;

void printUsage();

int main(int argc, char ** argv) {
    
  void * mallocs[LOOPCOUNTS];
  int i = 0;
  
  printUsage();

  for (i = 0; i < LOOPCOUNTS; i++) {
    mallocs[i] = malloc(defaultObjectSize);
  }

  printUsage();
  for (i = LOOPCOUNTS % 2; i < LOOPCOUNTS; i += 2) {
    free(mallocs[i]);
  }
  
  printUsage(); 
  for (i = LOOPCOUNTS - 1; i >= 0; i -= 2) {
    mallocs[i] = realloc(mallocs[i], 5*defaultObjectSize);
  }

  printUsage();

}


int parseLine(char* line){
  int i = strlen(line);
  while (*line < '0' || *line > '9') line++;
  line[i-3] = '\0';
  i = atoi(line);
  return i;
}


int getValue(){ //Note: this value is in KB!
  FILE* file = fopen("/proc/self/status", "r");
  int result = -1;
  char line[128];


  while (fgets(line, 128, file) != NULL){
    if (strncmp(line, "VmSize:", 7) == 0){
      result = parseLine(line);
      break;
    }
  }
  fclose(file);
  return result;
}

int getPhysicalValue(){ //Note: this value is in KB!
        FILE* file = fopen("/proc/self/status", "r");
        int result = -1;
        char line[128];
    

        while (fgets(line, 128, file) != NULL){
            if (strncmp(line, "VmRSS:", 6) == 0){
                result = parseLine(line);
                break;
            }
        }
        fclose(file);
        return result;
    }

void printUsage() {
  
  // int value = getValue();
  // printf("virtual mem usage: %d\n", value);

  int value = getPhysicalValue();
  printf("physical mem usage: %d\n", value);
  //struct rusage * usage;


  //struct rusage {
  //             struct timeval ru_utime; /* user CPU time used */
  //             struct timeval ru_stime; /* system CPU time used */
  //             long   ru_maxrss;        /* maximum resident set size */
  //             long   ru_ixrss;         /* integral shared memory size */
  //             long   ru_idrss;         /* integral unshared data size */
  //             long   ru_isrss;         /* integral unshared stack size */
  //             long   ru_minflt;        /* page reclaims (soft page faults) */
  //             long   ru_majflt;        /* page faults (hard page faults) */
  //             long   ru_nswap;         /* swaps */
  //             long   ru_inblock;       /* block input operations */
  //             long   ru_oublock;       /* block output operations */
  //            long   ru_msgsnd;        /* IPC messages sent */
  //             long   ru_msgrcv;        /* IPC messages received */
  //            long   ru_nsignals;      /* signals received */
  //             long   ru_nvcsw;         /* voluntary context switches */
  //            long   ru_nivcsw;        /* involuntary context switches */
  //         };

  // getrusage fungerar tydligen inte så bra på linux. Vissa fält går ej att använda.
  //getrusage(RUSAGE_SELF, usage);

  //printf("shared memmory size: %ld - %ld - %ld\n", usage->ru_ixrss, usage->ru_idrss, usage->ru_isrss);

  //printf("Time: %ld.%lds\n", end.tv_sec, end.tv_usec);
}
