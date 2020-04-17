#include <signal.h>
#include <string.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <syslog.h>
#include <unistd.h>

#include "Tintin_reporter.hpp"
#include "server.hpp"

#define LOCKFILE "/var/lock/matt-daemon.lock"

int g_lockfd = 0;

void exit_prog(int code)
{
	Tintin_reporter::get().log(LOG_NOTICE, "Shutting down daemon");
	unlink(LOCKFILE);
	flock(g_lockfd, LOCK_UN);
	close(g_lockfd);
	exit(code);
}

void handle_signal(int sig)
{
	Tintin_reporter::get().log("Received shutdown signal");
	exit_prog(EXIT_SUCCESS);
}

void daemonize()
{
	int pid = fork();
	if (pid < 0)
		exit_prog(EXIT_FAILURE);
	if (pid > 0)
		exit_prog(EXIT_SUCCESS);
	if (setsid() < 0)
		exit_prog(EXIT_FAILURE);
	umask(0);
	fclose(stdout);
	fclose(stderr);
	fclose(stdin);
}

int main()
{
	// acquire resource
	int fd = open(LOCKFILE, O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	if (fd < 0) {
		perror("Unable to open lock file " LOCKFILE);
		return EXIT_FAILURE;
	}
	int fail = flock(fd, LOCK_EX | LOCK_NB);
	if (fail) {
		perror("Unable to lock file " LOCKFILE);
		close(fd);
		return EXIT_FAILURE;
	}

	daemonize();

	// set signal handlers
	g_lockfd = fd;
	signal(SIGINT, handle_signal);
	signal(SIGQUIT, handle_signal);
	signal(SIGTERM, handle_signal);
	Tintin_reporter::get().log(LOG_INFO, "Daemon running");

	// server
	int sock = setup_server();
	if (sock < 0) {
		Tintin_reporter::get().log(LOG_CRIT, "Unable to initialize server: %s",
								   strerror(errno));
		exit_prog(EXIT_FAILURE);
	}
	listen_and_serve(sock);
	exit_prog(EXIT_SUCCESS);
}
