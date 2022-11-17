#include "utils.h"

client_status_t *get_user_status(char *user) {
	for (int i = 0; i < status_size; i++) {
		if (strcmp(user, statuses[i]->user_id) == 0) {
			return statuses[i];
		}
	}
	return NULL;
}

struct token_rights_t *get_token_status(char *token) {
	for (int i = 0; i < token_rights_size; i++) {
		if (strcmp(token, token_rights[i]->token) == 0) {
			return token_rights[i];
		}
	}
	return NULL;
}

void read_files() {
	int size;
	char buffer[BUFF_LEN];

	/* read the ids of the clients */
	FILE *f = fopen(client_file, "r");

	fscanf(f, "%d", &size);
	user_ids = malloc(size * (sizeof(char*)));
	for (int i = 0; i < size; i++) {
		fscanf(f, "%s", buffer);
		user_ids[i] = malloc((strlen(buffer) + 1) * sizeof(char));
		strcpy(user_ids[i], buffer);
		user_ids[i][strlen(buffer)] = '\0';
	}
	users_no = size;
	statuses = malloc(users_no * sizeof(client_status_t));

	/* read the name of the resources */
	f = fopen(resources_file, "r");

	fscanf(f, "%d", &size);

	resources = malloc(size * (sizeof(char*)));
	for (int i = 0; i < size; i++) {
		fscanf(f, "%s", buffer);
		resources[i] = malloc((strlen(buffer) + 1) * sizeof(char));
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

	approvals = malloc(sizeof(struct approvals_t));
	while (fgets(buffer, BUFF_LEN, f)) {
		buffer[strlen(buffer) - 1] = '\0';

		if (strcmp(buffer, "*,-") == 0) {
			approvals[i].permission = NULL;
			approvals[i].file = NULL;
			approvals[i].size = 0;
			i += 1;
			approvals = realloc(approvals, (i + 1) * sizeof(struct approvals_t));
			continue;
		}

		approvals[i].permission = malloc(resources_no * sizeof(char*));
		approvals[i].file = malloc(resources_no * sizeof(char*));

		token = strtok(buffer, delim);

		count = 0;
		while (token) {
			if (count % 2 != 0) {
				approvals[i].permission[count / 2] = malloc((strlen(token) + 1) * sizeof(char));
				strcpy(approvals[i].permission[count / 2], token);
				approvals[i].permission[count / 2][strlen(token)] = '\0';
			} else {
				approvals[i].file[count / 2] = malloc((strlen(token) + 1) * sizeof(char));
				strcpy(approvals[i].file[count / 2], token);
				approvals[i].file[count / 2][strlen(token)] = '\0';
			}
			count += 1;
			token = strtok(NULL, delim);
		}
		approvals[i].size = count / 2;
		i += 1;
		approvals = realloc(approvals, (i + 1) * sizeof(struct approvals_t));
	}
	approvals_no = i;
	approvals_index = 0;
	token_rights = malloc(approvals_no * sizeof(struct token_rights_t));

	token_rights_size = 0;
	status_size = 0;
}