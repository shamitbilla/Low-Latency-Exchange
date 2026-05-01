
# PAS (Persistence and Analytics Server)

**Planned Architecture:**
- ZeroMQ subscriber to listen for Matching Engine broadcasts
- RabbitMQ for async queuing
- Prisma ORM + PostgreSQL for persistence
- Separate consumer for trade and order events

**Status:** Pending implementation (Gateway & Engine tested successfully)
