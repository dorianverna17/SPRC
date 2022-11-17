#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "rpc_oauth.h"

#define BUFF_LEN 255
#define TOKEN_LEN 15
#define FILE_NAME_LEN 50
#define ACTION_NAME_LEN 50

struct approvals_t {
	char **file;
	char **permission;
	int size;
};

struct token_rights_t {
	char *token;
	struct approvals_t *rights;
};

/*
 * structure aimed to specify which access
 * token is assigned to a certain
 * authorization token 
 */
typedef struct user_access_token_t {
	char *authorization_token;
	char *access_token;
	int timeout;
} user_access_token;

void read_files();
client_access_t *get_user_status(char *user);
struct token_rights_t *get_token_status(char *token);
user_access_token *get_token_pair_auth(char *auth_token);
user_access_token *get_token_pair_access(char *access_token);
bool check_resource_existence(char *resource);