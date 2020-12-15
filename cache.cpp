#include "cache.h"
#include <vector>


/*
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
	unsigned RecyckeAddress(unsigned set,unsigned way);

	
	double Get_Num_Hits();
	double Get_Num_Query();
	unsigned Get_Time();
	unsigned Get_Victim(unsigned address); //try to change
	void Set_LRU(unsigned way, unsigned set);//try to change
	unsigned Get_LRU(unsigned set);
	bool Is_Tag_in_Set(unsigned tag,unsigned set);
	unsigned Which_Way_in_Set(unsigned tag, unsigned set);
	unsigned GetSet(unsigned address);
	unsigned GetTag(unsigned address);
	void Assign(unsigned adderess,unsigned way,bool dirty);
	unsigned FindTag(unsigned set, unsigned way);
	unsigned FindDirty(unsigned address, unsigned way);
	void Set_Valid(unsigned set, unsigned way,bool valid);
	void Set_Drirty(unsigned set, unsigned way, bool dirty);
};
*/
void Level::Set_Drirty(unsigned set, unsigned way, bool dirty) {
	this->Table[set][way].dirty = dirty;
	
	return;
	
}

unsigned Level::FindTag(unsigned set, unsigned way) {
	return this->Table[set][way].tag;
}

unsigned Level::FindDirty(unsigned address, unsigned way){
	unsigned set = GetSet(address);
	return Table[set][way].dirty;
}

void Level::Assign(unsigned address,unsigned way,bool dirty) {
	unsigned tag = this->GetTag(address);
	unsigned set = this->GetSet(address);
	this->Set_LRU(way,set);
	this->Table[set][way].tag = tag;
	this->Table[set][way].valid = true;
	this->Table[set][way].dirty = dirty;
	return;
}
void Level::Set_Valid(unsigned set, unsigned way,bool valid) {
	this->Table[set][way].valid = valid;
	return;
}

double Level::Get_Num_Hits() {
	return this->Hits;
}
double Level::Get_Num_Query() {
	return this->Request_Num;
}
//counstructor
Level::Level(unsigned block_size,
	unsigned level_size, unsigned l_assoc, unsigned level_cyc) {
	this->Block_Size = block_size;
	this->Level_Size = level_size;
	this->L_Assoc = l_assoc;
	this->Level_Cyc = level_cyc;
	unsigned Num_of_blocks = pow(2, this->Level_Size - this->Block_Size); ///= pow(2,10-4)= 2^6
	this->Ways_Num= pow(2, this->L_Assoc);
	this->Sets_Num = Num_of_blocks / this->Ways_Num; //=2^6/2^2=2^4
	vector<vector<BLOCK>> NewTable(this->Sets_Num, vector<BLOCK>(this->Ways_Num));
	for(unsigned i=0; i<this->Sets_Num;i++)
		for (unsigned j = 0; j < this->Ways_Num; j++)
		{
			NewTable[i][j].dirty = false; //not must
			NewTable[i][j].LRU = 0;
			NewTable[i][j].valid = false;
			NewTable[i][j].tag = 0; //not must

		}
	this->Table = NewTable;
	

}
// check tag in the set, if not found return false
bool Level::Is_Tag_in_Set(unsigned tag, unsigned set){
	
	for (unsigned i = 0; i < this->Ways_Num; i++) {
		if(this->Table[set][i].valid ==true)
		{
			if(this->Table[set][i].tag==tag)
			return true;
		
		}
	}

	return false; 

}
//using only after getting true from Is_Tag_in_Set so it will find always the way
unsigned Level::Which_Way_in_Set(unsigned tag, unsigned set)
{
	for (unsigned i = 0; i < this->Ways_Num; i++)
	{
		if (this->Table[set][i].tag == tag) {
			return this->Table[set][i].tag;
		}
	}
	return 0;//not supposed to get here
}


unsigned Level::RecyckeAddress(unsigned set,unsigned way) {
	return ((this->Table[set][way].tag<< this->Set_Size) + set);
}

//choose way to write at him
unsigned Level::Get_Victim(unsigned address) {
	unsigned set = this->GetSet(address);
	//check if there is there is an empty way
	for (unsigned i = 0; i < this->Ways_Num; i++) {
		if (this->Table[set][i].valid == false)
		{
			return i;
		}
	}
	//in the case the cache is full
	return this->Get_LRU(set);
}

