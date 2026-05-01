#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include "orderbook.hpp"

class OrderBookManager{
private:

	std::unordered_map<std::string, std::shared_ptr<OrderBook>> orderBookMap;
	std::unordered_set<std::string> registeredTickers = {"AAPL", "GOOG", "MSFT", "RELIANCE"};

	OrderBookManager() = default;
	OrderBookManager(const OrderBookManager&) = delete;
	OrderBookManager& operator=(const OrderBookManager&) = delete;
	
public:
	static OrderBookManager& getInstance(){
		static OrderBookManager instance;
		return instance;
	}

	bool registerTicker(const std::string& ticker);
	bool isTickerValid(const std::string& ticker);
	std::shared_ptr<OrderBook> getOrderBook(const std::string& ticker);
};
