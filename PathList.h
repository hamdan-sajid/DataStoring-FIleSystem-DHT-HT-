#include <iostream>
#include <string>

using namespace std;

class PathNode
{
public:
	string filePath;
	PathNode* next;
	PathNode(string p)
	{
		filePath = p;
		next = NULL;
	}
};

class PathList
{
public:
	PathNode* root;
	int size;
	PathList()
	{
		size = 0;
		root = NULL;
	}

	void insert(string p)
	{
		PathNode* newNode = new PathNode(p);
		if (root == nullptr)
		{
			// If the list is empty, make the new node the root
			root = newNode;
		}
		else
		{
			// Otherwise, traverse to the end and add the new node
			PathNode* current = root;
			while (current->next != nullptr)
			{
				current = current->next;
			}
			current->next = newNode;
		}

		// Increment the size of the list
		size++;
	}

	void remove(string p)
	{
		if (root == nullptr)
		{
			// If the list is empty, nothing to remove
			return;
		}

		// If the node to be removed is the root
		if (root->filePath == p)
		{
			PathNode* temp = root;
			root = root->next;
			delete temp;
			size--;
			return;
		}

		// Traverse the list to find the node to remove
		PathNode* current = root;
		while (current->next != nullptr && current->next->filePath != p)
		{
			current = current->next;
		}

		// If the node is found, remove it
		if (current->next != nullptr)
		{
			PathNode* temp = current->next;
			current->next = current->next->next;
			delete temp;
			size--;
		}
	}
	void remove(int p)
	{
		if (root == nullptr)
		{
			// If the list is empty, nothing to remove
			return;
		}

		// If the node to be removed is the root
		if (p == 1)
		{
			PathNode* temp = root;
			root = root->next;
			delete temp;
			size--;
			return;
		}

		// Traverse the list to find the node to remove
		PathNode* current = root;
		int i = 1;
		while (current->next != nullptr && i == p)
		{
			current = current->next;
			i++;
		}

		// If the node is found, remove it
		if (current->next != nullptr)
		{
			PathNode* temp = current->next;
			current->next = current->next->next;
			delete temp;
			size--;
		}
	}


	// Function to print the contents of the list
	void print()
	{
		PathNode* current = root;
		int i = 1;
		while (current != nullptr)
		{
			ifstream fileRead;
			cout << i << " : " << current->filePath << endl;
			fileRead.open(current->filePath);
			string Data = GetFileData(fileRead);
			cout << "Data the File Contain :" << endl;
			cout << Data << "\n\n" << endl;
			fileRead.close();
			current = current->next;
			i++;
		}
	}

	string GetFileData(ifstream& fileRead)
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

};
