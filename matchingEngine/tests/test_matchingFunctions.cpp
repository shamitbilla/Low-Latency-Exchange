// test_matchingFunctions.cpp
#include "catch.hpp"
#include "../include/matchingFunctions.hpp"
#include "../include/orderbookManager.hpp"

TEST_CASE("matchingFunctions tests for AAPL", "[matchingFunctions]") {
	auto& manager = OrderBookManager::getInstance();

	// Ensure ticker is registered
	if (!manager.isTickerValid("AAPL")) {
		REQUIRE(manager.registerTicker("AAPL") == true);
	}

	auto orderBook = manager.getOrderBook("AAPL");
	// Create a sample order
	Order o1;
	o1.id = 1;
	o1.userId = 100;
	o1.ticker = "AAPL";
	o1.orderSide = BUY;
	o1.price = 150.0;
	o1.quantity = 10;
	o1.orderType = LIMIT;
	o1.orderStatus = OPEN;

	SECTION("Add Order") {
	    bool added = addOrder("AAPL", o1);
	    REQUIRE(added == true);

	    // Confirm order exists in order book
	    auto orderBook = manager.getOrderBook("AAPL");
	    REQUIRE(orderBook->getOrder(o1.id).has_value());
	}

	SECTION("Update Order") {
		orderBook->addOrder(o1);
		o1.price = 155.0;

		bool updated = updateOrder("AAPL", o1);
		REQUIRE(updated == true);

		// Confirm updated price
		auto orderBook = manager.getOrderBook("AAPL");
		auto updatedOrder = orderBook->getOrder(o1.id).value();
		REQUIRE(updatedOrder.price == 155.0);
	}

	SECTION("Delete Order") {
		orderBook->addOrder(o1);

		bool deleted = deleteOrder("AAPL", o1.id);
		REQUIRE(deleted == true);

		auto orderBook = manager.getOrderBook("AAPL");
		REQUIRE_FALSE(orderBook->getOrder(o1.id).has_value());
	}

	SECTION("Execute Trade") {
	// === Case 1: Exact fill ===
		Order buyOrder1{1, 100, "AAPL", BUY, 150.0, 10, LIMIT, OPEN};
		Order sellOrder1{2, 200, "AAPL", SELL, 150.0, 10, LIMIT, OPEN};
		std::list<Order*> orderList1 = { &sellOrder1 };

		int traded1 = executeTrade(buyOrder1, orderList1);

		REQUIRE(traded1 == 10);
		REQUIRE(buyOrder1.orderStatus == FILLED);
		REQUIRE(sellOrder1.orderStatus == FILLED);
		REQUIRE(sellOrder1.quantity == 10); // quantity unchanged since fully filled

		// === Case 2: Buyer smaller (partial fill for seller) ===
		Order buyOrder2{3, 101, "AAPL", BUY, 150.0, 5, LIMIT, OPEN};
		Order sellOrder2{4, 201, "AAPL", SELL, 150.0, 10, LIMIT, OPEN};
		std::list<Order*> orderList2 = { &sellOrder2 };

		int traded2 = executeTrade(buyOrder2, orderList2);

		REQUIRE(traded2 == 5);
		REQUIRE(buyOrder2.orderStatus == FILLED);
		REQUIRE(sellOrder2.orderStatus == PARTIALLY_FILLED);
		REQUIRE(sellOrder2.quantity == 5); // remaining 5

		// === Case 3: Buyer larger (partial fill for buyer, multiple sellers) ===
		Order buyOrder3{5, 102, "AAPL", BUY, 150.0, 25, LIMIT, OPEN};
		Order sellOrder3a{6, 202, "AAPL", SELL, 150.0, 10, LIMIT, OPEN};
		Order sellOrder3b{7, 203, "AAPL", SELL, 150.0, 10, LIMIT, OPEN};
		std::list<Order*> orderList3 = { &sellOrder3a, &sellOrder3b };

		int traded3 = executeTrade(buyOrder3, orderList3);

		REQUIRE(traded3 == 20);
		REQUIRE(buyOrder3.orderStatus == PARTIALLY_FILLED);
		REQUIRE(sellOrder3a.orderStatus == FILLED);
		REQUIRE(sellOrder3b.orderStatus == FILLED);

		// === Case 4: No sellers (no trade) ===
		Order buyOrder4{8, 104, "AAPL", BUY, 150.0, 10, LIMIT, OPEN};
		std::list<Order*> emptyList;

		int traded4 = executeTrade(buyOrder4, emptyList);

		REQUIRE(traded4 == 0);
		REQUIRE(buyOrder4.orderStatus == OPEN);
	}

}

