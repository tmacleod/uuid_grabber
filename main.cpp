#include <windows.h>
#include <array>
#include <iostream>
#include <string>

std::wstring executeCommand(const wchar_t* cmd) {
    std::array<wchar_t, 128> buffer;
    std::wstring result;
    std::unique_ptr<FILE, decltype(&_pclose)> pipe(_wpopen(cmd, L"r"), _pclose);
    if (!pipe) {
        throw std::runtime_error("_wpopen() failed!");
    }
    while (fgetws(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
    // Get the UUID
    std::wstring uuid = executeCommand(L"wmic csproduct get uuid");

    // Create the message box
    MessageBoxW(NULL, uuid.c_str(), L"Windows UUID", MB_OK);

    // Open clipboard
    if (!OpenClipboard(NULL)) {
        std::cerr << "Cannot open the Clipboard" << std::endl;
        return 0;
    }

    // Empty Clipboard
    EmptyClipboard();

    // Allocate a global memory object for the text
    HGLOBAL hglbCopy = GlobalAlloc(GMEM_MOVEABLE, (uuid.size() + 1) * sizeof(wchar_t));
    if (hglbCopy == NULL) {
        std::cerr << "Could not allocate memory" << std::endl;
        CloseClipboard();
        return 0;
    }

    // Lock the handle and copy the text to the buffer
    LPWSTR lpwstrCopy = (LPWSTR)GlobalLock(hglbCopy);
    memcpy(lpwstrCopy, uuid.c_str(), (uuid.size() + 1) * sizeof(wchar_t));
    lpwstrCopy[uuid.size()] = (wchar_t)0;    // null character
    GlobalUnlock(hglbCopy);

    // Place the handle on the clipboard
    SetClipboardData(CF_UNICODETEXT, hglbCopy);

    CloseClipboard();

    return 0;
}
