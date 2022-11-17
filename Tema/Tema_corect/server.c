#include <stdio.h>
#include <rpc/rpc.h>

#include "utils.h"
#include "token.h"
#include "rpc_oauth.h"

authorization_token_t *request_authorization_1_svc(char **p, struct svc_req *cl) {
	authorization_token_t *t;
	char *user = *p;
	client_status_t *status;

	t = malloc(sizeof(struct authorization_token_t));
	t->token = NULL;
	for (int i = 0; i < users_no; i++) {
		printf("alright %s\n", user);
		printf("%s fuck %s\n", user, user_ids[i]);
		printf("alright\n");
		if (strcmp(user, user_ids[i]) == 0) {
			strcpy(t->token, generate_access_token(user));
			t->status = STATUS_OK;

			status = get_user_status(user);
			if (!status) {
				statuses[status_size] = malloc(sizeof(struct client_status_t));
				strcpy(statuses[status_size]->user_id, user);
				status_size += 1;
			}
			strcpy(statuses[status_size]->authorization_token, t->token);
			statuses[status_size]->access_token = NULL;
		}
	}
	if (!t->token)
		t->status = USER_NOT_FOUND;

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

char *approve_request_token_1_svc(char *p, struct svc_req *cl) {
	// token_rights_t rights = get_token_status(p, token_rights, token_rights_size);
	// if (!rights) {

	// }

	// if (!(approvals[approvals_index].size == 0)) {
		
	// }

	approvals_index += 1;

	return p;
}