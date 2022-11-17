#include <stdio.h>
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
extern struct approvals_t *approvals;

authorization_token_t *request_authorization_1_svc(char *p, struct svc_req *cl) {
	static authorization_token_t *t;
	
	return t;
}

access_token_t *request_access_token_1_svc(client_status_t *p, struct svc_req *cl) {
	static access_token_t *t;
	
	return t;
}

int *validate_delegated_action_1_svc(void *p, struct svc_req *cl) {
	int *t = malloc(sizeof(int));
	
	*t = token_timeout;

	return t;
}

approve_request_token_t *approve_request_token_1_svc(void *p, struct svc_req *cl) {
	static approve_request_token_t *t;
	
	return t;
}