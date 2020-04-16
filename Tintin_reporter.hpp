#pragma once

#include <string>

class Tintin_reporter {
  private:
	int _option;
	int _facility;

  public:
	Tintin_reporter(void);
	Tintin_reporter(std::string &s);
	Tintin_reporter(const char *s);

	~Tintin_reporter(void);

	static Tintin_reporter &get(void);
	void log(std::string &s, ...);
    void log(int priority, std::string &s, ...);
	void log(const char *s, ...);
    void log(int priority, const char *s, ...);
    void log(int priority, const char *s, va_list ap);
};
