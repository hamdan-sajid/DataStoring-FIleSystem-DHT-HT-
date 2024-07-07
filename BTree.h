#include <iostream>
#include <queue>
#include <iomanip>
using namespace std;

template <class adt>
class Node
{
public:
    adt* keys;        // Array of keys/Values...This is the data part
    int minPosKeys;   // This is the minimum number of possible values
    int currentIndex; // Current empty index of keys OR there are currentIndex-1 keys currently
    int size;         // Max size of a node
    Node** childs;    // size+1 childs
    Node* parent;     // 2 Parent for better referencing
    bool isNodeLeaf;  // A boolean to save weather this node is leaf or not

    Node(int tt)
    {
        minPosKeys = tt;
        size = 2 * minPosKeys - 1;
        keys = new adt[size];
        childs = new Node<adt> *[size + 1];
        for (int i = 0; i < size + 1; i++)
        {
            childs[i] = NULL;
        }
        currentIndex = 0;
        parent = NULL;
        isNodeLeaf = true;
    }

    adt getMidValue(adt d)
    {
        // Return the middle value of keys so that we can add it to the parent
        return keys[minPosKeys - 1];
    }

    int getIndex(adt d)
    {
        // Getting the index of the insertion index, so that we can know where should we add the child
        int i = currentIndex - 1;
        while (i >= 0 && keys[i].key != d.key)
        {
            i--;
        }
        return i;
    }

    void splitNode(adt d, Node<adt>*& root);
    void insertInNode(adt d, Node<adt>*& root);
    void traverse();
    void del(adt& d, Node<adt>*& current);
    void borrowLeft(adt& d, int& i, Node<adt>*& temp);
    void mergeLeft(adt& d, int& i, Node<adt>*& temp);
    void mergeRight(adt& d, int& i, Node<adt>*& temp);
    void fix(int& min, Node<adt>*& temp);
    void findSucc(int& min, Node<adt>*& temp);
    void findPred(int& min, Node<adt>*& temp);

    // A simple traversing for the node
    void traverseNode()
    {
        for (int i = 0; i < currentIndex; i++)
        {
            cout << keys[i].key << " ";
        }
    }
};


// Function to split Node
template <class adt>
void Node<adt>::splitNode(adt d, Node<adt>*& root)
{
    Node<adt>* rightNode = new Node<adt>(minPosKeys); // New node, for half of the existing node

    int i = currentIndex - 1;
    while (i >= 0 && keys[i].key > d.key)
    {
        i--;
    }

    int j = 0;
    // Moving right half values to the rightNode
    for (int k = minPosKeys; k < currentIndex; k++)
    {
        rightNode->keys[j++] = keys[k];
    }

    currentIndex = minPosKeys - 1;
    rightNode->currentIndex = j;

    // This is for root, as root has no parent
    if (parent == NULL)
    {
        Node<adt>* newRoot = new Node<adt>(minPosKeys); // A new root, because in this case we are adding 1 in height
        newRoot->isNodeLeaf = false;                    // Setting the root to notLeaf

        adt midValue = getMidValue(d);
        newRoot->insertInNode(midValue, root);

        cout << " in Root" << endl;

        if (d.key > midValue.key)
        {
            rightNode->insertInNode(d, root);
        }
        else
        {
            this->insertInNode(d, root);
        }

        newRoot->childs[0] = this;
        newRoot->childs[1] = rightNode;

        parent = newRoot; // Setting the newRoot as a parent of current root and rightNode
        rightNode->parent = newRoot;
        root = newRoot; // Setting the new root


    }
    else
    {
        adt midValue = getMidValue(d);
        parent->insertInNode(midValue, root);

        int parentIndex = parent->getIndex(midValue);

        // Move existing child pointers to make space for the new child
        for (int k = parent->currentIndex; k > parentIndex + 1; k--)
        {
            parent->childs[k] = parent->childs[k - 1];
        }

        parent->childs[parentIndex + 1] = rightNode;
        rightNode->parent = parent;

        if (d.key > midValue.key)
        {
            rightNode->insertInNode(d, root);
        }
        else
        {
            this->insertInNode(d, root);
        }

        // Update isNodeLeaf property of the parent
        parent->isNodeLeaf = false;

    }

    // For adding the childs of the splitting nodes
    for (int k = minPosKeys; k <= size; k++)
    {
        rightNode->childs[k - minPosKeys] = childs[k];
        if (childs[k] != NULL)
        {
            childs[k]->parent = rightNode;
            childs[k] = NULL;
        }
    }

    for (int i = 0; i <= rightNode->currentIndex; i++)
    {
        if (rightNode->childs[i] != NULL)
        {
            rightNode->isNodeLeaf = false;
            break;
        }
    }

}

