#pragma once
#include <functional>
#include <list>
#include <map>
#include <optional>
#include <string>
#include <sys/types.h>
#include <unordered_map>
#include "order.hpp"

class OrderBookManager;

class OrderBook{
private:
	friend class OrderBookManager;

	OrderBook(const std::string& ticker);
	OrderBook(const OrderBook&) = delete;
	OrderBook& operator=(const OrderBook&)=delete;

	std::string ticker;
	int lastTradedPrice;

	std::map<double, std::list<Order>> sellOrders;
	std::map<double, std::list<Order>, std::greater<int>> buyOrders;
	
	std::map<double, int> sellVolume;
	std::map<double, int> buyVolume;

	// For fast lookup
	std::unordered_map<int,std::list<Order>::iterator> orderMap;
	// helper function
	Order* getOrderPtr(int orderId);

public:
	Order addOrder(const Order& order);
	std::optional<Order> getOrder(int orderId);
	bool updateOrder(Order& order);
	bool deleteOrder(int orderId);
	int getVolume(OrderSide side, int price);
	std::list<Order*> match(Order& order);
	bool setLastTradedPrice(int price);
	
};
