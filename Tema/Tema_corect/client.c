#include <stdio.h>
#include <rpc/rpc.h> 

#include "rpc_oauth.h"

#define RMACHINE "localhost"
#define BUFF_LEN 255

enum instruction_t {
	REQUEST = 0,
	MODIFY = 1,
	EXECUTE = 2,
	DELETE = 3,
	INSERT = 4,
	READ = 5
};

typedef struct client_action_t {
	char *user_id;
	enum instruction_t action;
	char *resource;
} client_action;

int read_client_actions(client_action **actions, char *file) {
	char str[BUFF_LEN];
	int size = 0;
	char *token;
	char delim[2] = ",";

	FILE *f = fopen(file, "r");
	delim[1] = '\0';

	*actions = malloc(sizeof(client_action));
	while (fgets(str, BUFF_LEN, f)) {
		/*
		 * now it is time to create the entry in the
		 * client action vector
		 */
		token = strtok(str, delim);

		if (token) {
			(*actions)[size].user_id = malloc((strlen(token) + 1) * sizeof(char));
			strcpy((*actions)[size].user_id, token);
		} else {
			continue;
		}

		token = strtok(NULL, delim);

		if (token) {
			if (strcmp(token, "REQUEST") == 0) {
				(*actions)[size].action = REQUEST;
			} else if (strcmp(token, "MODIFY") == 0) {
				(*actions)[size].action = MODIFY;
			} else if (strcmp(token, "EXECUTE") == 0) {
				(*actions)[size].action = EXECUTE;
			} else if (strcmp(token, "DELETE") == 0) {
				(*actions)[size].action = DELETE;
			} else if (strcmp(token, "INSERT") == 0) {
				(*actions)[size].action = INSERT;
			} else if (strcmp(token, "READ") == 0) {
				(*actions)[size].action = READ;
			} else {
				continue;
			}
		} else {
			continue;
		}

		token = strtok(NULL, delim);

		if (token) {
			(*actions)[size].resource = malloc((strlen(token) + 1) * sizeof(char));
			if (token[strlen(token) - 1] == '\n')
				token[strlen(token) - 1] = '\0';
			strcpy((*actions)[size].resource, token);
		} else {
			continue;
		}

		
		size += 1;
		(*actions) = realloc((*actions), (size + 1) * sizeof(client_action));
	}

	fclose(f);

	return size;
}

void print_actions(client_action *actions, int size) {
	for (int i = 0; i < size; i++) {
		printf("%s, %d, %s\n",
			actions[i].user_id,
			actions[i].action,
			actions[i].resource);
	}
}

client_status_t *get_user_status(char *user, client_status_t **statuses, int size) {
	for (int i = 0; i < size; i++) {
		if (strcmp(user, statuses[i]->user_id) == 0) {
			return statuses[i];
		}
	}
	return NULL;
}

void execute_actions(client_action *actions, int size, CLIENT *handle) {
	int action;
	char *user;
	authorization_token_t *authorization_token = NULL;
	access_token_t *access_token = NULL;

	struct client_status_t **statuses = malloc(size * sizeof(struct client_status_t*));
	struct client_status_t *current_status = NULL;
	int statuses_size = 0;

	for (int i = 0; i < size; i++) {
		action = actions[i].action;
		user = actions[i].user_id;

		current_status = get_user_status(user, statuses, statuses_size);
		if (current_status == NULL) {
			current_status = malloc(sizeof(struct client_status_t));
			current_status->user_id = malloc((strlen(user) + 1) * sizeof(char));
			strcpy(current_status->user_id, user);
			current_status->authorization_token = NULL;
			current_status->access_token = NULL;
			statuses[statuses_size] = current_status;
			statuses_size += 1;
		}

		if (action == REQUEST) {
			/* ask for request authorization token */
			authorization_token = request_authorization_1(&user, handle);

			/* check if received token is valid */
			if (authorization_token->status == USER_NOT_FOUND) {
				printf("USER_NOT_FOUND\n");
				continue;
			} else if (authorization_token->status != STATUS_OK) {
				printf("ERROR\n");
			}
			if (!current_status->authorization_token)
				current_status->authorization_token = malloc(BUFF_LEN * sizeof(char));
			strcpy(current_status->authorization_token, authorization_token->token);

			// /* ask for access token */
			// access_token = request_access_token_1(current_status, handle);
			// /* check if received token is valid */
			// if (access_token->status == REQUEST_DENIED) {
			// 	printf("REQUEST_DENIED\n");
			// 	continue;
			// } else if (access_token->status != STATUS_OK) {
			// 	printf("ERROR\n");
			// }
			// if (!current_status->access_token)
			// 	current_status->access_token = malloc(BUFF_LEN * sizeof(char));
			// strcpy(current_status->access_token, access_token->token);

			// printf("%s -> %s", authorization_token->token, access_token->token);
		}
	}
}

int main(int argc, char **argv) {
	/* variabila clientului */
	CLIENT *handle;

	int *res;
	
	handle=clnt_create(
		RMACHINE,		/* numele masinii unde se afla server-ul */
		RPC_OAUTH_PROGRAM,		/* numele programului disponibil pe server */
		RPC_OAUTH_VERS,		/* versiunea programului */
		"tcp");			/* tipul conexiunii client-server */
	
	if(handle == NULL) {
		perror("");
		return -1;
	}

	if (argc < 2) {
		perror("Not enough arguments");
		return -1;
	}

	client_action *actions = NULL;
	int size = read_client_actions(&actions, argv[1]);

	print_actions(actions, size);

	execute_actions(actions, size, handle);

	for (int i = 0; i < size; i++) {
		free(actions[i].user_id);
		free(actions[i].resource);
	}
	free(actions);

	return 0;
}
