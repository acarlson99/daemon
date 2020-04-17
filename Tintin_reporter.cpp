#include "Tintin_reporter.hpp"
#include <iostream>
#include <stdarg.h>
#include <syslog.h>

static Tintin_reporter g_r = Tintin_reporter();

Tintin_reporter::Tintin_reporter(void) { Tintin_reporter(NULL); }
Tintin_reporter::Tintin_reporter(std::string &s) { Tintin_reporter(s.c_str()); }
Tintin_reporter::Tintin_reporter(const char *s)
{
	_option = 0;
	_facility = LOG_DAEMON;
	openlog(s, _option, _facility);
}

Tintin_reporter::~Tintin_reporter() { closelog(); }

Tintin_reporter &Tintin_reporter::get() { return g_r; }

void Tintin_reporter::log(std::string &s, ...)
{
	va_list ap;

	va_start(ap, s);
	log(LOG_INFO, s.c_str(), ap);
	va_end(ap);
}

void Tintin_reporter::log(int priority, std::string &s, ...)
{
	va_list ap;

	va_start(ap, s);
	log(priority, s.c_str(), ap);
	va_end(ap);
}

void Tintin_reporter::log(const char *s, ...)
{
	va_list ap;

	va_start(ap, s);
	log(LOG_INFO, s, ap);
	va_end(ap);
}

void Tintin_reporter::log(int priority, const char *s, ...)
{
	va_list ap;

	va_start(ap, s);
	log(priority, s, ap);
	va_end(ap);
}

void Tintin_reporter::log(int priority, const char *s, va_list ap)
{
	vsyslog(_facility | priority, s, ap);
}
