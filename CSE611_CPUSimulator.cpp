#include <iostream>
#include <deque>
#include <bit>
#include <math.h>
#include <bitset>

void dispatch(int string) {
	int operation = string >> 26;
	int source = ((string << 6) >> 26) & (128-1);
	int sink = ((string << 12) >> 26) & (128 - 1);
	int dest = (string << 18) >> 18;
	std::cout << "string: " << std::bitset<32>(string) << std::endl;
	std::cout << "operation: " << std::bitset<6>(operation) << std::endl;
	std::cout << "source: " << std::bitset<32>(source) << std::endl;
	std::cout << "sink: " << std::bitset<32>(sink) << std::endl;
	std::cout << "dest: " << std::bitset<14>(dest) << std::endl;
	switch (operation) {
	case 0: //nop
		std::cout << "nop" << std::endl;
		break;
	case 1: //addi
		std::cout << "add " << (source >> 5 ? "#" : "r") << source % 32 << ", " << (sink >> 5 ? "#" : "r") << sink % 32 << ", r" << (int)dest << std::endl;
		break;
	case 2: //subi
		std::cout << "sub " << (source >> 5 ? "#" : "r") << source % 32 << ", " << (sink >> 5 ? "#" : "r") << sink % 32 << ", r" << (int)dest << std::endl;
		break;
	case 3: //muli
		std::cout << "mul " << (source >> 5 ? "#" : "r") << source % 32 << ", " << (sink >> 5 ? "#" : "r") << sink % 32 << ", r" << (int)dest << std::endl;
		break;
	case 4: //divi
		std::cout << "div " << (source >> 5 ? "#" : "r") << source % 32 << ", " << (sink >> 5 ? "#" : "r") << sink % 32 << ", r" << (int)dest << std::endl;
		break;
	case 5: //brezi
		std::cout << "brez " << (source >> 5 ? "#" : "r") << std::endl;
		break;
	case 6: //breqi
		std::cout << "breq " << (source >> 5 ? "#" : "r") << source % 32 << ", " << (sink >> 5 ? "#" : "r") << sink % 32 << std::endl;
		break;
	case 7: //brlti
		std::cout << "brlt " << (source >> 5 ? "#" : "r") << source % 32 << ", " << (sink >> 5 ? "#" : "r") << sink % 32 << std::endl;
		break;
	case 8: //brgti
		std::cout << "brgt " << (source >> 5 ? "#" : "r") << source % 32 << ", " << (sink >> 5 ? "#" : "r") << sink % 32 << std::endl;
		break;
	case 9: //br			
		std::cout << "br " << std::endl;
		break;
	case 10: //AND
		std::cout << "and " << (source >> 5 ? "#" : "r") << source % 32 << ", " << (sink >> 5 ? "#" : "r") << sink % 32 << ", r" << (int)dest << std::endl;
		break;
	case 11: //OR
		std::cout << "or " << (source >> 5 ? "#" : "r") << source % 32 << ", " << (sink >> 5 ? "#" : "r") << sink % 32 << ", r" << (int)dest << std::endl;
		break;
	case 12: //XOR
		std::cout << "xor " << (source >> 5 ? "#" : "r") << source % 32 << ", " << (sink >> 5 ? "#" : "r") << sink % 32 << ", r" << (int)dest << std::endl;
		break;
	case 13: //shift left
		std::cout << "shl " << (source >> 5 ? "#" : "r") << source % 32 << ", " << (sink >> 5 ? "#" : "r") << sink % 32 << ", r" << (int)dest << std::endl;
		break;
	case 14: //shift right
		std::cout << "shr " << (source >> 5 ? "#" : "r") << source % 32 << ", " << (sink >> 5 ? "#" : "r") << sink % 32 << ", r" << (int)dest << std::endl;
		break;
	case 15: //ld
		std::cout << "ld " << (sink >> 5 ? "#" : "r") << sink % 32 << ", r" << (int)dest << std::endl;
		break;
	case 16: //st
		std::cout << "st " << (source >> 5 ? "#" : "r") << source % 32 << ", " << (sink >> 5 ? "#" : "r") << std::endl;
		break;
	case 25: //end
		std::cout << "end  " << std::endl;
		break;
	}
}

#pragma region Queue
template <typename utype>class self_queue {
private:
	utype* data;
	int size;
	int max_size;
	int head;
public:
	self_queue() {
		this->size = 0;
		this->max_size = 0;
		this->head = 0;
		this->data = NULL;
	}

	void set(int size) {
		this->max_size = size;
		this->size = this->head = 0;
		this->data = new utype[size];
	}

	bool full() {
		return this->size == this->max_size;
	}

	bool empty() {
		return this->size == 0;
	}

	void push_back(utype input) {
		if (this->full())
			return;
		int tail = (this->head + this->size) % this->max_size;;
		data[tail] = input;
		this->size++;
	}

	utype* back() {
		if (this->empty())
			return NULL;
		int tail = (this->head + this->size - 1) % this->max_size;
		return this->data + tail;
	}

	utype* front() {
		if (this->empty())
			return NULL;
		return this->data + head;
	}

	void pop_front() {
		if (this->empty())
			return;
		this->head = (this->head + 1) % this->max_size;
		this->size--;
	}

};
#pragma endregion

#pragma region Instruciton
class instruction {
public:
	int PC;
	int branch_PC;
	int operation;
	//register numbers 
	int dest;
	int source;
	int sink;
	//renamed register numbers
	int rDest;
	int rSource;
	int rSink;
	//register data
	int dSource;
	int dSink;

	int result;

	bool integer;
	int memory; // write = 1 ,read = -1, none = 0
	bool prediction;
	int* PHT;
	bool T;
	bool completed;

	instruction() {
		this->PC = 0;
		this->branch_PC = 0;
		this->operation = -1;
		this->dest = -1;
		this->source = -1;
		this->sink = -1;
		this->rDest = -1;
		this->rSource = -1;
		this->rSink = -1;
		this->dSource = 0;
		this->dSink = 0;
		this->result = 0;
		this->memory = 0;
		this->prediction = false;
		this->PHT = NULL;
		this->T = false;
		this->completed = false;
		this->integer = false;
	}

