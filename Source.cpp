#include <iostream>
#include <string>
#include <ctime>
#include <windows.h>
#include "DHT.h"
using namespace std;

bool isValidInput = true;
void askRequestDisplay();

int main()
{
    RingDistributedHashTable Server;
    Server.displayLogo();
    Server.initalize();

    int choice; //For menu choice 
    int check; //For menu choice
    int machine_id; // For inputs
    do { // Printing main menu till program exit
        system("cls");
        Server.displayLogo();
        askRequestDisplay();
        //cin >> check; //User choice
        if (!(std::cin >> check)) {
            // If input is not an integer, clear input buffer
            std::cin.clear();
            std::cin.ignore();
            std::cout << "Invalid input. Please enter a number.\n";
            check = 9;
        }
        else
        {
            switch (check)
            {
            case 1: // Insert data by giving File Path
                isValidInput = true;
                system("cls");
                Server.displayLogo();
                Server.InitiateInsertRequest();
                break;
            case 2: // Search File Path , by Giving Data
                isValidInput = true;
                system("cls");
                Server.displayLogo();

                Server.InitiateSearchRequest();
                break;
            case 3: // Delete File Path/File By giving key
                isValidInput = true;
                system("cls");
                Server.displayLogo();
                Server.InitiateDeleteRequest();
                break;
            case 4: // Print Any Machine Routing Table
                isValidInput = true;
                system("cls");
                Server.displayLogo();

                Server.ShowRoutingTable();
                break;
            case 5: // Print B Tree of Any Machine
                isValidInput = true;
                system("cls");
                Server.displayLogo();

                Server.ShowBTree();
                break;
            case 6: // Insert a machine inside the ring
                isValidInput = true;
                system("cls");
                Server.displayLogo();
                Server.InitiateInsertMachineRequest();
                break;
            case 7: // Delete a machine inside the ring
                isValidInput = true;
                system("cls");
                Server.displayLogo();
                Server.InitiateDeleteMachineRequest();
                break;
            case 0:
                isValidInput = true;
                system("cls");
                Server.displayLogo();
                cout << "Exiting menu....\n";
                break;
            default:
                cin.ignore();
                isValidInput = false;
                break;
            }
        }
    } while (check != 0); // If user wants to exit menu
}

void askRequestDisplay()
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_BLUE);
    cout << "\n\n"
        << endl;
    if (!isValidInput)
    {
        cout << "\t\t\t\t\t              Invalid Input.\n\t\t\t\t\tPlease only choose the options provided." << endl;
    }
    cout << "\t\t\t\t    "
        << "_______________________________________________\n";
    cout << "\t\t\t\t    "
        << "||--------------------------------------------|\n";
    cout << "\t\t\t\t    "
        << "||1 - Insert File by giving Directory         |" << endl;
    cout << "\t\t\t\t    "
        << "||2 - Search by giving Directory/Path         |" << endl;
    cout << "\t\t\t\t    "
        << "||3 - Delete File by giving Key               |" << endl;
    cout << "\t\t\t\t    "
        << "||4 - Print Routing Table of Any Machine      |" << endl;
    cout << "\t\t\t\t    "
        << "||5 - Print B-Tree of Any Machine             |" << endl;
    cout << "\t\t\t\t    "
        << "||6 - ADD Machine                             |" << endl;
    cout << "\t\t\t\t    "
        << "||7 - DELETE Machine                          |" << endl;
    cout << "\t\t\t\t    "
        << "||0 - Exit Menu                               |" << endl;
    cout << "\t\t\t\t    "
        << "||____________________________________________|\n";
    cout << "\t\t\t\t    "
        << "                Enter Choice : ";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}