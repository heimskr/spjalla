#include <chrono>

#include "spjalla/core/Client.h"

namespace Spjalla {
	void Client::heartbeatLoop() {
		while (heartbeatAlive) {
			for (const auto fn: heartbeatListeners)
				(*fn)(heartbeatPeriod);
			
			std::this_thread::sleep_for(PingPong::Util::TimeType(heartbeatPeriod));
		}
	}

	void Client::addHeartbeatListener(const Client::HeartbeatListener &fn) {
		heartbeatListeners.push_back(fn);
	}

	void Client::removeHeartbeatListener(const Client::HeartbeatListener &fn) {
		auto iter = std::find(heartbeatListeners.begin(), heartbeatListeners.end(), fn);
		if (iter != heartbeatListeners.end())
			heartbeatListeners.erase(iter);
	}

	void Client::initHeartbeat() {
		heartbeatAlive = true;
		heartbeat = std::thread(&Client::heartbeatLoop, this);
	}
}
