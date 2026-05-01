#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <string>
#include "base64.hpp"
#include "order.pb.h"
#include "orderEvent.pb.h"
#include "tradeEvent.pb.h"
#include "zmq.hpp"
#include <iostream>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstring>

using namespace std;

int main() {
	zmq::context_t context(1);

	zmq::socket_t subscriber(context, zmq::socket_type::sub);
	subscriber.connect("tcp://localhost:5555");
	subscriber.set(zmq::sockopt::subscribe, "");

	cout << "Welcome to multicast eerver :)" << endl;

	const char* MULTICAST_IP = "239.255.0.1";
	const int PORT = 12345;

	int udp_sock = socket(AF_INET, SOCK_DGRAM, 0);

	if(udp_sock < 0){
		spdlog::error("Udp Socket doesn't exist");
		return 1;
	}

	sockaddr_in multicast_addr{};
	multicast_addr.sin_family = AF_INET;
	multicast_addr.sin_addr.s_addr = inet_addr(MULTICAST_IP);
	multicast_addr.sin_port = htons(PORT);

	cout << "Multicast Server succesfully running on " << MULTICAST_IP << ":" << PORT << endl;

	string binaryData;

	proto::OrderEvent orderEvent;
	proto::TradeEvent tradeEvent;

	string ticker, event, side;
	u_int64_t quantity;
	double price;
	
	string marketFeedMessage;
	while(true){
		zmq::message_t message;
		auto b = subscriber.recv(message, zmq::recv_flags::none);

		string msg_str(static_cast<char*>(message.data()), message.size());

		binaryData = base64::from_base64(msg_str);

		if(orderEvent.ParseFromString(binaryData)){
			ticker = orderEvent.event();
			event = proto::OrderEventType_Name(orderEvent.event());

			side = proto::OrderSide_Name(orderEvent.order().orderside());

			quantity = orderEvent.order().quantity();

			price = orderEvent.order().price();

			marketFeedMessage = fmt::format("{}|{}|{}|{}|{:.2f} \n", 
				ticker,
				event,
				side,
				quantity,
				price
			);

			cout << "inside orderEvent";

		}
		else if(tradeEvent.ParseFromString(msg_str)){
			ticker = tradeEvent.trades(0).ticker();
			event = proto::OrderEventType_Name(tradeEvent.event());
			quantity = 0;
			for(auto tx: tradeEvent.trades()){
				quantity += tx.quantity();
			}

			price = tradeEvent.trades(0).price();

			marketFeedMessage = fmt::format("{}|{}|{}|{:.2f} \n", 
				ticker,
				event,
				quantity,
				price
			);

			cout << "inside tradeEvenr";
		}
		else{
			cout << "kuch toh gadbad hai";
		}


		

		ssize_t n = sendto(udp_sock, marketFeedMessage.c_str(), marketFeedMessage.size(), 0, (struct sockaddr*)&multicast_addr, sizeof(multicast_addr));

		if(n < 0){
			spdlog::error("Error occured while sending");
		}
	}

	close(udp_sock);
	return 0;
}

