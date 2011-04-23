#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <map>
#include <string>
#include <list>
#include <iostream>
#include <fstream>

using namespace std;


enum side_t { ODD, EVEN };
side_t ODD_OR_EVEN = ODD;
typedef std::pair< int, fstream * > id_file;
typedef std::map< std::string, id_file > space_map;
typedef space_map::iterator space_iterator;

space_map SPACE;


#ifdef MAKE_ODD
#define ID(odd,even) odd
#define NUM_ARGS 1
#define MOD2 1
#define ODD_OR_EVEN_NAME "odd"
#elif defined MAKE_EVEN
#define ID(odd,even) even
#define NUM_ARGS 1
#define MOD2 0
#define ODD_OR_EVEN_NAME "even"
#else
#define ID(odd,even) (ODD_OR_EVEN == ODD ? odd : even)
#define NUM_ARGS 2
#define MOD2 (ODD_OR_EVEN == ODD ? 1 : 0)
#define ODD_OR_EVEN_NAME (ODD_OR_EVEN == ODD ? "odd" : "even")
#endif

inline bool correct(int id) { return id % 2 == MOD2; }


int open_spaces(const list< string > &spacenames)
{
	list< string >::const_iterator it = spacenames.begin();
	for (; it != spacenames.end(); ++it) {
		fstream *f = new fstream();
		string filename = "id/"+ *it +".";
		filename += ODD_OR_EVEN_NAME;
		f->open(filename.c_str());
		int id = 0;
		*f >> id;
		if (! correct(id)) {
			cerr << "initial ticket is not "
				<< ODD_OR_EVEN_NAME << ": " << *it
				<< "/" << id << endl;
			exit(-1);
		}
		cout << "id = " << id << endl;
		SPACE[*it] = id_file(id,f);
	}
}

int server_iteration(int sock)
{
	struct sockaddr_in client;
	socklen_t addr_len;
	char space[80];
	char output[128];
	int n;
	int id = 0;
	space_iterator it;

	addr_len = sizeof(client);
	bzero(space,sizeof(space));
	n = recvfrom(sock,space,60,0
			,(struct sockaddr *) &client, &addr_len);
	printf("ticket space: %s", space);
	space[strlen(space) - 1] = '\0';
	it = SPACE.find(space);
	if (it == SPACE.end()) {
		id = -1;
	} else {
		id = it->second.first = it->second.first + 2;
		fstream &f(*it->second.second);
		f.seekp(0);
		f << id << endl;
	}
	sprintf(output, "%s: %d\n", space, id);
	sendto(sock,output,strlen(output),0
			,(struct sockaddr *) &client, sizeof(client));
	return 0;
}

int main(int argc, const char **argv)
{
	int sock = 0;
	struct sockaddr_in server;

	// check number of arguments
	if (argc < NUM_ARGS) {
		std::cerr << "Not enough arguments\n";
		return -1;
	} else if (argc > NUM_ARGS) {
		std::cerr << "Too many arguments\n";
		return -1;
	}


#ifdef DYNAMIC_ODD_OR_EVEN
	// check value of odd or even argument
	std::string odd_or_even(argv[1]);
	if (odd_or_even == "odd") {
		ODD_OR_EVEN = ODD;
	} else if (odd_or_even == "even") {
		ODD_OR_EVEN = EVEN;
	} else {
		std::cerr << "Argument should be odd or even\n";
		return -1;
	}
#endif

	list< string > spacenames;
	spacenames.push_back("dog");
	open_spaces(spacenames);

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