	void dispatch(int string) {
		int operation = string >> 26;
		int source = ((string << 6) >> 26) & (128 - 1);
		int sink = ((string << 12) >> 26) & (128 - 1);
		int dest = (string << 18) >> 18;
		if (source >> 5) {
			this->dSource = source % 32;
		}
		else {
			this->source = source % 32;
		}
		if (sink >> 5) {
			this->dSink = sink % 32;
		}
		else {
			this->sink = sink % 32;
		}
		this->dest = dest;
		switch (operation) {
		case 0: //nop
			this->dest = -1;
			this->source = -1;
			this->sink = -1;
			this->operation = 0;
			break;
		case 1: //addi
			this->operation = 1;
			break;
		case 2: //subi
			this->operation = 2;
			break;
		case 3: //muli
			this->operation = 3;
			break;
		case 4: //divi
			this->operation = 4;
			break;
		case 5: //brezi
			this->branch_PC = dest << 2;
			this->dest = -1;
			this->sink = -1;
			this->T = true;
			this->operation = 5;
			break;
		case 6: //breqi
			this->branch_PC = dest << 2;
			this->dest = -1;
			this->T = true;
			this->operation = 6;
			break;
		case 7: //brlti
			this->branch_PC = dest << 2;
			this->dest = -1;
			this->T = true;
			this->operation = 7;
			break;
		case 8: //brgti
			this->branch_PC = dest << 2;
			this->dest = -1;
			this->T = true;
			this->operation = 8;
			break;
		case 9: //br			
			this->branch_PC = dest << 2;
			this->dest = -1;
			this->sink = -1;
			this->source = -1;
			this->prediction = true;
			this->operation = 9;
			break;
		case 10: //AND
			this->operation = 10;
			break;
		case 11: //OR
			this->operation = 11;
			break;
		case 12: //XOR
			this->operation = 12;
			break;
		case 13: //shift left
			this->operation = 13;
			break;
		case 14: //shift right
			this->operation = 14;
			break;
		case 15: //ld
			this->memory = -1;
			this->source = -1;
			this->operation = 15;
			break;
		case 16: //st
			this->memory = 1;
			this->dest = -1;
			this->operation = 16;
			break;
		case 17: //addf
			this->operation = 1;
			break;
		case 18: //subf
			this->operation = 2;
			break;
		case 19: //mulf
			this->operation = 3;
			break;
		case 20: //divf
			this->operation = 4;
			break;
		case 21: //brezf
			this->branch_PC = dest << 2;
			this->dest = -1;
			this->sink = -1;
			this->T = true;
			this->operation = 5;
			break;
		case 22: //breqf
			this->branch_PC = dest << 2;
			this->dest = -1;
			this->T = true;
			this->operation = 6;
			break;
		case 23: //brltf
			this->branch_PC = dest << 2;
			this->dest = -1;
			this->T = true;
			this->operation = 7;
			break;
		case 24: //brgtf
			this->branch_PC = dest << 2;
			this->dest = -1;
			this->T = true;
			this->operation = 8;
			break;
		case 25:
			this->operation = 17;
			break;
		default:
			this->operation = 0;
		}
		if (operation < 17) {
			this->integer = true;
		}
	}

	bool miss() {
		if (this->PHT)
			return this->T != this->prediction;
		else
			return false;
	}
};
#pragma endregion

#pragma region Ports

class access_ports {
private:
	int time;
	int *ports;
	int size;
	int max_size;
public:
	access_ports() {
		this->time = 0;
		this->ports = NULL;
		this->size = 0;
		this->max_size = 0;
	}

	void set(int time, int size) {
		this->time = time;
		this->max_size = size;
		this->ports = new int[size];
		for (int i = 0; i < this->max_size; i++) {
			ports[i] = 0;
		}
	}

	bool full() { //return true if no ports are available
		return this->size == this->max_size;
	}

	int add_request() { //add access request to queue, if no ports available, return negative
		if (this->full())
			return -1;
		for (int i = 0; i < this->max_size; i++) {
			if (this->ports[i] == 0) {
				this->ports[i] = this->time;
				this->size++;
				return i;
			}
		}// put a timer in the queue
		return -1;
	}

	bool access(int port) {
		if (port < 0)
			return false;
		if (--this->ports[port] <= 0) {
			this->size--;
			return true;
		}
		
		return false;
	}

	void reset() {
		for (int i = 0; i < this->max_size; i++) {
			ports[i] = 0;
		}
		this->size = 0;
	}
};

template <typename utype> class port {
private:
	int port_no; //Port idle: -2, Port requesting: -1, Port waiting: > 0
	utype* access_unit;
public:
	port(utype* access_unit) {
		this->access_unit = access_unit;
		this->port_no = -2;
	}
	port() {
		this->access_unit = NULL;
		this->port_no = -2;
	}

	void set(utype* access_unit) {
		this->access_unit = access_unit;
	}

	bool access() { //if memory port is idle, return false. If memory port is 
		if (this->access_unit) {
			if (this->port_no == -2)
				return false;
			if (this->port_no == -1) {
				this->port_no = this->access_unit->request();
				return false;
			}

			if (this->access_unit->access(this->port_no)) {
				this->port_no = -2;
				return true;
			}
		}
		return false;
	}

	utype* unit() {
		return access_unit;
	}

	void request() {
		if (this->port_no == -2) {
			this->port_no = -1;
		}
	}

	bool waiting() {
		return this->port_no != -2;
	}

	void reset() {
		this->port_no = -2;
		this->access_unit->reset_ports();
	}
};

#pragma endregion

#pragma region Memory
class memory_unit {
private:
	int *data;
	access_ports ports;
public:
	memory_unit(int size, int ports, int time) {
		this->ports.set(time, ports);
		this->data = new int[size];
	}

	bool access(int port) {
		return this->ports.access(port);
	}

	int read(int address) {
		return this->data[address >> 2];
	}

	void read(instruction* input) {
		input->result = this->data[input->dSink >> 2];
	}

	void write(int address, int data) {
		this->data[address >> 2] = data;
	}

	void write(instruction *input) {
		this->data[input->dSink >> 2] = input->dSource;
	}

	int request() {
		return this->ports.add_request();
	}

	void reset_ports() {
		this->ports.reset();
	}
};
#pragma endregion

#pragma region Cache

struct cache_entry {
	int data;
	bool valid;

	void write(instruction* input) {
		this->data = input->dSource;
	}
	void read(instruction* input) {
		input->result = this->data;
	}
};

class cache_block {
	public:
		cache_entry* entry;
		int tag;
		bool FIFO;
		

		cache_entry* read(int offset) {
			return this->entry + (offset >> 2);
		}

		bool valid(int offset) {
			return this->entry[offset >> 2].valid;
		}

};

class cache {
	private:
		cache_block* block; // cache data unit
		int entries;
		int block_size;
		int set_size;
		access_ports ports;
	public:

