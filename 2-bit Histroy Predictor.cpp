#include<iostream>
#include<string>
#include<vector>
#include<sstream>
#include<iomanip>
#include<algorithm>
using namespace std;
int code_register[32]{ 0 };   // register
class instruction {
public:
	string ins;
	string rd;
	string rs1;
	string rs2;
	string ins_address;
	instruction(string ins_address_temp, string ins_temp, string rd_temp, string rs1_temp, string rs2_temp) {
		ins_address = hex_to_bit(ins_address_temp);
		ins = ins_temp;
		rd = rd_temp;
		rs1 = rs1_temp;
		rs2 = rs2_temp;
	}
	string bit_to_tag(string ins_address, int entry) {   //bit to tag depend on entry
		int counter = 0;
		int i = 1;
		int max = 1;
		while (i != entry)
		{
			i *= 2;
			counter++;
			max += i;
			if (entry < max)
			{
				break;
			}
		}
		if (entry == 1)
		{
			counter = 1;
		}
		i = 0;
		int pos = ins_address.length() - 3;
		string newaddress = "";
		while (counter != i)
		{
			newaddress.insert(newaddress.begin(), ins_address[pos]);
			pos--;
			i++;
		}
		return newaddress;
	}
	string hex_to_bit(string hex) {          // hex translate to bit
		int i = 0;
		string binary = "";
		if (hex[0] == '0' && hex[1] == 'x')
		{
			hex.erase(0, 2);
		}
		while (hex[i]) {

			switch (hex[i]) {
			case '0':
				binary += "0000";
				break;
			case '1':
				binary += "0001";
				break;
			case '2':
				binary += "0010";
				break;
			case '3':
				binary += "0011";
				break;
			case '4':
				binary += "0100";
				break;
			case '5':
				binary += "0101";
				break;
			case '6':
				binary += "0110";
				break;
			case '7':
				binary += "0111";
				break;
			case '8':
				binary += "1000";
				break;
			case '9':
				binary += "1001";
				break;
			case 'A':
			case 'a':
				binary += "1010";
				break;
			case 'B':
			case 'b':
				binary += "1011";
				break;
			case 'C':
			case 'c':
				binary += "1100";
				break;
			case 'D':
			case 'd':
				binary += "1101";
				break;
			case 'E':
			case 'e':
				binary += "1110";
				break;
			case 'F':
			case 'f':
				binary += "1111";
				break;
			default:
				cout << "\nInvalid hexadecimal digit "
					<< hex[i];
			}
			i++;
		}
		return binary;
	}
};
class BTB {
public:
	string BTB_table[4] = { "SN","SN","SN","SN" };       // BTB
	string Tag = "";
	int misprediction = 0;
	int branch_counter = 0;
	bool branch_or_not[2] = { false,false };        //history table
	bool predict = false;							//store the predict
};
void output_ins(instruction code) {            // output the ins
	cout << code.ins << " ";
	cout << code.rd << ",";
	if (code.rs2 != "")
	{
		cout << code.rs1 << ",";
		cout << code.rs2 << endl;
	}
	else
	{
		cout << code.rs1 << endl;
	}
}
string int_to_hex(int i)               // integer to hex
{
	std::stringstream stream;
	stream << "0x"
		<< std::setfill('0') << std::setw(i * 2)
		<< std::hex << i;
	return stream.str();
}
void split_ins(string temp, vector <string>& lable, vector<int>& lable_count, int& lable_counter, int& address_count, vector<instruction>& code) {
	string insAddress = "";
	if (temp.find("0x") != string::npos)
	{
		insAddress = "0x";
		for (int i = 2; i < temp.length(); i++)
		{
			if (temp[i] == ' ' || (temp[i] >= 'a' && temp[i] <= 'z'))
			{
				temp.erase(0, i);
				break;
			}
			insAddress += temp[i];
		}
	}
	else
	{
		if (temp.find(":") != string::npos)
		{
			temp.erase(remove_if(temp.begin(), temp.end(), isspace), temp.end());
			temp.pop_back();
			lable.push_back(temp);
			lable_count.push_back(lable_counter);
			return;
		}
		else
		{
			insAddress = int_to_hex(address_count);
			address_count += 4;
			for (int i = 0; i < temp.length(); i++)
			{
				if (temp[i] != ' ')
				{
					temp.erase(0, i);
					break;
				}
			}
		}
	}
	string ins = "";
	for (int i = 0; i < temp.length(); i++)
	{
		if (temp[i] == ' ')
		{
			temp.erase(0, i + 1);
			break;
		}
		ins += temp[i];
	}
	string rd = "";
	for (int i = 0; i < temp.length(); i++)
	{
		if (temp[i] == ',')
		{
			temp.erase(0, i + 1);
			break;
		}
		rd += temp[i];
	}
	string rs1 = "";
	bool end_of_line = false;
	for (int i = 0; i < temp.length(); i++)
	{
		if (temp[i] == ',')
		{
			temp.erase(0, i + 1);
			break;
		}
		else if (i == temp.length() - 1)            //if ins to the end then remove all the words
		{
			end_of_line = true;
		}
		rs1 += temp[i];
	}
	if (end_of_line)
	{
		temp.clear();
	}
	string rs2 = "";
	temp.erase(remove_if(temp.begin(), temp.end(), isspace), temp.end());
	for (int i = 0; i < temp.length(); i++)
	{
		if (temp[i] == ',')
		{
			temp.erase(0, i + 1);
			break;
		}
		rs2 += temp[i];
	}
	instruction temp_code(insAddress, ins, rd, rs1, rs2);
	code.push_back(temp_code);
	lable_counter++;
}
string erasecommit(string temp) {           //erase the ';'of back commits
	if (temp.find(";") != string::npos) {
		int pos = temp.find(";");
		temp.erase(pos, temp.length() - pos);
	}
	return temp;
}
string translateBit(int max, int num)   //dex to binary 
{										//max:assign bit to max-1
	int* a = new int[max];
	int i;
	for (i = 0; num > 0; i++)
	{
		a[i] = num % 2;
		num = num / 2;
	}
	for (int j = i; j < max; j++)
	{
		a[j] = 0;
	}
	string temp;
	for (int i = 0; i < max; i++)
	{
		temp += a[i] + '0';
	}
	return temp;
}
int translate_int(string num) {   //bit to integer
	int num_temp = 0;
	int num_count = 2;
	reverse(num.begin(), num.end());
	if (num[0] == '1')
	{
		num_temp += 1;
	}
	for (int i = 1; i < num.length(); i++)
	{
		if (num[i] == '1')
		{
			num_temp += num_count;
		}
		num_count *= 2;
	}
	return num_temp;
}
/******************************ALU*********************/
void ins_li(instruction code) {
	code.rd.erase(0, 1);   //remove 'R'
	if (code.rd != "0")
	{
		code_register[stoi(code.rd)] = stoi(code.rs1);
	}
}
void ins_addi(instruction code) {
	code.rd.erase(0, 1);   //remove 'R'
	if (code.rd != "0")
	{
		code_register[stoi(code.rd)] += stoi(code.rs2);
	}
}
void ins_add(instruction code) {
	code.rd.erase(0, 1);   //remove 'R'
	code.rs1.erase(0, 1);   //remove 'R'
	code.rs2.erase(0, 1);   //remove 'R'
	if (code.rd != "0")
	{
		code_register[stoi(code.rd)] = code_register[stoi(code.rs2)] + code_register[stoi(code.rs1)];
	}
}
void ins_sub(instruction code) {
	code.rd.erase(0, 1);   //remove 'R'
	code.rs1.erase(0, 1);   //remove 'R'
	code.rs2.erase(0, 1);   //remove 'R'
	if (code.rd != "0")
	{
		code_register[stoi(code.rd)] = code_register[stoi(code.rs1)] - code_register[stoi(code.rs2)];
	}
}
void ins_andi(instruction code) {
	code.rd.erase(0, 1);   //remove 'R'
	int remainder_counter = 0;
	for (int i = 0; i < stoi(code.rs2); i++)
	{
		if (remainder_counter == 0)
		{
			remainder_counter = 1;
		}
		else
		{
			remainder_counter *= 2;
		}
	}
	code_register[stoi(code.rd)] %= remainder_counter;
}
bool ins_beq(instruction code) {
	code.rd.erase(0, 1);   //remove 'R'
	code.rs1.erase(0, 1);   //remove 'R'
	if (code_register[stoi(code.rd)] == code_register[stoi(code.rs1)])
	{
		return true;
	}
	else
	{
		return false;
	}
}
bool ins_bne(instruction code) {
	code.rd.erase(0, 1);   //remove 'R'
	code.rs1.erase(0, 1);   //remove 'R'
	if (code_register[stoi(code.rd)] != code_register[stoi(code.rs1)])
	{
		return true;
	}
	else
	{
		return false;
	}
}
bool ins_blt(instruction code) {
	code.rd.erase(0, 1);   //remove 'R'
	code.rs1.erase(0, 1);   //remove 'R'
	if (code_register[stoi(code.rd)] <= code_register[stoi(code.rs1)])
	{
		return true;
	}
	else
	{
		return false;
	}
}
bool ins_bge(instruction code) {
	code.rd.erase(0, 1);   //remove 'R'
	code.rs1.erase(0, 1);   //remove 'R'
	if (code_register[stoi(code.rd)] >= code_register[stoi(code.rs1)])
	{
		return true;
	}
	else
	{
		return false;
	}
}
void ins_slli(instruction code) {
	code.rd.erase(0, 1);   //remove 'R'
	code.rs1.erase(0, 1);   //remove 'R'
	if (code.rd != "0")
	{
		int temp_interger = stoi(code.rs1) << stoi(code.rs2);
		code_register[stoi(code.rd)] = temp_interger;
	}
}
void ins_or(instruction code) {
	code.rd.erase(0, 1);   //remove 'R'
	code.rs1.erase(0, 1);   //remove 'R'
	code.rs2.erase(0, 1);   //remove 'R'
	if (code.rd != "0")
	{
		int temp_interger = stoi(code.rs1) | stoi(code.rs2);
		code_register[stoi(code.rd)] = temp_interger;
	}
}
void ins_xor(instruction code) {
	code.rd.erase(0, 1);   //remove 'R'
	code.rs1.erase(0, 1);   //remove 'R'
	code.rs2.erase(0, 1);   //remove 'R'
	if (code.rd != "0")
	{
		int temp_interger = stoi(code.rs1) ^ stoi(code.rs2);
		code_register[stoi(code.rd)] = temp_interger;
	}
}
void ins_and(instruction code) {
	code.rd.erase(0, 1);   //remove 'R'
	code.rs1.erase(0, 1);   //remove 'R'
	code.rs2.erase(0, 1);   //remove 'R'
	if (code.rd != "0")
	{
		int temp_interger = stoi(code.rs1) & stoi(code.rs2);
		code_register[stoi(code.rd)] = temp_interger;
	}
}
void ins_srli(instruction code) {
	code.rd.erase(0, 1);   //remove 'R'
	code.rs1.erase(0, 1);   //remove 'R'
	if (code.rd != "0")
	{
		int temp_interger = stoi(code.rs1) >> stoi(code.rs2);
		code_register[stoi(code.rd)] = temp_interger;
	}
}
/******************************ALU*********************/
void prediction(BTB& code) {					// predict the branch
	code.predict = false;						// intial the answer
	if (code.BTB_table[code.branch_counter] == "ST" || code.BTB_table[code.branch_counter] == "WT")
	{
		code.predict = true;
	}
}
void check_predict(BTB& code, bool branchornot) {
	// check the predict is correct or not & output the answer
	// branchornot is the answer whether branch is taken or not
	cout << "(";
	if (code.branch_counter == 0)
	{
		cout << "00, ";
	}
	else if (code.branch_counter == 1)
	{
		cout << "01, ";
	}
	else if (code.branch_counter == 2)
	{
		cout << "10, ";
	}
	else
	{
		cout << "11, ";
	}
	for (int i = 0; i < 3; i++)
	{
		cout << code.BTB_table[i] << ",";
	}
	cout << code.BTB_table[3] << ") ";
	if (code.predict)
	{
		cout << "T ";
	}
	else
	{
		cout << "N ";
	}
	if (branchornot == true)
	{
		cout << "T  ";
	}
	else
	{
		cout << "N  ";
	}
	if (code.predict != branchornot)		// if taken or not taken is not correct due to predict
	{
		code.misprediction++;
	}
	if (branchornot)                   // update the BTB table
	{
		if (code.BTB_table[code.branch_counter] == "SN")
		{
			code.BTB_table[code.branch_counter] = "WN";
		}
		else if (code.BTB_table[code.branch_counter] == "WN")
		{
			code.BTB_table[code.branch_counter] = "WT";
		}
		else if (code.BTB_table[code.branch_counter] == "WT")
		{
			code.BTB_table[code.branch_counter] = "ST";
		}
	}
	else
	{
		if (code.BTB_table[code.branch_counter] == "ST")
		{
			code.BTB_table[code.branch_counter] = "WT";
		}
		else if (code.BTB_table[code.branch_counter] == "WT")
		{
			code.BTB_table[code.branch_counter] = "WN";
		}
		else if (code.BTB_table[code.branch_counter] == "WN")
		{
			code.BTB_table[code.branch_counter] = "SN";
		}
	}
	code.branch_or_not[0] = code.branch_or_not[1];   // update the history table
	code.branch_or_not[1] = branchornot;
	if (code.branch_or_not[0] == false && code.branch_or_not[1] == false)
	{
		code.branch_counter = 0;
	}
	else if (code.branch_or_not[0] == false && code.branch_or_not[1] == true)
	{
		code.branch_counter = 1;						// store which table to use
	}
	else if (code.branch_or_not[0] == true && code.branch_or_not[1] == false)
	{
		code.branch_counter = 2;
	}
	else if (code.branch_or_not[0] == true && code.branch_or_not[1] == true)
	{
		code.branch_counter = 3;
	}
	cout << "misprediction: " << code.misprediction << endl;  // output misprediction
}
void output_all_entries(vector<BTB> BTB_code) {
	cout << "All entries:" << endl;
	for (int i = 0; i < BTB_code.size(); i++)
	{
		cout << "entry: " << i << " " << "(";
		if (BTB_code[i].branch_counter == 0)
		{
			cout << "00, ";
		}
		else if (BTB_code[i].branch_counter == 1)
		{
			cout << "01, ";
		}
		else if (BTB_code[i].branch_counter == 2)
		{
			cout << "10, ";
		}
		else if (BTB_code[i].branch_counter == 3)
		{
			cout << "11, ";
		}
		for (int j = 0; j < 3; j++)
		{
			cout << BTB_code[i].BTB_table[j] << ",";
		}
		cout << BTB_code[i].BTB_table[3] << ") " << endl;
	}
}
void run_code(vector<instruction> code, vector<string> lable, vector<int>lable_count, vector<BTB>& BTB_code) {
	int ins_pointer = 0;				//pointer to ins
	int last_pointer = ins_pointer;		//store the pointer now at
	while (ins_pointer != code.size())	//if pointer to the end then off
	{
		bool isBranch = false;			//store the ins is branch or not
		bool branchornot = false;		//store is taken or not taken
		bool checking = false;			//store the tag is found in BTB_table or not
		int BTB_counter = 0;			//store the BTB_pointer
		last_pointer = ins_pointer;		//store the pointer now at 
		for (int i = 0; i < BTB_code.size(); i++)
		{
			if (BTB_code[i].Tag == code[ins_pointer].ins_address)
			{
				prediction(BTB_code[i]);
				BTB_counter = i;
				checking = true;
				break;
			}
		}
		if (code[ins_pointer].ins == "li")
		{
			ins_li(code[ins_pointer]);
			ins_pointer++;
		}
		else if (code[ins_pointer].ins == "addi")
		{
			ins_addi(code[ins_pointer]);
			ins_pointer++;
		}
		else if (code[ins_pointer].ins == "add")
		{
			ins_add(code[ins_pointer]);
			ins_pointer++;
		}
		else if (code[ins_pointer].ins == "sub")
		{
			ins_sub(code[ins_pointer]);
			ins_pointer++;
		}
		else if (code[ins_pointer].ins == "andi")
		{
			ins_andi(code[ins_pointer]);
			ins_pointer++;
		}
		else if (code[ins_pointer].ins == "beq")
		{
			isBranch = true;
			if (ins_beq(code[ins_pointer]))
			{
				branchornot = true;
				bool lable_found = false;
				for (int i = 0; i < lable.size(); i++)             //search lable to jump
				{
					if (lable[i] == code[ins_pointer].rs2)
					{
						ins_pointer = lable_count[i];         //branch to instruction if true
						lable_found = true;
						break;
					}
				}
				if (!lable_found)
				{
					cout << "lable cant found!" << endl;
					system("pause");
				}
			}
			else
			{
				ins_pointer++;
			}
		}
		else if (code[ins_pointer].ins == "bne")
		{
			isBranch = true;
			if (ins_bne(code[ins_pointer]))
			{
				branchornot = true;
				bool lable_found = false;
				for (int i = 0; i < lable.size(); i++)             //search labkle to jump
				{
					if (lable[i] == code[ins_pointer].rs2)
					{
						ins_pointer = lable_count[i];         //branch to instruction if true
						lable_found = true;
						break;
					}
				}
				if (!lable_found)
				{
					cout << "lable cant found!" << endl;
					system("pause");
				}
			}
			else
			{
				ins_pointer++;
			}
		}
		else if (code[ins_pointer].ins == "blt")
		{
			isBranch = true;
			if (ins_blt(code[ins_pointer]))
			{
				branchornot = true;
				bool lable_found = false;
				for (int i = 0; i < lable.size(); i++)             //search labkle to jump
				{
					if (lable[i] == code[ins_pointer].rs2)
					{
						ins_pointer = lable_count[i];         //branch to instruction if true
						lable_found = true;
						break;
					}
				}
				if (!lable_found)						//Exception protect
				{
					cout << "lable cant found!" << endl;
					system("pause");
				}
			}
			else
			{
				ins_pointer++;
			}
		}
		else if (code[ins_pointer].ins == "bge")
		{
			isBranch = true;
			if (ins_bge(code[ins_pointer]))
			{
				branchornot = true;
				bool lable_found = false;
				for (int i = 0; i < lable.size(); i++)             //search labkle to jump
				{
					if (lable[i] == code[ins_pointer].rs2)
					{
						ins_pointer = lable_count[i];         //branch to instruction if true
						lable_found = true;
						break;
					}
				}
				if (!lable_found)				//Exception protect
				{
					cout << "lable cant found!" << endl;
					system("pause");
				}
			}
			else
			{
				ins_pointer++;
			}
		}
		else if (code[ins_pointer].ins == "slli")
		{
			ins_slli(code[ins_pointer]);
			ins_pointer++;
		}
		else if (code[ins_pointer].ins == "srli")
		{
			ins_srli(code[ins_pointer]);
			ins_pointer++;
		}
		else if (code[ins_pointer].ins == "or")
		{
			ins_or(code[ins_pointer]);
			ins_pointer++;
		}
		else if (code[ins_pointer].ins == "and")
		{
			ins_and(code[ins_pointer]);
			ins_pointer++;
		}
		else if (code[ins_pointer].ins == "xor")
		{
			ins_xor(code[ins_pointer]);
			ins_pointer++;
		}
		else                                                      // if ins is not support
		{
			cout << code[ins_pointer].ins << " " << "not supprot" << endl;
			system("pause");
		}
		if (isBranch)
		{
			if (!checking)				//means the BTB_table is not set up for the tag
			{
				BTB_counter = translate_int(code[last_pointer].ins_address);
				BTB_code[BTB_counter].Tag = code[last_pointer].ins_address;
				prediction(BTB_code[BTB_counter]);
			}
			cout << "entry:" << translate_int(code[last_pointer].ins_address) << '\t';
			output_ins(code[last_pointer]);
			check_predict(BTB_code[BTB_counter], branchornot);
			output_all_entries(BTB_code);
		}
		else
		{
			cout << "Instruction: ";
			output_ins(code[last_pointer]);
		}
	}

}


