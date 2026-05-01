import * as zmq from "zeromq";

async function startGateway() {
	console.log("🚀 Gateway starting...");

	// PUSH socket: send orders to engine
	const pushSock = new zmq.Push();
	await pushSock.connect("tcp://127.0.0.1:5556");

	// SUB socket: receive broadcasts from engine
	const subSock = new zmq.Subscriber();
	await subSock.connect("tcp://127.0.0.1:5555");
	subSock.subscribe("");

	console.log("✅ Gateway connected to engine");

	// Send message
	const msg = "Hola from gateway";
	console.log("📤 Sending:", msg);
	await pushSock.send(msg);

	// Wait for only 1 broadcast
	const [reply] = await subSock.receive();
	console.log("📥 Received broadcast:", reply?.toString());

	// Close sockets after receiving one
	await subSock.close();
	await pushSock.close();

	console.log("✅ Gateway finished and closed sockets");
}

startGateway();
