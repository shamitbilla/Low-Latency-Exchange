#include "../../include/orderbookManager.hpp"
#include <memory>
#include <string>
#include <spdlog/spdlog.h>
#include "../../include/orderbook.hpp"

using namespace std;

bool OrderBookManager::registerTicker(const string& ticker){
	if(ticker.empty()){
		spdlog::warn("Cannot register an empty ticker");
		return false;
	}

	auto [it, inserted] = registeredTickers.insert(ticker);

	if(!inserted){
		spdlog::warn("Ticker is already registered");
		return false;
	}

	spdlog::info("Ticker {} is registered successfully.", ticker);

	return true;
}

shared_ptr<OrderBook> OrderBookManager::getOrderBook(const string& ticker){
	
	if(registeredTickers.find(ticker) == registeredTickers.end()){
		spdlog::error("Ticker is not registered");
		return nullptr;
	}

	auto it = orderBookMap.find(ticker);

	if(it != orderBookMap.end()){
		spdlog::info("Returning existing OrderBook for {}", ticker);
		return it->second;
	}
	else{
		spdlog::info("Creating a new orderbook for {}", ticker);

		OrderBook* rawOrderBookPtr = new OrderBook(ticker);

		shared_ptr<OrderBook> newOrderBook(rawOrderBookPtr);
		newOrderBook->ticker = ticker;
		
		orderBookMap[ticker] = newOrderBook;

		return newOrderBook;
	}
}


bool OrderBookManager::isTickerValid(const string& ticker){

	if(ticker.empty()){
		spdlog::warn("Cannot register an empty ticker");
		return false;
	}

	if(registeredTickers.count(ticker)){
		return true;
	}
	else{
		return false;
	}
}