		cache(int entries, int block_size, int set_size, int ports, int time) {
			this->ports.set(time, ports);
			this->block = new cache_block[entries];
			this->entries = entries; //number of 
			this->block_size = block_size; //number of bytes addressable per block
			this->set_size = set_size;

			for (int i = 0; i < entries; i++) {
				this->block[i].entry = new cache_entry[block_size/4];
				if (i % set_size == 0)
					this->block[i].FIFO = 1;
				for (int j = 0; j < block_size/4; j++) {
					this->block[i].entry[j].data = 0;
					this->block[i].entry[j].valid = false;
				}
			}

		}

		cache_entry* find(int address) {
			//get offset, tag, and index from memory address
			int offset = (address&(this->block_size - 1));
			int tag = address >> (int)log2((double)(this->entries * this->block_size/this->set_size));
			int index = (address >> (int)log2((double)this->block_size))&(this->entries/this->set_size-1);
			//find tag from tag list, set-associative listed in order in array
			cache_entry* data = NULL;
			for (int i = 0; i < this->set_size; i++) {

				if (tag == this->block[index * this->set_size + i].tag && (this->block[index * this->set_size + i].valid(offset))) //if tag matches and data is valid, get data
					data = (this->block[index * this->set_size + i].read(offset));
			}
			
			return data;
		}

		cache_entry* find(instruction* input) {
			return this->find(input->dSink);
		}

		template <typename memtype>
		void miss(int address, memtype *memory) {
			//get offset, tag, and index from memory address
			int offset = (address & (this->block_size - 1));
			int tag = address >> (int)log2((double)(this->entries * this->block_size/this->set_size));
			int index = (address >> (int)log2((double)this->block_size)) & (this->entries / this->set_size - 1);
			
			//find next cache block to be overwritten, i.e. with FIFO bit set
			cache_block* block = NULL;
			for (int i = 0; i < this->set_size; i++) {

				if (this->block[index * this->set_size + i].FIFO) {
					block = this->block+(index * this->set_size + i);
					block->FIFO = false;
					this->block[index * this->set_size + ((i + 1) % set_size)].FIFO = true; //find next block in set for 
					block->tag = tag;
					break;
				}
			}
			//write data into block
			address = (address >> (int)log2((double)this->block_size)) << (int)log2((double)this->block_size);
			if (block) {
				for (int i = 0; i < block_size / 4; i++) {
					block->entry[i].data = memory->read(address + i * 4);
					block->entry[i].valid = true;
				}
			}
		}

		template <typename memtype>
		void miss(instruction* input, memtype* memory) {
			this->miss(input->dSink, memory);
		}

		int read(int address) {
			cache_entry* entry = this->find(address);
			if (entry)
				return entry->data;
			return 0;
		}

		void invalidate(int address) {
			cache_entry* data = this->find(address);
			if (data)
				data->valid = false;
		}

		void invalidate(instruction* input) {
			cache_entry* data = this->find(input->dSink);
			if (data)
				data->valid = false;
		}

		bool write(int address, int data) {
			cache_entry* entry = this->find(address);
			if (entry) {
				entry->data = data;
				entry->valid = true;
				return true;
			}
			return false;
		}

		int request() {
			return this->ports.add_request();
		}

		bool access(int port) {
			return this->ports.access(port);
		}

		void reset_ports() {
			this->ports.reset();
		}

		void print() {

			for (int i = 0; i < entries; i++) {
				if (i % this->set_size == 0) {
					std::cout << "Index " << std::bitset<3>(i / set_size) << std::endl;
				}
				std::cout << "\tTag: " << (this->block[i].tag) << "\tFIFO: " << this->block[i].FIFO << std::endl;
				for (int j = 0; j < block_size/4; j++) {
					//std::cout << "\t\tOffset: " << std::bitset<4>((long)j*4) << "\tData: " << this->block[i].entry[j].data << "\tValid: " << this->block[i].entry[j].valid << std::endl;
				}
			}
		}

		~cache() {
			for (int i = 0; i < this->entries; i++) {
				delete this->block[i].entry;
			}
			delete this->block;
		}
};
#pragma endregion

#pragma region Branch_Predictor
class branch_predictor {
private:
	int entries;
	int m;
	int n;
	int* PHT;
	int BHR;
	access_ports ports;
public:
	branch_predictor(int m, int n, int entries, int ports, int time) {
		this->ports.set(time, ports);
		this->m = m;
		this->n = n;
		this->entries = entries;
		this->PHT = new int[(int)pow(2, m) * entries];
		BHR = 0;
		for (int i = 0; i < (int)pow(2, m) * entries; i++) {
			PHT[i] = (int)(pow(2, n - 1) - 1);
		}
	}



	int* find(int PC) { //return pointer to PHT value so that edits can happen in reorder buffer
		//get index of PHT to look at
		int index = ((PC >> 2) % this->entries) * (int)pow(2, this->m) + this->BHR;
		return this->PHT + index;
	}

	bool predict(int* PHT) {
		return *PHT >= pow(2, n - 1);
	}

	void predict(instruction* input) {
		input->PHT = this->find(input->PC);
		input->prediction = this->predict(input->PHT);
	}

	void update(bool T, int* PHT) {
		BHR = ((BHR % (int)pow(2, m - 1)) << 1) | T;

		if (T) {
			(*PHT)++;
			if (*PHT > pow(2, n) - 1)
				* PHT = (int)pow(2, n) - 1;
		}
		else {
			(*PHT)--;
			if (*PHT < 0)
				* PHT = 0;
		}
	}

	void retire(instruction* input) {
		BHR = ((BHR % (int)pow(2, m - 1)) << 1) | input->T;

		if (input->T) {
			(*(input->PHT))++;
			if (*(input->PHT) > pow(2, n) - 1)
				*(input->PHT) = (int)pow(2, n) - 1;
		}
		else {
			(*(input->PHT))--;
			if (*(input->PHT) < 0)
				*(input->PHT) = 0;
		}
	}

	int request() {
		return this->ports.add_request();
	}

	bool access(int port) {
		return this->ports.access(port);
	}

	void reset_ports() {
		this->ports.reset();
	}

	void print() {
		std::cout << std::bitset<8>(this->BHR) << std::endl;
		for (int i = 0; i < this->entries; i++) {
			for (int j = 0; j < pow(2, m); j++) {
				std::cout << this->PHT[i * (int)pow(2, m) + j] << " ";
			}
			std::cout << std::endl;
		}
	}

	~branch_predictor() {
		delete this->PHT;
	}
};
#pragma endregion

#pragma region Registers
class register_rename {
private:
	int* table;
	int arch_size;
	int spec_size;
	int next;
public:
	register_rename(int arch_size, int spec_size) {
		this->arch_size = arch_size;
		this->next = arch_size;
		this->spec_size = spec_size;
		this->table = new int[arch_size];
		for (int i = 0; i < arch_size; i++)
			this->table[i] = i;
	}

