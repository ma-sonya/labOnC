#include <stdio.h>
#include "Frames.h"
#include "Client.h"
#include "Order.h"
#include "Data_In.h"
#include "Data_Out.h"


int main()
{
	struct Client client;
	struct Order order;

	while (1)
	{
		int id;
		int key;
		char error[64];

		printf("\t   Choose option:\n\t0 - Quit");
		printf("\n\t1 - Insert client");
		printf("\n\t2 - Get client");
		printf("\n\t3 - Update client");
		printf("\n\t4 - Delete client");
		printf("\n\t5 - Insert order");
		printf("\n\t6 - Get order");
		printf("\n\t7 - Update order");
		printf("\n\t8 - Delete order");
		printf("\n\t9 - Info\n\n");

		printf(">> ");
		scanf("%d", &key);
		printf("\n");

		switch (key) {
		case 0:
			return 0;
		case 1:
			readClient(&client);
			insertClient(client);
			break;
		case 2:
			printf("Enter ID: ");
			scanf("%d", &id);			
			if (getClient(&client, id, error))
			{
				printClient(client);
			}
			else
			{
				printf("Error: %s\n", error);
			}
			break;
		case 3:
			printf("Enter ID: ");
			scanf("%d", &id);
			client.id = id;

			readClient(&client);
			updateClient(client, error) ? printf("Updated successfully\n") : printf("Error: %s\n", error);
			break;
		case 4:
			printf("Enter ID: ");
			scanf("%d", &id);
			deleteClient(id, error) ? printf("Deleted successfully\n") : printf("Error: %s\n", error);
			break;
		case 5:
			printf("Enter client\'s ID: ");
			scanf("%d", &id);
			if (getClient(&client, id, error))
			{
				order.clientId = id;
				printf("Enter order ID: ");
				scanf("%d", &id);
				order.ordertId = id;

				readOrder(&order);
				insertOrder(client, order, error);
				printf("Inserted successfully. To access, use client\'s and order\'s IDs\n");
			}
			else 
			{
				printf("Error: %s\n", error);
			}				
			break;
		case 6:
			printf("Enter client\'s ID: ");
			scanf("%d", &id);

			if (getClient(&client, id, error))
			{
				printf("Enter order ID: ");
				scanf("%d", &id);				
				if (getOrder(client, &order, id, error))
				{
					printOrder(order, client);
				}
				else
				{
					printf("Error: %s\n", error);
				}
			}
			else
			{
				printf("Error: %s\n", error);
			}
			break;
		case 7:
			printf("Enter client\'s ID: ");
			scanf("%d", &id);

			if (getClient(&client, id, error))
			{
				printf("Enter order ID: ");
				scanf("%d", &id);

				if (getOrder(client, &order, id, error))
				{
					readOrder(&order);
					updateOrder(order, id);
					printf("Updated successfully\n");
				}
				else
					printf("Error: %s\n", error);
			}
			else
				printf("Error: %s\n", error);
			break;
		case 8:
			printf("Enter Client\'s ID: ");
			scanf("%d", &id);

			if (getClient(&client, id, error))
			{
				printf("Enter order ID: ");
				scanf("%d", &id);

				if (getOrder(client, &order, id, error))
				{
					deleteOrder(client, order, id, error);
					printf("Deleted successfully\n");
				}
				else
					printf("Error: %s\n", error);
			}
			else
				printf("Error: %s\n", error);
			break;
		case 9:
			info();
			break;

		default:
			printf("Invalid input, please try again\n");
		}

		printf("\n----------------------\n\n");
	}

	return 0;
}