template <class adt>
void Node<adt>::insertInNode(adt d, Node<adt>*& root)
{
    // If there is a space, then just simple insert it
    if (currentIndex < size)
    {
        int i = currentIndex - 1;
        while (i >= 0 && keys[i].key >= d.key)
        {
            if (keys[i].key == d.key)
            {
                cout << "\nThe key " << d.key << " is already in the Storage" << endl;
                cout << "Inserting in its next data member" << endl;
                keys[i].pathList.insert(d.filePath);
                return;
            }
            keys[i + 1] = keys[i];
            i--;
        }
        keys[i + 1] = d;
        currentIndex++;
    }
    else
    {
        // Checking if there is already same key here:
        //Checking if there is already same key here:
        int i = 0;
        while (i >= 0 && i < size)
        {
            if (keys[i].key == d.key)
            {
                cout << "\nThe key " << d.key << " is already in the Storage" << endl;
                cout << "Inserting in its next data member" << endl;
                keys[i].pathList.insert(d.filePath);
                return;
            }
            i++;
        }

        splitNode(d, root);
    }
}

template <class adt>
void del(adt& d, Node<adt>*& current)
{
    int i = 0;
    while (i < current->currentIndex && current->keys[i].key < d.key)
        i++;

    if (i != current->size - 1)
        while (i < current->currentIndex - 1)
        {
            current->keys[i] = current->keys[i + 1];
            i++;
        }
    current->currentIndex--;
}

template <class adt>
void borrowLeft(adt& d, int& i, Node<adt>*& temp)
{
    Node<adt>* leftchild = temp->childs[i - 1]; //left child
    adt leftmax = leftchild->keys[leftchild->currentIndex - 1];    // max key in left

    adt parentkey = temp->keys[i - 1];  //apporiate key in parent

    temp->keys[i - 1] = leftmax;  //apporiate key in parent

    del(leftmax, leftchild); //deleting max in child
    temp->childs[i]->insertInNode(parentkey, temp->childs[i]); //inserting parent in node in which key is to be deleted

    del(d, temp->childs[i]); // deleting the key that is asked by user
}

template <class adt>
void mergeLeft(adt& d, int& i, Node<adt>*& temp)
{
    Node<adt>* leftchild = temp->childs[i - 1]; // left child
    adt parentkey = temp->keys[i - 1]; // temp has parent so getting parent key

    del(d, temp->childs[i]);   // deleting the required key
    int k = leftchild->currentIndex;
    for (int j = 0; j < k; j++) //inserting keys of left child into right one
    {
        temp->childs[i]->insertInNode(leftchild->keys[j], temp->childs[j]);
    }

    temp->childs[i]->insertInNode(parentkey, temp->childs[i]); //inserting parent key in right child
    del(parentkey, temp); // deleting key in parent node



    k = temp->currentIndex + 1;
    for (int j = i - 1; j < k; j++) // moving children back one step
    {
        temp->childs[j] = temp->childs[j + 1];
    }
    temp->childs[k] = NULL; // removing dublicates


}

template <class adt>
void borrowRight(adt& d, int& i, Node<adt>*& temp)
{
    Node<adt>* rightchild = temp->childs[i + 1]; //right child
    adt rightmin = rightchild->keys[0]; // get min key

    adt parentkey = temp->keys[i];  //apporiate key in parent

    temp->keys[i] = rightmin;   //apporiate key in parent

    del(rightmin, rightchild); //deleting min in child
    temp->childs[i]->insertInNode(parentkey, temp->childs[i]); //inserting parent in node in which key is to be deleted

    del(d, temp->childs[i]); // deleting the key that is asked by user
}

template <class adt>
void mergeRight(adt& d, int& i, Node<adt>*& temp)
{
    Node<adt>* rightchild = temp->childs[i + 1];
    adt parentkey = temp->keys[i];

    del(d, temp->childs[i]);
    int k = rightchild->currentIndex;
    for (int j = 0; j < k; j++)
    {
        temp->childs[i]->insertInNode(rightchild->keys[j], temp->childs[j]);
    }
    temp->childs[i]->insertInNode(parentkey, temp->childs[i]); //inserting parent key in right child
    del(parentkey, temp); // deleting key in parent node

    k = temp->currentIndex + 1;
    for (int j = 1; j < k; j++) // moving children back one step
    {
        temp->childs[j] = temp->childs[j + 1];
    }
    temp->childs[k] = NULL; // removing dublicates
}

