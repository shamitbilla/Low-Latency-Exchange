#include <chrono>
#include <fmt/format.h>
#include <string>
#include "orderEvent.hpp"
#include "orderEvent.pb.h"
#include "tradeEvent.hpp"
#include "tradeEvent.pb.h"
#include "zmq.hpp"
#include <sys/types.h>
#include <thread>
#include <spdlog/spdlog.h>
#include "engineRequest.pb.h"
#include "orderEvent.hpp"
#include "base64.hpp"
#include <fmt/core.h>
#include "protoConvertor.hpp"
#include "matchingFunctions.hpp"

using namespace std;
zmq::message_t prepareTradeMessage(tradeEvent event);
zmq::message_t prepareOrderMessage(orderEvent event);

int main() {
	int PUB_PORT = 5555;
	int PULL_PORT = 5556;

	zmq::context_t context(1);

	zmq::socket_t puller(context, zmq::socket_type::pull);
	puller.bind(fmt::format("tcp://*:{}", PULL_PORT));

	zmq::socket_t publisher(context, zmq::socket_type::pub);
	publisher.bind(fmt::format("tcp://*:{}",PUB_PORT));


	std::this_thread::sleep_for(std::chrono::seconds(2));
	spdlog::info("MatchingEngine running on port {} and {}",PUB_PORT, PULL_PORT);

	proto::engineRequest req;
	string msg;

	zmq::message_t incoming;
	string binaryData; 

	zmq::message_t identity;
	zmq::message_t empty;
	zmq::message_t request;

	while(true){

		auto result = puller.recv(incoming, zmq::recv_flags::none);
		string encodedReq(static_cast<char*>(incoming.data()), incoming.size());
		spdlog::info("Recieved push: ", encodedReq);
		
		binaryData = base64::from_base64(encodedReq); 

		if(req.ParseFromString(binaryData)){
			switch(req.action()){
				case proto::ADD_ORDER: {
					Order order = ProtoConverter::fromProtoToStruct(req.order());
					orderEvent event = addOrder(req.ticker(), order);
					zmq::message_t message = prepareOrderMessage(event);
					publisher.send(message, zmq::send_flags::none);

					if(event.event == TRADE_EXECUTABLE){
						tradeEvent trades = executeTrade(order);
						zmq::message_t message = prepareTradeMessage(trades);
						publisher.send(message, zmq::send_flags::none);
					}

					break;
				}

				case proto::UPDATE_ORDER:{
					Order order = ProtoConverter::fromProtoToStruct(req.order());
					orderEvent event = updateOrder(order.ticker, order);
					zmq::message_t message = prepareOrderMessage(event);
					publisher.send(message, zmq::send_flags::none);

					if(event.event == TRADE_EXECUTABLE){
						tradeEvent trades = executeTrade(order);
						zmq::message_t message = prepareTradeMessage(trades);
						publisher.send(message, zmq::send_flags::none);
					}

					break;
				}

				case proto::CANCEL_ORDER:{
					u_int64_t orderId = req.order_id();
					orderEvent event = deleteOrder(req.ticker(), orderId);
					zmq::message_t message = prepareOrderMessage(event);
					publisher.send(message, zmq::send_flags::none);

					break;
				}
			
			}
		}

		spdlog::info("Event Broadcasted");

	}

	return 0;
	

}

zmq::message_t prepareTradeMessage(tradeEvent event){
	proto::TradeEvent protoTradeEvent = ProtoConverter::fromStructToProto(event);
	string encodedResponse = base64::to_base64(protoTradeEvent.SerializeAsString());

	zmq::message_t message(encodedResponse.begin(), encodedResponse.end());
	return message;
}

zmq::message_t prepareOrderMessage(orderEvent event){
	proto::OrderEvent protoOrderEvent = ProtoConverter::fromStructToProto(event);
	string response = protoOrderEvent.SerializeAsString();
	string encodedResponse = base64::to_base64(response);
	zmq::message_t message(encodedResponse.begin(), encodedResponse.end());
	
	return message;
}
