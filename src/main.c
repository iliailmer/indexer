// TODO: Add Search
// TODO: Add checks for last indexed paths
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#define RED 1
#define BLACK 0

typedef struct {
  char *cmd;
  void (*funct)();
} CmdTable; // basically a table of coimmands, currently O(n)

typedef struct FileTreeNode {
  char filename[256];
  long mem_usage;
  time_t last_modified;
  size_t size;
  int color;
  struct FileTreeNode *left;
  struct FileTreeNode *right;
  struct FileTreeNode *parent;
} FileTreeNode; // Red-Black tree node for file system info storage

FileTreeNode *root = NULL;

// RB Tree functions

void rotate_left(FileTreeNode **root, FileTreeNode *x) {
  FileTreeNode *y = x->right;
  x->right = y->left;
  if (y->left != NULL) {
    y->left->parent = x;
  }
  y->parent = x->parent;
  if (x->parent == NULL) {
    *root = y;
  } else if (x == x->parent->left) {
    x->parent->left = y;
  } else {
    x->parent->right = y;
  }
  y->left = x;
  x->parent = y;
}

void rotate_right(FileTreeNode **root, FileTreeNode *x) {
  FileTreeNode *y = x->left;
  x->left = y->right;
  if (y->right != NULL) {
    y->right->parent = x;
  }
  y->parent = x->parent;
  if (x->parent == NULL) {
    *root = y;
  } else if (x == x->parent->right) {
    x->parent->right = y;
  } else {
    x->parent->left = y;
  }
  y->right = x;
  x->parent = y;
}

void insert_fixup(FileTreeNode **root, FileTreeNode *z) {
  while (z->parent != NULL && z->parent->color == RED) {
    if (z->parent == z->parent->parent->left) {
      FileTreeNode *y = z->parent->parent->right;
      if (y != NULL && y->color == RED) { // Case 1: Uncle is RED
        z->parent->color = BLACK;
        y->color = BLACK;
        z->parent->parent->color = RED;
        z = z->parent->parent;
      } else {
        if (z == z->parent->right) { // Case 2: z is right child
          z = z->parent;
          rotate_left(root, z);
        }
        // Case 3: z is left child
        z->parent->color = BLACK;
        z->parent->parent->color = RED;
        rotate_right(root, z->parent->parent);
      }
    } else { // Mirror cases for right subtree
      FileTreeNode *y = z->parent->parent->left;
      if (y != NULL && y->color == RED) {
        z->parent->color = BLACK;
        y->color = BLACK;
        z->parent->parent->color = RED;
        z = z->parent->parent;
      } else {
        if (z == z->parent->left) {
          z = z->parent;
          rotate_right(root, z);
        }
        z->parent->color = BLACK;
        z->parent->parent->color = RED;
        rotate_left(root, z->parent->parent);
      }
    }
  }
  (*root)->color = BLACK; // Ensure root is always BLACK
}

// Insert a new file into the Red-Black Tree
void insert_file(FileTreeNode **root, const char *filename, long size,
                 time_t modified_time) {
  FileTreeNode *z = (FileTreeNode *)malloc(sizeof(FileTreeNode));
  if (!z)
    return;

  strcpy(z->filename, filename);
  z->size = size;
  z->last_modified = modified_time;
  z->color = RED;
  z->left = z->right = z->parent = NULL;

  FileTreeNode *y = NULL;
  FileTreeNode *x = *root;

  while (x != NULL) { // Standard BST insertion
    y = x;
    if (strcmp(filename, x->filename) < 0) // Sort by filename
      x = x->left;
    else
      x = x->right;
  }

  z->parent = y;
  if (y == NULL)
    *root = z;
  else if (strcmp(filename, y->filename) < 0)
    y->left = z;
  else
    y->right = z;

  insert_fixup(root, z); // Fix violations
}

DIR *open_dir(char *path) {
  printf("%s\b", path);
  DIR *dir = opendir(path);
  if (dir == NULL) {
    perror("opendir failed");
    return NULL;
  }
  return dir;
}

void print_files(FileTreeNode *node) {
  if (node == NULL)
    return;
  print_files(node->left);
  printf("%-20s ", node->filename);
  print_files(node->right);
}

void list_files(char *path) {
  // NOTE: lists files in the directory provided
  DIR *dir = open_dir(path);
  struct dirent *_dir;
  struct stat file_stat;
  char fullpath[1024];
  if (dir == NULL) {
    return;
  }
  while ((_dir = readdir(dir)) != NULL) {
    snprintf(fullpath, sizeof(fullpath), "%s/%s", path, _dir->d_name);
    if (stat(fullpath, &file_stat) == 0) {
      insert_file(&root, _dir->d_name, file_stat.st_size, file_stat.st_mtime);
    }
  }
  print_files(root);
  closedir(dir);
}

void get_input(char *cmd, size_t bufsize) {
  // NOTE: Receives an input from terminal / user
  printf("\n>  ");
  fgets(cmd, bufsize, stdin);
  cmd[strcspn(cmd, "\n")] = '\0';
}

void exit_program(void) {
  printf("Exiting...\n");
  exit(0);
}

void get_info(char *path) {
  DIR *dir = open_dir(path);
  struct dirent *_dir;
  struct stat file_stat;
  char fullpath[1024];
  if (dir == NULL) {
    return;
  }
  printf("Permissions  Size (bytes)  Last Modified        Name\n");
  printf("------------------------------------------------------\n");
  while ((_dir = readdir(dir)) != NULL) {
    snprintf(fullpath, sizeof(fullpath), "%s/%s", path, _dir->d_name);
    if (stat(fullpath, &file_stat) == 0) {
      insert_file(&root, _dir->d_name, file_stat.st_size, file_stat.st_mtime);
    }
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
