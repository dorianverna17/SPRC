#define TOKEN_LEN 15
#define FILE_NAME_LEN 50
#define INSTRUCTION_LEN 50

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
	string token<TOKEN_LEN>;
};

struct access_token_t {
	status_t status;
	string token<TOKEN_LEN>;
	string reset_token<TOKEN_LEN>;
	int timeout;
};

struct client_access_t {
	string user_id<TOKEN_LEN>;
	string authorization_token<TOKEN_LEN>;
	int reset;
};

struct action_request_t {
	string instruction<INSTRUCTION_LEN>;
	string resource<FILE_NAME_LEN>;
	string access_token<TOKEN_LEN>;
};

program RPC_OAUTH_PROGRAM {
	version RPC_OAUTH_VERS {
		authorization_token_t REQUEST_AUTHORIZATION(string) = 1;
		access_token_t REQUEST_ACCESS_TOKEN(client_access_t*) = 2;
		int VALIDATE_DELEGATED_ACTION(action_request_t*) = 3;
		string APPROVE_REQUEST_TOKEN(string) = 4;
	} = 1;
} = 1234;