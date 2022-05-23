#pragma once
#include <stdio.h>
#include <stdlib.h>
#include "Order.h"
#include "Client.h"

int getClient(struct Client* client, int id, char* error);

int checkFileExistence(FILE* indexTable, FILE* database, char* error)
{
	// DB files do not exist yet
	if (indexTable == NULL || database == NULL)
	{
		strcpy(error, "database files are not created yet");
		return 0;
	}

	return 1;
}

int checkIndexExistence(FILE* indexTable, char* error, int id)
{
	fseek(indexTable, 0, SEEK_END);

	long indexTableSize = ftell(indexTable);

	if (indexTableSize == 0 || id * sizeof(struct Indexer) > indexTableSize)
	{
		strcpy(error, "such ID doesn't exist");
		return 0;
	}

	return 1;
}

int checkRecordExistence(struct Indexer indexer, char* error)
{
	// Record's been removed
	if (!indexer.exists)
	{
		strcpy(error, "the record you\'re looking for doesn't exist");
		return 0;
	}

	return 1;
}


int checkKeyPairUniqueness(struct Client client, int productId)
{
	FILE* ordersDb = fopen(ORDER_DATA, "r+b");
	struct Order order;

	fseek(ordersDb, client.firstOrderAddress, SEEK_SET);

	for (int i = 0; i < client.ordersCount; i++)
	{
		fread(&order, ORDER_SIZE, 1, ordersDb);
		fclose(ordersDb);

		if (order.ordertId == productId)
		{
			return 0;
		}

		ordersDb = fopen(ORDER_DATA, "r+b");
		fseek(ordersDb, order.nextAddress, SEEK_SET);
	}

	fclose(ordersDb);

	return 1;
}

void info()
{
	FILE* indexTable = fopen("client.ind", "rb");

	if (indexTable == NULL)
	{
		printf("Error: database files are not created yet\n");
		return;
	}

	int clientCount = 0;
	int orderCount = 0;

	fseek(indexTable, 0, SEEK_END);
	int indAmount = ftell(indexTable) / sizeof(struct Indexer);

	struct Client client;

	char dummy[64];

	for (int i = 1; i <= indAmount; i++)
	{
		if (getClient(&client, i, dummy) && client.id > 0)
		{
			clientCount++;
			orderCount += client.ordersCount;

			printf("Client #%d has %d orders\n", i, client.ordersCount);
		}
	}

	fclose(indexTable);

	printf("Number of clients: %d\n", clientCount);
	printf("Number of orders: %d\n", orderCount);
}
