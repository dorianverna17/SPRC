#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BUFF_LEN 255

struct approvals_t {
	char **file;
	char **permission;
	int size;
};

int token_timeout;
char *client_file;
char *resources_file;
char *approvals_file;

char **user_ids;
char **resources;
struct approvals_t *approvals;

int users_no;
int resources_no;
int approvals_no;

void read_files();