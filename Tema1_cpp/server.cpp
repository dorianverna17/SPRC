#include <iostream>
#include <rpc/rpc.h>

#include "utils.h"
#include "token.h"
#include "rpc_oauth.h"

extern int token_timeout;
extern char *client_file;
extern char *resources_file;
extern char *approvals_file;

extern char **user_ids;
extern char **resources;
extern struct approvals_t **approvals;

extern int token_rights_size;
extern struct token_rights_t **token_rights;

extern int users_no;
extern int resources_no;
extern int approvals_no;

extern int status_size;
extern struct client_access_t **statuses;

/* index used  */
extern int approvals_index;

extern user_access_token **token_pairs;
extern int token_pairs_size;

authorization_token_t *request_authorization_1_svc(char **p, struct svc_req *cl) {
	authorization_token_t *t;
	char *user = *p, *tok;
	client_access_t *status;
	user_access_token *pair;

	/* print server message */
	printf("BEGIN %s AUTHZ\n", user);

	t = (authorization_token_t*) malloc(sizeof(authorization_token_t));
	t->token = NULL;
	for (int i = 0; i < users_no; i++) {
		if (strcmp(user, user_ids[i]) == 0) {
			t->token = generate_access_token(user);
			t->status = STATUS_OK;

			status = get_user_status(user);
			if (!status) {
				statuses[status_size] = (client_access_t*) malloc(sizeof(client_access_t));
				statuses[status_size]->user_id = (char *) malloc((strlen(user) + 1) * sizeof(char));
				strcpy(statuses[status_size]->user_id, user);
				status = statuses[status_size];
				status_size += 1;
			}

			status->authorization_token =
				(char *) malloc((strlen(t->token) + 1) * sizeof(char));
			strcpy(status->authorization_token, t->token);

			/* print the request token */
			printf("  RequestToken = %s\n", status->authorization_token);

			/* add a new entry in the token pairs list */
			pair = (user_access_token *) malloc(sizeof(user_access_token));
			pair->authorization_token = t->token;
			pair->access_token = NULL;
			pair->timeout = -1;
			token_pairs[token_pairs_size] = pair;
			token_pairs_size += 1;
		}
	}
	if (!t->token) {
		t->status = USER_NOT_FOUND;
		t->token = (char *) malloc(TOKEN_LEN * sizeof(char));
		strcpy(t->token, "");
	}

	return t;
}

access_token_t *request_access_token_1_svc(client_access_t *p, struct svc_req *cl) {
	access_token_t *t;
	char *auth = p->authorization_token;
	token_rights_t *rights = get_token_status(auth);
	user_access_token *pair;

	t = (access_token_t *) malloc(sizeof(access_token_t));
	if (!rights) {
		t->token = NULL;
		t->status = REQUEST_DENIED;
		return t;
	}

	if (rights->rights->size == 0) {
		t->token = (char *) malloc(TOKEN_LEN * sizeof(char));
		strcpy(t->token, "");
		t->status = REQUEST_DENIED;
	} else {
		t->token = generate_access_token(auth);
		/* print the access token */
		printf("  AccessToken = %s\n", t->token);
		t->status = STATUS_OK;
	
		/* update the token pairs list */
		pair = get_token_pair_auth(auth);
		pair->access_token = t->token;
		pair->timeout = token_timeout;
	}

	return t;
}

int *validate_delegated_action_1_svc(action_request_t *p, struct svc_req *cl) {
	int *t = (int *) malloc(sizeof(int));
	char *action = (char *) malloc(ACTION_NAME_LEN * sizeof(char));

	instruction_t instruction = p->instruction;
	char *resource = p->resource;
	char *access_token = p->access_token;

	/* default value */
	*t = PERMISSION_DENIED;

	if (instruction == MODIFY) {
		strcpy(action, "MODIFY");
	} else if (instruction == EXECUTE) {
		strcpy(action, "EXECUTE");
	} else if (instruction == DELETE) {
		strcpy(action, "DELETE");
	} else if (instruction == INSERT) {
		strcpy(action, "INSERT");
	} else if (instruction == READ) {
		strcpy(action, "READ");
	}

	user_access_token *pair;

	pair = get_token_pair_access(access_token);

	if (pair == NULL) {
		*t = PERMISSION_DENIED;
		printf("DENY (%s,%s,,%d)\n", action, resource, 0);
	} else if (pair->timeout <= 0) {
		*t = TOKEN_EXPIRED;
		printf("DENY (%s,%s,,%d)\n", action, resource, 0);
	} else if (!check_resource_existence(p->resource)) {
		*t = RESOURCE_NOT_FOUND;
		pair->timeout -= 1;
		printf("DENY (%s,%s,%s,%d)\n", action, resource, pair->access_token, pair->timeout);
	} else if (!check_operation_permitted(action, access_token, resource)) {
		*t = OPERATION_NOT_PERMITTED;
		pair->timeout -= 1;
		printf("DENY (%s,%s,%s,%d)\n", action, resource, pair->access_token, pair->timeout);
	} else {
		*t = PERMISSION_GRANTED;
		pair->timeout -= 1;
		printf("PERMIT (%s,%s,%s,%d)\n", action, resource, pair->access_token, pair->timeout);

		// /* erase the access token if it expired */
		// if (pair->timeout <= 0) {
		// 	strcpy(pair->access_token, "");
		// }
	}

	return t;
}

char **approve_request_token_1_svc(char **p, struct svc_req *cl) {
	token_rights_t *rights = get_token_status(*p);
	if (!rights) {
		token_rights[token_rights_size] = (token_rights_t *) malloc(sizeof(token_rights_t));
		token_rights[token_rights_size]->token = (char *) malloc((strlen(*p) + 1) * sizeof(char));
		strcpy(token_rights[token_rights_size]->token, *p);
		rights = token_rights[token_rights_size];
		token_rights_size += 1;
	}

	rights->rights = approvals[approvals_index];

	approvals_index += 1;

	return p;
}