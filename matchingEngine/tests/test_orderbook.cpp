// test_orderbook.cpp
#include "catch.hpp"
#include "../include/orderbook.hpp"
#include <chrono>

TEST_CASE("OrderBook basic functionality", "[OrderBook]") {
    OrderBook ob("AAPL");

    // Create orders
    Order o1 {1, 101, "AAPL", BUY, 150.0, 10, LIMIT, OPEN, std::chrono::system_clock::now()};
    Order o2 {2, 102, "AAPL", SELL, 155.0, 5, LIMIT, OPEN, std::chrono::system_clock::now()};
    Order o3 {3, 103, "AAPL", BUY, 150.0, 20, LIMIT, OPEN, std::chrono::system_clock::now()};

    SECTION("adding orders") {
        ob.addOrder(o1);
        ob.addOrder(o2);
        ob.addOrder(o3);

        auto got1 = ob.getOrder(1);
        REQUIRE(got1.has_value());
        REQUIRE(got1->id == 1);
        REQUIRE(got1->quantity == 10);

        auto got2 = ob.getOrder(2);
        REQUIRE(got2.has_value());
        REQUIRE(got2->price == 155.0);
    }

    SECTION("updating orders") {
        ob.addOrder(o1);
        o1.quantity = 15;
        bool updated = ob.updateOrder(o1);
        REQUIRE(updated);

        auto got1 = ob.getOrder(1);
        REQUIRE(got1.has_value());
        REQUIRE(got1->quantity == 15);
    }

    SECTION("Deleting orders") {
        ob.addOrder(o1);
        ob.addOrder(o2);

        bool deleted = ob.deleteOrder(1);
        REQUIRE(deleted);

        auto got1 = ob.getOrder(1);
        REQUIRE(!got1.has_value());
    }

    SECTION("Volume tracking") {
        ob.addOrder(o1);
        ob.addOrder(o3); // same price as o1

        int vol = ob.getVolume(BUY, 150.0);
        REQUIRE(vol == 30); // 10 + 20
    }
}

