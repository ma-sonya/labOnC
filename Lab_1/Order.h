#pragma once
#pragma warning(disable:4996)

#include <stdio.h>
#include <stdlib.h>
#include "Frames.h"
#include "Verify.h"
#include "Client.h"

#define ORDER_DATA "order.fl"
#define ORDER_GARBAGE "order_garbage.txt"
#define ORDER_SIZE sizeof(struct Order)

int updateClient(struct Client client, char* error);

void reopenDatabase(FILE* database)
{
	fclose(database);
	database = fopen(ORDER_DATA, "r+b");
}

void linkAddresses(FILE* database, struct Client client, struct Order order)
{
	reopenDatabase(database);								// ������� ����� �� ������� � �� �����

	struct Order previous;

	fseek(database, client.firstOrderAddress, SEEK_SET);

	for (int i = 0; i < client.ordersCount; i++)		    // ���������� ��'������ ������ �� �������� ��������
	{
		fread(&previous, ORDER_SIZE, 1, database);
		fseek(database, previous.nextAddress, SEEK_SET);
	}

	previous.nextAddress = order.selfAddress;				// ��'����� ������
	fwrite(&previous, ORDER_SIZE, 1, database);				// �������� ��������� ����� �� �����
}

void relinkAddresses(FILE* database, struct Order previous, struct Order order, struct Client* client)
{
	// ���� ���� �����������
	if (order.selfAddress == client->firstOrderAddress)
	{
		if (order.selfAddress == order.nextAddress)			// ���� ���� �����
			client->firstOrderAddress = -1;					// ��������� ������
		else                                                
			client->firstOrderAddress = order.nextAddress;  
	}
	// ���� � ����������
	else
	{
		if (order.selfAddress == order.nextAddress)			// ������� �����
		{
			previous.nextAddress = previous.selfAddress;    
		}
		else                                                
		{
			previous.nextAddress = order.nextAddress;		
		}

		fseek(database, previous.selfAddress, SEEK_SET);	
		fwrite(&previous, ORDER_SIZE, 1, database);			
	}
}

void overwriteGarbageAddress(int garbageCount, FILE* garbageZone, struct Order* record)
{
	long* delIds = (long*)malloc(garbageCount * sizeof(long));		// �������� ���� �� ������ ��������� �����

	for (int i = 0; i < garbageCount; i++)
		fscanf(garbageZone, "%d", delIds + i);				

	record->selfAddress = delIds[0];						
	record->nextAddress = delIds[0];

	fclose(garbageZone);									// ������� garbage_zone �� �������� ��� ������� ������
	fopen(ORDER_GARBAGE, "wb");							    
	fprintf(garbageZone, "%d", garbageCount - 1);			

	for (int i = 1; i < garbageCount; i++)
		fprintf(garbageZone, " %d", delIds[i]);				// �������� ����� ��������� �����

	free(delIds);											// ��������� ���'���
	fclose(garbageZone);									
}

void noteDeletedOrder(long address)
{
	FILE* garbageZone = fopen(ORDER_GARBAGE, "rb");			// "rb": ��������� ������� ���� ��� �������

	int garbageCount;
	fscanf(garbageZone, "%d", &garbageCount);

	long* delAddresses = (long*)malloc(garbageCount * sizeof(long)); // �������� ���� �� ������� ������

	for (int i = 0; i < garbageCount; i++)
		fscanf(garbageZone, "%ld", delAddresses + i);		

	fclose(garbageZone);									// �������� ����� � ���������� ��������
	garbageZone = fopen(ORDER_GARBAGE, "wb");				
	fprintf(garbageZone, "%ld", garbageCount + 1);			// ����� ����� ��������� �����

	for (int i = 0; i < garbageCount; i++)
		fprintf(garbageZone, " %ld", delAddresses[i]);		

	fprintf(garbageZone, " %d", address);					// ����� �������� �������� ������
	free(delAddresses);										// ��������� ���'���
	fclose(garbageZone);									
}

int insertOrder(struct Client client, struct Order order, char* error)
{
	order.exists = 1;

	FILE* database = fopen(ORDER_DATA, "a+b");
	FILE* garbageZone = fopen(ORDER_GARBAGE, "rb");

	int garbageCount;

	fscanf(garbageZone, "%d", &garbageCount);

	if (garbageCount)
	{
		overwriteGarbageAddress(garbageCount, garbageZone, &order);
		reopenDatabase(database);								// ������� ����� ������� �����
		fseek(database, order.selfAddress, SEEK_SET);			// ������� ������ �� "�����" ��� ����������
	}
	else                                                        // ��������� ����
	{
		fseek(database, 0, SEEK_END);

		int dbSize = ftell(database);

		order.selfAddress = dbSize;
		order.nextAddress = dbSize;
	}

	fwrite(&order, ORDER_SIZE, 1, database);					// �������� ���������� �� �����

	if (!client.ordersCount)								    // ��������� ����
		client.firstOrderAddress = order.selfAddress;
	else                                                        // ���������� �
		linkAddresses(database, client, order);

	fclose(database);

	client.ordersCount++;										// ������ ������� ���������
	updateClient(client, error);

	return 1;
}

int getOrder(struct Client client, struct Order* order, int orderId, char* error)
{
	if (!client.ordersCount)									// � �볺��� ���� ���������
	{
		strcpy(error, "This client has no orders yet");
		return 0;
	}

	FILE* database = fopen(ORDER_DATA, "rb");


	fseek(database, client.firstOrderAddress, SEEK_SET);		// �������� ������ �����
	fread(order, ORDER_SIZE, 1, database);

	for (int i = 0; i < client.ordersCount; i++)				// ������ �������� ����� �� ���� ����������
	{
		// ��������
		if (order->ordertId == orderId)
		{
			fclose(database);
			return 1;
		}

		fseek(database, order->nextAddress, SEEK_SET);
		fread(order, ORDER_SIZE, 1, database);
	}

	// �� ��������
	strcpy(error, "No such order in database");
	fclose(database);
	return 0;
}

																// �� ���� ������ ���������� � ���������� ����������
int updateOrder(struct Order order, int orderId)
{
	FILE* database = fopen(ORDER_DATA, "r+b");

	fseek(database, order.selfAddress, SEEK_SET);
	fwrite(&order, ORDER_SIZE, 1, database);
	fclose(database);

	return 1;
}

int deleteOrder(struct Client client, struct Order order, int orderId, char* error)
{
	FILE* database = fopen(ORDER_DATA, "r+b");
	struct Order previous;

	fseek(database, client.firstOrderAddress, SEEK_SET);

	do		                                                    // ����� ����������� ������ 
	{															
		fread(&previous, ORDER_SIZE, 1, database);
		fseek(database, previous.nextAddress, SEEK_SET);
	} while (previous.nextAddress != order.selfAddress && order.selfAddress != client.firstOrderAddress);

	relinkAddresses(database, previous, order, &client);
	noteDeletedOrder(order.selfAddress);						// �������� ������ ���������� ������ � �������

	order.exists = 0;											

	fseek(database, order.selfAddress, SEEK_SET);				
	fwrite(&order, ORDER_SIZE, 1, database);					
	fclose(database);

	client.ordersCount--;										// ��������� ������� ���������
	updateClient(client, error);

	return 1;
}