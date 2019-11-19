#include <chrono>

#include "spjalla/core/client.h"

namespace spjalla {
	void client::heartbeat_loop() {
		while (heartbeat_alive) {
			for (const auto fn: heartbeat_listeners)
				(*fn)(heartbeat_period);
			
			std::this_thread::sleep_for(pingpong::util::timetype(heartbeat_period));
		}
	}

	void client::add_heartbeat_listener(const client::heartbeat_listener &fn) {
		heartbeat_listeners.push_back(fn);
	}

	void client::remove_heartbeat_listener(const client::heartbeat_listener &fn) {
		auto iter = std::find(heartbeat_listeners.begin(), heartbeat_listeners.end(), fn);
		if (iter != heartbeat_listeners.end())
			heartbeat_listeners.erase(iter);
	}

	void client::init_heartbeat() {
		heartbeat_alive = true;
		heartbeat = std::thread(&client::heartbeat_loop, this);
	}
}
