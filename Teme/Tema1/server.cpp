#include <iostream>
#include <rpc/rpc.h>

#include "utils.h"
#include "token.h"
#include "rpc_oauth.h"

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
 * the function that creates an authorization token
 * for the specified user
 */
authorization_token_t *request_authorization_1_svc(char **p, struct svc_req *cl) {
	authorization_token_t *t;
	char *user = *p, *tok;
	client_access_t *status;
	user_access_token *pair;

	/* print server message */
	printf("BEGIN %s AUTHZ\n", user);

	t = (authorization_token_t*) malloc(sizeof(authorization_token_t));
	t->token = NULL;

	/* check if the user specified has a valid user id */
	for (int i = 0; i < users_no; i++) {
		if (strcmp(user, user_ids[i]) == 0) {
			/* generate token */
			t->token = generate_access_token(user);
			t->status = STATUS_OK;

			/* fetch/generate user status entry */
			status = get_user_status(user);
			if (!status) {
				statuses[status_size] = (client_access_t*) malloc(sizeof(client_access_t));
				statuses[status_size]->user_id = (char *) malloc((strlen(user) + 1) * sizeof(char));
				strcpy(statuses[status_size]->user_id, user);
				status = statuses[status_size];
				status_size += 1;
			}

			/* store the authorization token created */
			status->authorization_token =
				(char *) malloc((strlen(t->token) + 1) * sizeof(char));
			strcpy(status->authorization_token, t->token);

			/* print the request token */
			printf("  RequestToken = %s\n", status->authorization_token);

			/* add a new entry in the token pairs list */
			pair = (user_access_token *) malloc(sizeof(user_access_token));
			pair->authorization_token = t->token;
			pair->access_token = NULL;
			pair->reset_token = NULL;
			pair->timeout = -1;
			token_pairs[token_pairs_size] = pair;
			token_pairs_size += 1;
		}
	}

	/* create response for user not found case */
	if (!t->token) {
		t->status = USER_NOT_FOUND;
		t->token = (char *) malloc(TOKEN_LEN * sizeof(char));
		strcpy(t->token, "");
	}

	return t;
}

/* function that returns an access token for the specified authorization token */
access_token_t *request_access_token_1_svc(client_access_t *p, struct svc_req *cl) {
	access_token_t *t;
	char *auth = p->authorization_token;
	token_rights_t *rights = get_token_status(auth);
	user_access_token *pair;

	t = (access_token_t *) malloc(sizeof(access_token_t));

	/*
	 * check if there is a valid approvals
	 * entry for the authorization token
	 */
	if (!rights) {
		t->token = NULL;
		t->reset_token = NULL;
		t->status = REQUEST_DENIED;
		t->timeout = 0;
		return t;
	}

	/* take care of the renewal process*/
	pair = get_token_pair_auth(auth);
	if (rights != NULL && p->reset == 1 && pair->timeout <= 0
		&& pair->access_token != NULL) {
		
		/* generate the new access and reset tokens */
		pair->reset = 1;
		pair->access_token = generate_access_token(pair->reset_token);
		pair->reset_token = generate_access_token(pair->access_token);
		pair->timeout = token_timeout;

		/* create the return structure */
		t->token = pair->access_token;
		t->status = STATUS_OK;
		t->timeout = token_timeout;
		t->reset_token = pair->reset_token;

		/* print the desired messages */
		printf("BEGIN %s AUTHZ REFRESH\n", get_user_status_auth(p->authorization_token)->user_id);
		printf("  AccessToken = %s\n", t->token);
		printf("  RefreshToken = %s\n", t->reset_token);

		return t;
	}

	/*
	 * check if there are permissions for the
	 * specified auth token, if not, then return
	 * a request denied error
	 */
	if (rights->rights->size == 0) {
		t->token = (char *) malloc(TOKEN_LEN * sizeof(char));
		strcpy(t->token, "");
		t->reset_token = (char *) malloc(TOKEN_LEN * sizeof(char));
		strcpy(t->reset_token, "");
		t->status = REQUEST_DENIED;
		t->timeout = 0;
	} else {
		t->token = generate_access_token(auth);
		
		/* print the access token */
		printf("  AccessToken = %s\n", t->token);
		t->status = STATUS_OK;
	
		/* update the token pairs list */
		pair = get_token_pair_auth(auth);
		pair->access_token = t->token;
		if (p->reset == 1) {
			pair->reset_token = generate_access_token(pair->access_token);
			printf("  RefreshToken = %s\n", pair->reset_token);
		} else {
			pair->reset_token = (char *) malloc(TOKEN_LEN * sizeof(char));
			strcpy(pair->reset_token, "");
		}
		pair->timeout = token_timeout;
		pair->reset = p->reset;

		t->reset_token = pair->reset_token;
		t->timeout = token_timeout;
	}

	return t;
}

