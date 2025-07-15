#include <wx/msgdlg.h>
#include "vmMainWnd.h"
#undef IN
#undef OUT
#include <wx/string.h>
#include <thread>
#include <iostream>
#include "vm.h"
#include "Virtual Machine Challenge.h"
//#include <intrin.h>

using namespace std::chrono_literals;

void vm::setRegValue(uint16_t a, uint16_t val)
{
	if(IS_REG(a) && IS_VALID_VALUE(val)) {
		int reg = a - (uint16_t)0x8000;
		cpuregisters[reg] = val;
	}
}

vm::vm() : processorState(state::waiting_for_file), sp(-1), pc(0), breakatpc(0xffff)
{
	reset();
}

bool vm::load(const wxChar *filename)
{
	reset();

	std::ifstream program(filename, std::ios::binary | std::ios::ate);
	if(!program) {
		wxString msg;
		msg << L"Error Loading File " << filename;
		wxMessageDialog dial(nullptr,
							 msg, wxT("Error"), wxOK | wxICON_ERROR);
		dial.ShowModal();
		return false;
	}

	std::streampos binSize = program.tellg();
	std::vector<unsigned char> buffer(binSize);
	program.seekg(0, std::ios::beg);
	program.read((char *)&buffer[0], binSize);
	memory.resize((binSize / 2) + 1);
	std::memcpy(&memory[0], &buffer[0], binSize);
	program.close();
	return true;
}

bool vm::savebin(const wxChar *filename)
{
	state savestate = processorState;
	if(processorState == state::running || processorState == state::processing_string) {
		processorState = state::paused;
		std::this_thread::sleep_for(100ms);
	}
	std::fstream bin(filename, std::ios::binary | std::ios::out | std::ios::trunc);
	if(bin.is_open()) {
		bin.write(reinterpret_cast<char *>(&this->memory[0]), memory.size() * sizeof(uint16_t));

		bin.close();
	}
	else {
		return false;
	}
	processorState = savestate;
	return true;
}

bool vm::savestate(const wxChar *filename)
{
	state savestate = processorState;
	if(processorState == state::running || processorState == state::processing_string) {
		processorState = state::paused;
		std::this_thread::sleep_for(100ms);
	}
	std::fstream state(filename, std::ios::binary | std::ios::out | std::ios::trunc);
	if(state.is_open()) {
		/*
		savestate
		cpuregisters
		program counter
		memory
		stack
		stack pointer
		*/
		uint16_t savestatevalue = (uint16_t)savestate;
		state.write(reinterpret_cast<char *>(&savestatevalue), sizeof(savestatevalue));
		state.write(reinterpret_cast<char *>(const_cast<uint16_t *>(&placeholder)), sizeof(placeholder));

		state.write(reinterpret_cast<char *>(&this->cpuregisters), sizeof(this->cpuregisters));
		state.write(reinterpret_cast<char *>(const_cast<uint16_t *>(&placeholder)), sizeof(placeholder));

		state.write(reinterpret_cast<char *>(&this->pc), sizeof(this->pc));
		state.write(reinterpret_cast<char *>(const_cast<uint16_t *>(&placeholder)), sizeof(placeholder));

		state.write(reinterpret_cast<char *>(&this->memory[0]), memory.size() * sizeof(uint16_t));
		state.write(reinterpret_cast<char *>(const_cast<uint16_t *>(&placeholder)), sizeof(placeholder));

		state.write(reinterpret_cast<char *>(&this->stack), sizeof(this->stack));
		state.write(reinterpret_cast<char *>(const_cast<uint16_t *>(&placeholder)), sizeof(placeholder));

		state.write(reinterpret_cast<char *>(&this->sp), sizeof(this->sp));
		state.write(reinterpret_cast<char *>(const_cast<uint16_t *>(&placeholder)), sizeof(placeholder));

		state.close();
	}
	else {
		return false;
	}
	processorState = savestate;
	return true;
}

/**
 * @brief Load the state of the VM File Structure is savestate, cpuregisters, program counter, memory, stack, stack pointer
 * @param filename File to load as the state. 
 * @return True if sussess False if failed.
 */
