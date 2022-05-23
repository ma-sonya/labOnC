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
	char name[64];			//��'�
	char date[128];			//���� ����������
	int status;				//������/������
	long firstOrderAddress; //����� ������� ����������
	int ordersCount;		//ʳ������ ���������
};

struct Order
{
	int clientId;			//ID �볺���
	int ordertId;			//ID ����������
	char origin[256];		
	char name[64];			//��'�
	char dateOrdered[128];	//����, ���� ���������
	int exists;				//��������
	long selfAddress;		//������ ����������
	long nextAddress;		//������ ���������
};
