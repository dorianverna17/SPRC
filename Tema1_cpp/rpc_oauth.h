/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#ifndef _RPC_OAUTH_H_RPCGEN
#define _RPC_OAUTH_H_RPCGEN

#include <rpc/rpc.h>


#ifdef __cplusplus
extern "C" {
#endif


enum status_t {
	STATUS_OK = 0,
	USER_NOT_FOUND = 1,
	REQUEST_DENIED = 2,
	PERMISSION_DENIED = 3,
	TOKEN_EXPIRED = 4,
	RESOURCE_NOT_FOUND = 5,
	OPERATION_NOT_PERMITTED = 6,
	PERMISSION_GRANTED = 7,
};
typedef enum status_t status_t;

enum instruction_t {
	REQUEST = 0,
	MODIFY = 1,
	EXECUTE = 2,
	DELETE = 3,
	INSERT = 4,
	READ = 5,
};
typedef enum instruction_t instruction_t;

struct authorization_token_t {
	status_t status;
	char *token;
};
typedef struct authorization_token_t authorization_token_t;

struct access_token_t {
	status_t status;
	char *token;
	char *reset_token;
	int timeout;
};
typedef struct access_token_t access_token_t;

struct client_access_t {
	char *user_id;
	char *authorization_token;
	int reset;
};
typedef struct client_access_t client_access_t;

struct action_request_t {
	instruction_t instruction;
	char *resource;
	char *access_token;
};
typedef struct action_request_t action_request_t;

#define RPC_OAUTH_PROGRAM 1234
#define RPC_OAUTH_VERS 1

#if defined(__STDC__) || defined(__cplusplus)
#define REQUEST_AUTHORIZATION 1
extern  authorization_token_t * request_authorization_1(char **, CLIENT *);
extern  authorization_token_t * request_authorization_1_svc(char **, struct svc_req *);
#define REQUEST_ACCESS_TOKEN 2
extern  access_token_t * request_access_token_1(client_access_t *, CLIENT *);
extern  access_token_t * request_access_token_1_svc(client_access_t *, struct svc_req *);
#define VALIDATE_DELEGATED_ACTION 3
extern  int * validate_delegated_action_1(action_request_t *, CLIENT *);
extern  int * validate_delegated_action_1_svc(action_request_t *, struct svc_req *);
#define APPROVE_REQUEST_TOKEN 4
extern  char ** approve_request_token_1(char **, CLIENT *);
extern  char ** approve_request_token_1_svc(char **, struct svc_req *);
extern int rpc_oauth_program_1_freeresult (SVCXPRT *, xdrproc_t, caddr_t);

#else /* K&R C */
#define REQUEST_AUTHORIZATION 1
extern  authorization_token_t * request_authorization_1();
extern  authorization_token_t * request_authorization_1_svc();
#define REQUEST_ACCESS_TOKEN 2
extern  access_token_t * request_access_token_1();
extern  access_token_t * request_access_token_1_svc();
#define VALIDATE_DELEGATED_ACTION 3
extern  int * validate_delegated_action_1();
extern  int * validate_delegated_action_1_svc();
#define APPROVE_REQUEST_TOKEN 4
extern  char ** approve_request_token_1();
extern  char ** approve_request_token_1_svc();
extern int rpc_oauth_program_1_freeresult ();
#endif /* K&R C */

/* the xdr functions */

#if defined(__STDC__) || defined(__cplusplus)
extern  bool_t xdr_status_t (XDR *, status_t*);
extern  bool_t xdr_instruction_t (XDR *, instruction_t*);
extern  bool_t xdr_authorization_token_t (XDR *, authorization_token_t*);
extern  bool_t xdr_access_token_t (XDR *, access_token_t*);
extern  bool_t xdr_client_access_t (XDR *, client_access_t*);
extern  bool_t xdr_action_request_t (XDR *, action_request_t*);

#else /* K&R C */
extern bool_t xdr_status_t ();
extern bool_t xdr_instruction_t ();
extern bool_t xdr_authorization_token_t ();
extern bool_t xdr_access_token_t ();
extern bool_t xdr_client_access_t ();
extern bool_t xdr_action_request_t ();

#endif /* K&R C */

#ifdef __cplusplus
}
#endif

#endif /* !_RPC_OAUTH_H_RPCGEN */