	void reset() {
		this->next = arch_size;
		for (int i = 0; i < this->arch_size; i++)
			this->table[i] = i;
	}

	void update(instruction* input) {
		if (input->source >= 0) {
			input->rSource = this->table[input->source];
		}

		if (input->sink >= 0) {
			input->rSink = this->table[input->sink];
		}

		if (input->dest >= 0) {
			input->rDest = this->next;
			this->table[input->dest] = this->next;
			this->next = (this->next + 1) % (this->spec_size+ this->arch_size);
			if (this->next == 0) {
				this->next = this->arch_size;
			}
		}



	}
	~register_rename() {
		delete table;
	}
};

class register_file {
private:
	int* data;
	bool* score_board;
	int size;
	access_ports ports;
public:

	register_file(int size, int ports, int time) {
		this->ports.set(time, ports);
		this->data = new int[size];
		score_board = new bool[size];
		this->size = size;
		for (int i = 0; i < size; i++) {
			this->score_board[i] = false;
		}
	}

	int request() {
		return this->ports.add_request();
	}

	bool access(int port) {
		return ports.access(port);
	}

	void reset_ports() {
		this->ports.reset();
	}

	bool hazard(instruction* input) { //check if there is a data hazard
		bool source, sink;
		source = (input->rSource >= 0 ? this->score_board[input->rSource] : false);
		sink = (input->rSink >= 0 ? this->score_board[input->rSink] : false);
		return source || sink;
	}

	void read(instruction* input) { //read data from register file for instruction, set scoreboard for dest
		if (input->rSource >= 0) 
			input->dSource = this->data[input->rSource];
		
		if (input->rSink >= 0)
			input->dSink = this->data[input->rSink];

		this->sb(input);

	}

	void sb(instruction* input) {
		if (input->rDest >= 0) {
			this->score_board[input->rDest] = true;
		}
	}

	void write(instruction* input) { //write data back to register file, clear scoreboard for dest
		if (input->rDest >= 0) {
			this->data[input->rDest] = input->result;
			this->score_board[input->rDest] = false;
		}

	}

	void retire(instruction* input) {
		if(input->dest >= 0)
			this->data[input->dest] = input->result;
	}

	void reset() {
		for (int i = 0; i < this->size; i++) {
			this->score_board[i] = false;
			if (i == 53) {
				i = 53;
			}
		}
	}

	void print_sb() {
		std::cout << "-----------Score Board------------" << std::endl;
		for (int i = 0; i < size; i++) {
			if (this->score_board[i])
				std::cout << " r" << i;
		}
		std::cout << std::endl;
	}

};

#pragma endregion

#pragma region Issue Queue
struct issue_node {
	instruction* data;
	issue_node* next;
};

class issue_queue {
private:
	issue_node* head;
	int max_size;
	int* issue_sb;
	int size;
	access_ports ports;
public:
	issue_queue(int size, int ports, int time) {
		this->ports.set(time, ports);
		this->head = NULL;
		this->max_size = size;
		this->size = 0;
		this->issue_sb = new int[size];
	}
	
	bool sb(instruction* input, int count) {
		for (int i = 0; i < count; i++) {
			if (input->rSink >= 0) {
				if (input->rSink == this->issue_sb[i]) {
					return false;
				}
			}
			if (input->rSource >= 0) {
				if (input->rSource == this->issue_sb[i]) {
					return false;
				}
			}
		}
		return true;
	}

	instruction* dispatch(register_file file) {
		instruction* output;
		issue_node *it, *prev;
		int count = 0;
		bool memory = false;
		it = this->head;
		prev = NULL;
		while (it != NULL) {
			if (!file.hazard((it->data)) && (!memory || it->data->memory == 0) && this->sb(it->data, count)) {
				output = it->data;
				if (prev) {
					prev->next = it->next;
				}
				else {
					this->head = (it->next ? it->next : NULL);
				}
				delete it;
				return output;
			}
			if (it->data->memory != 0) {
				memory = true;
			}
			this->issue_sb[count] = it->data->rDest;
			count++;
			prev = it;
			it = it->next;
		}

		return NULL;
	}
	bool empty() {
		return size == 0;
	}

	bool full() {
		return max_size == size;
	}

	bool add(instruction* input) {
		if (this->full())
			return false;

		issue_node* it, * element;
		element = new issue_node;
		element->data = input;
		element->next = NULL;
		if (this->head == NULL)
		{
			this->head = element;
			return true;
		}
		it = this->head;
		while (it->next != NULL) {
			it = it->next;
		}
		it->next = element;
		return true;
		
	}

	int request() {
		return this->ports.add_request();
	}

	bool access(int port) {
		return this->ports.access(port);
	}

	void reset_ports() {
		this->ports.reset();
	}

	void reset() {
		issue_node* it, * prev;
		prev = this->head;
		while (prev) {
			it = prev->next;
			delete prev;
			prev = it;
		}
		this->size = 0;
		this->head = NULL;
	}

	void print() {
		std::cout << "-------------Issue Queue " << "-----------" << std::endl;
		issue_node* it;
		it = this->head;
		while (it) {
			std::cout << "PC: " << it->data->PC << ", rSource: " << it->data->rSource << ", rSink: " << it->data->rSink << ", rDest: " << it->data->rDest << std::endl;
			it = it->next;
		}
	}

	~issue_queue() {
		issue_node * it, * prev;
		prev = this->head;
		while(prev){
			it = prev->next;
			delete prev;
			prev = it;
		}
	}
};

#pragma endregion

