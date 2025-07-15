#include "vmdism.h"

void vmDisassembler::doDisassembly()
{
	//char buffer[0x100]{0};
	std::stringstream buffer;

	for(size_t i = 0; i < (memory.size() - 1);)
	{
		buffer.str("");
		buffer.clear();

		if(IS_VALID_OPCODE(memory[i]) && i < 0x17b4 ) {
			uint16_t op = memory[i++];
			OpcodeInfo opInfo = opcodes[op];

			buffer << "[0x" << std::uppercase << std::setfill('0') << std::setw(4) << std::hex << i - 1 << "] " <<
				std::setfill(' ') << std::setw(5) << opInfo.name;
			for(std::size_t j = 0; j < opInfo.nargs; j++) {
				if(IS_REG(memory[i]))
					buffer << (j ? ",     r" : "     r") << memory[i] - 0x8000;
				else
					buffer << (j ? ", 0x" : " 0x") << std::uppercase << std::setfill('0') << std::setw(4) << std::hex << memory[i];

				i++;
			}

			if(opInfo.name == "OUT") {
				char byte = (char)memory[i - 1];
				buffer << DecodeANSII(byte);
			}
			writer << buffer.str() << std::endl;
		}
		else {
			uint16_t byte = memory[i++];
			buffer << "[0x" << std::uppercase << std::setfill('0') << std::setw(4) << std::hex << i - 1 << "] 0x" <<
				std::setfill('0') << std::setw(4) << std::hex << byte << DecodeANSII(byte);

			writer << buffer.str() << std::endl;
		}
	}
	writer.close();
}

std::string vmDisassembler::DecodeANSII(char ansii)
{
	switch(ansii)
	{
	case '\n':
		return "  ; New Line";
		break;
	case '\r':
		return "  ; Line Feed";
		break;
	case '\t':
		return "  ; Tab";
		break;
	case '\b':
		return "  ; backspace";
		break;
	case ' ':
		return " ";
		break;
	default:
		std::string ret = "  ; ";
		ret.append((char *)&ansii, 1);
		return ret;
		break;
	}
}

void vmDisassembler::disassemble(const char *in, const char *out)
{
	std::ifstream program(in, std::ios::binary | std::ios::ate);
	if(!program) {
		wxString errormessage;
		errormessage << "Error loading " << in << "for reading";
		wxMessageBox(errormessage, wxT("ERROR"), wxICON_ERROR);
		return;
	}

	writer.open(out, std::ios::out | std::ios::trunc);

	if(!writer) {
		wxString errormessage;
		errormessage << "Couldn't open " << out << " for writing";
		wxMessageBox(errormessage, wxT("ERROR"), wxICON_ERROR);
		return;
	}

	std::streampos progSize = program.tellg();
	std::vector<unsigned char> buffer(progSize);
	program.seekg(0, std::ios::beg);
	program.read((char *)&buffer[0], progSize);
	memory.resize((progSize / 2) + 1);
	std::memcpy(&memory[0], &buffer[0], progSize);
	program.close();

	doDisassembly();
}

void vmDisassembler::disassemble(std::vector<uint16_t> in, const char *out)
{
	writer.open(out, std::ios::out | std::ios::trunc);

	if(!writer) {
		wxString errormessage;
		errormessage << "Couldn't open " << out << " for writing";
		wxMessageBox(errormessage, wxT("ERROR"), wxICON_ERROR);
		return;
	}

	memory.resize(in.size());
	memory = in;

	doDisassembly();
}
