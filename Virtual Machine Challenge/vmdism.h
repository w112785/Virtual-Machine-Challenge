#pragma once
#include "vm.h"
#include <sstream>
#include <fstream>
#include <wx/dialog.h>
#include <wx/msgdlg.h>
#include <wx/radiobut.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/stattext.h>
#include <chrono>
#include <iomanip>
#include <vector>

// Class Deffinessions are in vmdism.cpp
class vmDisassembler
{
private:
    std::ofstream writer;
    std::vector<uint16_t> memory;

    struct OpcodeInfo
    {
        const char *name;
        uint16_t nargs;
    };

    const OpcodeInfo opcodes[OPCODE_COUNT] =
    {
        {"HLT", 0},
        {"SET", 2},
        {"PUSH", 1},
        {"POP", 1},
        {"EQ", 3},
        {"GT", 3},
        {"JMP", 1},
        {"JT", 2},
        {"JF", 2},
        {"ADD", 3},
        {"MULT", 3},
        {"MOD", 3},
        {"AND", 3},
        {"OR", 3},
        {"NOT", 2},
        {"RMEM", 2},
        {"WMEM", 2},
        {"CALL", 1},
        {"RET", 0},
        {"OUT", 1},
        {"IN", 1},
        {"NOP", 0}
    };

    void doDisassembly();
    std::string DecodeANSII(char ansii);

public:
    void disassemble(const char *in, const char *out);
    void disassemble(std::vector<uint16_t> in, const char *out);
};


// Class Deffinessions are in vmdismdlg.cpp
class vmDismDLG : public wxDialog {
public:
    vmDismDLG(wxWindow *parent, std::vector<uint16_t> mem);
    virtual ~vmDismDLG() = default;

protected:
    std::vector<uint16_t> memory;
    wxRadioButton *radioDecompMemory;
    wxRadioButton *radioDecompFile;
    wxTextCtrl *txtctrlFileLoadFrom;
    wxButton *btnLoadFile;
    wxStaticText *m_staticText2;
    wxTextCtrl *txtctrlSaveFileTo;
    wxButton *btnSaveFile;
    wxButton *btnDecompile;
    wxButton *btnCancel;

    void btnLoadFileOnButtonClick(wxCommandEvent &evt);
    void btnSaveFileOnButtonClick(wxCommandEvent &evt);
    void btnDecompileOnButtonClick(wxCommandEvent &evt);
    void btnCancelOnButtonClick(wxCommandEvent &evt);

    void SetupWindow();
};