#pragma region Execution
template <typename utype> class execution_unit {
private:
	int add_time;
	int sub_time;
	int mul_time;
	int div_time;
	int shift_time;
	int br_time;
	int wait_time;
	bool finish;
public:

	execution_unit() {
		this->add_time = 0;
		this->sub_time = 0;
		this->mul_time = 0;
		this->div_time = 0;
		this->shift_time = 0;
		this->br_time = 0;
		this->wait_time = 0;
		this->finish = false;
	}

	void set(int add_time, int sub_time, int mul_time, int div_time, int shift_time, int br_time) {
		this->add_time = add_time;
		this->sub_time = sub_time;
		this->mul_time = mul_time;
		this->div_time = div_time;
		this->shift_time = shift_time;
		this->br_time = br_time;
	}

	void start(instruction* input) {
		switch (input->operation) {
			case 1:
				this->wait_time = this->add_time;
				break;
			case 2:
				this->wait_time = this->sub_time;
				break;
			case 3:
				this->wait_time = this->mul_time;
				break;
			case 4:
				this->wait_time = this->div_time;
				break;
			case 5:
			case 6:
			case 7:
			case 8:
			case 9:
				this->wait_time = this->br_time;
				break;
			case 10:
			case 11:
			case 12:
			case 13:
			case 14:
				this->wait_time = this->shift_time;
				break;
			default:
				this->wait_time = 1;
		}
	}

	bool started() {
		return this->wait_time > 0;
	}

	bool finished() { // check if alu is done with computation
		return this->finish;
	}
	 
	void unfinish() { //when instruciton leaves alu set finish state back to false;
		this->finish = false;
	}

	bool wait() {
		if (this->wait_time > 0)
			return --(this->wait_time) == 0;
		return true;
	}

	void compute(instruction *input) {
		int dSource = input->dSource;
		int dSink = input->dSink;
		utype source = std::bit_cast<utype>(dSource);
		utype sink = std::bit_cast<utype>(dSink);
		switch (input->operation) {
		case 1:
			input->result = std::bit_cast<int>(source + sink);
			break;
		case 2:
			input->result = std::bit_cast<int>(source - sink);
			break;
		case 3:
			input->result = std::bit_cast<int>(source * sink);
			break;
		case 4:
			input->result = std::bit_cast<int>(source / sink);
			break;
		case 5:
			input->T = source == 0;
			break;
		case 6:
			input->T = source == sink;
			break;
		case 7:
			input->T = source < sink;
			break;
		case 8: //brgti
			input->T = source > sink;
			break;
		case 10: //AND
			input->result = source & sink;
			break;
		case 11: //OR
			input->result = source | sink;
			break;
		case 12: //XOR
			input->result = source & sink;
			break;
		case 13: //shift left
			input->result = source << sink;
			break;
		case 14: //shift right
			input->result = source >> sink;
			break;
		}
		std::cout << "PC: " << input->PC << ", rDest: " << input->rDest << ", Dest: " << input->dest << ", rSource: " << input->rSource << ", source: " << input->source << ", dSource" << input->dSource << ", rSink: " << input->rSink << ", Sink: " << input->sink << ", dSink: " << input->dSink << ", result: "<< input->result << ", T: " << input->T << std::endl;
		this->finish = true;
	}

	void reset() {
		this->wait_time = 0;
		this->finish = false;
	}
};
#pragma endregion

#pragma region Reorder Buffer
class reorder_buffer {
private:
	self_queue<instruction> queue;
	int size;
	access_ports ports;
public:
	reorder_buffer(int size, int ports, int time) {
		this->queue.set(size);
		this->size = size;
		this->ports.set(time, ports);
	}

	int request() {
		return this->ports.add_request();
	}

	bool access(int port) {
		return this->ports.access(port);
	}

	void reset_ports() {
		this->ports.reset();
	}

	bool full() {
		return this->queue.full();
	}

	instruction* add(instruction input) {
		if (this->full()) {
			return NULL;
		}
		this->queue.push_back(input);
		return this->queue.back();
	}

	instruction* front() {
		return this->queue.front();
	}

	void pop() {
		this->queue.pop_front();
	}

	bool empty() {
		return this->queue.empty();
	}

	void print() {
		std::cout << "-------------ROB head------------" << std::endl;
		if (this->front()) {
			std::cout << "PC: " << this->front()->PC << std::endl;
		}
		
	}

};
#pragma endregion

#pragma region Processor
class processor {
private:
	//global parameters
	bool repair;
	bool running;
	bool stopping;
	int execution_pipes;
	int PC;

	//architectual units
	reorder_buffer* ROB;
	memory_unit* RAM;
	cache* L1;
	cache* L2;
	cache* I_L1;
	register_file* ireg;
	register_rename* RAT;
	branch_predictor* gshare;
	issue_queue* iIQ;
	execution_unit<int>* iALU[2];

	//ports
	port<memory_unit> ROB_mem_port; port<register_file> ROB_reg_port; port<branch_predictor> ROB_branch_port; port<cache> ROB_L1_port; port<cache> ROB_L2_port; //ROB ports
	port<memory_unit> imem_port[2];//integer memory read
	port<cache> icache_port[2]; //integer cache read/write
	port<cache> icache2_port[2]; //integer cache read/write
	port<register_file> imem_reg_port[2]; //integer memory stage register write
	port<register_file> ireg_port[2];//integer register flie
	port<issue_queue> iregre_IQ_port; //integer register rename ports
	port<cache> dispatch_cache_port; port<branch_predictor> dispatch_branch_port; port<memory_unit> dispatch_mem_port; port<reorder_buffer> dispatch_ROB_port;//dispatch ports

	//flow controls
	instruction* ROB_head;
	instruction* imem_instruction[2];
	cache_entry* icache_entry[2];
	cache_entry* icache2_entry;
	instruction* iALU_instruction[2];
	instruction* ireg_instruction[2];
	instruction* iIQ_instruction[2];
	instruction* iregre_instruction;
	int dispatch_string;
	cache_entry* IL1_entry;
	instruction* dispatch_instruction;

	//cycle metrics
	int L1_misses;
	int L1_cycles;
	int L2_misses;
	int L2_cycles;
	int iL1_misses;
	int iL1_cycles;
	int branch_misses;
	int repair_cycles;

	
public:
	processor(reorder_buffer* ROBp,
	memory_unit* RAMp,
	cache* L1p,
	cache* L2p,
	cache* I_L1p,
	register_file* iregp,
	register_rename* RATp,
	branch_predictor* gsharep,
	issue_queue* iIQp,
	execution_unit<int>* iALU1,
	execution_unit<int>* iALU2) {
		repair = false;
		running = true;
		stopping = false;
		execution_pipes = 2;
		PC = 0;

		ROB = ROBp;
		RAM = RAMp;
		L1 = L1p;
		L2 = L2p;
		I_L1 = I_L1p;
		ireg = iregp;
		RAT = RATp;
		gshare = gsharep;
		iIQ = iIQp;
		iALU[0] = iALU1;
		iALU[1] = iALU2;

		ROB_mem_port.set(RAM); ROB_reg_port.set(ireg); ROB_branch_port.set(gshare); ROB_L1_port.set(L1); ROB_L2_port.set(L2); //ROB ports
		imem_port[0].set(RAM); imem_port[1].set(RAM); //integer memory read
		icache_port[0].set(L1); icache_port[1].set(L1); //integer cache read/write
		icache2_port[0].set(L2); icache2_port[1].set(L2); //integer cache read/write
		imem_reg_port[0].set(ireg); imem_reg_port[1].set(ireg); //integer memory stage register write
		ireg_port[0].set(ireg); ireg_port[1].set(ireg); //integer register flie
		iregre_IQ_port.set(iIQ); //integer register rename ports
		dispatch_cache_port.set(I_L1); dispatch_branch_port.set(gshare); dispatch_mem_port.set(RAM); dispatch_ROB_port.set(ROB);//dispatch ports

		ROB_head = NULL;

		imem_instruction[0] = imem_instruction[1] = NULL;

		icache_entry[0] = icache_entry[1] = NULL;
		icache2_entry = NULL;

		iALU_instruction[0] = iALU_instruction[1] = NULL;

		ireg_instruction[0] = ireg_instruction[1] = NULL;

		iIQ_instruction[0] = iIQ_instruction[1] = NULL;

		iregre_instruction = NULL;

		dispatch_string = 0;
		IL1_entry = NULL;
		dispatch_instruction = NULL;

		L1_misses = 0;
		L1_cycles = 0;
		L2_misses = 0;
		L2_cycles = 0;
		iL1_misses = 0;
		iL1_cycles = 0;
		branch_misses = 0;
		repair_cycles = 0;
	}

