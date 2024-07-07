#include <iostream>
#include <string>
#include <fstream>
#include <ctime>
#include <cmath>
#include <cstdint>
#include <conio.h> 
#include <windows.h>

#include "FileData.h"
#include "BTree.h"
#include "SHA1.h"
#include "RoutingTable.h"


using namespace std;

class Machine
{
public:
	int ID;
	Machine* next;
	int FTPSpace;
	RoutingTable* Ftp;
	BTree<FileData>* btree;

	Machine(long long id, int RoutingSpace, int orderBtree)
	{
		this->ID = id;
		this->next = NULL;
		this->FTPSpace = RoutingSpace;
		this->btree = new BTree<FileData>(orderBtree);
		this->Ftp = new RoutingTable;
	}
	void showFTp()
	{
		cout << "Routing Table of Machine " << this->ID << " :- " << endl;
		RoutingTable* current = Ftp;
		int i = 0;
		while (current != NULL)
		{
			cout << i << "     " << current->current->ID << endl;
			current = current->next;
			i++;
		}
		return;
	}
};


class RingDistributedHashTable
{
private:
	Machine* MachinesHead;
	Machine* MachineEnd;
	Machine** AllMachines;

	int IdentifierSpace;
	long long MaxNumberOfMachines;
	int MachinesActive;
	int OrderMofBtree;
	SHA1 checksum;

public:
	RingDistributedHashTable()
	{

		this->MachinesHead = NULL;
		this->MachineEnd = NULL;
		this->AllMachines = NULL;
		this->MaxNumberOfMachines = 0;
		this->IdentifierSpace = 0;
		this->MachinesActive = 0;
		this->OrderMofBtree = 0;


	}

