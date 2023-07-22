#include <fcntl.h>
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char **argv) {
  if(argc <= 1) {
    fprintf(stderr, "no process given\nusage example: ./proc-reader self\n");

    return 1;
  }

  if(argc > 2) {
    fprintf(stderr, "extraneous parameters given, expected 1.\nusage example: ./proc-reader self\n");

    return 1;
  }

  unsigned procsize = strlen(argv[1]);

  char procpath[PATH_MAX + 1];
  procpath[procsize + 6] = 0;

  memcpy(procpath, "/proc/", 6);
  memcpy(procpath + 6, argv[1], procsize);

  int procdir = open(procpath, O_DIRECTORY);

  if(procdir == -1) {
    char *err = strerror(errno);
    
    fprintf(stderr, "error: %s\n", err);

    free(err);

    return 1;
  }

  struct DIR *dir = fdopendir(openat(procdir, "fd", O_DIRECTORY));

  close(procdir);

  char buffer[PATH_MAX + 1];
  buffer[PATH_MAX] = 0;

  pid_t selfpid = getpid();

  for(struct dirent *dsrc = readdir(dir); dsrc != 0x0; dsrc = readdir(dir)) {
    if(dsrc->d_type != DT_LNK) continue;

    int size = readlinkat(*(int *) dir, dsrc->d_name, buffer, PATH_MAX);

    if(size == -1) {
      char *err = strerror(errno);

      fprintf(stderr, "error: %s\n", err);

      free(err);
      free(dsrc);
      closedir(dir);

      return 0;
    }

    buffer[size] = 0;

    printf("fd: %s -> %s ", dsrc->d_name, buffer);

    if(strncmp(buffer, "/proc/", 6) == 0) {
      buffer[strlen(buffer) - 3] = 0;
      
      pid_t pid = atoi(buffer + 6);

      if(pid == selfpid) printf("(created by this command)");
    }

    printf("\n");

    free(dsrc);
  }

  closedir(dir);
}
