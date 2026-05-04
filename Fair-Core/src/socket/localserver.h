#pragma once
#include <string>
namespace localserver {
	void init();
	void shutdown();
	void send(const std::string& message);
	bool check_port(int port);
}