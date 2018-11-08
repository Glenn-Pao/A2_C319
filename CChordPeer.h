#ifndef CHORDPEER_H
#define CHORDPEER_H
#include <string>
#include <fstream>
#include <map>
using namespace std;
/*
External notes on pointer system
Difference between *p1 and **p2
*p1 = &x -> holds address of x. p1 points to x
**p2 = &p1 -> holds address of p1. p2 is a pointer to p1
therefore p2 is a pointer to a pointer
*/

class CChordPeer
{
private:
	long theID;							//the peer ID
	long theChordSize;						//hash function needs this
	int n;								//num of peers in the chord
	bool removed;							//is it removed?

	CChordPeer* previous;						//the previous peer
	CChordPeer** theFingerTable;					//the finger table
	map<long, string> peerDataInfo;					//the peer data
public:
	CChordPeer(int n, long ID, CChordPeer* prev, CChordPeer* successor, bool removed = false);
	CChordPeer(int n, bool removed = false);
	CChordPeer();
	~CChordPeer();

	//-- other useful functions --//
	void MakeFingerTable(long terminatedID, CChordPeer* replacement, long from, long to, long count = 0);

	long getID();
	CChordPeer* getPredecessor();
	string getData(long theKey);

	void removeData(long target);
	void transferData(long first, string second);

	void setID(long theID);
	void setPrevPeer(CChordPeer* previous);

	void writeToDumpFile();

	//-- minimum requirement functions --//
	//Create and initialize a new Chord system of size 2^n
	void StartChord(int n);
	//Adds one new peer to your Chord system
	void InsertPeer(long ID);
	//Removes a given peer from Chord
	CChordPeer* RemovePeer(long ID);
	//This function searches for the peer responsible for the given (hash-)key
	CChordPeer* Lookup(long key);
	//Computes a hash key for a given data item
	long Hash(string hashTarget);
	//This function simulates that a peer inserts a data item into the Chord. The data item is given as a string parameter
	void InsertData(string data);
	//This function simulates that a peer requests the removal of a data item identified by the parameter string from the Chord
	void DeleteData(string data);
	// This function dumps all data currently being held by the Chord to a file “dump.txt” (one line for each data item), removes each of the peers in the Chord(delete finger tables), then terminates the	simulation.
	void Terminate();
	//This function will print information about the peer that is responsible for the given key
	void Print(long ID);
};
#endif
