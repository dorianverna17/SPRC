#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "rpc_oauth.h"

#define BUFF_LEN 255

struct approvals_t {
	char **file;
	char **permission;
	int size;
};

struct token_rights_t {
	char *token;
	struct approvals_t rights;
};

int token_timeout;
char *client_file;
char *resources_file;
char *approvals_file;

char **user_ids;
char **resources;
struct approvals_t *approvals;

int token_rights_size;
struct token_rights_t **token_rights;

int users_no;
int resources_no;
int approvals_no;

int status_size;
struct client_status_t **statuses;

/* index used  */
int approvals_index;

void read_files();
client_status_t *get_user_status(char *user);
struct token_rights_t *get_token_status(char *token);