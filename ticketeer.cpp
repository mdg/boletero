#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <map>
#include <string>


enum side_t { ODD, EVEN };
side_t ODD_OR_EVEN = ODD;
typedef std::map< std::string, int > space_id_map;
typedef space_id_map::iterator space_id_iterator;
space_id_map SPACE_ID;

bool odd(void)
{
	return ODD_OR_EVEN == ODD;
}

#define ID(odd,even) (ODD_OR_EVEN == ODD ? odd : even)


int server_iteration(int sock)
{
	struct sockaddr_in client;
	socklen_t addr_len;
	char space[80];
	char output[128];
	int n;
	int id = 0;
	space_id_iterator it;

	addr_len = sizeof(client);
	bzero(space,sizeof(space));
	n = recvfrom(sock,space,60,0
			,(struct sockaddr *) &client, &addr_len);
	printf("received something: %s", space);
	space[strlen(space) - 1] = '\0';
	it = SPACE_ID.find(space);
	if (it == SPACE_ID.end()) {
		SPACE_ID[space] = id = ID(1,2);
	} else {
		id = it->second = it->second + 2;
	}
	sprintf(output, "%s: %d\n", space, id);
	sendto(sock,output,strlen(output),0
			,(struct sockaddr *) &client, sizeof(client));
	return 0;
}

int main(void)
{
	int sock = 0;
	struct sockaddr_in server;

	sock = socket(AF_INET,SOCK_DGRAM,0);

	bzero(&server,sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(21212);
	bind(sock,(struct sockaddr *) &server, sizeof(server));

	for (;;) {
		server_iteration(sock);
	}
	return 0;
}