template <class adt>
void IntmergeRight(int& i, Node<adt>*& temp)
{
    Node<adt>* leftchild = temp->childs[i - 1]; //left child
    adt parentkey = temp->keys[i - 1];

    int kr = leftchild->currentIndex;
    int k = temp->currentIndex;
    for (int j = 0; j < kr; j++)
    {
        temp->childs[i]->insertInNode(leftchild->keys[j], temp->childs[j]);
    }
    temp->childs[i]->insertInNode(parentkey, temp->childs[i]); //inserting parent key in right child
    del(parentkey, temp); // deleting key in parent node

    int l = temp->childs[i]->currentIndex;
    //////////

    for (int j = 0; j < l; j++)
    {
        temp->childs[i]->childs[j + 1] = temp->childs[i]->childs[j];
    }



    for (int j = 0; j < l; j++)
    {
        temp->childs[i]->childs[j] = leftchild->childs[j];
        if (temp->childs[i]->childs[j])
            temp->childs[i]->childs[j]->parent = temp->childs[i];
    }



    for (int j = i - 1; j < k; j++)
    {
        temp->childs[j] = temp->childs[j + 1];
    }
    temp->childs[k] = NULL;


}

template <class adt>
void IntmergeLeft(int& i, Node<adt>*& temp)
{
    Node<adt>* rightchild = temp->childs[i + 1];
    adt parentkey = temp->keys[i];

    int k = rightchild->currentIndex;
    for (int j = 0; j < k; j++)
    {
        temp->childs[i]->insertInNode(rightchild->keys[j], temp->childs[j]);
    }
    temp->childs[i]->insertInNode(parentkey, temp->childs[i]); //inserting parent key in right child
    k = temp->currentIndex;
    del(parentkey, temp); // deleting key in parent node
    //plus due to deletion
    for (int j = i + 1; j < k; j++) // moving children back one step
    {
        temp->childs[j] = temp->childs[j + 1];
    }
    //temp->childs[k] = NULL;

    int l = 0;
    for (int j = 0; j < temp->childs[i]->size; j++)
    {
        if (temp->childs[i]->childs[j] == NULL)
            break;
        l++;
    }


    for (int j = 0; j < rightchild->size; j++)
    {
        if (rightchild->childs[j] == NULL)
            break;
        temp->childs[i]->childs[l] = rightchild->childs[j];
        if (temp->childs[i]->childs[l])
            temp->childs[i]->childs[l]->parent = temp->childs[i];
        l++;
    }

    for (int j = 0; j < rightchild->currentIndex + 1; j++)
    {
        rightchild->childs[j] = NULL;
    }
    temp->childs[k] = NULL;


}

template <class adt>
void IntborrowRight(int& i, Node<adt>*& temp)
{
    Node<adt>* leftchild = temp->childs[i - 1]; //left child
    adt leftmax = leftchild->keys[leftchild->currentIndex - 1];    // max key in left

    adt parentkey = temp->keys[i - 1];  //apporiate key in parent

    temp->keys[i - 1] = leftmax;  //apporiate key in parent

    del(leftmax, leftchild); //deleting max in child
    temp->childs[i]->insertInNode(parentkey, temp->childs[i]); //inserting parent in node in which key is to be deleted

    int y = leftchild->currentIndex;
    Node<adt>* tr = leftchild->childs[y + 1];

    for (int j = 0; j < temp->childs[i]->currentIndex; j++)
    {
        temp->childs[i]->childs[j + 1] = temp->childs[i]->childs[j];
    }
    temp->childs[i]->childs[0] = tr;
    if (temp->childs[i]->childs[0])
    {
        temp->childs[i]->childs[0]->parent = temp->childs[i];
    }
    leftchild->childs[y + 1] = NULL;


}