bool vm::loadstate(const wxChar *filename)
{
	state loadstate = state::waiting_for_file;
	if(processorState == state::running || processorState == state::processing_string) {
		processorState = state::paused;
		std::this_thread::sleep_for(100ms);
	}

	std::fstream myfile(filename, std::ios::in | std::ios::binary | std::ios::ate);
	std::vector<uint16_t> buffer;
	std::vector<uint16_t> membuffer;

	if(myfile.is_open()) {
		std::streampos binSize = myfile.tellg();
		std::vector<unsigned char> tmp(binSize);
		myfile.seekg(0, std::ios::beg);
		myfile.read(reinterpret_cast<char *>(&tmp[0]), binSize);
		buffer.resize((binSize / 2) + 1);
		std::memcpy(&buffer[0], &tmp[0], binSize);
		myfile.close();
		tmp.clear();
		membuffer.resize((binSize / 2) + 1);
		std::fill(membuffer.begin(), membuffer.end(), 0xffff);
	}
	else return false;

	uint8_t areacount = 0;
	uint16_t cnt = 0;
	int32_t tmpsp = 0;

	for(uint16_t offset : buffer)
	{

		/*
		savestate
		cpuregisters
		program counter
		memory
		stack
		stack pointer
		*/
		if(offset == placeholder) {
			areacount++;
			cnt = 0;
			continue;
		}
		switch(areacount)
		{
		case 0:
			loadstate = (state)offset;
			break;
		case 1:
			this->cpuregisters[cnt++] = offset;
			break;
		case 2:
			this->pc = offset;
			break;
		case 3:
			membuffer[cnt++] = offset;
			break;
		case 4:
			this->stack[cnt++] = offset;
			break;
		case 5:
			if(cnt == 0) {
				tmpsp = offset;
				cnt++;
			}
			else if(cnt == 1) {
				tmpsp = tmpsp + (offset << 16);
				this->sp = tmpsp;
			}
			break;
		case 6:
			membuffer.erase(std::remove(membuffer.begin(), membuffer.end(), 0xffff), membuffer.end());
			this->memory.resize(membuffer.size());
			std::memcpy(&memory[0], &membuffer[0], membuffer.size() * sizeof(uint16_t));
		}
	}

	myfile.close();
	buffer.clear();
	membuffer.clear();
	reloadedstate = true;
	processorState = loadstate;
	wxGetApp().GetTopWindow()->GetEventHandler()->CallAfter(&vmMainWnd::UpdateUI);
	return true;
}
void vm::start()
{
	if(!reloadedstate) {
		processorState = state::running;
	}
#ifdef _WINDOWS
	HRESULT hr = SetThreadDescription(GetCurrentThread(), L"A VM THREAD");
	if(FAILED(hr)) {
		processorState = state::error;
	}
#endif
	vmProc();
}

void vm::vmProc()
{
	uint32_t ticks = 1;

	while(processorState != state::error && processorState != state::stopped && pc < memory.size()) {

		if(pc >= memory.size()) {
			HLT();
			continue;
		}

		if(pc == breakatpc) {
#ifdef _DEBUG
			__debugbreak();
#endif
		}

		if(processorState == state::running || processorState == state::processing_string) {
			uint16_t op = memory[pc++];

			if(IS_VALID_OPCODE(op)) {
				(this->*jmpTable[op])();
			}
			else {
				wxString errorstring;
				errorstring << "Invalid instruction 0x" << wxString::Format("%04x", op)  << " at address 0x" << wxString::Format("%04x", pc - 1) << '\n';
				wxGetApp().GetTopWindow()->GetEventHandler()->CallAfter<vmMainWnd, wxString, wxString>(&vmMainWnd::UpdateOutput, errorstring);
			}
		}
		else {
			std::this_thread::sleep_for(1000ms);
		}

		if(!(ticks % 0x100)) {
			wxGetApp().GetTopWindow()->GetEventHandler()->CallAfter(&vmMainWnd::UpdateUI);
			std::this_thread::sleep_for(5ms);
			ticks = 0;
		}

		ticks++;
	}
	wxGetApp().GetTopWindow()->GetEventHandler()->CallAfter(&vmMainWnd::UpdateUI);
}

bool vm::fix_teleporter()
{
	if(this->processorState == state::waiting_for_file)
		return false;
	state oldstate = this->processorState;

	uint16_t newfuction[] = {0x0001, 0x8001, 0x8007, 0x0009, 0x8000, 0x8001, 0x0001, 0x0012};
	if(this->processorState == state::running || state::waiting_for_string == this->processorState) {
		this->processorState = state::paused;
		std::this_thread::sleep_for(100ms);
	}
	uint16_t *memlocation = &this->memory[0x178b];
	for(int x = 0; x <= 7; x++) {
		*memlocation = newfuction[x];
		memlocation++;
	}

	processorState = oldstate;

	return true;
}

void vm::HLT()
{
	stop();
}

void vm::SET()
{
	uint16_t a = memory[pc++];
	uint16_t b = getMemory(memory[pc++]);
	setRegValue(a, b);
}

