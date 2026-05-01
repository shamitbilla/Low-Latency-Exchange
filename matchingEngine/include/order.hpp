#pragma once

#include <chrono>
#include <cstdint>
#include <string>

enum OrderSide{
	BUY,
	SELL
};

enum OrderType{
	LIMIT,
	MARKET
};

enum OrderStatus{
	OPEN,
	FILLED,
	PARTIALLY_FILLED,
	CANCELLED
};

struct Order{

	uint64_t id;
	uint64_t userId;
	std::string ticker;
	OrderSide orderSide;
	
	double price;
	uint64_t quantity;
	OrderType orderType;
	OrderStatus orderStatus; 
	
	std::chrono::time_point<std::chrono::system_clock> timestamp;
	
};
