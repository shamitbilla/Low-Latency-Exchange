#pragma once

#include "orderEvent.hpp"
#include <chrono>
#include <sys/types.h>
#include <vector>

struct transaction{
	std::string ticker;
	u_int64_t buyOrderId;
	u_int64_t sellOrderId;
	u_int64_t buyerId;
	u_int64_t sellerId;
	double price;
	u_int64_t quantity;
	std::chrono::time_point<std::chrono::system_clock> timestamp;
};

struct tradeEvent{
	OrderEventType event;
	std::vector<transaction> trades;
};
