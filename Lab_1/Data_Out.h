#pragma once

#include <stdio.h>
#include "Client.h"
#include "Frames.h"

void printClient(struct Client client)
{
	printf("Client\'s name: %s\n", client.name);
	printf("Number of orders: %d\n", client.ordersCount);

}

void printOrder(struct Order order, struct Client client)
{
	printf("Client's name: %s\n", client.name);
	printf("Order name: %s\n", order.name);
	printf("Origin: %s\n", order.origin);
	printf("Ordered: %s\n", order.dateOrdered);
	//printf("Order Id: %s\n", order.ordertId);
}