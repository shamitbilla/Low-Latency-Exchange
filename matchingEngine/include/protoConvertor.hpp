#pragma once

#include <chrono>
#include <vector>
#include <cstdint>

#include "order.hpp"
#include "orderEvent.hpp"
#include "tradeEvent.hpp"

#include "order.pb.h"
#include "orderEvent.pb.h"
#include "tradeEvent.pb.h"

class ProtoConverter {
public:
    // ------------------ Order ------------------
    static proto::Order fromStructToProto(const Order& o) {
        proto::Order msg;
        msg.set_id(o.id);
        msg.set_userid(o.userId);
        msg.set_ticker(o.ticker);
        msg.set_orderside(static_cast<proto::OrderSide>(o.orderSide));
        msg.set_price(o.price);
        msg.set_quantity(o.quantity);
        msg.set_ordertype(static_cast<proto::OrderType>(o.orderType));
        msg.set_orderstatus(static_cast<proto::OrderStatus>(o.orderStatus));
        msg.set_timestamp(static_cast<int64_t>(
            std::chrono::duration_cast<std::chrono::milliseconds>(
                o.timestamp.time_since_epoch()
            ).count()
        ));
        return msg;
    }

    static Order fromProtoToStruct(const proto::Order& msg) {
        Order o;
        o.id = msg.id();
        o.userId = msg.userid();
        o.ticker = msg.ticker();
        o.orderSide = static_cast<OrderSide>(msg.orderside());
        o.price = msg.price();
        o.quantity = msg.quantity();
        o.orderType = static_cast<OrderType>(msg.ordertype());
        o.orderStatus = static_cast<OrderStatus>(msg.orderstatus());
        o.timestamp = std::chrono::time_point<std::chrono::system_clock>(
            std::chrono::milliseconds(msg.timestamp())
        );
        return o;
    }

    // ------------------ OrderEvent ------------------
    static proto::OrderEvent fromStructToProto(const orderEvent& e) {
        proto::OrderEvent msg;
        msg.set_event(static_cast<proto::OrderEventType>(e.event));
        // explicitly call the Order converter and CopyFrom into the mutable field
        msg.mutable_order()->CopyFrom(ProtoConverter::fromStructToProto(e.order));
        msg.set_message(e.message);
        return msg;
    }

    static orderEvent fromProtoToStruct(const proto::OrderEvent& msg) {
        orderEvent e;
        e.event = static_cast<OrderEventType>(msg.event());
        // convert proto::Order -> Order struct
        e.order = ProtoConverter::fromProtoToStruct(msg.order());
        e.message = msg.message();
        return e;
    }

    // ------------------ TradeEvent ------------------
    static proto::TradeEvent fromStructToProto(const tradeEvent& e) {
        proto::TradeEvent msg;
        msg.set_event(static_cast<proto::OrderEventType>(e.event));
        for (const auto& t : e.trades) {
            proto::Transaction* tx = msg.add_trades();
            tx->set_ticker(t.ticker);
            tx->set_buy_order_id(t.buyOrderId);
            tx->set_sell_order_id(t.sellOrderId);
            tx->set_buyer_id(t.buyerId);
            tx->set_seller_id(t.sellerId);
            tx->set_price(t.price);
            tx->set_quantity(t.quantity);
            tx->set_timestamp(static_cast<int64_t>(
                std::chrono::duration_cast<std::chrono::milliseconds>(
                    t.timestamp.time_since_epoch()
                ).count()
            ));
        }
        return msg;
    }

    static tradeEvent fromProtoToStruct(const proto::TradeEvent& msg) {
        tradeEvent e;
        e.event = static_cast<OrderEventType>(msg.event());
        e.trades.reserve(msg.trades_size());
        for (const auto& t : msg.trades()) {
            transaction tx;
            tx.ticker = t.ticker();
            tx.buyOrderId = t.buy_order_id();
            tx.sellOrderId = t.sell_order_id();
            tx.buyerId = t.buyer_id();
            tx.sellerId = t.seller_id();
            tx.price = t.price();
            tx.quantity = t.quantity();
            tx.timestamp = std::chrono::time_point<std::chrono::system_clock>(
                std::chrono::milliseconds(t.timestamp())
            );
            e.trades.push_back(std::move(tx));
        }
        return e;
    }
};