void vm::PUSH()
{
	uint16_t a = memory[pc++];
	stack[++sp] = getMemory(a);
}

void vm::POP()
{
	if(sp < 0 || sp > STACK_MAX) {
		HLT();
		return;
	}
	uint16_t a = memory[pc++];
	setRegValue(a, stack[sp--]);
}

void vm::EQ()
{
	uint16_t a = memory[pc++];
	uint16_t b = getMemory(memory[pc++]);
	uint16_t c = getMemory(memory[pc++]);
	setRegValue(a, b == c);
}

void vm::GT()
{
	uint16_t a = memory[pc++];
	uint16_t b = getMemory(memory[pc++]);
	uint16_t c = getMemory(memory[pc++]);
	setRegValue(a, b > c);
}

void vm::JMP()
{
	uint16_t addr = getMemory(memory[pc]);
	pc = addr;
}

void vm::JT()
{
	uint16_t a = getMemory(memory[pc++]);
	uint16_t b = getMemory(memory[pc++]);
	if(a > 0) pc = b;
}

void vm::JF()
{
	uint16_t a = getMemory(memory[pc++]);
	uint16_t b = getMemory(memory[pc++]);
	if(!a) pc = b;
}

void vm::ADD()
{
	uint16_t a = memory[pc++];
	uint16_t b = getMemory(memory[pc++]);
	uint16_t c = getMemory(memory[pc++]);
	setRegValue(a, (b + c) % (uint16_t)0x8000);
}

void vm::MULT()
{
	uint16_t a = memory[pc++];
	uint16_t b = getMemory(memory[pc++]);
	uint16_t c = getMemory(memory[pc++]);
	setRegValue(a, (b * c) % (uint16_t)0x8000);
}

void vm::MOD()
{
	uint16_t a = memory[pc++];
	uint16_t b = getMemory(memory[pc++]);
	uint16_t c = getMemory(memory[pc++]);
	setRegValue(a, (b % c) % (uint16_t)0x8000);
}

void vm::AND()
{
	uint16_t a = memory[pc++];
	uint16_t b = getMemory(memory[pc++]);
	uint16_t c = getMemory(memory[pc++]);
	setRegValue(a, (b & c));
}

void vm::OR()
{
	uint16_t a = memory[pc++];
	uint16_t b = getMemory(memory[pc++]);
	uint16_t c = getMemory(memory[pc++]);
	setRegValue(a, (b | c));
}

void vm::NOT()
{
	uint16_t a = memory[pc++];
	uint16_t b = getMemory(memory[pc++]);
	setRegValue(a, b ^ (uint16_t)0x7FFF);
}

void vm::RMEM()
{
	uint16_t a = memory[pc++];
	uint16_t b = memory[getMemory(memory[pc++])];
	setRegValue(a, b);
}

void vm::WMEM()
{
	uint16_t a = getMemory(memory[pc++]);
	uint16_t b = getMemory(memory[pc++]);
	memory[a] = b;
}

void vm::CALL()
{
	sp++;
	if(sp > -1) {
		stack[sp] = pc + 1;
		callstack.push_back(pc - 1);
		pc = getMemory(memory[pc]);
	}
}

void vm::RET()
{
	if(sp < 0 || sp > STACK_MAX) {
		HLT();
		return;
	}
	uint16_t a = stack[sp--];
	/* using a callstack was added after the Save state and is currently not added to the save state                          *
	 * No current plans to add the call stack to the save state and is only used to track the area's of the akerman function. */
	if(!callstack.empty()) {
		callstack.pop_back();
	}
	pc = a;
}

void vm::OUT()
{
	uint16_t a = getMemory(memory[pc++]);
	wxGetApp().GetTopWindow()->GetEventHandler()->CallAfter<vmMainWnd, char, char>(&vmMainWnd::UpdateOutput, static_cast<char>(a));
}

void vm::IN()
{
	if(processorState == state::processing_string) {
		uint16_t a = memory[pc++];
		char c = inputstring.str[inputstring.position++].GetValue();
		setRegValue(a, c);
		if(inputstring.position >= inputstring.str.size()) {
			processorState = state::running;
		}
	}
	else if (processorState == state::running) {
		processorState = state::waiting_for_string;
	}

	wxGetApp().GetTopWindow()->GetEventHandler()->CallAfter(&vmMainWnd::UpdateUI);
}

void vm::commandstring(wxString &string)
{
	inputstring.str = string;
	inputstring.position = 0;
	--pc;
	processorState = state::processing_string;
}

void vm::NOP()
{
}