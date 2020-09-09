#include <stdio.h>
#include <iostream>
#include <string>
#include <map>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif // _WIN32
using namespace std;

enum
{
	CURR_USD = 0,
	CURR_KRW = 1,
	CURR_VND = 2,
	CURR_CNT
};

enum
{
	TYPE_CREDIT = 0,
	TYPE_DEBIT,
	TYPE_CNT
};
class Account
{
public:
	Account()
	{
		m_type = TYPE_DEBIT;
		m_currencyType = CURR_USD;
		m_balance = 0;
		m_LimitDP = 100;
		m_LimitWD = 100;
	}

	~Account()
	{

	}

	bool setType(unsigned char type)
	{
		if (type > TYPE_CNT) return false;
		m_type = type;
		return true;
	}

	unsigned char getType() { return m_type; }

	void setBalance(__int64 bal)
	{
		m_balance = bal;
	}

	__int64 getBalance()
	{
		return m_balance;
	}

	void setLimitDP(__int64 limitDP)
	{
		m_LimitDP = limitDP;
	}

	__int64 getLimitDP()
	{
		return m_LimitDP;
	}

	void setLimitWD(__int64 limitWD)
	{
		m_LimitWD = limitWD;
	}

	__int64 getLimitWD()
	{
		return m_LimitWD;
	}

	void setCurrencyType(unsigned short	currencyType)
	{
		m_currencyType = currencyType;
	}

	unsigned short getCurrencyType()
	{
		return m_currencyType;
	}

	bool deposit(__int64 amount)
	{
		if (amount <= 0)
		{
			printf("Deposit Amount should be greater than 0!");
			return false;
		}

		if (amount > m_LimitDP)
		{
			printf("Withdraw Amount exceed your limit deposit amount!");
			return false;
		}

		m_balance += amount;
		return true;
	}

	bool withdraw(__int64 amount)
	{
		if (amount <= 0) 
		{
			printf("Withdraw Amount should be greater than 0!");
			return false;
		}

		if (m_type == TYPE_DEBIT && amount > m_balance)
		{
			printf("Withdraw Amount exceed your balance!");
			return false;
		}

		if (amount > m_LimitWD)
		{
			printf("Withdraw Amount exceed your limit withdraw amount!");
			return false;
		}

		m_balance -= amount;
		return true;
	}

	void printBalance()
	{
		printf("Your balance: %lld\n", m_balance);
	}
private:
	unsigned char	m_type; //credit, debit
	__int64			m_balance;
	__int64			m_LimitWD;	// limit withdraw
	__int64			m_LimitDP;	// limit deposit
	unsigned short	m_currencyType; // 0:usd, 1:krw example
};

void requestInputMenu(int &op)
{
	printf("Please select operation:\n");
	printf("\t 1: Check Balance\n");
	printf("\t 2: Deposit\n");
	printf("\t 3: Withdraw\n");
	printf("\t 0: exit\n");
	//printf("\t 0: Turn off ATM\n");
	scanf("%d", &op);
	fflush(stdin);
}

string requestInsertCard(bool isAdd = false)
{
	char id[32];
	if (isAdd)
		printf("Please Insert Id!\n");
	else
		printf("Please Insert Card!\n");
	scanf("%s", id);
	fflush(stdin);
	return id;
}

void requestInputPIN(int &PIN)
{
	printf("Please input PIN:\n");
	scanf("%d", &PIN);
	fflush(stdin);
}

map<string, int> g_mapId;
map<string, Account*> g_mapAccount;

bool validateId(string id, int pin)
{
	map<string, int>::iterator it = g_mapId.find(id);
	if (it == g_mapId.end())
	{
		printf("Card is not existed in system\n");
		return false;
	}

	if (it->second != pin)
	{
		printf("PIN is not match\n");
		return false;
	}

	return true;
}

bool addAccount(string id, int PIN, Account *acc)
{
	if (id.length() == 0)
	{
		printf("Id invalid!\n");
		return false;
	}
	/*
	if (PIN < 100000 || PIN >=1000000 )
	{
		printf("PIN invalid!\n");
		return false;
	}
	*/

	if (acc == NULL)
	{
		return false;
	}

	if (g_mapId.find(id) != g_mapId.end())
	{
		printf("Sorry, Account Id is existed!\n");
		return false;
	}

	g_mapId[id] = PIN;
	g_mapAccount[id] = acc;

	printf("Account Id is added successfully!\n");
	return true;
}

