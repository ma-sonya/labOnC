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
	reopenDatabase(database);								// Змінюємо режим на читання з та запис

	struct Order previous;

	fseek(database, client.firstOrderAddress, SEEK_SET);

	for (int i = 0; i < client.ordersCount; i++)		    // Пробігаємомо зв'язаний список до останньої поставки
	{
		fread(&previous, ORDER_SIZE, 1, database);
		fseek(database, previous.nextAddress, SEEK_SET);
	}

	previous.nextAddress = order.selfAddress;				// Зв'язуємо адреси
	fwrite(&previous, ORDER_SIZE, 1, database);				// Заносимо оновлений запис до файлу
}

void relinkAddresses(FILE* database, struct Order previous, struct Order order, struct Client* client)
{
	// Якщо немає попередника
	if (order.selfAddress == client->firstOrderAddress)
	{
		if (order.selfAddress == order.nextAddress)			// Лише один запис
			client->firstOrderAddress = -1;					// Неможлива адреса
		else                                                
			client->firstOrderAddress = order.nextAddress;  
	}
	// Якщо є попередник
	else
	{
		if (order.selfAddress == order.nextAddress)			// Останній запис
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
	long* delIds = (long*)malloc(garbageCount * sizeof(long));		// Виділяємо місце під список видалених адрес

	for (int i = 0; i < garbageCount; i++)
		fscanf(garbageZone, "%d", delIds + i);				

	record->selfAddress = delIds[0];						
	record->nextAddress = delIds[0];

	fclose(garbageZone);									// Очищуємо garbage_zone та записуємо нові видалені адреси
	fopen(ORDER_GARBAGE, "wb");							    
	fprintf(garbageZone, "%d", garbageCount - 1);			

	for (int i = 1; i < garbageCount; i++)
		fprintf(garbageZone, " %d", delIds[i]);				// Записуємо решту видалених адрес

	free(delIds);											// Звільняємо пам'ять
	fclose(garbageZone);									
}

void noteDeletedOrder(long address)
{
	FILE* garbageZone = fopen(ORDER_GARBAGE, "rb");			// "rb": відкриваємо бінарний файл для читання

	int garbageCount;
	fscanf(garbageZone, "%d", &garbageCount);

	long* delAddresses = (long*)malloc(garbageCount * sizeof(long)); // Виділяємо місце під видалені адреси

	for (int i = 0; i < garbageCount; i++)
		fscanf(garbageZone, "%ld", delAddresses + i);		

	fclose(garbageZone);									// Очищення файлу з видаленими адресами
	garbageZone = fopen(ORDER_GARBAGE, "wb");				
	fprintf(garbageZone, "%ld", garbageCount + 1);			// Запис нових видалених адрес

	for (int i = 0; i < garbageCount; i++)
		fprintf(garbageZone, " %ld", delAddresses[i]);		

	fprintf(garbageZone, " %d", address);					// Запис останньої видаленої адреси
	free(delAddresses);										// Звільняємо пам'ять
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
		reopenDatabase(database);								// Змінюємо режим доступу файлу
		fseek(database, order.selfAddress, SEEK_SET);			// Ставимо курсор на "сміття" для перезапису
	}
	else                                                        // Видалених немає
	{
		fseek(database, 0, SEEK_END);

		int dbSize = ftell(database);

		order.selfAddress = dbSize;
		order.nextAddress = dbSize;
	}

	fwrite(&order, ORDER_SIZE, 1, database);					// Записуємо замовлення до файлу

	if (!client.ordersCount)								    // Замовлень немає
		client.firstOrderAddress = order.selfAddress;
	else                                                        // Замовлення є
		linkAddresses(database, client, order);

	fclose(database);

	client.ordersCount++;										// Зросла кількість замовлень
	updateClient(client, error);

	return 1;
}

int getOrder(struct Client client, struct Order* order, int orderId, char* error)
{
	if (!client.ordersCount)									// В клієнта немає замовлень
	{
		strcpy(error, "This client has no orders yet");
		return 0;
	}

	FILE* database = fopen(ORDER_DATA, "rb");


	fseek(database, client.firstOrderAddress, SEEK_SET);		// Отримуємо перший запис
	fread(order, ORDER_SIZE, 1, database);

	for (int i = 0; i < client.ordersCount; i++)				// Шукаємо потрібний запис по коду замовлення
	{
		// Знайдено
		if (order->ordertId == orderId)
		{
			fclose(database);
			return 1;
		}

		fseek(database, order->nextAddress, SEEK_SET);
		fread(order, ORDER_SIZE, 1, database);
	}

	// Не знайдено
	strcpy(error, "No such order in database");
	fclose(database);
	return 0;
}

																// На вхід подано замовлення з оновленими значеннями
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

	do		                                                    // Пошук попередника запису 
	{															
		fread(&previous, ORDER_SIZE, 1, database);
		fseek(database, previous.nextAddress, SEEK_SET);
	} while (previous.nextAddress != order.selfAddress && order.selfAddress != client.firstOrderAddress);

	relinkAddresses(database, previous, order, &client);
	noteDeletedOrder(order.selfAddress);						// Заносимо адресу видаленого запису у видалені

	order.exists = 0;											

	fseek(database, order.selfAddress, SEEK_SET);				
	fwrite(&order, ORDER_SIZE, 1, database);					
	fclose(database);

	client.ordersCount--;										// Зменшення кількості замовлень
	updateClient(client, error);

	return 1;
}