template <class adt>
void IntborrowLeft(int& i, Node<adt>*& temp)
{
    Node<adt>* rightchild = temp->childs[i + 1]; //right child
    adt rightmin = rightchild->keys[0]; // get min key

    adt parentkey = temp->keys[i];  //apporiate key in parent

    temp->keys[i] = rightmin;   //apporiate key in parent

    del(rightmin, rightchild); //deleting min in child
    temp->childs[i]->insertInNode(parentkey, temp->childs[i]); //inserting parent in node in which key is to be deleted

    Node<adt>* tr = rightchild->childs[0];
    for (int j = 0; j < rightchild->currentIndex + 1; j++)
    {
        rightchild->childs[j] = rightchild->childs[j + 1];
    }

    temp->childs[i]->childs[temp->childs[i]->currentIndex] = tr;
    if (temp->childs[i]->childs[temp->childs[i]->currentIndex])
    {
        temp->childs[i]->childs[temp->childs[i]->currentIndex]->parent = temp->childs[i];
    }
    rightchild->childs[rightchild->currentIndex + 1] = NULL;

}

template <class adt>
void fix(int& min, Node<adt>*& current)
{
    Node<adt>* temp = current->parent;
    int i = 0;
    for (int j = 0; j < temp->size; j++) // finding the correct children
    {
        if (temp->childs[j] == current)
            break;
        i++;
    }
    int k = temp->currentIndex; // index of last child in parent
    if (i == k)
    {
        if (temp->childs[i - 1]->currentIndex == min)
        {
            IntmergeRight(i, temp);
        }
        else
        {
            IntborrowRight(i, temp);
        }
    }
    else
    {
        if (temp->childs[i + 1]->currentIndex == min)
        {
            if (temp->childs[i]->currentIndex > min)
            {
                IntborrowRight(i, temp);
            }
            else
            {
                IntmergeLeft(i, temp);
            }
        }
        else
        {
            IntborrowLeft(i, temp);
        }
    }

}

template <class adt>
adt& findSucc(int& min, Node<adt>*& temp, Node<adt>*& root)
{
    while (temp->isNodeLeaf == false)
    {
        temp = temp->childs[0];
    }

    adt tt = temp->keys[0];

    del(tt, temp);
    if (temp->currentIndex < min)
    {
        fix(min, temp);
        temp = temp->parent;
        while (temp->currentIndex < min && temp != root)
        {
            fix(min, temp);
            temp = temp->parent;
        }
    }
    return tt;
}

template <class adt>
adt& findPred(int& min, Node<adt>*& temp, Node<adt>*& root)
{
    while (temp->isNodeLeaf == false)
    {
        temp = temp->childs[temp->currentIndex];
    }
    adt tt = temp->keys[temp->currentIndex - 1];
    del(tt, temp);

    if (temp->currentIndex < min)
    {
        fix(min, temp);
        temp = temp->parent;
        while (temp->currentIndex < min && temp != root)
        {
            fix(min, temp);
            temp = temp->parent;
        }
    }

    return tt;
}

template <class adt>
class BTree
{

public:
    Node<adt>* root;
    int minPosKeys;


    BTree(int tt)
    {
        minPosKeys = tt;
        root = NULL;
    }

    void insert(adt d)
    {
        cout << "\n\n------------STARTING INSERT NODE--------------" << endl;
        // Simple insert in root
        if (root == NULL)
        {
            cout << "This is the FIRST DATA item in the STORAGE" << endl;
            cout << "Inserting " << d.key << " in root" << endl;
            Node<adt>* temp = new Node<adt>(minPosKeys);
            temp->keys[0] = d;
            temp->currentIndex++;
            root = temp;
        }
        else
        {
            cout << "Traversing in the STORAGE SPACE" << endl;
            Node<adt>* current = root;
            if (root->isNodeLeaf == true)
            {
                if (root->currentIndex < root->size)
                {
                    cout << "Root is the only LEAF NODE" << endl;
                }
            }
            // Traversing to the leafNode plus to the appropriate key
            while (current->isNodeLeaf == false)
            {
                int i = 0;
                while (i < current->currentIndex && current->keys[i].key <= d.key)
                {
                    cout << "Moving through " << current->keys[i].key << " , ";
                    if (current->keys[i].key == d.key)
                    {
                        cout << "\nThe key " << d.key << " is already in the Storage" << endl;
                        cout << "Inserting in its next data member" << endl;
                        current->keys[i].pathList.insert(d.filePath);
                        return;
                    }
                    i++;
                }
                current = current->childs[i];
                cout << "to its CHILD ";
            }
            current->insertInNode(d, root);
        }
        cout << endl;
    }

