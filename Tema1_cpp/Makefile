RPC = rpc_oauth
CFLAGS = -Wall -Wno-unused-variable -Wno-uninitialized-variable

RPC_GEN = $(RPC).h
PATH_TO_FILE = rpc_oauth_svc.c

build: $(RPC_GEN)
	g++ -o server server.cpp $(RPC)_svc.c $(RPC)_xdr.c utils.cpp -lnsl $(CFLAGS)
	g++ -o client client.cpp $(RPC)_clnt.c $(RPC)_xdr.c -lnsl $(CFLAGS)

$(RPC).h: rpc_oauth.x
	if test -f ${PATH_TO_FILE}; then \
		touch intermediate_file.txt; \
		cat rpc_oauth_svc.c > intermediate_file.txt; \
		rpcgen -C rpc_oauth.x; \
		cat intermediate_file.txt > rpc_oauth_svc.c; \
		rm intermediate_file.txt; \
    else \
		rpcgen -C rpc_oauth.x; \
	fi

clean:
	rm -f client server rpc_oauth.h *.o