	void cycle_up() {
		//------------reorder buffer---------------
		if (!ROB->empty()) {
			if (ROB->front()->miss() && !repair) { // check branch misprediction
				repair = true;
				ROB_reg_port.reset();
				ROB_L1_port.reset();
				PC = (ROB->front()->T ? ROB->front()->branch_PC : ROB->front()->PC);
				ROB_branch_port.request();
				branch_misses++;
			}
			if (repair) {
				if (ROB->front()->memory > 0 && ROB->front()->completed) {
					ROB_L1_port.request();
				}
				if (ROB_L1_port.access()) {
					ROB_L1_port.unit()->invalidate(ROB->front());
				}
				if (ROB_branch_port.access()) {
					ROB_branch_port.unit()->retire(ROB->front());
				}
				if (!ROB_L1_port.waiting() && !ROB_branch_port.waiting()) {
					ROB->pop();
				}
				repair_cycles++;
			}
			else if (ROB->front()->operation == 0) {
				ROB->pop();
				ROB_head = (ROB->empty() ? NULL : ROB->front());
			}
			else if (ROB->front()->operation == 17)
			{
				running = false;
				return;
			}
			else if(ROB->front()->completed){
				if (ROB_head) {
					if (ROB_head->memory > 0) {
						if (ROB_L2_port.access()) {
							icache2_entry = ROB_L2_port.unit()->find(ROB_head);
						}
						if (ROB_mem_port.access()) {
							ROB_mem_port.unit()->write(ROB_head);
							if (icache2_entry) {
								icache2_entry->write(ROB_head);
							}
							else {
								ROB_L2_port.unit()->miss(ROB_head, ROB_mem_port.unit());
							}
						}
					}

					if (ROB_reg_port.access()) {
						ROB_reg_port.unit()->retire(ROB_head);
					}

					if (ROB_branch_port.access()) {
						ROB_branch_port.unit()->retire(ROB_head);
					}
				}
				// write back to architectual registers

				if (!ROB_mem_port.waiting() && !ROB_reg_port.waiting() && !ROB_branch_port.waiting() && !ROB_L2_port.waiting()) {
					if (ROB_head) {
						ROB->pop();
						ROB_head = (ROB->empty() ? NULL : ROB->front());
					}
					else {
						ROB_head = ROB->front();
					}
					if (ROB_head) {
						if (ROB_head->memory > 0) {
							ROB_mem_port.request();
							ROB_L2_port.request();
						}
						if (ROB_head->PHT)
							ROB_branch_port.request();
						ROB_reg_port.request();
					}

				}
			}

		}
		else {
			repair = false;
		}

		//---------------memory--------------------

		if (!repair) {
			for (int i = 0; i < execution_pipes; i++) {
				if (imem_instruction[i]) {
					if (imem_instruction[i]->memory > 0) { //if memory write, only write to cache, write to memory in reorder buffer

						if (icache_port[i].access()) { //access cahce
							icache_entry[i] = icache_port[i].unit()->find(imem_instruction[i]);
							if (icache_entry[i]) {
								icache_entry[i]->write(imem_instruction[i]);//if in cache, write
							}
							else {
								L1_misses++;
								icache2_port[i].request();//if not in cache request memory access
							}
						}

						if (icache2_port[i].access()) { //access cahce
							icache_entry[i] = icache2_port[i].unit()->find(imem_instruction[i]);
							if (icache_entry[i]) {
								icache_entry[i]->write(imem_instruction[i]);//if in cache, write
								icache_port[i].unit()->miss(imem_instruction[i], icache2_port[i].unit());//cache miss logic
							}
							else {
								L2_misses++;
								imem_port[i].request();//if not in cache request memory access
							}
						}

						if (imem_port[i].access()) {//access memory
							icache_port[i].unit()->miss(imem_instruction[i], imem_port[i].unit());//cache miss logic
							icache2_port[i].unit()->miss(imem_instruction[i], imem_port[i].unit());
							icache_entry[i] = icache_port[i].unit()->find(imem_instruction[i]);
							icache_entry[i]->write(imem_instruction[i]);
						}
						if (icache2_port[i].waiting()) {
							L1_cycles++;
						}
						if (imem_port[i].waiting()) {
							L1_cycles++;
							L2_cycles++;
						}
						if (!imem_port->waiting() && !icache_port[i].waiting() && !icache2_port[i].waiting()) {
							imem_instruction[i]->completed = true;
							imem_instruction[i] = NULL;
							icache_port[i].request();
						}
					}
					else if (imem_instruction[i]->memory < 0) { //memory read

						if (icache_port[i].access()) { //access cahce
							icache_entry[i] = icache_port[i].unit()->find(imem_instruction[i]);
							if (icache_entry[i]) {
								icache_entry[i]->read(imem_instruction[i]);//if in cache, write
								imem_reg_port[i].request();
							}
							else {
								L1_misses++;
								icache2_port[i].request();//if not in cache request L2 access
							}
						}

						if (icache2_port[i].access()) { //access cahce L2
							icache_entry[i] = icache2_port[i].unit()->find(imem_instruction[i]);
							if (icache_entry[i]) {
								icache_port[i].unit()->miss(imem_instruction[i], icache2_port[i].unit());//cache miss logic
								icache_entry[i]->read(imem_instruction[i]);//if in cache, write
								imem_reg_port[i].request();
							}
							else {
								L2_misses++;
								imem_port[i].request();//if not in cache request memory access
							}
						}

						if (imem_port[i].access()) {//access memory
							icache_port[i].unit()->miss(imem_instruction[i], imem_port[i].unit());//cache miss logic
							icache2_port[i].unit()->miss(imem_instruction[i], imem_port[i].unit());
							imem_port[i].unit()->read(imem_instruction[i]);//read from memory
							imem_reg_port[i].request();
						}

						if (imem_reg_port[i].access()) {//access register file only after read data from memory
							imem_reg_port[i].unit()->write(imem_instruction[i]);
						}
						if (icache2_port[i].waiting()) {
							L1_cycles++;
						}
						if (imem_port[i].waiting()) {
							L1_cycles++;
							L2_cycles++;
						}
						if (!imem_port->waiting() && !icache_port[i].waiting() && !imem_reg_port[i].waiting() && !icache2_port[i].waiting()) {
							imem_instruction[i]->completed = true;
							imem_instruction[i] = NULL;
							icache_port[i].request();
						}
					}

					else { //no memory read/write
						if (!imem_reg_port[i].waiting()) {
							imem_reg_port[i].request();
						}
						if (imem_reg_port[i].access()) {
							imem_reg_port[i].unit()->write(imem_instruction[i]);
						}
						if (!imem_reg_port[i].waiting()) {
							imem_instruction[i]->completed = true;
							imem_instruction[i] = NULL;
						}
					}
				}
				else {
					icache_port[i].request();
				}
			}
		}
		else {//if in repair reset port requests
			for (int i = 0; i < execution_pipes; i++) {
				if (imem_instruction[i]) {
					if (!icache_port[i].waiting()) {
						icache_port[i].unit()->invalidate(imem_instruction[i]);
					}
					imem_port[i].reset();
					icache2_port[i].reset();
					imem_reg_port[i].reset();
					imem_instruction[i] = NULL;
				}
			}
		}

		//------------execution unit---------------
		if (!repair) {
			//integer
			for (int i = 0; i < execution_pipes; i++) {
				if (iALU_instruction[i]) {
					if ((iALU[i]->finished())) {
						if (!imem_instruction[i]) {
							imem_instruction[i] = iALU_instruction[i];
							iALU_instruction[i] = NULL;
							iALU[i]->unfinish();
						}
					}
					else {
						if (iALU[i]->started()) {
							if (iALU[i]->wait()) {
								iALU[i]->compute(iALU_instruction[i]);
							}
						}
						else {
							iALU[i]->start(iALU_instruction[i]);
						}
					}
				}
			}

			//float
		}
		else {//if in repair reset port requests
			for (int i = 0; i < execution_pipes; i++) {
				if (iALU_instruction[i]) {
					iALU[i]->reset();
					iALU_instruction[i] = NULL;
				}
			}
		}
		//------------register file----------------
		if (!repair) {
			//integer
			for (int i = 0; i < execution_pipes; i++) {
				if (ireg_instruction[i]) {
					if (ireg_port[i].access()) {
						ireg_port[i].unit()->read(ireg_instruction[i]);
					}
					if (!ireg_port[i].waiting()) {
						if (!iALU_instruction[i]) {
							iALU_instruction[i] = ireg_instruction[i];
							ireg_instruction[i] = NULL;
							ireg_port[i].request();
						}
					}
				}
				else {
					ireg_port[i].request();
				}
			}
		}
		else {//if in repair reset port requests
			ireg->reset();
			for (int i = 0; i < execution_pipes; i++) {
				if (ireg_instruction[i]) {
					ireg_port[i].reset();
					ireg_instruction[i] = NULL;
				}
			}
		}


		//-------------issue queue-----------------
		if (!repair) {
			//integer
			for (int i = 0; i < execution_pipes; i++) {
				if (iIQ_instruction[i]) {
					if (!ireg_instruction[i]) {
						ireg_instruction[i] = iIQ_instruction[i];
						iIQ_instruction[i] = NULL;
					}
				}
				else {
					iIQ_instruction[i] = iIQ->dispatch(*ireg); //access to scoreboard is 1 cycle
					if(iIQ_instruction[i])
						ireg->sb(iIQ_instruction[i]);
				}
			}
		}
		else {
			iIQ->reset();
			for (int i = 0; i < execution_pipes; i++) {
				if (iIQ_instruction[i]) {
					iIQ_instruction[i] = NULL;
				}
			}
		}

		//----------register renaming--------------
		if (!repair) {
			//integer
			if (iregre_instruction) { //if there is an instruction, rename registers
				if (iregre_IQ_port.access()) {
					if (iregre_IQ_port.unit()->full()) {//check if issue queue is full
						iregre_IQ_port.request();
					}
				}

				if (!iregre_IQ_port.waiting()) { //if there is 
					RAT->update(iregre_instruction);
					iregre_IQ_port.unit()->add(iregre_instruction);
					iregre_instruction = NULL;
					iregre_IQ_port.request();
				}
			}
			else {
				iregre_IQ_port.request();
			}

		}
		else {
			RAT->reset();
			if (iregre_instruction) {
				iregre_IQ_port.reset();
				iregre_instruction = NULL;
			}
		}

		//-------dispatch and branch prediction-------

		if (!repair) {
			if (!stopping) {
				if (dispatch_cache_port.access()) {
					IL1_entry = dispatch_cache_port.unit()->find(PC);
					if (IL1_entry) {
						dispatch_string = IL1_entry->data;
					}
					else {
						iL1_misses++;
						dispatch_mem_port.request();
					}
				}

				if (dispatch_mem_port.access()) {
					dispatch_string = dispatch_mem_port.unit()->read(PC);
					dispatch_cache_port.unit()->miss(PC, dispatch_mem_port.unit());
				}

				if (dispatch_ROB_port.access()) {
					if (dispatch_ROB_port.unit()->full()) {
						dispatch_ROB_port.request();
					}
					else {
						dispatch_instruction = dispatch_ROB_port.unit()->add(instruction());
					}

				}

				dispatch_branch_port.access();

				if (dispatch_mem_port.waiting()) {
					iL1_cycles++;
				}

				if (!dispatch_cache_port.waiting() && !dispatch_branch_port.waiting() && !dispatch_mem_port.waiting() && !dispatch_ROB_port.waiting()) {
					if (dispatch_instruction) { //if dispatch instruction is ready and register rename step is ready, decode and pass
						if (!iregre_instruction) {

							dispatch_instruction->dispatch(dispatch_string); // dispatch logic
							std::cout << "Dispatch PC: " << PC << "\n";
							dispatch(dispatch_string);
							dispatch_instruction->PC = PC;

							if (dispatch_instruction->T)
								dispatch_branch_port.unit()->predict(dispatch_instruction);

							if (dispatch_instruction->prediction) {
								PC = dispatch_instruction->branch_PC;
							}
							else {
								PC = PC + 4;
							}
							if (dispatch_instruction->operation == 17) {
								stopping = true;
							}

							iregre_instruction = dispatch_instruction;
							dispatch_instruction = NULL;
							dispatch_cache_port.request();
							dispatch_branch_port.request();
							dispatch_ROB_port.request();
						}
					}
					else { //if there is no instruction, request ports to fetch
						dispatch_cache_port.request(); //do not request mem port
						dispatch_branch_port.request();
						dispatch_ROB_port.request();
					}
				}
			}
		}
		else {
			std::cout << "NEW REPAIR PC " << PC << std::endl;
			dispatch_instruction = NULL;
			stopping = false;
			dispatch_cache_port.reset();
			dispatch_branch_port.reset();
			dispatch_ROB_port.reset();
			dispatch_mem_port.reset();
		}
	}

