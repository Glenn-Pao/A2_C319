#include "CChordPeer.h"
#include <iostream>
#include <math.h>
#include <sstream>

CChordPeer::CChordPeer(int n, long ID, CChordPeer* prev, CChordPeer* successor, bool removed)
{
	this->n = n;
	this->removed = removed;
	theID = ID;
	previous = prev;
	theChordSize = 1 << n;
	
	//establish the finger table
	theFingerTable = new CChordPeer*[n];

	int i = 0;
	while (i < n)
	{
		theFingerTable[i] = successor;
		i++;
	}
	//update the data this peer holds & remove the info from old peer
	//check if it is not empty
	if (successor->peerDataInfo.begin() != successor->peerDataInfo.end()) 
	{
		map<long, string>::iterator it = successor->peerDataInfo.begin();

		while (it != successor->peerDataInfo.end()) 
		{
			//if the first <= this ID
			if (it->first <= this->getID()) 
			{
				transferData(it->first, it->second);
				//remove the next peer info
				successor->peerDataInfo.erase(it++);
			}
			//if the first > successor's predecessing ID
			else if (it->first > successor->getPredecessor()->getID())
			{
				//check if first > this ID and < successor's predecessing ID
				if (it->first > this->theID && this->theID < successor->getPredecessor()->getID())
				{
					transferData(it->first, it->second);
					successor->peerDataInfo.erase(it++);
				}
			}
			//if the first < successor's predessing ID
			else if (it->first < successor->getPredecessor()->getID()) 
			{
				//check if first < this ID and successor's predessing ID
				if (it->first < this->theID && this->theID < successor->getPredecessor()->getID())
				{
					transferData(it->first, it->second);
					//remove the next peer info
					successor->peerDataInfo.erase(it++);
				}
			}
			else 
			{
				//proceed to the next
				++it;
			}
		}
	}

	//in the scenario where there are only 2 peers in the system
	prev->theFingerTable[0] = this;
	successor->setPrevPeer(this);

	//create the finger table
	MakeFingerTable(-1, this, prev->theID, theID);
}
CChordPeer::CChordPeer(int n, bool removed)
{
	this->n = n;
	this->removed = removed;
	theID = 0;
	previous = this;
	theChordSize = 1 << n;

	//establish the finger table
	theFingerTable = new CChordPeer*[n];

	int i = 0;
	while (i < n)
	{
		theFingerTable[i] = this;	//initialize to self since there is only 1 peer here
		i++;
	}
}
//default constructor
CChordPeer::CChordPeer()
{
	this->removed = false;
	theID = 0;
	previous = this;
}
CChordPeer::~CChordPeer()
{
	//free all dynamic memory
	CChordPeer* temp = previous;
	removed = true;

	if (previous) 
	{
		previous = NULL;
		delete previous;
	}

	if (theFingerTable) 
	{
		delete[] theFingerTable;
		theFingerTable = NULL;
	}

	if (temp != NULL && !temp->removed)
		delete temp;
}
void CChordPeer::transferData(long first, string second)
{
	pair<long, string> theNewPair;
	theNewPair.first = first;
	theNewPair.second = second;
	this->peerDataInfo.insert(theNewPair);
}
//Create and initialize a new Chord system of size 2^n
void CChordPeer::StartChord(int n)
{
	this->n = n;
	theChordSize = 1 << n;

	//establish the finger table
	theFingerTable = new CChordPeer*[n];

	int i = 0;
	while (i < n)
	{
		theFingerTable[i] = this;	//initialize to self since there is only 1 peer here
		i++;
	}
}
//Adds one new peer to your Chord system
void CChordPeer::InsertPeer(long ID)
{
	//get the successor ID
	CChordPeer* successor = Lookup(ID);

	//target isn't a peer
	if (successor->getID() != ID)
	{
		//establish the new peer
		CChordPeer *newPeer = new CChordPeer(n, ID, successor->getPredecessor(), successor);

		cout << "Peer " << ID << " added" << endl;
	}
	else
	{
		cout << "Peer already exists!" << endl;
	}
	//cout << "=======================" << endl;
	successor = NULL;
	delete successor;
}
//Removes a given peer from Chord
CChordPeer* CChordPeer::RemovePeer(long ID)
{
	CChordPeer *targetToRemove = Lookup(ID);

	//if there is more than 1 peer left
	if (targetToRemove->getPredecessor() != targetToRemove)
	{
		CChordPeer *successor = targetToRemove->theFingerTable[0];
		CChordPeer *predecessor = targetToRemove->getPredecessor();

		//establish the successor and predecessor
		successor->setPrevPeer(predecessor);
		predecessor->theFingerTable[0] = successor;

		//set the flag that this peer is going to be removed
		removed = true;

		map<long, string>::iterator it = targetToRemove->peerDataInfo.begin();
		while (it != targetToRemove->peerDataInfo.end()) 
		{
			//transfer the data to successor
			pair<long, std::string> thePair;
			thePair.first = it->first;
			thePair.second = it->second;
			successor->peerDataInfo.insert(thePair);
			targetToRemove->peerDataInfo.erase(it++);
		}
		predecessor->MakeFingerTable(-1, successor, predecessor->getID(), targetToRemove->getID());

		//remove the node once the finger table has been reorganized
		targetToRemove->previous = NULL;

		for (int i = 0; i < targetToRemove->n; i++)
		{
			targetToRemove->theFingerTable[i] = NULL;
		}

		cout << "Peer " << targetToRemove->getID() << " has been removed." << endl;
		//cout << "=======================" << endl;
		delete targetToRemove;
		return successor;
	}
	//it is only 1
	else 
	{	
		delete targetToRemove;
		cout << "This is the last peer. CHORD is terminated." << endl;
		return NULL;
	}
}
void CChordPeer::removeData(long target)
{
	if (peerDataInfo.find(target) != peerDataInfo.end())
	{
		peerDataInfo.erase(target);
	}
	else 
	{
		cout << "Error: Data does not exist.";
	}
	
}
//This function searches for the peer responsible for the given (hash-)key
CChordPeer* CChordPeer::Lookup(long key)
{
	CChordPeer *successor = this->theFingerTable[0];

	//need to see if it is showing the correct ID
	cout << theID;

	//it's the only node in the network so...nothing happens
	//OR if the key is the same as the current node ID
	if (this == getPredecessor() || theID == key)
	{
		cout << endl;
		return this;
	}
	//if the key < this ID
	else if (key < theID)
	{
		if (key <= successor->getID() && successor->getID() < theID)
		{
			cout << ">" << successor->getID() << endl;
			return successor;
		}
	}
	//if the key > this ID
	else if (key > theID)
	{
		//check if key <= successor ID
		//OR if key >= successor ID but successor ID < this ID
		if (key <= successor->getID() || (key >= successor->getID() && successor->getID() < theID))
		{
			cout << ">" << successor->getID() << endl;
			return successor;
		}
	}
	
	//search for the next successor using brute force method if it passes through to this point
	unsigned long nodeID = 0;
	int indexSuccessor = 0;		//the index in the finger table of the next successor
	int i = 0;

	while (i < n)
	{
		if (key == theFingerTable[i]->getID() && !theFingerTable[i]->removed)
		{
			indexSuccessor = i;
			break;
		}
		else if (key > theFingerTable[i]->getID())
		{
			//find out if successor's finger table's ID is < i's finger table's ID
			if (theFingerTable[indexSuccessor]->getID() < theFingerTable[i]->getID() && !theFingerTable[i]->removed)
			{
				indexSuccessor = i;
			}
		}
		i++;
	}
	cout << ">";

	return theFingerTable[indexSuccessor]->Lookup(key);	//search the next peer
}
//Computes a hash key for a given data item
long CChordPeer::Hash(string hashTarget)
{
	//the initial pseudo code given
	/*
	BEGIN Hash (string)
	UNSIGNED INTEGER key = 0;
	FOR_EACH character IN string
	key = ((key << 5) + key) ^ character;
	END FOR_EACH
	RETURN key;
	END Hash
	*/

	unsigned long hashedNum = 0;	//int may not be enough. use long instead
	int i = 0;						//iterator
	while (i < hashTarget.length())
	{
		//bitshift to the left 5 times
		hashedNum = ((hashedNum << 5) + hashedNum) ^ hashTarget[i];
		i++;
	}
	//use the remainder. it's likely to be really big
	unsigned long finalHash = hashedNum % theChordSize;

	return finalHash;
}
//This function simulates that a peer inserts a data item into the Chord. The data item is given as a string parameter
void CChordPeer::InsertData(string data)
{
	//hash the data
	long target = Hash(data);
	CChordPeer *thePeer = Lookup(target);

	thePeer->transferData(target, data);

	cout << data << " (key = " << target << ") from " << thePeer->getID() << " has been inserted." << endl;
	//cout << "=======================" << endl;
	thePeer = NULL;
	delete thePeer;
}
//This function simulates that a peer requests the removal of a data item identified by the parameter string from the Chord
void CChordPeer::DeleteData(string data)
{
	//hash the data
	long target = Hash(data);
	CChordPeer *thePeer = Lookup(target);

	thePeer->removeData(target);

	cout << data << " (key = " << target << ") from " << thePeer->getID() << " has been removed." << endl;
	//cout << "=======================" << endl;
}
//write to a dump file
void CChordPeer::writeToDumpFile()
{
	ofstream file;
	
	//open the file
	file.open ("dump.txt");
	if(file.good())
	{
		CChordPeer *thePeer = Lookup(this->getPredecessor()->getID());
		//lookup every possible peer
		while(thePeer->getPredecessor() != thePeer)
		{
			//establish another reference
			CChordPeer* anotherRef = Lookup(thePeer->getID());

			//record data into file
			//the node data
			file << "Data at node " << anotherRef->getID() << " :" << endl;
			for (map<long, string>::iterator it = anotherRef->peerDataInfo.begin(); it != anotherRef->peerDataInfo.end(); it++)
			{
				file << it->second << endl;
			}
			
			//the finger table data
			file << "The finger table of node " << anotherRef->getID() << " :" << endl;

			for (int i = 0; i < n; i++) 
			{
				file << anotherRef->theFingerTable[i]->getID() << " ";
			}
			file << endl;
			
			//do the printout
			anotherRef->Print(anotherRef->getID());
						
			//search for its predecessor
			thePeer = anotherRef->getPredecessor();

			//remove the peer
			thePeer->RemovePeer(anotherRef->getID());
			
			//dump the memory reference point
			anotherRef = NULL;
			delete anotherRef;
		}
		//record the final data into file
		//the node data
		file << "Data at node " << thePeer->getID() << " :" << endl;

		for (map<long, string>::iterator it = thePeer->peerDataInfo.begin(); it != thePeer->peerDataInfo.end(); it++)
		{
			file << it->second << endl;
		}
		
		//the finger table data
		file << "The finger table of node " << thePeer->getID() << " :" << endl;

		for (int i = 0; i < n; i++) 
		{
			file << thePeer->theFingerTable[i]->getID() << " ";
		}
		file << endl;
		
		//do the printout
		thePeer->Print(thePeer->getID());
					
		//remove itself
		thePeer->RemovePeer(thePeer->getID());
	}
	file.close();
}
// This function dumps all data currently being held by the Chord to a file dump.txt (one line for each data item), removes each of the peers in the Chord(delete finger tables), then terminates the	simulation.
void CChordPeer::Terminate()
{
	//basically, dump everything and record the details
	writeToDumpFile();
	
}
//This function will print information about the peer that is responsible for the given key
void CChordPeer::Print(long ID)
{
	CChordPeer* thePeer = Lookup(ID);
	cout << "Data at node " << thePeer->getID() << " :" << endl;

	for (map<long, string>::iterator it = thePeer->peerDataInfo.begin(); it != thePeer->peerDataInfo.end(); it++)
	{
		cout << it->second << endl;
	}

	cout << "The finger table of node " << thePeer->getID() << " :" << endl;

	for (int i = 0; i < n; i++) 
	{
		cout << thePeer->theFingerTable[i]->getID() << " ";
	}
	cout << endl;
	//dump the memory
	thePeer = NULL;
	delete thePeer;
}
void CChordPeer::MakeFingerTable(long terminatedID, CChordPeer* replacement, long from, long to, long count)
{
	//begin the finger table process
	CChordPeer* successor = theFingerTable[0];
	unsigned long nodeID = 0;	//prevent negative numbers as ID numbers

	if (terminatedID != theID || count < (theChordSize / 2))
	{
		//create a starting point
		if (successor->getPredecessor() != this)
		{
			theFingerTable[0] = successor->getPredecessor();
		}
		else
		{
			for (int i = 0; i < n; i++)
			{
				//get the next nodeID
				nodeID = theID + pow(2, i);
				nodeID = nodeID % theChordSize;

				//if node ID is the destination
				if (nodeID == to)
				{
					theFingerTable[i] = replacement;
				}
				//if the node ID is more than the min but less than the max
				else if (nodeID > from && nodeID <= to)
				{
					theFingerTable[i] = replacement;
				}
				//if the node ID is more than from and is greater than or equal to "to" but to < from
				else if (nodeID > from && nodeID >= to && to < from)
				{
					theFingerTable[i] = replacement;
				}
				//if the node ID is less than from and is smaller than or equal to "to" but to < from
				else if (nodeID < from && nodeID <= to && to < from)
				{
					theFingerTable[i] = replacement;
				}
			}
		}

		//set up the previous node's table
		count++;

		//reached the end
		if (terminatedID == -1)
		{
			previous->MakeFingerTable(theID, replacement, from, to, count);
		}
		else
		{
			//continue this cycle
			previous->MakeFingerTable(terminatedID, replacement, from, to, count);
		}
	}
	else
	{
		//finger table is fixed
		return;
	}
}
long CChordPeer::getID()
{
	return theID;
}
CChordPeer* CChordPeer::getPredecessor()
{
	return previous;
}
string CChordPeer::getData(long theKey)
{
	if (peerDataInfo.find(theKey) != peerDataInfo.end())
	{
		return peerDataInfo[theKey];
	}
	else
	{
		cout << "Data cannot be found in this map.." << endl;
	}
	return "";
}
void CChordPeer::setID(long theID)
{
	this->theID = theID;
}
void CChordPeer::setPrevPeer(CChordPeer* previous)
{
	this->previous = previous;
}