int main() {
	vector<instruction> code;         // store the ins all the info
	vector<string> lable;             // store the lable
	vector<int>lable_count;			  // store the lable postion
	int address_count = 0;			// if ins address not given then use this integer
	int lable_counter = 0;			 // store the last of lable postion
	string temp;
	for (int i = 1; i < 32; i++)
	{
		code_register[i] = -1;           //initial the register
	}
	while (getline(cin, temp))   //input all the code until EOF
	{
		if (temp != "")
		{
			temp = erasecommit(temp);                 //erase ';' behind all the commits
			split_ins(temp, lable, lable_count, lable_counter, address_count, code);  // exarct the code
		}
	}
	vector<BTB>BTB_code;                    // initial the BTB
	cin.clear();                           //clean EOF state
	cout << "\x1B[2J\x1B[H";               //clear the console message
	cout << "Please input entry(entry>0):";
	string entry_temp;
	getline(cin, entry_temp);           // start input integer to entry
	int entry = 0;
	while (entry == 0)                  //checking input is valid or not 
	{
		bool check_entry = true;
		for (int i = 0; i < entry_temp.length(); i++)
		{
			if (!isdigit(entry_temp[i]))
			{
				check_entry = false;
				break;
			}
		}
		if (check_entry)
		{
			entry = stoi(entry_temp);
			if (entry < 0)
			{
				check_entry = false;
			}
		}
		else
		{
			cout << "Invail input!" << endl;
			cout << "Please input entry(entry>0):";
			getline(cin, entry_temp);
		}
	}
	BTB_code.resize(entry);				//due to entry to resize the BTB
	for (int i = 0; i < code.size(); i++)
	{
		code[i].ins_address = code[i].bit_to_tag(code[i].ins_address, entry);    //let address turn to tag
	}
	run_code(code, lable, lable_count, BTB_code);         //start to run the code
	cout << "Done" << endl;
	system("pause");
}