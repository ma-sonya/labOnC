#pragma once

struct Indexer
{
	int id;
	int address;
	int exists;
};

struct Client
{
	int id;					//ID
	char name[64];			//Ім'я
	char date[128];			//Дата відвідування
	int status;				//Онлайн/офлайн
	long firstOrderAddress; //Номер першого замовлення
	int ordersCount;		//Кількість замовлень
};

struct Order
{
	int clientId;			//ID Клієнта
	int ordertId;			//ID Замовлення
	char origin[256];		
	char name[64];			//Ім'я
	char dateOrdered[128];	//Дата, коли замовлено
	int exists;				//Наявність
	long selfAddress;		//Адреса замовлення
	long nextAddress;		//Адреса замовника
};