bool continueProcess()
{
	printf("Do you want to continue?\n\tInput 1: continue, 0: exit\n");
	int con = 0;
	scanf("%d", &con);
	fflush(stdin);
	if (con == 1) return true;
	return false;
}

void sleep(int milliseconds) // Cross-platform sleep function
{
#ifdef _WIN32
	Sleep(milliseconds);
#else
	usleep(milliseconds * 1000);
#endif // _WIN32
}

void removeAllAccount()
{
	printf("Are you sure to remove all account out of system?(Y/N)");
	char c = 'N';
	scanf("%c", &c);
	fflush(stdin);
	if (c != 'Y') return;
	
	g_mapId.clear();

	map<string, Account*>::iterator it = g_mapAccount.begin();
	for (; it != g_mapAccount.end(); it++)
	{
		if (it->second) delete it->second;
	}

	g_mapAccount.clear();
}

bool requestAddAccount()
{
	printf("Do you want to add account into system?(Y/N)\n");
	char c = 'N';
	scanf("%c", &c);
	fflush(stdin);
	if (c != 'Y') return false;

	string id = requestInsertCard(true);
	int pin = 0;
	requestInputPIN(pin);
	Account *acc = new Account();

	printf("Please input card type: 0-Credit , 1-Debit\n");
	unsigned char type = TYPE_DEBIT;
	scanf("%d", &type);
	fflush(stdin);
	if (!acc->setType(type))
	{
		printf("TYPE CARD(%d) is not exist in system\n", type);
		return true;
	}

	printf("Please input limit deposit:\n");
	__int64 limit = 10000;
	scanf("%lld", &limit);
	fflush(stdin);
	acc->setLimitDP(limit);

	printf("Please input limit withdraw:\n");
	limit = 10000;
	scanf("%lld", &limit);
	fflush(stdin);
	acc->setLimitWD(limit);
	
	if (!addAccount(id, pin, acc))
	{
		printf("Add new account failed!\n");
		delete acc;
	}

	return true;
}

int main(int argc, char *argv[])
{
	Account *acc= new Account();
	acc->setType(TYPE_DEBIT);
	acc->setLimitDP(100000);
	acc->setCurrencyType(CURR_USD);
	acc->setLimitWD(10000);
	acc->setBalance(1000000);

	string id = "tan";
	int pin = 123456;
	addAccount(id, pin, acc);

	printf("=========Admin Operation==========\n");
	while (requestAddAccount())
	{

	}

	printf("===========User Operation=========\n");
	bool exit = false;
	while (true)
	{
		string id = requestInsertCard();
		if (id.compare("exit") == 0)
		{
			printf("Goodbye!\n");

			sleep(2000);
			break;
		}

		int pin = 0;
		requestInputPIN(pin);

		if (false == validateId(id, pin)) continue;
	
SELECT_OP:
		int op = 0;
		requestInputMenu(op);

		switch (op)
		{
		case 1: // balance 
			{
				g_mapAccount[id]->printBalance();
				if (continueProcess()) goto SELECT_OP;
				else break;
			}
		case 2: // Deposit0
			{
				__int64 amount = 0;

				printf("please input deposit amount:\n");
				scanf("%lld" ,&amount);
				fflush(stdin);
				if (g_mapAccount[id]->deposit(amount))
				{
					g_mapAccount[id]->printBalance();
				}
				if (continueProcess()) goto SELECT_OP;
				else break;
			}
		case 3: // Withdraw
			{
				__int64 amount = 0;
				printf("please input deposit amount:\n");
				scanf("%lld", &amount);
				fflush(stdin);
				if (g_mapAccount[id]->withdraw(amount))
				{
					g_mapAccount[id]->printBalance();
				}
				if (continueProcess()) goto SELECT_OP;
				else break;
			}
		case 0:		
			break;
		default:
			goto SELECT_OP;
		}
	}
	return 0;
}
