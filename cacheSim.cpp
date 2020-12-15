/* 046267 Computer Architecture - Winter 20/21 - HW #2 */

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include "cache.h"




using std::FILE;
using std::string;
using std::cout;
using std::endl;
using std::cerr;
using std::ifstream;
using std::stringstream;



int main(int argc, char **argv) {

	if (argc < 19) {
		cerr << "Not enough arguments" << endl;
		return 0;
	}

	// Get input arguments

	// File
	// Assuming it is the first argument
	char* fileString = argv[1];
	ifstream file(fileString); //input file stream
	string line;
	if (!file || !file.good()) {
		// File doesn't exist or some other error
		cerr << "File not found" << endl;
		return 0;
	}

	unsigned MemCyc = 0, BSize = 0, L1Size = 0, L2Size = 0, L1Assoc = 0,
			L2Assoc = 0, L1Cyc = 0, L2Cyc = 0, WrAlloc = 0;

	for (int i = 2; i < 19; i += 2) {
		string s(argv[i]);
		if (s == "--mem-cyc") {
			MemCyc = atoi(argv[i + 1]);
		} else if (s == "--bsize") {
			BSize = atoi(argv[i + 1]);
		} else if (s == "--l1-size") {
			L1Size = atoi(argv[i + 1]);
		} else if (s == "--l2-size") {
			L2Size = atoi(argv[i + 1]);
		} else if (s == "--l1-cyc") {
			L1Cyc = atoi(argv[i + 1]);
		} else if (s == "--l2-cyc") {
			L2Cyc = atoi(argv[i + 1]);
		} else if (s == "--l1-assoc") {
			L1Assoc = atoi(argv[i + 1]);
		} else if (s == "--l2-assoc") {
			L2Assoc = atoi(argv[i + 1]);
		} else if (s == "--wr-alloc") {
			WrAlloc = atoi(argv[i + 1]);
		} else {
			cerr << "Error in arguments" << endl;
			return 0;
		}
	}

	while (getline(file, line)) {

		stringstream ss(line);
		string address;
		char operation = 0; // read (R) or write (W)
		if (!(ss >> operation >> address)) {
			// Operation appears in an Invalid format
			cout << "Command Format error" << endl;
			return 0;
		}

		// DEBUG - remove this line
		cout << "operation: " << operation;

		string cutAddress = address.substr(2); // Removing the "0x" part of the address

		// DEBUG - remove this line
		cout << ", address (hex)" << cutAddress;

		unsigned long int num = 0;
		num = strtoul(cutAddress.c_str(), NULL, 16);

		// DEBUG - remove this line
		cout << " (dec) " << num << endl;

	}

	double L1MissRate;
	double L2MissRate;
	double avgAccTime;

	printf("L1miss=%.03f ", L1MissRate);
	printf("L2miss=%.03f ", L2MissRate);
	printf("AccTimeAvg=%.03f\n", avgAccTime);

	return 0;
}

class Level {
	unsigned Mem_Cycle;
	unsigned Block_Size;
	unsigned Level_Size;
	unsigned L_Assoc;
	unsigned Level_Cyc;
	unsigned Write_Alloc;
	//Level* Higher_L;
	//Level* Lower_L;
	unsigned Tot_Time;
	unsigned Block_in_Way;
	unsigned Request_Num;
	unsigned Hits;
	unsigned Miss;
	unsigned Set_Size;
	unsigned Ways_Num;
	vector<vector<unsigned>> Tag_Vec;
	vector<vector<bool>> Valid_Vec;
	vector<vector<bool>> Dirty_Vec;
	vector<vector<unsigned>> LRU_Vec;



public:
	//counstructor
	Level(unsigned mem_cyc, unsigned block_size, unsigned write_alloc, unsigned level_size, unsigned l_assoc,
		unsigned level_cyc);

	// Read
	void Read(unsigned address, unsigned level_num, bool* valid_bit);

	// Write
	void Write(unsigned address, unsigned level_num);

	//void Set_Higher_L(Level* L);
	//void Set_Lower_L(Level* L);
	double Get_Rate();
	unsigned Get_Time();
	void Get_Victim(unsigned adress, int* way, unsigned* block);
	void Set_LRU(unsigned way, unsigned set);
	unsigned Get_LRU(unsigned set);
	bool Check_Adress_L(unsigned adress, unsigned* way);
	unsigned GetSet(unsigned address);
	unsigned GetTag(unsigned address);
};

class Cache {
	Level L1;
	Level L2;
	unsigned Cnt_Requests;

public:
	//constructor
	Cache(unsigned mem_cyc, unsigned block_size, unsigned L1_size, unsigned L2_size, unsigned L1_assoc,
		unsigned L2_assoc, unsigned level1_cyc, unsigned level2_cyc, unsigned write_alloc);
	void Read(unsigned address);
	void Write(unsigned address);
	double Get_L1_Rate();
	double Get_L2_Rate();
	double Avg_Time();
	void snoop(Level higher_level, unsigned address);

};