/* procedure that returns the status of the action that was made by the user */
int *validate_delegated_action_1_svc(action_request_t *p, struct svc_req *cl) {
	int *t = (int *) malloc(sizeof(int));
	char *instruction = p->instruction;
	char *resource = p->resource;
	char *access_token = p->access_token;

	/* default value */
	*t = PERMISSION_DENIED;

	user_access_token *pair;

	pair = get_token_pair_access(access_token);

	/* check if the operation is a valid one */
	if (strcmp(instruction, "MODIFY") != 0 &&
		strcmp(instruction, "EXECUTE") != 0 &&
		strcmp(instruction, "DELETE") != 0 &&
		strcmp(instruction, "INSERT") != 0 &&
		strcmp(instruction, "READ") != 0) {
		*t = OPERATION_NOT_PERMITTED;
		pair->timeout -= 1;
		printf("DENY (%s,%s,%s,%d)\n", instruction, resource, pair->access_token, pair->timeout);
		return t;
	}

	/*
	 * if there is no entry associated for the access token given,
	 * then return PERMISSION_DENIED
	 * 
	 * if the token is expired, return TOKEN_EXPIRED
	 * 
	 * if the resource doesn't exist, return RESOURCE_NOT_FOUND
	 * 
	 * if the rights are not suitable for the operation wanted,
	 * then return OPERATION_NOT_PERMITTED
	 * 
	 * if none of the above applies, return PERMISSION_GRANTED
	 */
	if (pair == NULL) {
		*t = PERMISSION_DENIED;
		printf("DENY (%s,%s,,%d)\n", instruction, resource, 0);
	} else if (pair->timeout <= 0) {
		*t = TOKEN_EXPIRED;
		printf("DENY (%s,%s,,%d)\n", instruction, resource, 0);
	} else if (!check_resource_existence(p->resource)) {
		*t = RESOURCE_NOT_FOUND;
		pair->timeout -= 1;
		printf("DENY (%s,%s,%s,%d)\n", instruction, resource, pair->access_token, pair->timeout);
	} else if (!check_operation_permitted(instruction, access_token, resource)) {
		*t = OPERATION_NOT_PERMITTED;
		pair->timeout -= 1;
		printf("DENY (%s,%s,%s,%d)\n", instruction, resource, pair->access_token, pair->timeout);
	} else {
		*t = PERMISSION_GRANTED;
		pair->timeout -= 1;
		printf("PERMIT (%s,%s,%s,%d)\n", instruction, resource, pair->access_token, pair->timeout);
	}

	return t;
}

/* procedure that asks for the approval of the final user */
char **approve_request_token_1_svc(char **p, struct svc_req *cl) {
	/* if there are no permissions for the auth token, then create an entry */
	token_rights_t *rights = get_token_status(*p);
	if (!rights) {
		token_rights[token_rights_size] = (token_rights_t *) malloc(sizeof(token_rights_t));
		token_rights[token_rights_size]->token = (char *) malloc((strlen(*p) + 1) * sizeof(char));
		strcpy(token_rights[token_rights_size]->token, *p);
		rights = token_rights[token_rights_size];
		token_rights_size += 1;
	}

	/* provide permissions for the token */
	rights->rights = approvals[approvals_index];
	approvals_index += 1;

	return p;
}