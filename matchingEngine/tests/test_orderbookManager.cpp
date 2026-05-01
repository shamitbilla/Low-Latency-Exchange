#include "catch.hpp"
#include "../include/orderbookManager.hpp"

// Test 1: Singleton property
TEST_CASE("OrderBookManager follows Singleton pattern", "[singleton]") {
    auto& manager1 = OrderBookManager::getInstance();
    auto& manager2 = OrderBookManager::getInstance();

    REQUIRE(&manager1 == &manager2); // both must refer to the same instance
}

// Test 2: Registering tickers
TEST_CASE("Registering tickers works correctly", "[registerTicker]") {
    auto& manager = OrderBookManager::getInstance();

    // first registration should succeed
    bool first = manager.registerTicker("AAPL");
    REQUIRE(first == true);

    // second registration of same ticker should fail
    bool second = manager.registerTicker("AAPL");
    REQUIRE(second == false);
}

// Test 3: Getting order books
TEST_CASE("getOrderBook returns valid and consistent order books", "[getOrderBook]") {
    auto& manager = OrderBookManager::getInstance();

    manager.registerTicker("GOOG");
    auto ob1 = manager.getOrderBook("GOOG");
    REQUIRE(ob1 != nullptr);

    // same ticker → same shared_ptr
    auto ob2 = manager.getOrderBook("GOOG");
    REQUIRE(ob1 == ob2);

    // different ticker → different order book instance
    manager.registerTicker("MSFT");
    auto ob3 = manager.getOrderBook("MSFT");
    REQUIRE(ob3 != nullptr);
    REQUIRE(ob3 != ob1);
}

// Test 4: getOrderBook auto-creation behavior (if applicable)
TEST_CASE("getOrderBook handles unregistered ticker safely", "[getOrderBook]") {
    auto& manager = OrderBookManager::getInstance();

    // depending on your implementation, it may auto-register or return nullptr
    auto ob = manager.getOrderBook("TSLA");
    REQUIRE(ob == nullptr);
}
