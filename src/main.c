#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

typedef struct {
  char *cmd;
  void (*funct)();
} CmdTable; // basically a hash table / dictionary of coimmands

void list_files(DIR *dir) {
  // NOTE: lists files in the directory provided
  struct dirent *_dir;
  while ((_dir = readdir(dir)) != NULL) {
    printf("%s\n", _dir->d_name);
  }
}
void get_cmd(char *cmd, size_t bufsize) {
  // NOTE: Receives an input from terminal / user
  fgets(cmd, bufsize, stdin);
  cmd[strcspn(cmd, "\n")] = '\0';
}
CmdTable cmd_table[] = {{"exit", NULL}, {"list", list_files}, {NULL, NULL}};
void run_cmd(const char *cmd, CmdTable cmd_table) {
  for (int i = 0; i < cmd_table[i].cmd != NULL; i++) {
    if (strcmp(cmd, cmd_table[i].cmd) == 0) {
    }
  }
}

int main(int argc, char *argv[]) {
  const char *path = (argc > 1) ? argv[1] : ".";
  int bufsize = 32;
  char cmd[bufsize];
  DIR *dir = opendir(path);
  if (dir == NULL) {
    perror("opendir failed");
    return 1;
  }
  get_cmd(cmd, bufsize);
  while (strcmp(cmd, "exit") != 0) {
    list_files(dir);
    get_cmd(cmd, bufsize);
  }

  if (strcmp(cmd, "exit") == 0) {
    printf("Exiting...\n");
  }
  closedir(dir);
  return 0;
}
