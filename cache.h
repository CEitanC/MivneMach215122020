#include <stdio.h>
#include <vector>
#include<cmath>


using namespace std;
using std::vector;

typedef struct _BLOCK {
	unsigned tag;
	unsigned LRU;
	bool dirty;
	bool valid;
} BLOCK, *PBLOCK;


class Level {
	vector<vector<BLOCK>> Table;
	unsigned Block_Size;
	unsigned Level_Size;
	unsigned L_Assoc;
	unsigned Level_Cyc;
	unsigned Tot_Time;
	unsigned Sets_Num;
	unsigned Request_Num;
	unsigned Hits;
	unsigned Misses;
	unsigned Set_Size;
	unsigned Ways_Num;


public:
	//counstructor
	Level(unsigned block_size, unsigned level_size, unsigned l_assoc,
		unsigned level_cyc);

	// Read
	bool Read(unsigned address);

	// Write
	bool Write(unsigned address);
	bool Write_allocate(unsigned address);
	unsigned RecyckeAddress(unsigned set, unsigned way);


	double Get_Num_Hits();
	double Get_Num_Query();
	unsigned Get_Time();
	unsigned Get_Victim(unsigned address); //try to change
	void Set_LRU(unsigned way, unsigned set);//try to change
	unsigned Get_LRU(unsigned set);
	bool Is_Tag_in_Set(unsigned tag, unsigned set);
	unsigned Which_Way_in_Set(unsigned tag, unsigned set);
	unsigned GetSet(unsigned address);
	unsigned GetTag(unsigned address);
	void Assign(unsigned adderess, unsigned way, bool dirty);
	unsigned FindTag(unsigned set, unsigned way);
	unsigned FindDirty(unsigned address, unsigned way);
	void Set_Valid(unsigned set, unsigned way, bool valid);
	void Set_Drirty(unsigned set, unsigned way, bool dirty);
};


class Cache {
	Level L1;
	Level L2;
	unsigned Cnt_Requests;
	bool Write_alloc;
	unsigned cnt_memory_time;
	unsigned Mem_Cyc;


public:
	//constructor
	Cache(unsigned mem_cyc, unsigned block_size_L1, unsigned block_size_L2, unsigned L1_size, unsigned L2_size, unsigned L1_assoc,
		unsigned L2_assoc, unsigned level1_cyc, unsigned level2_cyc, bool write_alloc);
	void Read(unsigned address);
	void Write(unsigned address);
	double Get_L1_Rate();
	double Get_L2_Rate();
	double Avg_Time();
	void snoop1(unsigned way, unsigned set);
	void snoop2(unsigned way, unsigned set);


};