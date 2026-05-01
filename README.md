# Low-Latency Trading Exchange

A low-latency trading exchange built from scratch in C++, hitting ~7ms latency.

## Architecture

- **Matching Engine** — price-time priority order book in C++, ZeroMQ/gRPC gateway, UDP multicast market data server
- **Engine Gateway** — TypeScript/gRPC gateway for order routing and client communication
- **PAS (Persistence & Analytics Server)** — ZeroMQ subscriber, RabbitMQ queuing, Prisma/PostgreSQL persistence (in progress)

## Performance

~7ms latency measured on localhost

## Stack

C++, TypeScript, ZeroMQ, gRPC, UDP Multicast, Kubernetes, RabbitMQ, Catch2