	bool run() {
		return this->running;
	}

	void print() {
		if (repair)
			std::cout<< "REPAIR\n";
		ROB->print();
		std::cout << "-----------MEM stage------------" << std::endl;
		for (int i = 0; i < execution_pipes; i++) {
			if (imem_instruction[i]) {
				std::cout << "----Pipe " << i + 1 << "----" << std::endl;
				std::cout << "PC: " << imem_instruction[i]->PC << ", " << (imem_instruction[i]->memory > 0 ? "st" : (imem_instruction[i]->memory < 0 ? "ld" : "wb")) << std::endl;
			}
		}
		std::cout << "-----------Execution stage------------" << std::endl;
		for (int i = 0; i < execution_pipes; i++) {
			if (iALU_instruction[i]) {
				std::cout << "----Pipe " << i + 1 << "----" << std::endl;
				std::cout << "PC: " << iALU_instruction[i]->PC << ", Operation: " << iALU_instruction[i]->operation << std::endl;
			}
		}
		std::cout << "-----------Register stage------------" << std::endl;
		for (int i = 0; i < execution_pipes; i++) {
			if (ireg_instruction[i]) {
				std::cout << "----Pipe " << i + 1 << "----" << std::endl;
				std::cout << "PC: " << ireg_instruction[i]->PC << ", rSource: " << ireg_instruction[i]->rSource << ", rSink: " << ireg_instruction[i]->rSink << ", rDest: " << ireg_instruction[i]->rDest << std::endl;
			}
		}
		ireg->print_sb();
		std::cout << "-----------issue wait stage------------" << std::endl;
		for (int i = 0; i < execution_pipes; i++) {
			if (iIQ_instruction[i]) {
				std::cout << "----Pipe " << i + 1 << "----" << std::endl;
				std::cout << "PC: " << iIQ_instruction[i]->PC << ", rSource: " << iIQ_instruction[i]->rSource << ", rSink: " << iIQ_instruction[i]->rSink << ", rDest: " << iIQ_instruction[i]->rDest << std::endl;
			}
		}
		iIQ->print();
		std::cout << "---------------Register Rename " << "---------------" << std::endl;
		if (iregre_instruction) {
			std::cout << "PC: " << iregre_instruction->PC << ", Source: " << iregre_instruction->source << ", Sink: " << iregre_instruction->sink << ", Dest: " << iregre_instruction->dest << std::endl;
		}

	}

