/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include "rpc_oauth.h"

bool_t
xdr_status_t (XDR *xdrs, status_t *objp)
{
	register int32_t *buf;

	 if (!xdr_enum (xdrs, (enum_t *) objp))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_authorization_token_t (XDR *xdrs, authorization_token_t *objp)
{
	register int32_t *buf;

	 if (!xdr_status_t (xdrs, &objp->status))
		 return FALSE;
	 if (!xdr_pointer (xdrs, (char **)&objp->token, sizeof (char), (xdrproc_t) xdr_char))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_access_token_t (XDR *xdrs, access_token_t *objp)
{
	register int32_t *buf;

	 if (!xdr_status_t (xdrs, &objp->status))
		 return FALSE;
	 if (!xdr_pointer (xdrs, (char **)&objp->token, sizeof (char), (xdrproc_t) xdr_char))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_approve_request_token_t (XDR *xdrs, approve_request_token_t *objp)
{
	register int32_t *buf;

	 if (!xdr_status_t (xdrs, &objp->status))
		 return FALSE;
	 if (!xdr_pointer (xdrs, (char **)&objp->token, sizeof (char), (xdrproc_t) xdr_char))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_client_status_t (XDR *xdrs, client_status_t *objp)
{
	register int32_t *buf;

	 if (!xdr_pointer (xdrs, (char **)&objp->user_id, sizeof (char), (xdrproc_t) xdr_char))
		 return FALSE;
	 if (!xdr_pointer (xdrs, (char **)&objp->authorization_token, sizeof (char), (xdrproc_t) xdr_char))
		 return FALSE;
	 if (!xdr_pointer (xdrs, (char **)&objp->access_token, sizeof (char), (xdrproc_t) xdr_char))
		 return FALSE;
	return TRUE;
}
