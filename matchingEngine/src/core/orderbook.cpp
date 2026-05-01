#include "../../include/orderbook.hpp"
#include "order.hpp"
#include <list>
#include <optional>
#include <spdlog/spdlog.h>
#include <string>
#include <sys/types.h>
using namespace std;

OrderBook::OrderBook(const std::string& ticker): ticker(ticker){
	spdlog::info("OrderBook created for ticker {}", ticker);
};

Order OrderBook::addOrder(const Order& order){

	list<Order>::iterator newOrderIt;

	if(order.orderType == MARKET){
		spdlog::info("Fuck off");
	}
	else{

		if(order.orderSide == BUY){
			newOrderIt = buyOrders[order.price].insert(buyOrders[order.price].end(), order);
			buyVolume[order.price] += order.quantity;
		}
		else{
			newOrderIt = sellOrders[order.price].insert(sellOrders[order.price].end(), order);
			sellVolume[order.price] += order.quantity;
		}
	}
	
	orderMap[order.id] = newOrderIt;
	spdlog::info("Order id {} added successfully", order.id);
	return order;

}

Order* OrderBook::getOrderPtr(int orderId){

	auto it = orderMap.find(orderId);

	if(it != orderMap.end()){
		list<Order>::iterator& order_it = it->second;
		return &(*order_it);
	}
	else{
		spdlog::info("Order not found");
		return nullptr;
	}

}

optional<Order> OrderBook::getOrder(int orderId){
	Order* order = getOrderPtr(orderId);

	if(order){
		return *order;
	}
	else{
		spdlog::info("Order not found");
		return nullopt;
	}
}

bool OrderBook::updateOrder(Order& updatedOrder){

	Order* order = getOrderPtr(updatedOrder.id);

	if(order){
		order->orderSide = updatedOrder.orderSide;
		order->price = updatedOrder.price;
		order->quantity = updatedOrder.quantity;
		order->orderType = updatedOrder.orderType;
		order->timestamp = chrono::system_clock::now();

		return true;
	}
	else{
		spdlog::info("Order not found");
		return false;
	}
}

bool OrderBook::deleteOrder(int orderId){
	auto map_it = orderMap.find(orderId);

	if(map_it == orderMap.end()){
		spdlog::warn("Delete failed: Order Id {} not found", orderId);
		return false;
	}

	auto order_it = map_it->second;

	Order& orderToDelete = *order_it; //check if bugs
	orderToDelete.orderStatus = CANCELLED;
	double price = orderToDelete.price;
	int quantity = orderToDelete.quantity;
	OrderSide side = orderToDelete.orderSide;

	if(side == BUY){
		auto list_map_it = buyOrders.find(price);
		
		if(list_map_it != buyOrders.end()){
			list_map_it->second.erase(order_it);

			buyVolume[price] -= quantity;

			if(list_map_it->second.empty()){
				buyOrders.erase(list_map_it);
				buyOrders.erase(price);
			}
		}
		else{
			spdlog::warn("No order exists with that price");
			return false;
		}


	}
	else{
		auto list_map_it = sellOrders.find(price);

		if(list_map_it != sellOrders.end()){
			list_map_it->second.erase(order_it);
			sellVolume[price] -= quantity;

			if(list_map_it->second.empty()){
				sellOrders.erase(list_map_it);
				sellOrders.erase(price);
			}
		}
		else{
			spdlog::warn("No order exists with that price");
			return false;
		}
	}

	orderMap.erase(map_it);

	spdlog::info("Order id {} deleted succesfully", orderToDelete.id);

	return true;

}

int OrderBook::getVolume(OrderSide side, int price){
	if(side == BUY){
		const int vol = buyVolume[price];
		return vol;
	}
	else{
		const int vol = sellVolume[price];
		return vol;
	}

}

list<Order*> OrderBook::match(Order& order) {
	list<Order*> matchedOrders;
	int quantityLeft = order.quantity;

	if (order.orderSide == BUY) {
		if (sellOrders.empty() || sellOrders.begin()->second.empty()) {
			spdlog::info("Sell order book seems empty");
			return {};
		}

		// Check if best sell price is higher than buy price
		if (sellOrders.begin()->first > order.price) {
			spdlog::info("No sell order to match");
			return {};
		}

		for (auto book_it = sellOrders.begin(); book_it != sellOrders.end() && quantityLeft > 0; ++book_it) {
			if (book_it->first > order.price) break; // stop if price not matchable

			auto& ordersAtPrice = book_it->second;
			for (auto list_it = ordersAtPrice.begin(); list_it != ordersAtPrice.end() && quantityLeft > 0; ++list_it) {
				matchedOrders.push_back(&(*list_it));
				quantityLeft -= list_it->quantity;
			}
		}

	}
	else { 
		if (buyOrders.empty() || buyOrders.begin()->second.empty()) {
			spdlog::info("Buy order book seems empty");
			return {};
		}

		// Check if best buy price is lower than sell price
		if (buyOrders.begin()->first < order.price) {
			spdlog::info("No buy order to match");
			return {};
		}

		for (auto book_it = buyOrders.begin(); book_it != buyOrders.end() && quantityLeft > 0; ++book_it) {
			if (book_it->first < order.price) break; // stop if price not matchable

			auto& ordersAtPrice = book_it->second;
			for (auto list_it = ordersAtPrice.begin(); list_it != ordersAtPrice.end() && quantityLeft > 0; ++list_it) {
				matchedOrders.push_back(&(*list_it));
				quantityLeft -= list_it->quantity;
		    	}
		}
	}
	spdlog::info("Matched!!!");
	return matchedOrders;
}

bool OrderBook::setLastTradedPrice(int price){
	if(price >= 0){
		lastTradedPrice = price;
		return true;
	}
	else{
		spdlog::error("Cost of a asset can never be negative");
		return false;
	}
}