    adt search(long long d)
    {
        if (root == NULL)
        {
            adt temp;
            temp.key = -1;
            return temp;
        }
        Node<adt>* current = root;
        bool flag = 0;
        if (root->isNodeLeaf)
        {
            int i = 0;
            while (i < current->currentIndex && current->keys[i].key <= d)
            {
                if (current->keys[i].key == d)
                {
                    return current->keys[i];
                }
                i++;
            }
            adt temp;
            temp.key = -1;
            return temp;
        }
        else
        {
            while (current->isNodeLeaf == false)
            {
                int i = 0;
                while (i < current->currentIndex && current->keys[i].key <= d)
                {
                    if (current->keys[i].key == d)
                    {
                        return current->keys[i];
                    }
                    i++;
                }
                current = current->childs[i];
            }
            if (current->isNodeLeaf)
            {
                int i = 0;
                while (i < current->currentIndex && current->keys[i].key <= d)
                {
                    if (current->keys[i].key == d)
                    {
                        return current->keys[i];
                    }
                    i++;
                }
            }
            adt temp;
            temp.key = -1;
            return temp;
        }
    }

    void checkPathLinkedList(adt key)
    {
        string Path = "";
        cout << "The Same Key Contain " << key.pathList.size << " File Paths : \n" << endl;
        key.pathList.print();
        int choice = 0;
        do {
            cout << "Enter Index You Want to Remove : ";
            cin >> choice;
            if (choice <= 0 && choice > key.pathList.size)
            {
                cout << "Please Try Again! Invalid Choice" << endl;
            }
            else
            {
                key.pathList.remove(choice);
                break;
            }
        } while (choice <= 0 && choice > key.pathList.size);
    }

    Node<adt>*& del_search(adt d)
    {
        Node<adt>* current = root;
        bool flag = 0;
        if (root->isNodeLeaf)
        {
            return current;
        }
        else
        {
            while (current->isNodeLeaf == false)
            {

                int i = 0;
                while (i < current->currentIndex)
                {
                    if (current->keys[i].key == d.key)
                    {
                        flag = 1;
                        break;
                    }
                    i++;
                }
                if (flag == 1)
                {
                    break;
                }
                i = 0;
                while (i < current->currentIndex && current->keys[i].key < d.key)
                {
                    i++;
                }
                current = current->childs[i];

            }
            return current;
        }
    }

