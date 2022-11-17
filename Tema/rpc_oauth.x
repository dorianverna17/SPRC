enum status_t {
	STATUS_OK = 0,
	USER_NOT_FOUND = 1,
	REQUEST_DENIED = 2,
	PERMISSION_DENIED = 3,
	TOKEN_EXPIRED = 4,
	RESOURCE_NOT_FOUND = 5,
	OPERATION_NOT_PERMITTED = 6,
	PERMISSION_GRANTED = 7
};

struct authorization_token_t {
	status_t status;
	char *token;
};

struct access_token_t {
	status_t status;
	char *token;
};

struct approve_request_token_t {
	status_t status;
	char *token;
};

struct client_status_t {
	char *user_id;
	char *authorization_token;
	char *access_token;
};

program RPC_OAUTH_PROGRAM {
	version RPC_OAUTH_VERS {
		authorization_token_t REQUEST_AUTHORIZATION(char*) = 1;
		access_token_t REQUEST_ACCESS_TOKEN(client_status_t*) = 2;
		int VALIDATE_DELEGATED_ACTION(void) = 3;
		approve_request_token_t APPROVE_REQUEST_TOKEN(void) = 4;
	} = 1;
} = 1234;