	void displayLogo()
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE);

		cout << "\t\t" << "" << endl;
		cout << "\t\t\t\t\t" << "||||||||||  |||||||||  |||||||||  ||||||||" << endl;
		cout << "\t\t\t\t\t" << "    ||      ||     ||  ||         ||      " << endl;
		cout << "\t\t\t\t\t" << "    ||      ||     ||  ||         ||      " << endl;
		cout << "\t\t\t\t\t" << "    ||      |||||||||  ||||||     ||||||||" << endl;
		cout << "\t\t\t\t\t" << "    ||      ||         ||               ||" << endl;
		cout << "\t\t\t\t\t" << "    ||      ||         ||               ||" << endl;
		cout << "\t\t\t\t\t" << "||||||||||  ||         ||         ||||||||" << endl;

		cout << "\t\t\t\t\t  Welcome to InterPlanetary File System" << endl;

		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	}

	void initalize()
	{
		char choice;
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN);
		cout << "\n\n";
		cout << "\t\t\t\t\t  Restore Prvious Server State (Y/N): ";
		cin >> choice;
		if (choice == 'Y' || choice == 'y')
		{
			this->RestoreServerState();
			return;
		}
		system("cls");
		displayLogo();
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN);
		cout << "\n\n\n";
		cout << "Please Enter Basic Information to Start the Server\n" << endl;

		//Taking input on size of identifer Space from user in bits
		while (this->IdentifierSpace <= 1 || this->IdentifierSpace >= 160)
		{
			cout << "Enter the size of Identifer Space in Bits : ";
			cin >> this->IdentifierSpace;
			if (this->IdentifierSpace <= 0 || this->IdentifierSpace >= 160)
				cout << "Please! Enter Valid Amount :)" << endl;
		}
		cout << endl;

		//Calculating the Max Number of Machines a Server can hold on basis of Identifer Space [2 ^ IdentifierSpace]
		long long Power = 2;
		for (int i = 1; i < this->IdentifierSpace; ++i)
		{
			Power *= 2;
		}
		this->MaxNumberOfMachines = Power;

		//Taking Input on Total No. of Machines Active That holds [Key,Value] Pairs at the Moment
		while (this->MachinesActive <= 0 || this->MachinesActive > MaxNumberOfMachines)
		{
			cout << "Enter Total Number of Machines : ";
			cin >> this->MachinesActive;
			if (this->MachinesActive <= 1 || this->MachinesActive > MaxNumberOfMachines)
				cout << "Please! Enter a Valid Amount" << endl;
		}
		cout << endl;

		this->AllMachines = new Machine * [MachinesActive] {NULL};

		cout << "Enter the Order M of Balanced Tree : ";
		cin >> this->OrderMofBtree;

		cout << endl;

		cin.ignore();
		string Choice = "";

		cout << "Assign Machine ID Manually(Y/N) : ";
		getline(cin, Choice);
		cout << endl;

		long long* IDs = new long long[MachinesActive] {0};
		if (Choice == "Y" || Choice == "yes" || Choice == "YES" || Choice == "y" || Choice == "Yes")
		{
			for (int i = 0; i < MachinesActive; ++i)
			{
				cout << "Enter Machine " << i + 1 << " ID : (0/" << this->MaxNumberOfMachines - 1 << ")  : ";
				cin >> IDs[i];



				string id = intToString(IDs[i]);   // Hashing of Machine ID

				checksum.update(id);
				const string hash = checksum.final();

				long long e = hashing(hash);
				IDs[i] = e;

				if (IDs[i] < 0 || IDs[i] >= this->MaxNumberOfMachines)   //Making Sure that Machine ID is with in Range
				{
					cout << "Please! Enter with In Range :)" << endl;
					i--;
				}
				for (int j = 0; j < i; ++j)                            //Duplicate Machine IDs are Not Allowed
				{
					if (IDs[i] == IDs[j])
					{
						cout << "ID Already Used Please Enter Some Unique IDs" << endl;
						i--;
						break;
					}
				}
				cout << endl;
			}
		}
		else
		{
			srand(time(0));
			for (int i = 0; i < MachinesActive; ++i)
			{
				IDs[i] = rand() % this->MaxNumberOfMachines;   //Random Machine ID locating
				for (int j = 0; j < i; ++j)
				{
					if (IDs[i] == IDs[j])                     //Duplicate Machine IDs are Not Allowed
					{
						i--;
						break;
					}
				}
			}
		}

		//Allocating Machines in Sorted Manner according to ID
		for (int i = 0; i < this->MachinesActive; ++i)
		{
			this->InsertMachine(IDs[i]);
		}

		this->AdjustRoutingTable();

		return;
	}

	void InitiateDeleteRequest()
	{
		cout << "\n_________________________INITIATING DELETE KEY REQUEST________________________________\n\n";
		long long Machineid = 0;
		Machine* currentMachine = NULL;
		this->DisplayAvaliableMachinesIDs();      //Display avaliable Machine to Ask user to pass an insert request to any machine in DHT
		while (currentMachine == NULL)
		{
			cout << "Enter Machine Number : ";
			cin >> Machineid;
			if (Machineid > MachinesActive || Machineid <= 0)
			{
				cout << "Please Enter a Valid Machine ID" << endl;
				continue;
			}
			currentMachine = AllMachines[Machineid - 1];
		}

		//Start the Delete Process 

		long long e;
		cout << "Enter Key : ";
		cin >> e;

		FileData deleteData;
		deleteData.key = e;

		cout << "Hashed Value Generated = " << e << endl;
		Machine* searchedMachine = this->SearchMachine2(e, currentMachine);

		searchedMachine->btree->remove(deleteData, true);

		cout << "\n\nB-Tree After Deletion : " << endl;
		searchedMachine->btree->print();
		cout << "____________________________________EXITING____________________________________________\n\n";
	}

	void InitiateSearchRequest()
	{
		cout << "\n_________________________INITIATING SEARCH REQUEST____________________________________\n\n";
		int choice = 0;
		cout << "Search By : " << endl;
		cout << "1 : File Data" << endl;
		cout << "2 : File Path" << endl;
		cin >> choice;
		if (choice == 1)
		{
			this->SearchRequestByFileData();
		}
		else if (choice == 2)
		{
			this->SearchRequestByFilePath();
		}
		cout << "____________________________________EXITING___________________________________________\n\n";
		cout << "Press any key to Continue";
		_getch();

	}

	void InitiateInsertRequest()
	{
		cout << "\n\n_________________________INITIATING INSERT FILE REQUEST_______________________________\n";
		//    C\Documents\FoxData.txt
		//    1 4 9 11 14 18 20 21 28
		//First show Machines the server have the user may select any of them
		string value = "";
		ifstream fileRead;
		cout << "\nEnter the Directory of File : ";
		cin.ignore();
		getline(cin, value);

		fileRead.open(value);

		if (!fileRead)
		{
			cout << "\n---------------------ERROR------------------------" << endl;
			cout << "File Does not Exist or The Given Path is Wrong" << endl;
			cout << "------------------REQUEST FAILED--------------------" << endl;
			cout << "____________________________________EXITING____________________________________________\n\n";
			return;
		}
		string Data = GetFileData(fileRead);
		fileRead.close();
		if (Data == "")          //check if not data is fetched from file or file is empty
		{
			cout << "\n---------------------ERROR------------------------" << endl;
			cout << "File Does not Contain any Data or File may be Corrupted" << endl;
			cout << "------------------REQUEST FAILED--------------------" << endl;
			cout << "____________________________________EXITING____________________________________________\n\n";
			return;
		}
		cout << endl;
		long long machineID = 0;
		Machine* currentMachine = NULL;
		this->DisplayAvaliableMachinesIDs();      //Display avaliable Machine to Ask user to pass an insert request to any machine in DHT
		while (currentMachine == NULL)
		{
			cout << "Enter Machine Number : ";
			cin >> machineID;
			if (machineID > MachinesActive || machineID <= 0)
			{
				cout << "Please Enter a Valid Machine ID" << endl;
				continue;
			}
			currentMachine = AllMachines[machineID - 1];
		}

		//Start the Insert Process
		system("cls");
		displayLogo();
		cout << "\n";

		long long e;

		checksum.update(Data);
		const string hash = checksum.final();

		e = hashing(hash);      //Passing the orignal data of file to be hashed and return key according to data instead of file name


		FileData filedata(value);
		filedata.key = e;

		cout << "\n=========================================" << endl;
		cout << "Hashed Value Generated = " << e << endl;
		cout << "=========================================" << endl;
		Machine* searchedMachine = NULL;

		searchedMachine = this->SearchMachine2(e, currentMachine);      //Searching Machine O(logn)
		searchedMachine->btree->insert(filedata);

		cout << "\n\nDo you want to see the Data in the file? (Y/N)" << endl;
		char dataYes;
		cin >> dataYes;
		if (dataYes == 'Y' || dataYes == 'y')
		{
			system("cls");
			displayLogo();
			cout << "\n\n";
			cout << "========================" << endl;
			cout << "Data the File Contain :" << endl;
			cout << "========================" << endl;
			cout << "____________________________________________________________________________________________________________" << endl;
			cout << Data << endl;
			cout << "____________________________________________________________________________________________________________" << endl;
			cout << "\n\n========================" << endl;
			cout << "B-Tree After Insertion : " << endl;
			cout << "========================" << endl;
			searchedMachine->btree->print();

		}
		cout << "Press any Key to Continue";
		_getch();
	}

	void InitiateInsertMachineRequest()
	{

		cout << "\n_________________________INITIATING ADD MACHINE REQUEST_______________________________\n\n";
		bool check = false;
		Machine* tempMachine = NULL;
		long long Machineid = -1;
		this->DisplayAvaliableMachinesIDs();
		while (!check)                            //Taking new Machine ID from user
		{
			cout << "Enter Machine ID : ";
			cin >> Machineid;


			string id = intToString(Machineid);   // Hashing of Machine ID

			checksum.update(id);
			const string hash = checksum.final();

			long long e = hashing(hash);

			Machineid = e;

			tempMachine = this->searchMachineByID(Machineid);
			if (tempMachine == NULL)
			{
				check = true;
			}
			else
			{
				cout << "Machine Already Exists! Please Try Again" << endl;
			}
		}
		if (this->MachinesActive + 1 >= this->MaxNumberOfMachines)
		{
			cout << "\n---------------------ERROR------------------------" << endl;
			cout << "Max Machines Already Exits" << endl;
			cout << "------------------REQUEST FAILED--------------------" << endl;
			cout << "Press any Key to Continue";
			_getch();
			return;
		}
		this->MachinesActive += 1;
		Machine* newMachine = this->InsertMachine(Machineid);
		this->AdjustRoutingTable();

		Machine* machineP_1 = this->MachinesHead;
		while (machineP_1->next->ID != newMachine->ID)     //Finding P-1 machine ID
		{
			machineP_1 = machineP_1->next;
		}
		Machine* nextMachine = newMachine->next;          //P+1 Machine is new Machine next Machine

		long long startID = newMachine->ID;
		long long endID = machineP_1->ID;

		for (startID; startID > endID; --startID)       //Removing the Data that has key less then equal to to new Machine from P+1 and inserting into new Machine
		{
			if (startID < 0)
			{
				startID = this->MaxNumberOfMachines - 1;
			}
			FileData newData = nextMachine->btree->search(startID);
			if (newData.key != -1)
			{
				cout << "\nRemoving Key " << newData.key << " With File Path " << newData.filePath << " Form Machine with ID [" << nextMachine->ID << "]" << endl;
				cout << "Inserting Key " << startID << " To Machine with ID [" << newMachine->ID << "]" << endl;
				nextMachine->btree->remove(newData, false);
				newMachine->btree->insert(newData);
			}
		}

		cout << "___________________________________EXITING_____________________________________________\n\n";
		cout << "Press any Key to Continue";
		_getch();
		return;
	}

	void InitiateDeleteMachineRequest()
	{

		cout << "\n_________________________INITIATING REMOVE MACHINE REQUEST____________________________\n\n";
		bool check = false;
		Machine* tempMachine = NULL;
		long long Machineid = -1;
		this->DisplayAvaliableMachinesIDs();
		while (!check)                            //Taking new Machine ID from user
		{
			cout << "Enter Machine ID : ";
			cin >> Machineid;


			tempMachine = this->searchMachineByID(Machineid);
			if (tempMachine == NULL)
			{
				cout << "Machine By this ID Does Not Exists! Please Try Again" << endl;
			}
			else
			{
				check = true;
			}
		}
		if (this->MachinesActive - 1 <= 1)
		{
			cout << "\n---------------------ERROR------------------------" << endl;
			cout << "Cannot Remove Machine! Minimum Limit Reached" << endl;
			cout << "------------------REQUEST FAILED--------------------" << endl;
			cout << "Press any key to Continue";
			_getch();
			return;
		}
		Machine* removedMachine = this->RemoveMachine(Machineid);
		this->MachinesActive -= 1;
		this->AdjustRoutingTable();

		Machine* machineP_1 = this->MachinesHead;
		while (machineP_1->next->ID < removedMachine->ID)
		{
			machineP_1 = machineP_1->next;
			if (machineP_1->next->ID == this->MachinesHead->ID)
				break;
		}
		cout << endl;
		this->DisplayAvaliableMachinesIDs();

		long long startID = removedMachine->ID;
		long long endID = machineP_1->ID;


		for (startID; startID != endID; --startID)       //Removing the Data that has key less then equal to to new Machine from P+1 and inserting into new Machine
		{
			if (startID < 0)
			{
				startID = this->MaxNumberOfMachines - 1;
			}
			FileData newData = removedMachine->btree->search(startID);
			if (newData.key != -1)
			{
				ifstream fileRead;
				fileRead.open(newData.filePath);
				string Data = GetFileData(fileRead);
				fileRead.close();

				long long e;

				checksum.update(Data);
				const string hash = checksum.final();

				e = Rehashing(hash);      //Passing the orignal data of file to be hashed and return key according to data instead of file name

				cout << "\nRemoving Key (" << newData.key << ") With File Path (" << newData.filePath << ") Form Machine with ID [" << removedMachine->ID << "]" << endl;


				cout << "\nReHashed Value Generated = " << e << endl;


				newData.key = e;


				Machine* searchedMachine = NULL;
				searchedMachine = this->SearchMachine2(e, this->MachinesHead);
				cout << "\nInserting Key (" << startID << ") To Machine with ID [" << searchedMachine->ID << "]" << endl;

				searchedMachine->btree->insert(newData);
			}
		}

		delete removedMachine;
		cout << "_____________________________________EXITING____________________________________________\n\n";
		cout << "Press any key to Continue";
		_getch();

	}

	void ShowBTree()
	{
		cout << "\n______________________________________________________________________________________\n\n";
		cout << endl;
		long long machineID = 0;
		Machine* currentMachine = NULL;
		this->DisplayAvaliableMachinesIDs();      //Display avaliable Machine to Ask user to pass an insert request to any machine in DHT
		while (currentMachine == NULL)
		{
			cout << "Enter Machine Number : ";
			cin >> machineID;
			if (machineID > MachinesActive || machineID <= 0)
			{
				cout << "Please Enter a Valid Machine ID" << endl;
				continue;
			}
			currentMachine = AllMachines[machineID - 1];
		}
		cout << "\nB-Tree of Machine [" << currentMachine->ID << "]\n" << endl;
		currentMachine->btree->printBTree(currentMachine->btree->root);
		long long choice = 1;
		cout << "\n\nPrint Any Key Data OR Enter [-1] to Exit" << endl;
		while (choice != -1)
		{
			cout << "Enter Key : ";
			cin >> choice;
			FileData filedata = currentMachine->btree->search(choice);

			if (filedata.key != -1)
			{
				cout << "\n------------------------------------------------\n" << endl;
				cout << "File Key  : " << filedata.key << endl;
				cout << endl;

				filedata.pathList.print();
				cout << "---------------------------------------------------" << endl;
			}
			else
			{
				cout << "Invalid Key!" << endl;
			}
		}

		cout << "_______________________________________________________________________________________\n\n";
		cout << "Press any key to Continue";
		_getch();
	}

	void ShowRoutingTable()
	{
		cout << "\n______________________________________________________________________________________\n\n";
		long long machineID = 0;
		Machine* currentMachine = NULL;
		this->DisplayAvaliableMachinesIDs();      //Display avaliable Machine to Ask user to pass an insert request to any machine in DHT
		while (currentMachine == NULL)
		{
			cout << "Enter Machine Number : ";
			cin >> machineID;
			if (machineID > MachinesActive || machineID <= 0)
			{
				cout << "Please Enter a Valid Machine ID" << endl;
				continue;
			}
			currentMachine = AllMachines[machineID - 1];
		}
		currentMachine->showFTp();
		cout << "_______________________________________________________________________________________\n\n";
		cout << "Press any key to Continue";
		_getch();
	}

	~RingDistributedHashTable()
	{
		cout << "______________________________SERVER CLOSING______________________________" << endl;

		//Storing The State of Server when server Shut Down

		ofstream filewrite;

		filewrite.open("ServerPreviousData.txt");

		filewrite << this->intToString(this->IdentifierSpace) << endl;
		filewrite << this->intToString(this->MaxNumberOfMachines) << endl;
		filewrite << this->intToString(this->MachinesActive) << endl;
		filewrite << this->intToString(this->OrderMofBtree) << endl;
		filewrite << endl;
		for (int i = 0; i < this->MachinesActive; ++i)
		{
			long long Id = AllMachines[i]->ID;
			filewrite << this->intToString(Id) << endl;

		}
		Machine* tempMachine = NULL;
		FileData tempData;
		filewrite << endl;
		for (long long i = 0; i < this->MaxNumberOfMachines; ++i)
		{
			cout << i << endl;
			tempMachine = this->SearchMachine2(i, this->MachinesHead);
			tempData = tempMachine->btree->search(i);

			if (tempData.key != -1)
			{
				string PathKeyData = "";
				PathNode* pathNode = tempData.pathList.root;    //Checking if the key may contain mutiple paths
				while (1)
				{
					PathKeyData += this->intToString(tempData.key);
					PathKeyData += " ";
					PathKeyData += pathNode->filePath;

					filewrite << PathKeyData << endl;

					if (pathNode->next == NULL)          //If No Linked List then Exit
					{
						break;
					}
					pathNode = pathNode->next;
					PathKeyData = "";

				}
			}

		}

		filewrite.close();

		//DeAllocating All Memory Used By the Server
		Machine* current = this->MachinesHead;
		delete[] AllMachines;
		for (int i = 0; i < this->MachinesActive; ++i)          //i Run Untill Machine Active in DHT
		{
			RoutingTable* currentFtp = current->Ftp;

			while (currentFtp != NULL)       //De Allocating Previous FTp Table
			{
				RoutingTable* temp = currentFtp;
				currentFtp = currentFtp->next;
				delete temp;
			}
			current->Ftp = new RoutingTable;
			Machine* temp = current;
			current = current->next;
			delete temp;
		}
		cout << "______________________________SERVER SHUT DOWN SUCCESSFUL______________________________" << endl;
	}

