import zmq from 'zeromq';
import * as grpc from '@grpc/grpc-js';
import { EngineGatewayServiceService } from './generated/engineGateway.js';
import { engineGatewayHandler, initHandler } from './engineGatewayHandler.js';
import { config } from './config.js';


const messages = [
	"CMkBEgRBQVBMGAAiIgjJARABGgRBQVBMIAApAAAAAADwZUAwCjgAQABIgOn2uAY=",
	"CMoBEgRBQVBMGAAiIgjKARACGgRBQVBMIAEpAAAAAADgZUAwBTgAQABIiun2uAY=",
	"CMsBEgRBQVBMGAAiIgjLARADGgRBQVBMIAEpAAAAAAAAAAAwCjgBQABIlOn2uAY=",
	"CMwBEgRHT09HGAAiIgjMARAEGgRHT09HIAApAAAAAADqpUAwCDgAQABInun2uAY=",
	"CM0BEgRHT09HGAAiIgjNARAFGgRHT09HIAEpAAAAAADgpUAwBTgAQABIqOn2uAY=",
	"CM4BEgRHT09HGAAiIgjOARAGGgRHT09HIAEpAAAAAADqpUAwAzgAQABIsun2uAY=",
	"CM8BEgRNU0ZUGAAiIgjPARAHGgRNU0ZUIAApAAAAAAAAAAAwFDgBQABIvOn2uAY=",
	"CNABEgRNU0ZUGAAiIgjQARAIGgRNU0ZUIAEpAAAAAADAckAwDzgAQABIxun2uAY=",
	"CNEBEgRNU0ZUGAAiIgjRARAJGgRNU0ZUIAEpAAAAAAAQc0AwBTgAQABI0On2uAY=",
	"CNIBEgRBQVBMGAAiIgjSARAKGgRBQVBMIAApAAAAAAAAZkAwCjgAQABI2un2uAY="
];

const pushSock = new zmq.Push();
await pushSock.connect("tcp://127.0.0.1:5556");

const subSock = new zmq.Subscriber();
await subSock.connect("tcp://127.0.0.1:5555");

subSock.subscribe("");

console.log("Gateway connected to engine succesfully");


initHandler({
	pushSock,
	subSock
});

const server = new grpc.Server();

server.addService(EngineGatewayServiceService, engineGatewayHandler);

server.bindAsync('0.0.0.0:50051', grpc.ServerCredentials.createInsecure(), () => {
	console.log("GRPC server is up successfully");
});


// async function main() {
// 	console.log("Gateway started");
//
// 	const pushSock = new zmq.Push();
// 	await pushSock.connect("tcp://127.0.0.1:5556");
//
// 	const subSock = new zmq.Subscriber();
// 	await subSock.connect("tcp://127.0.0.1:5555");
//
// 	subSock.subscribe("");
//
// 	console.log("Gateway connected to engine succesfully");
//
// 	await pushSock.send(messages[0]!);
//
// 	const [reply] = await subSock.receive();
//
// 	console.log("Recieved Broadcast", reply?.toString());
//
// 	await subSock.close();
// 	await pushSock.close();
// }

// main();


