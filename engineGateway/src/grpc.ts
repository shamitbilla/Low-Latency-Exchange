import * as grpc from '@grpc/grpc-js';
import { EngineGatewayServiceService } from './generated/engineGateway.js';
import { engineGatewayHandler } from './engineGatewayHandler.js';

const server = new grpc.Server();

server.addService(EngineGatewayServiceService, engineGatewayHandler);

server.bindAsync('0.0.0.0:50051', grpc.ServerCredentials.createInsecure(), () => {
	console.log("GRPC server is up successfully");
});