    void remove(adt d, bool check)
    {
        Node<adt>* current = del_search(d);
        bool del_flag = 0;
        for (int j = 0; j < current->currentIndex; j++)
        {
            if (current->keys[j].key == d.key)
            {
                del_flag = 1;
                cout << "\n\nDeleting : " << current->keys[j].key << endl;
                if (current->keys[j].pathList.root->next != NULL && check == true)
                {
                    this->checkPathLinkedList(current->keys[j]);
                    return;
                }
                break;
            }
        }
        if (del_flag == 1)
        {
            int min = current->size + 1;
            if (min % 2 == 1)
                min++;

            min /= 2;
            min--;



            if (current->isNodeLeaf) //leaf node
            {
                if (current->currentIndex > min) // leaf node has more than enough keys
                {
                    del(d, current);
                    cout << "The current node[" << d.key << "] is a leaf node and we can simply delete" << endl;
                }
                else   // leaf node have minimum keys
                {
                    if (current->parent == NULL || current->parent->currentIndex == 0)  // deletion will be normal if node is root
                    {
                        cout << "The current node[" << d.key << "] is root and we can simply delete" << endl;
                        del(d, current);
                        if (current->currentIndex == 0)
                        {
                            cout << "Whole tree has been deleted!" << endl;
                            root = NULL;
                        }
                    }
                    else
                    {
                        Node<adt>* temp = current->parent;
                        int i = 0;
                        while (i < temp->currentIndex && temp->keys[i].key < d.key) // finding the correct children
                        {
                            i++;
                        }
                        if (i != 0 && temp->childs[i - 1] && temp->childs[i - 1]->currentIndex > min) // will always be prefered unless its it the first children
                        {
                            borrowLeft(d, i, temp);
                            cout << "The current node[" << d.key << "] is a leaf node and adjacent left node has abundant keys so it will borrow from left" << endl;
                        }
                        else if (i != temp->size && temp->childs[i + 1] && temp->childs[i + 1]->currentIndex > min)
                        {
                            borrowRight(d, i, temp);
                            cout << "The current node[" << d.key << "] is a leaf node and adjacent right node has abundant keys so it will borrow from right" << endl;

                        }
                        else if (i != 0 && temp->childs[i - 1] && temp->childs[i - 1]->currentIndex + current->currentIndex < current->size)
                        {
                            mergeLeft(d, i, temp); // will happen if both siblings have minimum keys
                            if (current->parent->currentIndex == 0 && current->parent->parent == NULL)
                            {
                                root = temp->childs[0];
                            }
                            cout << "Both siblings have minimum number of keys so it will attempt to merge with left sibling" << endl;
                        }
                        else
                        {
                            mergeRight(d, i, temp);  //will only merger when no child at left
                            if (current->parent->currentIndex == 0 && current->parent->parent == NULL)
                            {
                                root = temp->childs[1];
                            }
                            cout << "Both siblings have minimum number of keys so it will attempt to merge with right sibling" << endl;
                        }

                        if (temp->parent && temp->currentIndex < min) // checking if parent has less than minimum keys after merging
                        {
                            if (temp != root)
                            {
                                while (temp != root && temp->currentIndex < min)
                                {
                                    print();
                                    cout << "Now parent is having problems regarding keys so it will either borrow or merge" << endl;
                                    fix(min, temp);
                                    temp = temp->parent;
                                }
                            }
                            if (temp->parent && temp->parent->currentIndex == 0 && temp->parent->parent == NULL)
                            {
                                root = temp;
                                temp->parent = NULL;
                            }
                        }


                    }
                }
            }
            else // internal node
            {
                if (current == root || current->currentIndex >= min) // leaf node has more than enough keys
                {
                    int i = 0;
                    while (i < current->currentIndex && current->keys[i].key < d.key)
                        i++;

                    Node<adt>* leftchild = current->childs[i];
                    Node<adt>* rightchild = current->childs[i + 1];

                    if (rightchild && rightchild->currentIndex >= min)
                    {
                        cout << "Current node[" << d.key << "] is an internal node so it is going to find successor" << endl;
                        adt tp = findSucc(min, rightchild, root);
                        del(d, current);
                        current->insertInNode(tp, current);
                    }
                    else if (leftchild && leftchild->currentIndex >= min)
                    {
                        cout << "Current node[" << d.key << "] is an internal node so it is going to find predeccesor" << endl;
                        adt tp = findPred(min, leftchild, root);
                        del(d, current);
                        current->insertInNode(tp, current);
                    }
                    else
                    {
                        cout << "Both children can not give keys so it will merge and shift its children" << endl;
                        fix(min, leftchild);
                        del(d, leftchild);
                        if (current->currentIndex < min && current != root)
                        {
                            print();
                            cout << "Now parent is having problems regarding keys so it will either borrow or merge" << endl;
                            fix(min, current);
                        }

                    }
                }
            }
        }
        else
        {
            cout << "\n---------------------ERROR------------------------" << endl;
            cout << "B-Tree Does not Contain the key required" << endl;
            cout << "------------------REQUEST FAILED--------------------" << endl;
            return;
        }
    }

    void printBTree(Node<adt>* root, int level = 0) {
        if (root != nullptr) {
            printBTree(root->childs[root->currentIndex], level + 1);
            for (int i = root->currentIndex - 1; i >= 0; --i) {
                for (int j = 0; j < level; ++j) {
                    std::cout << "   ";
                }
                std::cout << root->keys[i].key << "\n";
                printBTree(root->childs[i], level + 1);
            }
        }
    }

    void print()
    {
        if (!root)
        {
            cout << "Empty B-tree." << endl;
            return;
        }

        queue<Node<adt>*> bfsQueue;
        bfsQueue.push(root);

        while (!bfsQueue.empty())
        {
            int nodesAtCurrentLevel = bfsQueue.size();

            // Traverse nodes at the current level
            for (int i = 0; i < nodesAtCurrentLevel; ++i)
            {
                Node<adt>* current = bfsQueue.front();
                bfsQueue.pop();

                // Print the current node
                current->traverseNode();
                cout << "   ";

                // Enqueue child nodes
                for (int j = 0; j <= current->size; ++j)
                {
                    if (current->childs[j])
                    {
                        bfsQueue.push(current->childs[j]);
                    }
                }
            }

            cout << endl;
        }
        cout << endl;
    }

};