//update our way to be the last which was used 
void Level::Set_LRU(unsigned way, unsigned set)
{
	for (unsigned i = 0; i < this->Ways_Num; i++) {
		if (this->Table[set][i].LRU > this->Table[set][way].LRU)
		{
			this->Table[set][i].LRU--; 
		}
	}
	unsigned newest = (this->Ways_Num) - 1;
	this->Table[set][way].LRU = newest;
	return;
}

//find the way which wasn't used lately
unsigned Level::Get_LRU(unsigned set) {
	unsigned min_val= this->Table[set][0].LRU;
	unsigned min_index = 0;
	for(unsigned i=1;i<this->Ways_Num;i++)
	{
		if(this->Table[set][i].LRU<min_val)
		{
			min_val = this->Table[set][i];
			min_index = i;
		}
	}
	return min_index;
}

//return the current set
unsigned Level::GetSet(unsigned address){
	return (((1<<this->Block_Size)-1)&(address >> (this->Set_Size+this->Block_Size)));//creat mask for middle bits like in HW1. if not working: return ( address >> this->blockSize ) % pow(2,setSize);
}

//return the current tag
unsigned Level::GetTag(unsigned address){
	return address >> (this->Set_Size + this->Block_Size);
}



//return access time
unsigned Level::Get_Time(){
	return this->Tot_Time;
}

bool Level::Read(unsigned address){
	this->Tot_Time += this->Level_Cyc;
	this->Request_Num++;
	unsigned set = this->GetSet(address);
	unsigned tag = this->GetTag(address);
	if (this->Is_Tag_in_Set(address)){ //if address is found in level
		unsigned way = this->Which_Way_in_Set(tag, set);
		this->Set_LRU(way, set);
		this->Hits++;
		return true;
	}
	return false;
}
//relevant to level 2, this func in fast doesnt write in the cache (dirty)
bool  Level::Write_allocate(unsigned address) {
	this->Request_Num++;
	unsigned set = this->GetSet(address);
	unsigned tag = this->GetTag(address);
	this->Tot_Time += this->Level_Cyc;
	this->Request_Num++;
	if (this->Is_Tag_in_Set(tag,set) == true)
	{
		unsigned way = this->Which_Way_in_Set(tag, set);
		this->Hits++;
		this->Set_LRU(way, set);
		return true;
	}
	this->Misses++;
	return false;
}

bool Level::Write(unsigned address) {
	this->Request_Num++;
	unsigned set = this->GetSet(address);
	unsigned tag=this->GetTag(address);
	this->Tot_Time += this->Level_Cyc;
	this->Request_Num++;

	if (this->Is_Tag_in_Set(tag,set) == true)
	{
		unsigned way = this->Which_Way_in_Set(tag, set);
		this->Hits++;
		this->Table[set][way].dirty = true;
		this->Set_LRU(way, set);
		return true;
	}
	this->Misses++;
	return false;
}

class Cache {
	Level L1;
	Level L2;
	unsigned Cnt_Requests;
	bool Write_alloc;
	unsigned cnt_memory_time;
	unsigned Mem_Cyc;


public:
	//constructor
	Cache(unsigned mem_cyc, unsigned block_size_L1,unsigned block_size_L2, unsigned L1_size, unsigned L2_size, unsigned L1_assoc,
		unsigned L2_assoc, unsigned level1_cyc, unsigned level2_cyc, bool write_alloc);
	void Read(unsigned address);
	void Write(unsigned address);
	double Get_L1_Rate();
	double Get_L2_Rate();
	double Avg_Time();
	void snoop1(unsigned way,unsigned set);
	void snoop2(unsigned way, unsigned set);
	

};

void Cache::snoop1(unsigned way, unsigned set){
	unsigned tag = this->L1.FindTag(set, way);
	unsigned address = this->L1.RecyckeAddress( set, way);
	bool dirty = this->L1.FindDirty(address, way);
	this->L1.Set_Valid(set, way, false);
	unsigned Set_L2 = this->L2.GetSet(address);
	unsigned Tag_L2 = this->L2.GetTag(address);
	unsigned Way_L2 = this->L2.Which_Way_in_Set(Tag_L2, Set_L2);//no need to chek if exisit in L2 cause we know it's(cause it's also in L1)
	this->L2.Set_Drirty(Set_L2, Way_L2, dirty);
	return;
}

