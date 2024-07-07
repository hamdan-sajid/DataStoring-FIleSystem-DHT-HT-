
# InterPlanetary File System (IPFS)

IPFS provides functionality similar to BitTorrent making it extremely easy to publish your content and make it accessible anywhere, anytime.

This Project is created on a Simlar Concept of IPFS and Handles only text(.txt) extension files.

Along with the Project a Document.pdf is attached which provides a brief description of the concepts that have been used in this Project. 


**How to Execute(Run) Project**

This is a very simple just download or Import the Solution Folder and run the .sln file in Micorsoft Visual Studio.

                                           OR  

Download all the File that have been included in main.cpp in the solution folder along with main.cpp and just simply run the main.cpp in the Code Editor of your own choice.


**Concepts Used**

1. Distributed Hash Table(DHT) for Handling Machines 

2. SHA1 for Hashing

3. Circular Linked for Routing Table(FTp)

4. Balanced Tree(BTree) for handling files of Machine.
                                                                            
**Commands**

1. initalize [Start Server and Load Machines]

2. InitiateInsertRequest [Insert a File by file Path]

3. InitiateSearchRequest [Search a File by File Path Or Data]

4. InitiateDeleteRequest [Delete a File by File Path Or Data]

5. ShowRoutingTable [Show Routing Table of any Machine]

6. ShowBTree [Show BTree of any Machine]

7. InitiateInsertMachineRequest [Insert a new Machine]

8. InitiateDeleteMachineRequest [Delete a Existing Machine]

**Bonus Feature**

The Server save its state once Started. When you update the Machine Data by inserting/Deleting or any other task and exit/Close the Program. The State of the server will be saved and you can continue with the same data as before when you again run the program. 

































