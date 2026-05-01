#pragma once

#include "order.hpp"
#include "orderEvent.hpp"
#include "tradeEvent.hpp"
#include <list>
#include <vector>

orderEvent addOrder(std::string ticker, Order &order);
orderEvent updateOrder(std::string ticker, Order &order);
orderEvent deleteOrder(std::string ticker, int orderId);
tradeEvent executeTrade(Order &order);
