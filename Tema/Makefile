RPC = rpc_oauth
CFLAGS = -Wall -Wno-unused-variable -Wno-uninitialized-variable

RPC_GEN = $(RPC).h $(RPC)_xdr.o $(RPC)_clnt.o

build: $(RPC_GEN)
	gcc -o server server.c $(RPC)_svc.c $(RPC)_xdr.c utils.c -lnsl $(CFLAGS)
	gcc -o client client.c $(RPC)_clnt.c $(RPC)_xdr.c utils.c -lnsl $(CFLAGS)

$(RPC).h: rpc_oauth.x
	rpcgen -C rpc_oauth.x

$(RPC)_xdr.c: rpc_oauth.x
	rpcgen -C rpc_oauth.x

$(RPC)_clnt.c: rpc_oauth.x
	rpcgen -C rpc_oauth.x

clean:
	rm -f client server rpc_oauth.h *.o