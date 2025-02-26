// TODO: add hash table in get info, support for cachinhg and specific file info
// TODO: min-heap: Enhance the list command to quickly find the N largest files.

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

typedef struct {
  char *cmd;
  void (*funct)();
} CmdTable; // basically a hash table / dictionary of coimmands

DIR *open_dir(char *path) {
  printf("%s\b", path);
  DIR *dir = opendir(path);
  if (dir == NULL) {
    perror("opendir failed");
    // exit(1);
    return NULL;
  }
  return dir;
}

void list_files(char *path) {
  // NOTE: lists files in the directory provided
  DIR *dir = open_dir(path);
  if (dir == NULL) {
    return;
  }
  struct dirent *_dir;
  while ((_dir = readdir(dir)) != NULL) {
    printf("%s\n", _dir->d_name);
  }
  closedir(dir);
}

void get_input(char *cmd, size_t bufsize) {
  // NOTE: Receives an input from terminal / user
  fgets(cmd, bufsize, stdin);
  cmd[strcspn(cmd, "\n")] = '\0';
}

void exit_program(void) {
  printf("Exiting...\n");
  exit(0);
}

void get_info(char *path) {
  DIR *dir = open_dir(path);
  printf("Permissions  Size (bytes)  Last Modified        Name\n");
  printf("------------------------------------------------------\n");
  struct dirent *_dir;
  struct stat file_stat;
  char fullpath[1024];
  while ((_dir = readdir(dir)) != NULL) {
    snprintf(fullpath, sizeof(fullpath), "%s/%s", path, _dir->d_name);
    if (stat(fullpath, &file_stat) == 0) {
      char timebuf[20];
      strftime(timebuf, sizeof(timebuf), "%Y-%m-%d %H:%M",
               localtime(&file_stat.st_mtime));
      printf("%c%c%c%c%c%c%c%c%c%c  %10lld  %s  %s\n",
             (S_ISDIR(file_stat.st_mode)) ? 'd' : '-',
             (file_stat.st_mode & S_IRUSR) ? 'r' : '-',
             (file_stat.st_mode & S_IWUSR) ? 'w' : '-',
             (file_stat.st_mode & S_IXUSR) ? 'x' : '-',
             (file_stat.st_mode & S_IRGRP) ? 'r' : '-',
             (file_stat.st_mode & S_IWGRP) ? 'w' : '-',
             (file_stat.st_mode & S_IXGRP) ? 'x' : '-',
             (file_stat.st_mode & S_IROTH) ? 'r' : '-',
             (file_stat.st_mode & S_IWOTH) ? 'w' : '-',
             (file_stat.st_mode & S_IXOTH) ? 'x' : '-', file_stat.st_size,
             timebuf, _dir->d_name);
    }
  }

  closedir(dir);
}

CmdTable cmd_table[] = {{"list", list_files},
                        {"exit", exit_program},
                        {"info", get_info},
                        {NULL, NULL}};

void run_cmd(char *input, CmdTable *cmd_table) {
  char *cmd = strtok(input, " ");
  char *args = strtok(NULL, "");
  if (!cmd)
    return; // Ignore empty input

  for (int i = 0; cmd_table[i].cmd != NULL; i++) {
    if (strcmp(cmd, cmd_table[i].cmd) == 0) {
      cmd_table[i].funct(args ? args : ""); // Pass arguments
      return;
    }
  }
  printf("Unknown command: %s\n", cmd);
}

int main(int argc, char *argv[]) {
  int bufsize = 32;
  char input[bufsize];
  get_input(input, bufsize);
  while (1) {
    run_cmd(input, cmd_table);
    get_input(input, bufsize);
  }

  return 0;
}