Cache::Cache(unsigned mem_cyc, unsigned block_size_L1, unsigned block_size_L2, unsigned L1_size, unsigned L2_size, unsigned L1_assoc,
	unsigned L2_assoc, unsigned level1_cyc, unsigned level2_cyc, bool write_alloc) {
	this->Mem_Cyc = mem_cyc;
	this->cnt_memory_time = 0;
	this->Cnt_Requests = 0;
	this->Write_alloc=write_alloc;
	Level L1_new = Level(block_size_L1, L1_size, L1_assoc, level1_cyc);
	Level L2_new = Level(block_size_L2, L2_size, L2_assoc, level2_cyc);
	this->L1 = L1_new;
	this->L2 = L2_new;

}

void Cache::snoop2(unsigned way, unsigned set) {
	unsigned address_victim = this->L2.RecyckeAddress(set, way);
	unsigned set_in_L1 = this->L1.GetSet(address_victim);
	unsigned way_in_L1;
	if ((this->L1.Check_Adress_L(address_victim, &way_in_L1) == true)) {
		this->L1.Set_Valid(set_in_L1, way_in_L1, false);
	}
	this->L2.Set_Valid(set, way, false);
	return;
}

void Cache::Write(unsigned address) {
	this->Cnt_Requests++;
	if (this->L1.Write(address) == true) { return; }
	//in case we don't wirte allocate
	if (this->Write_alloc == false) {
		if (this->L2.Write(address) == true) {
			return;
		}
		else
		{
			this->cnt_memory_time += this->Mem_Cyc;
			return;

		}
	}
						

	//in case the address wasn't found in L1 and write allocate

	if (this->L2.Write_allocate(address) == true)
	{
		unsigned victim = this->L1.Get_Victim(address);
		//snoop victim (using funcs of Level)
		//unsigned tag_L2 = this->L2.GetTag(address);
		unsigned set_L1 = this->L1.GetSet(address);
		unsigned tag_victim = this->L1.FindTag(set_L1, victim);
		unsigned address = this->L1.RecyckeAddress(set_L1, victim);
		
		this->snoop1(address);
		//assign "victim"
		this->L1.Assign(address, victim,true); // check if need false or L2.dirty?
		return;
	}

	//if the address not in the cache
	this->cnt_memory_time += this->Mem_Cyc;
	if (this->Write_alloc == false)
	{
		return;
	}
	//not write allocate
	unsigned victim = this->L2.Get_Victim(address);
	this->snoop(victim);
	this->L2.Assign(address, victim);
	victim = this->L1.Get_Victim(address);
	this->snoop(victim);

	this->L1.Assign(address, victim);
	return;


}

void Cache::Read(unsigned address){
	if (this->L1.Read(address)){//found in L1
		return;
	}
	else if (this->L2.Read(address)){//found in L2, bring to L1
		unsigned victim = this->L1.Get_Victim(address);
		unsigned dirty = this->L1.FindDirty(address,victim);
		this->L1.Assign(address, victim, dirty);
		return;
	}
	else{//bring from memory
		this->cnt_memory_time += this->Mem_Cyc;
		unsigned victim = this->L2.Get_Victim(address);
		unsigned dirty= this->L2.FindDirty(address,victim);
		this->L2.Assign(address, victim, dirty);
		victim = this->L1.Get_Victim(address);
		dirty = this->L1.FindDirty(address,victim);
		this->L1.Assign(address, victim, dirty);
		return;
	}
}

double Cache::Get_L1_Rate(){
	return ((double)this->L1.Get_Num_Hits/(double)this->L1.Get_Num_Query);
}
double Cache::Get_L2_Rate(){
	return ((double)this->L2.Get_Num_Hits/(double)this->L2.Get_Num_Query);
}
double Cache::Avg_Time(){
	double tot_time = this->cnt_memory_time + this->L1.Get_Time + this->L2.Get_Time;
	double tot_queries = this->Cnt_Requests + this->L1.Get_Num_Query + this->L2.Get_Num_Query;
	return (tot_time / tot_queries);
}