#pragma once

#include "order.hpp"

enum OrderEventType{
	ORDER_ADDED,
	ORDER_CANCELLED,
	ORDER_UPDATED,
	TRADE_EXECUTABLE,
	TRADE_EXECUTED,
	ORDER_ADD_FAILED,
	ORDER_UPDATE_FAILED,
	ORDER_CANCEL_FAILED,
	TRADE_EXECUTE_FAILED
};


struct orderEvent{
	OrderEventType event;
	Order order;
	std::string message;
};
