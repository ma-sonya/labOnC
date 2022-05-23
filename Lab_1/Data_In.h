#pragma once
#include <stdio.h>
#include <string.h>
#include "Frames.h"
#include "Client.h"

void readClient(struct Client* client)
{
	char name[32];
	int status;

	name[0] = '\0';

	printf("Enter client\'s name: ");
	scanf("%s", name);
	strcpy(client->name, name);

	//printf("Enter client\'s status: ");
	//scanf("%d", &status);
	client->status = 1;
	//client->status = status;
	client->ordersCount = 0;
}

void readOrder(struct Order* order)
{

	char origin[256];
	char name[64];
	char dateOrdered[128];
	name[0] = dateOrdered[0] = origin[0] = '\0';

	printf("Enter order\'s name: ");
	scanf("%s", name);
	strcpy(order->name, name);

	printf("Enter order\'s origin: ");
	scanf("%s", origin);
	strcpy(order->origin, origin);

	printf("Date ordered: ");
	scanf("%s", dateOrdered);
	strcpy(order->dateOrdered, dateOrdered);

}