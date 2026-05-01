import dotenv from 'dotenv';
import z from 'zod';

dotenv.config();

const envSchema = z.object({
	GRPC_SECRET: z.string()
});

export const config = envSchema.parse(process.env);
