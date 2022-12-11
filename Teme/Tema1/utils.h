#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "rpc_oauth.h"

#define BUFF_LEN 255
#define TOKEN_LEN 15
#define FILE_NAME_LEN 50
#define ACTION_NAME_LEN 50

/*
 * structure used for storing the resource
 * access rights/permissions for a specific
 * user at some point.
 * It is assigned to an authorization token
 */
struct approvals_t {
	char **file;
	char **permission;
	int size;
};

/*
 * structure that is used in order to assign
 * a set of permissions to a certain user,
 * identified by the authorization token
 */
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
	char *reset_token;
	int timeout;
	int reset;
} user_access_token;

/* function that reads the input from the files */
void read_files();

/* 
 * function that returns the details (auth token and
 * if it uses automatic token renewal) of a user
 * (given the user id)
 */
client_access_t *get_user_status(char *user);

/* 
 * function that returns the details (auth token, user
 * id and if it uses automatic token renewal) of a user
 * (given the authorization token)
 */
client_access_t *get_user_status_auth(char *auth);

/*
 * function that returns the set of permissions
 * associated with a certain token
 */
struct token_rights_t *get_token_status(char *token);

/*
 * function that returns all the tokens currently
 * associated with the given authorization token
 */
user_access_token *get_token_pair_auth(char *auth_token);

/*
 * function that returns all the tokens currently
 * associated with the given access token
 */
user_access_token *get_token_pair_access(char *access_token);

/* function that checks if the resource given exists */
bool check_resource_existence(char *resource);

/*
 * function that checks if the set of permissions
 * for a certain access token allows the given
 * action to take place on the given resource
 */
bool check_operation_permitted(char *action, char *access_token, char *resource);