private:

	void RestoreServerState()
	{
		ifstream fileRead;
		fileRead.open("ServerPreviousData.txt");

		int lines = 0;
		int MachineEndLine = 5;
		int i = 0;
		long long* IDs = new long long[1];

		string Data;


		while (getline(fileRead, Data))
		{
			if (lines == 0)
			{
				this->IdentifierSpace = stoi(Data);
			}
			else if (lines == 1)
			{
				this->MaxNumberOfMachines = stoi(Data);
			}
			else if (lines == 2)
			{
				this->MachinesActive = stoi(Data);
				IDs = new long long[this->MachinesActive];
			}
			else if (lines == 3)
			{
				this->OrderMofBtree = stoi(Data);
				MachineEndLine += this->MachinesActive;
			}
			else if (lines >= 5 && lines < MachineEndLine)
			{
				IDs[i] = stoi(Data);
				i++;
			}
			else if (lines == MachineEndLine)
			{
				for (int i = 0; i < this->MachinesActive; ++i)
				{
					this->InsertMachine(IDs[i]);
				}
				this->AdjustRoutingTable();
			}
			else if (lines > MachineEndLine)
			{
				string key = "";
				string path = "";
				int i = 0;
				for (i = 0; Data[i] != ' '; ++i)
				{
					key += Data[i];
				}
				key += '\0';
				for (i = i + 1; Data[i] != '\0'; ++i)
				{
					path += Data[i];
				}
				path += '\0';
				cout << key << "   " << path << endl;

				FileData filedata(path);
				filedata.key = stoi(key);

				ifstream file;
				file.open(filedata.pathList.root->filePath);

				if (file)
				{
					Machine* searchedMachine = this->SearchMachine2(filedata.key, this->MachinesHead);
					searchedMachine->btree->insert(filedata);
				}
			}
			lines += 1;
		}
		fileRead.close();
		system("cls");
		cout << "             Welcome to InterPlanetary File System (IPFS)\n\n\n" << endl;
		Machine* current = this->MachinesHead;
		for (int i = 0; i < MachinesActive; ++i)
		{
			current->showFTp();
			current = current->next;
			cout << endl;
		}
		return;
	}

	Machine* FindSucc(int key)
	{
		Machine* current = this->MachinesHead;
		if (key > this->MachineEnd->ID)
			return current;
		while (key > current->ID)
		{
			current = current->next;
			if (current == this->MachinesHead)
				break;
		}
		return current;
	}

	void AdjustRoutingTable(Machine*& current)
	{
		int Ftp = 0;
		for (int i = 0; i < this->IdentifierSpace; ++i)
		{
			int power = 1;
			for (int j = 0; j < (i + 1) - 1; ++j)
			{
				power *= 2;
			}
			Ftp = current->ID + power;
			if (Ftp >= this->MaxNumberOfMachines)
			{
				Ftp = Ftp % MaxNumberOfMachines;
			}
			Machine* Succ = this->FindSucc(Ftp);

			if (current->Ftp->current == NULL)
			{
				current->Ftp->current = Succ;
			}
			else
			{
				RoutingTable* newFTp = new RoutingTable;
				newFTp->current = Succ;
				RoutingTable* FtpCurrent = current->Ftp;
				while (FtpCurrent->next != NULL)
				{
					FtpCurrent = FtpCurrent->next;
				}
				FtpCurrent->next = newFTp;
				newFTp->previous = FtpCurrent;
			}
		}
		return;
	}

	void AdjustRoutingTable()
	{
		Machine* current = this->MachinesHead;

		delete[] AllMachines;
		AllMachines = new Machine * [MachinesActive];

		for (int i = 0; i < this->MachinesActive; ++i)          //i Run Untill Machine Active in DHT
		{
			AllMachines[i] = current;
			RoutingTable* currentFtp = current->Ftp;

			while (currentFtp != NULL)       //De Allocating Previous FTp Table
			{
				RoutingTable* temp = currentFtp;
				currentFtp = currentFtp->next;
				delete temp;
			}
			current->Ftp = new RoutingTable;
			AdjustRoutingTable(current);       //Function to set the Routing Table of this Machine
			current = current->next;
		}
		return;
	}

	void DisplayAvaliableMachinesIDs()
	{
		cout << "All Machines :-" << endl;
		Machine* current = this->MachinesHead;
		for (int i = 0; i < this->MachinesActive; ++i)
		{
			cout << "Machine " << i + 1 << " ID " << " = " << current->ID << endl;
			current = current->next;
		}
		cout << endl;
		return;
	}

	Machine* searchMachineByID(long long id)
	{
		Machine* current = this->MachinesHead;
		while (id != current->ID)
		{
			current = current->next;
			if (current == MachinesHead)
			{
				current = NULL;
				break;
			}
		}
		return current;
	}

	Machine* InsertMachine(long long id)
	{
		Machine* newMachine = new Machine(id, this->IdentifierSpace, this->OrderMofBtree);
		if (this->MachinesHead == NULL)
		{
			this->MachinesHead = newMachine;                 //if root is NULL new Machine comming will be the Head on All Machines
			this->MachinesHead->next = this->MachinesHead;
			this->MachineEnd = newMachine;                   //End will be equal to the start when only one machine Exit
		}
		else
		{
			Machine* current = this->MachinesHead;
			Machine* previous = NULL;

			if (current->ID > newMachine->ID)               //is new Machine is to be inserted on the Head
			{
				while (current->next != this->MachinesHead)  //Finding Last Node on Circulat Linked List
				{
					current = current->next;
				}
				newMachine->next = this->MachinesHead;       //Create the new Machine as Head and reLink
				current->next = newMachine;
				this->MachinesHead = newMachine;
			}
			else
			{
				while (current->ID < newMachine->ID)
				{
					previous = current;
					current = current->next;
					if (current == this->MachinesHead)
					{
						current = NULL;
						break;
					}
				}
				if (current == NULL)                        //If Machine is to be inserted at the end
				{
					previous->next = newMachine;
					newMachine->next = this->MachinesHead;
					this->MachineEnd = newMachine;
				}
				else                                       //If Machine is to be inserted in the Middle
				{
					previous->next = newMachine;
					newMachine->next = current;
				}
			}
		}
		return newMachine;
	}

	Machine* RemoveMachine(long long id)
	{
		Machine* current = this->MachinesHead;
		Machine* previous = this->MachineEnd;
		Machine* deleteMachine = NULL;
		while (id != current->ID)
		{
			previous = current;
			current = current->next;
			if (current == MachinesHead)
			{
				current = NULL;
				break;
			}
		}
		deleteMachine = current;
		if (deleteMachine->ID == this->MachinesHead->ID)
		{
			this->MachinesHead = current->next;
		}
		else if (deleteMachine->ID == this->MachineEnd->ID)
		{
			this->MachineEnd = previous;
		}
		previous->next = current->next;

		return deleteMachine;
	}

	void SearchRequestByFileData()
	{
		int lines = 0;
		cout << "\nEnter the number of Lines Data Contain : ";
		cin >> lines;
		string value = "";
		string linesbylineData = "";

		cout << "\nEnter Data of File to be Searched : " << endl;
		cin.ignore();
		for (int i = 0; i < lines; ++i)
		{
			if (i != 0)
				value += "\n";
			getline(cin, linesbylineData);
			value += linesbylineData;
		}


		if (value == "")
		{
			cout << "\n---------------------ERROR------------------------" << endl;
			cout << "Please! Enter some Valid Data" << endl;
			cout << "----------------------Exiting-----------------------" << endl;
			return;
		}
		cout << endl;
		long long Machineid = 0;
		Machine* currentMachine = NULL;
		this->DisplayAvaliableMachinesIDs();      //Display avaliable Machine to Ask user to pass an insert request to any machine in DHT
		while (currentMachine == NULL)
		{
			cout << "Enter Machine Number : ";
			cin >> Machineid;
			if (Machineid > MachinesActive || Machineid <= 0)
			{
				cout << "Please Enter a Valid Machine ID" << endl;
				continue;
			}
			currentMachine = AllMachines[Machineid - 1];
		}

		checksum.update(value);
		const string hash = checksum.final();

		long long e = hashing(hash);

		Machine* searchedMachine = this->SearchMachine2(e, currentMachine);

		FileData filedata = searchedMachine->btree->search(e);

		bool found = false;
		if (filedata.key != -1)                      //checking if key is not -1 then some key is Found
		{
			PathNode* pathNode = filedata.pathList.root;    //checking if the key may contain mutiple paths
			while (1)
			{
				ifstream fileRead;

				fileRead.open(pathNode->filePath);

				string Data = this->GetFileData(fileRead);

				fileRead.close();

				bool check = true;
				for (int i = 0; Data[i] != '\0'; ++i)
				{
					if (Data[i] != value[i] || value[i] == '\0')
					{
						check = false;
						break;
					}
				}

				if (check)      //reading file and comnparing with given input
				{
					cout << "\n--------------------FILE FOUND---------------------" << endl;
					cout << "File Path : " << pathNode->filePath << endl;
					cout << "File Key  : " << filedata.key << endl;
					cout << "---------------------------------------------------" << endl;
					found = true;
					break;
				}
				if (pathNode->next == NULL)
				{
					break;
				}
				pathNode = pathNode->next;

			}
		}
		if (filedata.key == -1 || found == false)      //Rehashing if the File May be Distributed when a Machine Leave
		{
			checksum.update(value);
			const string hash = checksum.final();

			e = Rehashing(hash);
			searchedMachine = this->SearchMachine2(e, currentMachine);
			filedata = searchedMachine->btree->search(e);

			if (filedata.key != -1)
			{

				PathNode* pathNode = filedata.pathList.root;

				while (1)
				{
					ifstream fileRead;

					fileRead.open(pathNode->filePath);

					string Data = this->GetFileData(fileRead);

					bool check = true;
					for (int i = 0; Data[i] != '\0'; ++i)
					{
						if (Data[i] != value[i] || value[i] == '\0')
						{
							check = false;
							break;
						}
					}

					fileRead.close();
					if (check)
					{
						cout << "\n--------------------FILE FOUND---------------------" << endl;
						cout << "File Path : " << pathNode->filePath << endl;
						cout << "File Key  : " << filedata.key << endl;
						cout << "---------------------------------------------------" << endl;
						found = true;
						break;
					}
					if (pathNode->next == NULL)
					{
						break;
					}
					pathNode = pathNode->next;
				}
			}
			if (filedata.key == -1 || found == false)
			{
				cout << "\n---------------------ERROR------------------------" << endl;
				cout << "No File Found" << endl;
				cout << "----------------------Exiting-----------------------" << endl;
				return;
			}
		}
	}

	void SearchRequestByFilePath()
	{
		long long e;
		string value = "";
		ifstream fileRead;
		cout << "\nEnter the Directory of File : ";
		cin.ignore();
		getline(cin, value);

		fileRead.open(value);

		if (!fileRead)         //check if the File Exists
		{
			cout << "\n---------------------ERROR------------------------" << endl;
			cout << "File Does not Exist or The Given Path is Wrong" << endl;
			cout << "------------------REQUEST FAILED--------------------" << endl;
			return;
		}
		string Data = GetFileData(fileRead);
		fileRead.close();
		if (Data == "")          //check if not data is fetched from file or file is empty
		{
			cout << "\n---------------------ERROR------------------------" << endl;
			cout << "File Does not Contain any Data or File may be Corrupted" << endl;
			cout << "------------------REQUEST FAILED--------------------" << endl;
			return;
		}

		cout << endl;
		long long Machineid = 0;
		Machine* currentMachine = NULL;
		this->DisplayAvaliableMachinesIDs();      //Display avaliable Machine to Ask user to pass an insert request to any machine in DHT
		while (currentMachine == NULL)
		{
			cout << "Enter Machine Number : ";
			cin >> Machineid;
			if (Machineid > MachinesActive || Machineid <= 0)
			{
				cout << "Please Enter a Valid Machine ID" << endl;
				continue;
			}
			currentMachine = AllMachines[Machineid - 1];
		}

		checksum.update(Data);
		const string hash = checksum.final();

		e = hashing(hash);      //Passing the orignal data of file to be hashed and return key according to data instead of file name

		Machine* searchedMachine = NULL;

		searchedMachine = this->SearchMachine2(e, currentMachine);      //Searching Machine O(logn)

		FileData filedata = searchedMachine->btree->search(e);

		bool found = false;
		if (filedata.key != -1)                      //checking if key is not -1 then some key is Found
		{
			PathNode* pathNode = filedata.pathList.root;    //checking if the key may contain mutiple paths
			while (1)
			{
				ifstream fileRead;

				fileRead.open(pathNode->filePath);

				string Data = this->GetFileData(fileRead);
				fileRead.close();

				cout << pathNode->filePath << endl;
				cout << value << endl;

				bool stringEqual = true;

				for (int i = 0; value[i] != '\0'; ++i)
				{
					if (value[i] != pathNode->filePath[i] && value[i] != '\\')
					{
						stringEqual = false;
						break;
					}
				}


				if (stringEqual)      //reading file and comnparing with given input
				{
					cout << "\n--------------------FILE FOUND---------------------" << endl;
					cout << "File Path : " << pathNode->filePath << endl;
					cout << "File Key  : " << filedata.key << endl;
					cout << "File Data : " << endl;
					cout << Data << endl;
					cout << "---------------------------------------------------" << endl;
					found = true;
					break;
				}
				if (pathNode->next == NULL)          //if No Linked List then Exit
				{
					break;
				}
				pathNode = pathNode->next;

			}
		}
		if (filedata.key == -1 || found == false)      //Rehashing if the File May be Distributed when a Machine Leave
		{
			checksum.update(Data);
			const string hash = checksum.final();

			e = Rehashing(hash);
			searchedMachine = this->SearchMachine2(e, currentMachine);
			filedata = searchedMachine->btree->search(e);


			if (filedata.key != -1)
			{

				PathNode* pathNode = filedata.pathList.root;

				while (1)
				{
					ifstream fileRead;

					fileRead.open(pathNode->filePath);

					string Data = this->GetFileData(fileRead);
					fileRead.close();

					bool stringEqual = true;

					for (int i = 0; value[i] != '\0'; ++i)
					{
						if (value[i] != pathNode->filePath[i] && value[i] != '\\')
						{
							stringEqual = false;
							break;
						}
					}

					if (stringEqual)
					{
						cout << "\n--------------------FILE FOUND---------------------" << endl;
						cout << "File Path : " << pathNode->filePath << endl;
						cout << "File Key  : " << filedata.key << endl;
						cout << "File Data : " << endl;
						cout << Data << endl;
						cout << "---------------------------------------------------" << endl;
						found = true;
						break;
					}
					if (pathNode->next == NULL)
					{
						break;
					}
					pathNode = pathNode->next;
				}
			}
			if (filedata.key == -1 || found == false)         //if file not found at the End printing Error Message
			{
				cout << "\n---------------------ERROR------------------------" << endl;
				cout << "No File Found" << endl;
				cout << "----------------------Exiting-----------------------" << endl;
				return;
			}
		}


	}

	Machine* SearchMachine2(long long e, Machine* currentMachine)
	{
		cout << "\n\n----------STARTING MACHINE SEARCH-------------" << endl;
		cout << "Start Searching From Machine : " << currentMachine->ID << endl;
		if (currentMachine->ID == e)                         //if the current machine is equal to hash value e return
		{
			cout << "Machine Found with ID : " << currentMachine->ID << endl;
			return currentMachine;
		}
		else                                                 //other wise find the perfectly matched Machine
		{
			bool machineFound = false;
			int i = 0;
			Machine* previousMachine = NULL;
			RoutingTable* currentFtp = currentMachine->Ftp;

			while (true)
			{
				if (currentMachine->ID == e)        //if the current machine is equal to hash value e break  
				{
					break;
				}
				if (previousMachine != NULL)       //if hash value lies in b/w previous Machine and Current Machine then current machine will be the perfect machine
				{
					if (e > previousMachine->ID && e < currentMachine->ID)
					{
						break;
					}
				}
				if (e > MachineEnd->ID && currentMachine->ID == this->MachinesHead->ID)  //if e is greater than last Machine Id and current machine is machine head than break
				{
					break;
				}
				if (e <= this->MachinesHead->ID && currentMachine->ID == this->MachinesHead->ID) //if hash values
				{
					break;
				}
				if (i == IdentifierSpace - 1)   //if FTP table end with last value left only choice is to move to that machine because e is Much bigger or smaller than that       
				{

					previousMachine = currentMachine;
					if (currentFtp->current->ID == currentMachine->ID)
					{
						currentMachine = currentFtp->previous->current;
						currentFtp = currentMachine->Ftp;
						i = 0;
						cout << "Moving to Machine with ID : " << currentMachine->ID << endl;
						continue;
					}
					currentMachine = currentFtp->current;
					currentFtp = currentMachine->Ftp;

					cout << "Moving to Machine with ID : " << currentMachine->ID << endl;
					i = 0;
					continue;
				}
				else if (currentMachine->ID < e && e <= currentMachine->Ftp->current->ID)       //if e > current Machine ID && e <= FTp[0]
				{
					previousMachine = currentMachine;

					currentMachine = currentMachine->Ftp->current;
					currentFtp = currentMachine->Ftp;

					cout << "Moving to Machine with ID : " << currentMachine->ID << endl;
					i = 0;
					continue;
				}
				else if (currentMachine->ID == this->MachineEnd->ID && e > this->MachineEnd->ID && currentMachine->Ftp->current->ID == this->MachinesHead->ID)
				{ //if current Machine is the Last Machine  and  e  >  Last Machine ID and FTp[0] is the Head Machine
					previousMachine = currentMachine;

					currentMachine = currentMachine->Ftp->current;
					currentFtp = currentMachine->Ftp;

					cout << "Moving to Machine with ID : " << currentMachine->ID << endl;
					i = 0;
					break;
				}
				else if (currentMachine->ID == this->MachineEnd->ID && currentMachine->Ftp->current->ID == this->MachinesHead->ID && e < this->MachinesHead->ID)
				{    //if current Machine is the Last Machine  and  e  <  Head Machine ID and FTp[0] is the Head Machine
					previousMachine = currentMachine;

					currentMachine = currentMachine->Ftp->current;
					currentFtp = currentMachine->Ftp;

					cout << "Moving to Machine with ID : " << currentMachine->ID << endl;
					i = 0;
					break;
				}
				else if (currentFtp->current->ID < e && e <= currentFtp->next->current->ID)     //if  e > Ftp[i]  and e <= FTp[i+1]
				{
					previousMachine = currentMachine;

					currentMachine = currentFtp->current;
					currentFtp = currentMachine->Ftp;

					cout << "Moving to Machine with ID : " << currentMachine->ID << endl;
					i = 0;
					continue;
				}
				else if (e > this->MachineEnd->ID)
				{    // if e > Last Machine  then check  FTp[i] < e  and  FTp[i+1] <  FTp[i] 
					if (currentFtp->current->ID < e && currentFtp->next->current->ID < currentFtp->current->ID)
					{
						previousMachine = currentMachine;
						currentMachine = currentFtp->current;
						currentFtp = currentMachine->Ftp;
						cout << "Moving to Machine with ID : " << currentMachine->ID << endl;
						i = 0;
						continue;
					}
				}
				currentFtp = currentFtp->next;
				i++;
			}
		}
		cout << "Machine Found with ID : " << currentMachine->ID << endl;
		return currentMachine;
	}

	long long hashing(string key)
	{
		long long hashed = 0;
		int prime = 31;
		for (int i = 0; key[i] != '\0'; ++i)
		{
			hashed = (hashed + key[i] * prime) % this->MaxNumberOfMachines;
		}

		return hashed % MaxNumberOfMachines;      //Mod by size so the hashed key fall in identifer space
	}

	long long Rehashing(string key)
	{
		const int base = 127; // Prime number for better distribution

		long long hashed = 0;
		for (char ch : key)
		{
			hashed = (hashed * base + ch) % MaxNumberOfMachines;
		}

		return hashed;
	}

	string GetFileData(ifstream& fileRead)      //Function to Read File Data and return it in a String
	{
		string readline = "";
		string Data = "";
		int count = 0;
		while (getline(fileRead, readline))
		{
			if (count != 0)
				Data += "\n";
			Data += readline;
			count += 1;
		}
		return Data;
	}

	std::string intToString(int number) {
		// Special case for 0
		if (number == 0)
			return "0";

		std::string result = "";
		bool isNegative = false;

		// Handle negative numbers
		if (number < 0) {
			isNegative = true;
			number = -number;
		}

		// Extract digits and build the string in reverse order
		while (number > 0) {
			char digit = '0' + (number % 10);
			result = digit + result;
			number /= 10;
		}

		// Add '-' for negative numbers
		if (isNegative) {
			result = '-' + result;
		}

		return result;
	}

};