	void print_stats() {
		std::cout << "L1 misses: " << L1_misses << std::endl;
		std::cout << "L1 miss time: " << L1_cycles << std::endl;
		std::cout << "L2 misses: " << L2_misses << std::endl;
		std::cout << "L2 miss time: " << L2_cycles << std::endl;
		std::cout << "I-L1 misses: " << iL1_misses << std::endl;
		std::cout << "I-L1 miss time: " << iL1_cycles << std::endl;
		std::cout << "Branch misprediction: " << branch_misses << std::endl;
		std::cout << "Repair time: " << repair_cycles << std::endl;
	}

};
#pragma endregion


int main()
{
	//--------------architechtural units---------------------
	branch_predictor gshare(2, 2, 256, 2, 2);

	reorder_buffer ROB(64, 8, 2);
	issue_queue iIQ(20, 8, 2);
	register_file ireg(100, 10, 2);

	cache I_L1(1024, 512, 1, 1, 3);
	cache L1(1024, 512, 1, 1, 3);
	cache L2(2048, 512, 2, 2, 10);
	
	register_rename RAT(32, 40);
	memory_unit RAM(256, 10, 30);
	execution_unit<int> iALU[2]; iALU[0].set(1, 1, 4, 5, 1, 2); iALU[1].set(1, 1, 4, 5, 1, 5);
	processor alpha(&ROB, &RAM, &L1, &L2, &I_L1, &ireg, &RAT, &gshare, &iIQ, &iALU[0], &iALU[1]);

	RAM.write(0,0b00110110000110100000000000000111);
	RAM.write(4,0b00000100011110000000000000001000);
	RAM.write(8,0b00111100000000100000000000000000);
	RAM.write(12,0b00000100100010010000000000001000);
	RAM.write(16,0b00111100000000100000000000000001);
	RAM.write(20,0b00000100100010010000000000001000);
	RAM.write(24,0b00111100000000100000000000000010);
	RAM.write(28,0b00001100000000000100000000000011);
	RAM.write(32,0b00000100000100001000000000000100);
	RAM.write(36,0b00100000010000001100000000001101);
	RAM.write(40,0b00000100000000000100000000000101);
	RAM.write(44,0b00000100010100001000000000000101);
	RAM.write(48,0b00100100000000000000000000001111);
	RAM.write(52,0b00001100000000000100000000000101);
	RAM.write(56,0b00001100010100001000000000000101);
	RAM.write(60,0b00000100011110110000000000001000);
	RAM.write(64,0b01000000010100100000000000000000);
	RAM.write(68,0b00000110000010100100000000000101);
	RAM.write(72,0b00010100010100000000000000011000);
	RAM.write(76,0b00001000010110000100000000000101);
	RAM.write(80,0b00000100001100000000000000000011);
	RAM.write(84,0b00000100100010010000000000001000);
	RAM.write(88,0b01000000001100100000000000000000);
	RAM.write(92,0b00100100000000000000000000010010);

	RAM.write(96,0b01100100000000000000000000000000);


	RAM.write(256, 2);
	RAM.write(260, 4);
	RAM.write(264, 7);

	int cycle = 0;
	while (alpha.run()) {
		alpha.cycle_up();
		std::cout << "Cycle: " << cycle << std::endl;
		alpha.print();
		std::cout << "\n\n\n";
		cycle++;
	}
	
	for (int i = 0; i < 20; i++) {
		std::cout << RAM.read(256 + 4 * i) << std::endl;
	}
	alpha.print_stats();
	return 0;
}