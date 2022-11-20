#include "utils.h"

/* ttl of a certain token */
extern int token_timeout;

/* files we read from */
extern char *client_file;
extern char *resources_file;
extern char *approvals_file;

/* existent users */
extern char **user_ids;
extern int users_no;

/* existent resources */
extern char **resources;
extern int resources_no;

/* existent file permissions */
extern struct approvals_t **approvals;
extern int approvals_no;

/* list of permissions assigned to a certain auth token */
extern int token_rights_size;
extern struct token_rights_t **token_rights;

/* the list of user- authorization token pairs */
extern struct client_access_t **statuses;
extern int status_size;

/* index used for the approvals line count read */
extern int approvals_index;

/* list of auth token - access token - reset token tuples */
extern user_access_token **token_pairs;
extern int token_pairs_size;

/* 
 * function that returns the details (auth token and
 * if it uses automatic token renewal) of a user
 * (given the user id)
 */
client_access_t *get_user_status(char *user) {
	for (int i = 0; i < status_size; i++) {
		if (strcmp(user, statuses[i]->user_id) == 0) {
			return statuses[i];
		}
	}
	return NULL;
}

/* 
 * function that returns the details (auth token, user
 * id and if it uses automatic token renewal) of a user
 * (given the authorization token)
 */
client_access_t *get_user_status_auth(char *auth) {
	for (int i = 0; i < status_size; i++) {
		if (strcmp(auth, statuses[i]->authorization_token) == 0) {
			return statuses[i];
		}
	}
	return NULL;
}

/*
 * function that returns the set of permissions
 * associated with a certain token
 */
struct token_rights_t *get_token_status(char *token) {
	for (int i = 0; i < token_rights_size; i++) {
		if (strcmp(token, token_rights[i]->token) == 0) {
			return token_rights[i];
		}
	}
	return NULL;
}

/*
 * function that returns all the tokens currently
 * associated with the given authorization token
 */
user_access_token *get_token_pair_auth(char *auth_token) {
	for (int i = 0; i < token_pairs_size; i++) {
		if (strcmp(auth_token, token_pairs[i]->authorization_token) == 0) {
			return token_pairs[i];
		}
	}
	return NULL;
}

/*
 * function that returns all the tokens currently
 * associated with the given access token
 */
user_access_token *get_token_pair_access(char *access_token) {
	for (int i = 0; i < token_pairs_size; i++) {
		if (token_pairs[i]->access_token &&
			(strcmp(access_token, token_pairs[i]->access_token) == 0)) {
			return token_pairs[i];
		}
	}
	return NULL;
}

/*
 * function that checks if the set of permissions
 * for a certain access token allows the given
 * action to take place on the given resource
 */
bool check_resource_existence(char *resource) {
	for (int i = 0; i < resources_no; i++) {
		if (strcmp(resource, resources[i]) == 0) {
			return true;
		}
	}
	return false;
}

/*
 * function that checks if a certain operation
 * doesn't violate the existing permissions
 */
bool check_operation_permitted(char *action, char *access_token, char *resource) {
	user_access_token *pair;

	pair = get_token_pair_access(access_token);
	
	token_rights_t *rights = get_token_status(pair->authorization_token);

	char instruction = action[0];

	if (action[0] == 'E')
		instruction = 'X';

	int length;
	for (int i = 0; i < rights->rights->size; i++) {
		if (strcmp(rights->rights->file[i], resource) == 0) {
			/* check if the desired instruction is in permissions */
			length = strlen(rights->rights->permission[i]);
			for (int j = 0; j < length; j++) {
				if (instruction == rights->rights->permission[i][j])
					return true;
			}
			return false;
		}
	}
	return false;
}

/* function that reads the input from the files */
void read_files() {
	int size;
	char buffer[BUFF_LEN];

	/* read the ids of the clients */
	FILE *f = fopen(client_file, "r");

	fscanf(f, "%d", &size);
	user_ids = (char **) malloc(size * (sizeof(char*)));
	for (int i = 0; i < size; i++) {
		fscanf(f, "%s", buffer);
		user_ids[i] = (char *) malloc((strlen(buffer) + 1) * sizeof(char));
		strcpy(user_ids[i], buffer);
		user_ids[i][strlen(buffer)] = '\0';
	}
	users_no = size;
	statuses = (client_access_t **) malloc(users_no * sizeof(client_access_t *));

	/* read the name of the resources */
	f = fopen(resources_file, "r");

	fscanf(f, "%d", &size);

	resources = (char **) malloc(size * (sizeof(char*)));
	for (int i = 0; i < size; i++) {
		fscanf(f, "%s", buffer);
		resources[i] = (char *) malloc((strlen(buffer) + 1) * sizeof(char));
		strcpy(resources[i], buffer);
		resources[i][strlen(buffer)] = '\0';
	}
	resources_no = size;

	/* read the permissions */
	f = fopen(approvals_file, "r");

	int count, i = 0;
	char *token;
	char delim[2] = ",";
	delim[1] = '\0';

	approvals = (struct approvals_t **) malloc(sizeof(struct approvals_t *));
	while (fgets(buffer, BUFF_LEN, f)) {
		buffer[strlen(buffer) - 1] = '\0';

		approvals[i] = (struct approvals_t *) malloc(sizeof(struct approvals_t));
		if (strcmp(buffer, "*,-") == 0) {
			approvals[i]->permission = NULL;
			approvals[i]->file = NULL;
			approvals[i]->size = 0;
			i += 1;
			approvals = (struct approvals_t **) realloc(approvals, (i + 1) * sizeof(struct approvals_t *));
			continue;
		}

		approvals[i]->permission = (char **) malloc(resources_no * sizeof(char*));
		approvals[i]->file = (char **) malloc(resources_no * sizeof(char*));

		token = strtok(buffer, delim);

		count = 0;
		while (token) {
			if (count % 2 != 0) {
				approvals[i]->permission[count / 2] = (char *) malloc((strlen(token) + 1) * sizeof(char));
				strcpy(approvals[i]->permission[count / 2], token);
				approvals[i]->permission[count / 2][strlen(token)] = '\0';
			} else {
				approvals[i]->file[count / 2] = (char *) malloc((strlen(token) + 1) * sizeof(char));
				strcpy(approvals[i]->file[count / 2], token);
				approvals[i]->file[count / 2][strlen(token)] = '\0';
			}
			count += 1;
			token = strtok(NULL, delim);
		}
		approvals[i]->size = count / 2;
		i += 1;
		approvals = (struct approvals_t* *) realloc(approvals, (i + 1) * sizeof(struct approvals_t *));
	}
	approvals_no = i;
	approvals_index = 0;
	token_rights = (struct token_rights_t **) malloc(approvals_no * sizeof(struct token_rights_t *));

	token_pairs = (user_access_token **) malloc(approvals_no * sizeof(user_access_token *));

	token_pairs_size = 0;
	token_rights_size = 0;
	status_size = 0;
}