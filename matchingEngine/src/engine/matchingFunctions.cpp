#include "matchingFunctions.hpp"
#include "order.hpp"
#include "orderEvent.hpp"
#include "orderbook.hpp"
#include "orderbookManager.hpp"
#include "tradeEvent.hpp"
#include <absl/types/compare.h>
#include <chrono>
#include <exception>
#include <list>
#include <spdlog/spdlog.h>
#include <sys/types.h>
#include <vector>
using namespace std;


orderEvent deleteOrder(string ticker, int orderId){
	auto& manager = OrderBookManager::getInstance();
	bool isTickerValid = manager.isTickerValid(ticker);

	if(isTickerValid){
		auto orderBook = manager.getOrderBook(ticker);
		bool orderDeleted = orderBook->deleteOrder(orderId);
		Order deletedOrder;
		deletedOrder.id = orderId;
		const OrderEventType eventType = ORDER_CANCELLED;
		orderEvent event{eventType, deletedOrder, ""};
		return event;
	}
	else{
		spdlog::error("Ticker is not registered");
		Order failedOrder;
		failedOrder.id = orderId;
		const OrderEventType eventType = ORDER_CANCEL_FAILED;
		orderEvent event{eventType, failedOrder, "Ticker not registered"};
		return event;
	}

}

orderEvent updateOrder(std::string ticker, Order &order){
	auto& manager = OrderBookManager::getInstance();
	bool isTickerValid = manager.isTickerValid(ticker);

	if(isTickerValid){
		auto orderBook = manager.getOrderBook(ticker);
		bool orderUpdated = orderBook->updateOrder(order);

		if(orderUpdated){

			list<Order*> orderList = orderBook->match(order); //might be buggy
			if(!orderList.empty()){
				orderEvent event{TRADE_EXECUTABLE, order, ""};
				return event;
			}

			Order updatedOrder = order;
			const OrderEventType eventType = ORDER_UPDATED;
			orderEvent event{eventType, updatedOrder, ""};
			return event;
			
		}
		else{
			Order failedOrder;
			failedOrder.id = order.id;
			const OrderEventType eventType = ORDER_UPDATE_FAILED;
			orderEvent event{eventType, failedOrder, "Order Update failed"};
			return event;
		}
	}
	else{
		spdlog::error("Ticker is not registered");
		Order failedOrder;
		failedOrder.id = order.id;
		const OrderEventType eventType = ORDER_UPDATE_FAILED;
		orderEvent event{eventType, failedOrder, "Ticker not registered"};
		return event;
	}
}

tradeEvent executeTrade(Order &order){
	auto& manager = OrderBookManager::getInstance();
	bool isTickerValid = manager.isTickerValid(order.ticker);
	if(!isTickerValid){
		tradeEvent failed{
			TRADE_EXECUTE_FAILED,
			{}
		};

		return failed;
	}
	auto orderBook = manager.getOrderBook(order.ticker);
	list<Order*> orderList = orderBook->match(order);

	int requiredQuantity = order.quantity;
	int tradedQuantity = 0;
	vector<transaction> trades;
	u_int64_t buyerId, sellerId, buyOrderId, sellOrderId;

	for(auto it = orderList.begin(); it != orderList.end() && requiredQuantity > 0; ++it){
		Order* currentOrder = *it;
		int currentQuantity = currentOrder->quantity;
		u_int64_t quantity = 0;


		if(requiredQuantity > currentQuantity){
			requiredQuantity -= currentQuantity;
			tradedQuantity += currentQuantity;
			quantity = currentQuantity;
			currentOrder->orderStatus = FILLED;
			order.orderStatus = PARTIALLY_FILLED;
		}
		else if(requiredQuantity < currentQuantity){
			currentQuantity -= requiredQuantity;
			currentOrder->quantity = currentQuantity;
			tradedQuantity += requiredQuantity;
			quantity = requiredQuantity;
			requiredQuantity = 0;
			
			order.orderStatus = FILLED;
			currentOrder->orderStatus = PARTIALLY_FILLED;

			break;
		}
		else{
			tradedQuantity += currentQuantity;
			quantity = currentQuantity;
			order.orderStatus = FILLED;
			currentOrder->orderStatus = FILLED;

			break;
		}


		if(order.orderSide == BUY){
			buyerId = order.userId;
			buyOrderId = order.id;
			sellerId = currentOrder->userId;
			sellOrderId = currentOrder->id;
		}
		else{
			buyerId = currentOrder->userId;
			buyOrderId = currentOrder->id;
			sellerId = order.userId;
			sellOrderId = order.id;
		}

		orderBook->setLastTradedPrice(currentOrder->price);

		transaction transaction{
			order.ticker,
			buyOrderId,
			sellOrderId,
			buyerId,
			sellerId,
			currentOrder->price,
			quantity,
			chrono::system_clock::now()
		};

		trades.push_back(transaction);

	}

	tradeEvent event{
		TRADE_EXECUTED,
		trades
	};

	return event;
}

orderEvent addOrder(std::string ticker, Order &order){
	auto& manager = OrderBookManager::getInstance();
	bool isTickerValid = manager.isTickerValid(ticker);

	if(isTickerValid){
		auto orderBook = manager.getOrderBook(ticker);
		optional<Order> orderExists = orderBook->getOrder(order.id);

		if(!orderExists){
			double price = order.price;
			list<Order*> orderList = orderBook->match(order);

			if(!orderList.empty()){
				orderEvent event{TRADE_EXECUTABLE, order, ""};
				return event;
			}
			else{
				if(order.orderType == MARKET){
					spdlog::info("Cancelling Market Order since no match found");
					order.orderStatus = CANCELLED;
					Order failedOrder;
					failedOrder.id = order.id;
					const OrderEventType eventType = ORDER_ADD_FAILED;
					orderEvent event{eventType, failedOrder, "Market Order couldn't match"};
					return event;
				}
				else{
					spdlog::info("Adding order to orderbook");
					order.orderStatus = OPEN;
					try{
						orderBook->addOrder(order);
						orderEvent event{ORDER_ADDED, order, ""};
						return event;
					}
					catch(exception e){
						spdlog::error("Error occured while adding order to orderbook: {}", e.what());
						Order failedOrder;
						failedOrder.id = order.id;
						const OrderEventType eventType = ORDER_ADD_FAILED;
						orderEvent event{eventType, failedOrder, "Error Occured while adding order"};
						return event;

					}

				}
			}
		}
		else{
			spdlog::error("An Order already exists with same orderId");
			Order failedOrder;
			failedOrder.id = order.id;
			const OrderEventType eventType = ORDER_ADD_FAILED;
			orderEvent event{eventType, failedOrder, "OrderId already exists"};
			return event;
		}
	}
	else{
		spdlog::error("Ticker is not registered");
		Order failedOrder;
		failedOrder.id = order.id;
		const OrderEventType eventType = ORDER_ADD_FAILED;
		orderEvent event{eventType, failedOrder, "Ticker not registered"};
		return event;
	}

}
