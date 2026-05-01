import type { sendUnaryData, ServerUnaryCall } from "@grpc/grpc-js";
import { gatewayResponse, type addOrUpdateGatewayRequest, type EngineGatewayServiceServer } from "./generated/engineGateway.js";
import { config } from './config.js';
import z from 'zod';
import { ActionType, engineRequest } from './generated/engineRequest.js'
import { Order } from "./generated/order.js";
import { OrderEvent, OrderEventType } from "./generated/orderEvent.js";
import { TradeEvent } from "./generated/tradeEvent.js";



let pushSock: any;
let subSock: any;

const orderSchema = z.object({
	id: z.coerce.bigint(),
	userId: z.coerce.bigint(),
	ticker: z.string(),
	orderSide: z.union([z.enum(["BUY", "SELL"]), z.number()]).optional(),
	price: z.number().optional(),
	quantity: z.coerce.bigint().optional(),
	orderType: z.union([z.enum(["LIMIT", "MARKET"]), z.number()]),
	orderStatus: z.union([z.enum(["OPEN", "FILLED", "PARTIALLY_FILLED", "CANCELLED"]), z.number()]),
	timestamp: z.string()
});




export const engineGatewayHandler: EngineGatewayServiceServer = {
	addOrder: async (
		call: ServerUnaryCall<addOrUpdateGatewayRequest, gatewayResponse>,
		callback: sendUnaryData<gatewayResponse>
	) => {
		if (call.request.secret != config.GRPC_SECRET) {
			callback(null, {
				status: "REJECTED",
				message: "Unauthorized Access"
			});
		}
		else {
			const result = orderSchema.safeParse(call.request.order);

			if (result.success) {
				const order = call.request.order;

				const orderId = parseInt(order!.id);
				const ticker = order?.ticker;
				const action = ActionType.ADD_ORDER;


				const request = {
					order,
					orderId,
					ticker: order?.ticker ?? "",
					action
				};

				// console.log(request);
				const encoded = engineRequest.encode(request).finish();
				const base64Encoded = Buffer.from(encoded).toString("base64");


				await pushSock.send(base64Encoded);
				// console.log(subSock);
				const [reply] = await subSock.receive();
				let tradeReply = "";
				const base64Decoded = Buffer.from(reply.toString(), "base64");

				const decoded = OrderEvent.decode(base64Decoded);

				if (decoded.event == OrderEventType.TRADE_EXECUTABLE) {
					tradeReply = await subSock.recieve();

					const base64Decoded = Buffer.from(tradeReply.toString(), "base64");

					const decoded = TradeEvent.decode(base64Decoded);

					const response = TradeEvent.toJSON(decoded);

					// console.log(response);

				}
				else {

					const base64Decoded = Buffer.from(reply.toString(), "base64");

					const decoded = OrderEvent.decode(base64Decoded);

					const response: any = OrderEvent.toJSON(decoded);

					console.log(response);

					const status = response.event;
					const message = response.message;

					const gateWayResponse: gatewayResponse = {
						status,
						message
					};

					callback(null, gateWayResponse);
				}

			}
			else {

				console.log(result.error);

				console.log("Recieved object: ", call.request);
				callback(null, {
					status: "REJECTED",
					message: "Schema Parsing failed"
				});
			}
		}
	}

};

export function initHandler(sockets: { pushSock: any, subSock: any }) {
	pushSock = sockets.pushSock;
	subSock = sockets.subSock;
}
