#include <iostream>
#include <rpc/rpc.h> 

#include "rpc_oauth.h"

#define RMACHINE "localhost"
#define BUFF_LEN 255

typedef struct client_action_t {
	char *user_id;
	enum instruction_t action;
	char *resource;
} client_action;

typedef struct client_status_t {
	char *user_id;
	char *authorization_token;
	char *access_token;
	char *reset_token;
	int timeout;
	int reset;
} client_status_t;

int read_client_actions(client_action **actions, char *file) {
	char str[BUFF_LEN];
	int size = 0;
	char *token;
	char delim[2] = ",";

	FILE *f = fopen(file, "r");
	delim[1] = '\0';

	*actions = (client_action *) malloc(sizeof(client_action));
	while (fgets(str, BUFF_LEN, f)) {
		/*
		 * now it is time to create the entry in the
		 * client action vector
		 */
		token = strtok(str, delim);

		if (token) {
			(*actions)[size].user_id = (char *) malloc((strlen(token) + 1) * sizeof(char));
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
			(*actions)[size].resource = (char *) malloc((strlen(token) + 1) * sizeof(char));
			if (token[strlen(token) - 1] == '\n')
				token[strlen(token) - 1] = '\0';
			strcpy((*actions)[size].resource, token);
		} else {
			continue;
		}

		
		size += 1;
		(*actions) = (client_action *) realloc((*actions), (size + 1) * sizeof(client_action));
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
	int action, *status_validate;
	char *user;
	char **tok_ret;

	authorization_token_t *authorization_token = NULL;
	access_token_t *access_token = NULL;

	struct client_status_t **statuses = (struct client_status_t **) malloc(size * sizeof(struct client_status_t*));
	struct client_status_t *current_status = NULL;
	int statuses_size = 0;

	for (int i = 0; i < size; i++) {
		action = actions[i].action;
		user = actions[i].user_id;

		current_status = get_user_status(user, statuses, statuses_size);
		if (current_status == NULL) {
			current_status = (struct client_status_t *) malloc(sizeof(struct client_status_t));
			current_status->user_id = (char *) malloc((strlen(user) + 1) * sizeof(char));
			strcpy(current_status->user_id, user);
			current_status->authorization_token = NULL;
			current_status->access_token = NULL;
			current_status->reset_token = NULL;
			statuses[statuses_size] = current_status;
			statuses_size += 1;
		}

		if (current_status->timeout <= 0 && current_status->reset == 1 && action != REQUEST) {
			client_access_t *current_access_status = (client_access_t *) malloc(sizeof(client_access_t));
			current_access_status->user_id = current_status->user_id;
			current_access_status->authorization_token = current_status->authorization_token;
			current_access_status->reset = 1;

			access_token = request_access_token_1(current_access_status, handle);

			if (!current_status->access_token)
				current_status->access_token = (char *) malloc(BUFF_LEN * sizeof(char));
			strcpy(current_status->access_token, access_token->token);

			if (current_status->reset == 1) {
				if (!current_status->reset_token)
					current_status->reset_token = (char *) malloc(BUFF_LEN * sizeof(char));
				strcpy(current_status->reset_token, access_token->reset_token);
			}

			current_status->timeout = access_token->timeout;
		}

		if (action == REQUEST) {
			/* ask for request authorization token */
			authorization_token = request_authorization_1(&actions[i].user_id, handle);

			/* check if received token is valid */
			if (authorization_token->status == USER_NOT_FOUND) {
				printf("USER_NOT_FOUND\n");
				continue;
			} else if (authorization_token->status != STATUS_OK) {
				printf("ERROR\n");
				continue;
			}
			if (!current_status->authorization_token)
				current_status->authorization_token = (char *) malloc(BUFF_LEN * sizeof(char));
			strcpy(current_status->authorization_token, authorization_token->token);

			/* now the end user has to approve the token */
			tok_ret = approve_request_token_1(&current_status->authorization_token, handle);
			if (strcmp(*tok_ret, authorization_token->token) != 0) {
				printf("ERROR\n");
				continue;
			}
			current_status->access_token = (char *) malloc((strlen(current_status->authorization_token) + 1) *
				sizeof(char));
			strcpy(current_status->access_token, "");

			/* ask for access token */
			client_access_t *current_access_status = (client_access_t *) malloc(sizeof(client_access_t));
			current_access_status->user_id = current_status->user_id;
			current_access_status->authorization_token = current_status->authorization_token;
			current_access_status->reset = actions[i].resource[0] - '0';
			access_token = request_access_token_1(current_access_status, handle);

			current_status->reset = actions[i].resource[0] - '0';

			/* check if received token is valid */
			if (access_token->status == REQUEST_DENIED) {
				printf("REQUEST_DENIED\n");
				continue;
			} else if (access_token->status != STATUS_OK) {
				printf("ERROR\n");
			}

			if (!current_status->access_token)
				current_status->access_token = (char *) malloc(BUFF_LEN * sizeof(char));
			strcpy(current_status->access_token, access_token->token);

			if (current_status->reset == 1) {
				if (!current_status->reset_token)
					current_status->reset_token = (char *) malloc(BUFF_LEN * sizeof(char));
				strcpy(current_status->reset_token, access_token->reset_token);
			}

			current_status->timeout = access_token->timeout;

			if (current_access_status->reset == 0)
				printf("%s -> %s\n", authorization_token->token, access_token->token);
			else if (current_access_status->reset == 1)
				printf("%s -> %s,%s\n", authorization_token->token, access_token->token, access_token->reset_token);
		} else {
			action_request_t *action_req = (action_request_t *) malloc(sizeof(action_request_t));

			// TODO - cast doesn't let us provide a not permited operation - test7
			action_req->instruction = (instruction_t) action;
			action_req->resource = actions[i].resource;
			
			current_status = get_user_status(user, statuses, statuses_size);

			if (!current_status) {
				action_req->access_token = (char *) malloc(1 * sizeof(char));
				action_req->access_token[0] = '\0';
			} else {
				action_req->access_token = current_status->access_token;
				if (!action_req->access_token) {
					action_req->access_token = (char *) malloc(1 * sizeof(char));
					action_req->access_token[0] = '\0';
				}
			}

			current_status->timeout -= 1;

			status_validate = validate_delegated_action_1(action_req, handle);
			if (!status_validate) {
				printf("ERROR\n");
			}

			if (*status_validate == PERMISSION_DENIED) {
				printf("PERMISSION_DENIED\n");
			} else if (*status_validate == PERMISSION_GRANTED) {
				printf("PERMISSION_GRANTED\n");
			} else if (*status_validate == TOKEN_EXPIRED) {
				printf("TOKEN_EXPIRED\n");
			} else if (*status_validate == RESOURCE_NOT_FOUND) {
				printf("RESOURCE_NOT_FOUND\n");
			} else if (*status_validate == OPERATION_NOT_PERMITTED) {
				printf("OPERATION_NOT_PERMITTED\n");
			}
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

	execute_actions(actions, size, handle);

	for (int i = 0; i < size; i++) {
		free(actions[i].user_id);
		free(actions[i].resource);
	}
	free(actions);

	return 0;
}
