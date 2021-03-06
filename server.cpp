#include <algorithm>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>
#include <vector>

#include "Tintin_reporter.hpp"

#define PORT		4242
#define BACKLOG		5
#define NUM_CLIENTS 5

#define BUFSIZE 1024

int setup_server(void)
{
	int sock_s;
	struct sockaddr_in server;

	// configure socket
	if ((sock_s = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		return (-1);
	int opt = 1;
	if (setsockopt(sock_s, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt))
		< 0)
		return (-1);
	server.sin_family = PF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(PORT);

	if (bind(sock_s, (struct sockaddr *)&server, sizeof(server)) < 0) {
		return (-1);
	}

	// listen && serve
	if (listen(sock_s, BACKLOG)) {
		return (-1);
	}
	return (sock_s);
}

int accept_client(struct sockaddr_in *client, int sock_s)
{
	size_t len;
	int sock_c;

	len = sizeof(struct sockaddr_in);
	if ((sock_c = accept(sock_s, (struct sockaddr *)client, (socklen_t *)&len))
		< 0)
		return (-1);
	return (sock_c);
}

int handle_conn(int sock_s, std::vector<int> &client_socks, fd_set &fd_list)
{
	int sock_c;
	struct sockaddr_in client;

	if ((sock_c = accept_client(&client, sock_s)) < 0) {
		Tintin_reporter::get().log(LOG_ERR, "Unable to accept client: %s",
								   strerror(errno));
		return (-1);
	}
	return (sock_c);
}

int handle_request(std::vector<int> &client_socks, fd_set &fd_list)
{
	int offset;
	char buf[BUFSIZ];
	std::string msg;

	for (unsigned i = 0; i < client_socks.size(); i++) {
		int client = client_socks[i];
		if (FD_ISSET(client, &fd_list)) {
			bzero(buf, sizeof(buf));
			offset = 0;
			for (;;) {
				if (recv(client, buf + offset, 1, 0) <= 0) {
					close(client_socks[i]);
					client_socks[i] = -1;
					break;
				}
				if (buf[offset] == '\n') {
					buf[offset] = '\0';
					break;
				}
				offset++;
				if (offset >= BUFSIZ - 1) {
					msg.append(buf);
					bzero(buf, sizeof(buf));
					offset = 0;
				}
			}
			msg.append(buf);
			if (msg.length() > 0) {
				Tintin_reporter::get().log(LOG_INFO, "Message received: %s",
										   msg.c_str());
				if (msg.compare("quit") == 0)
					return (1);
			}
			offset = 0;
		}
	}
	return (0);
}

void listen_and_serve(int sock_s)
{
	fd_set fd_list;
	int max_sock;
	std::vector<int> client_socks;

	while (1) {
		max_sock = sock_s;
		FD_ZERO(&fd_list);
		FD_SET(sock_s, &fd_list);
		for (int client : client_socks) {
			if (client > 0)
				FD_SET(client, &fd_list);
			if (client > max_sock)
				max_sock = client;
		}
		if (select(max_sock + 1, &fd_list, NULL, NULL, NULL) < 0) {
			Tintin_reporter::get().log(LOG_CRIT, "Failed to select: %s",
									   strerror(errno));
			return;
		}
		if (FD_ISSET(sock_s, &fd_list)) {
			int client = handle_conn(sock_s, client_socks, fd_list);
			if (client > 0)
				client_socks.push_back(client);
		}
		if (handle_request(client_socks, fd_list))
			return;
		client_socks.erase(
			std::remove_if(client_socks.begin(), client_socks.end(),
						   [](const int &fd) { return fd <= 0; }),
			client_socks.end());
	}
}
