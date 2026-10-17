#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct {
  char **args;
  size_t argc;
} wish_cmd;

void tag();
void err();
void init_cmd(wish_cmd *cmd, size_t max_argc);
void push_cmd_arg(wish_cmd *cmd, char *arg);
void free_cmd(wish_cmd *cmd);
void free_all_cmds(wish_cmd *cmds, size_t cmds_size);

int main(int argc, char *argv[]) {
  const char *delimiter = " \t\n";

  char *line = NULL;
  size_t line_size = 0;

  wish_cmd *cmds = NULL;
  size_t cmds_size = 0;

  if (argc == 1) {
    tag();
    while (getline(&line, &line_size, stdin)) {
      cmds = calloc(line_size, sizeof(wish_cmd));

      // need to verify there's at least one entry
      char *tok = strsep(&line, delimiter);
      if (tok != NULL && strlen(tok) != 0) {
        init_cmd(&cmds[0], line_size);
        push_cmd_arg(&cmds[0], tok);
        cmds_size = 1;
      } else
        exit(0);

      while ((tok = strsep(&line, delimiter))) {
        if (strlen(tok) == 0)
          continue;
        if (strcmp("&", tok) == 0) {
          init_cmd(&cmds[cmds_size++], line_size);
          continue;
        }
        push_cmd_arg(&cmds[cmds_size - 1], tok);
      }

      for (size_t i = 0; i < cmds_size; i++) {
        wish_cmd cmd = cmds[i];
        if (strcmp(cmd.args[0], "exit") == 0) {
          if (cmd.argc != 1) {
            err();
            continue;
          }
          goto cleanup;
        } else if (strcmp(cmd.args[0], "cd") == 0) {
          printf("not impl\n");
        } else if (strcmp(cmd.args[0], "path") == 0) {
          printf("not impl\n");
        }
      }
      tag();
      free_all_cmds(cmds, cmds_size);
    }
  } else if (argc > 1) {
    // need to use argv[1] containing the filename for input and output
  }
cleanup:
  free_all_cmds(cmds, cmds_size);
  exit(0);
}

void tag() {
  const char tag[7] = "wish> ";
  write(STDOUT_FILENO, tag, strlen(tag));
}

void err() {
  const char error_message[30] = "An error has occurred\n";
  write(STDERR_FILENO, error_message, strlen(error_message));
}

void init_cmd(wish_cmd *cmd, size_t max_argc) {
  *cmd = (wish_cmd){.args = calloc(max_argc, sizeof(char *)), .argc = 0};
}

void push_cmd_arg(wish_cmd *cmd, char *arg) {
  cmd->args[cmd->argc++] = strdup(arg);
}

void free_cmd(wish_cmd *cmd) {
  for (size_t i = 0; i < cmd->argc; i++) {
    free(cmd->args[i]);
  }
  free(cmd->args);
}

void free_all_cmds(wish_cmd *cmds, size_t cmds_size) {
  for (size_t i = 0; i < cmds_size; i++) {
    free_cmd(&cmds[i]);
  }
  free(cmds);
}