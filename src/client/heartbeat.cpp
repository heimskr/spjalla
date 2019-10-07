#include <chrono>

#include "spjalla/core/client.h"

namespace spjalla {
	void client::heartbeat_loop() {
		while (heartbeat_alive) {
			for (const auto fn: heartbeat_listeners)
				fn(heartbeat_period);
			
			std::this_thread::sleep_for(pingpong::util::timetype(heartbeat_period));
		}
	}

	void client::add_heartbeat_listener(const std::function<void(int)> &fn) {
		heartbeat_listeners.push_back(fn);
	}

	void client::init_heartbeat() {
		heartbeat_alive = true;
		heartbeat = std::thread(&client::heartbeat_loop, this);
	}
}
