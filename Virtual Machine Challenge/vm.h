#pragma once

#include <vector>
#include <fstream>
#include <chrono>
#include <deque>
#include <array>
#undef IN
#undef OUT
#include <wx/string.h>

#define IS_REG(x)			((x) > (uint16_t)0x7FFF)
#define IS_VALID_VALUE(x)	((x) < (uint16_t)0x8000)
#define IS_VALID_OPCODE(x)	((x) < (uint16_t)0x0016)

constexpr uint16_t placeholder = 0xfccc;
constexpr uint16_t STACK_MAX = 0x8000;
constexpr uint16_t OPCODE_COUNT = 0x0016;

enum class state : uint16_t {
	running,
	paused,
	stopped,
	error,
	waiting_for_string,
	processing_string,
	waiting_for_file
};

enum class registers : uint16_t {
	R_R0,
	R_R1,
	R_R2,
	R_R3,
	R_R4,
	R_R5,
	R_R6,
	R_R7,
};

class vm {
protected:
	/*
	 *    -----------------------------------------
	 *    | R0 | R1 | R2 | R3 | R4 | R5 | R6 | R7 |
	 *    -----------------------------------------
	 */
	std::array<uint16_t, (int)registers::R_R7+1> cpuregisters;

	//uint16_t cpuregisters2[(int)registers::R_R7+1]{0};
	uint16_t pc;
	uint16_t breakatpc;

	std::vector<uint16_t> memory;
	std::deque<uint16_t> callstack;
	std::array<uint16_t, STACK_MAX> stack;
	//uint16_t stack[STACK_MAX]{0};
	int32_t sp;
	state processorState;
	struct {
		wxString str;
		int position = 0;
	} inputstring;
	
	bool reloadedstate = false;

	uint16_t getMemory(uint16_t val) {
		return IS_REG(val) ? cpuregisters[val - (uint16_t)0x8000] : val;
	}

	void setRegValue(uint16_t a, uint16_t val);

public:

	void reset() {
		cpuregisters.fill(0);
		stack.fill(0xffff);

		memory.clear();
		callstack.clear();
		sp = -1;
		pc = 0;

		reloadedstate = false;
	}

	vm();

	uint16_t getregister(registers reg) const
	{
		return cpuregisters[(int)reg];
	}

	uint16_t GetProgramCounter() noexcept { return pc; }
	int32_t GetStackPointer() noexcept { return sp; }

	void stop() noexcept { processorState = state::stopped; }

	std::vector<uint16_t> GetMemory() const noexcept {
		return memory;
	}

	bool load(const wxChar *filename);
	bool savebin(const wxChar *filename);
	bool savestate(const wxChar *filename);
	bool loadstate(const wxChar *filename);
	void start();
	void vmProc();
	void commandstring(wxString &str);
	bool fix_teleporter();

	bool IsPaused() const noexcept { return (processorState == state::paused); }
	bool IsWaitingForString() const noexcept { return (processorState == state::waiting_for_string); }

	void pause() noexcept { processorState = state::paused; }
	void resume() noexcept { if(processorState == state::paused) processorState = state::running; }
	state GetState() const noexcept { return processorState; }

private:

	vm(const vm &) = delete;

	// Opcodes
	void HLT();
	void SET();
	void PUSH();
	void POP();
	void EQ();
	void GT();
	void JMP();
	void JT();
	void JF();
	void ADD();
	void MULT();
	void MOD();
	void AND();
	void OR();
	void NOT();
	void RMEM();
	void WMEM();
	void CALL();
	void RET();
	void OUT();
	void IN();
	void NOP();

	void (vm:: *jmpTable[OPCODE_COUNT])(void) =
	{
		&vm::HLT,
		&vm::SET,
		&vm::PUSH,
		&vm::POP,
		&vm::EQ,
		&vm::GT,
		&vm::JMP,
		&vm::JT,
		&vm::JF,
		&vm::ADD,
		&vm::MULT,
		&vm::MOD,
		&vm::AND,
		&vm::OR,
		&vm::NOT,
		&vm::RMEM,
		&vm::WMEM,
		&vm::CALL,
		&vm::RET,
		&vm::OUT,
		&vm::IN,
		&vm::NOP
	};

	friend class DebugRegisters; // So we can better edit the system flags and registers without a bunch of Geters and seters
};

