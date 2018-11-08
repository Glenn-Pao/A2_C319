#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <string>
#include "CChordPeer.h"

using namespace std;

//Reads a set of instructions from a given text file. The file contains one instruction per line.Anything after a hash '#' symbol is to be treated as comment and is to be ignored.
void Read(string filename, CChordPeer* chord) 
{
	//open file stream
	ifstream fs;
	fs.open(filename.c_str());
	string line;

	//yeah, the file stream is ok
	if (fs.good())
	{
		//read every line
		while (getline(fs, line)) 
		{
			int theCommandType = 0;
		
			//search for comment character token in the instruction command
			string instruction = line.substr(0, line.find('#'));

			//get the attribute
			string attrib = instruction.substr(instruction.find(' ') + 1, instruction.length());
			//spot for white space that might be present after removing "#"
			size_t whiteSpace = instruction.find(' ');
			if (whiteSpace != std::string::npos)
			{
				//remove the stuff after "#" then process the actual command
				instruction.erase(whiteSpace, instruction.length());
			}

			//find the space in between the attribute
			//eg. InitChord" "5
			size_t spacebar = attrib.find_last_of(" ");

			if (spacebar != string::npos && isdigit(attrib[0]))
			{
				attrib.erase(attrib.find_last_of(" "), attrib.length());
			}
			
			// the different processes
			if (instruction == "StartChord") 
			{
				theCommandType = 0;
			}
			else if (instruction == "InsertPeer")
			{
				theCommandType = 1;
			}
			else if (instruction == "RemovePeer")
			{
				theCommandType = 2;
			}
			else if (instruction == "InsertData")
			{
				theCommandType = 3;
			}
			else if(instruction == "Print")
			{
				theCommandType = 4;
			}
			else if(instruction == "DeleteData")
			{
				theCommandType = 5;
			}
			else if(instruction == "Lookup")
			{
				theCommandType = 6;
			}
			else if(instruction == "Terminate")
			{
				theCommandType = 7;
			}

			//evaluate and execute
			switch(theCommandType)
			{
			case 0:		//start chord
				{
					if (chord == NULL) 
					{
						cout << "Student Name: Almeda Glenn" << endl;
						cout << "ID: 6212578" << endl;
						chord = new CChordPeer();
						chord->StartChord(atoi(attrib.c_str()));
					}
					else 
					{
						delete chord;
						cout << "Start Chord Command detected! Restarting the Chord.." << endl;
						cout << "Student Name: Almeda Glenn" << endl;
						cout << "ID: 6212578" << endl;
						chord = new CChordPeer();
						chord->StartChord(atoi(attrib.c_str()));
					}
				}
				break;
			case 1:		//insert peer
				chord->InsertPeer(atol(attrib.c_str()));
				break;
			case 2:		//remove peer
				{
					long tempID = atol(attrib.c_str());
					if (atol(attrib.c_str()) == chord->getID())
					{
						//if chord being removed, we need other peer to execute our func
						chord = chord->RemovePeer(atol(attrib.c_str()));

						if (chord == NULL)
							cout << "No more peers to remove" << endl;
					}
					else 
					{
						chord->RemovePeer(atol(attrib.c_str()));
					}
				}
				break;
			case 3:		//insert data
				chord->InsertData(attrib);
				break;
			case 4:		//print
				chord->Print(atol(attrib.c_str()));
				break;
			case 5:		//delete data
				chord->DeleteData(attrib);
				break;
			case 6:		//lookup
				chord->Lookup(atol(attrib.c_str()));
				break;
			case 7:		//terminate
				chord->Terminate();
				break;
			}
		}
		fs.close();
	}
	else 
	{
		cout << "Can't read the file.." << endl;
	}
}
int main(int argc, const char * argv[]) 
{
	CChordPeer* chord = NULL;
	if (argc > 0) 
	{
		cout << "Student Name: Almeda Glenn" << endl;
		cout << "ID: 6212578" << endl;
		Read(argv[1], chord);
	}
	else 
	{
		cout << "Missing the file name. Please enter a file name." << endl;
	}


	delete chord;

	return 0;
}
