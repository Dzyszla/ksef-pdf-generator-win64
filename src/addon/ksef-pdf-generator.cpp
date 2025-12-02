#include <windows.h>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <process.h>
#include <io.h>
#include <fcntl.h>

// Funkcje pomocnicze
std::string GetExecutablePath() {
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    std::string path(buffer);
    size_t pos = path.find_last_of("\\/");
    return (pos != std::string::npos) ? path.substr(0, pos + 1) : "";
}

std::string GetTempFilePath(const std::string& prefix, const std::string& extension) {
    char tempPath[MAX_PATH];
    char tempFile[MAX_PATH];
    
    if (GetTempPathA(MAX_PATH, tempPath) == 0) {
        return "";
    }
    
    if (GetTempFileNameA(tempPath, prefix.c_str(), 0, tempFile) == 0) {
        return "";
    }
    
    // Zmień rozszerzenie
    std::string result(tempFile);
    size_t dotPos = result.find_last_of('.');
    if (dotPos != std::string::npos) {
        result = result.substr(0, dotPos);
    }
    return result + extension;
}

int ExecuteProcess(const std::string& exePath, const std::vector<std::string>& args, std::string& output, std::string& error) {
    std::string commandLine = "\"" + exePath + "\"";
    for (const auto& arg : args) {
        commandLine += " \"" + arg + "\"";
    }
    
    HANDLE hChildStd_OUT_Rd = NULL;
    HANDLE hChildStd_OUT_Wr = NULL;
    HANDLE hChildStd_ERR_Rd = NULL;
    HANDLE hChildStd_ERR_Wr = NULL;
    
    SECURITY_ATTRIBUTES saAttr;
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;
    
    // Utwórz pipe dla stdout
    if (!CreatePipe(&hChildStd_OUT_Rd, &hChildStd_OUT_Wr, &saAttr, 0)) {
        error = "Nie można utworzyć pipe dla stdout";
        return -1;
    }
    if (!SetHandleInformation(hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0)) {
        error = "Nie można ustawić informacji dla stdout";
        CloseHandle(hChildStd_OUT_Rd);
        CloseHandle(hChildStd_OUT_Wr);
        return -1;
    }
    
    // Utwórz pipe dla stderr
    if (!CreatePipe(&hChildStd_ERR_Rd, &hChildStd_ERR_Wr, &saAttr, 0)) {
        error = "Nie można utworzyć pipe dla stderr";
        CloseHandle(hChildStd_OUT_Rd);
        CloseHandle(hChildStd_OUT_Wr);
        return -1;
    }
    if (!SetHandleInformation(hChildStd_ERR_Rd, HANDLE_FLAG_INHERIT, 0)) {
        error = "Nie można ustawić informacji dla stderr";
        CloseHandle(hChildStd_OUT_Rd);
        CloseHandle(hChildStd_OUT_Wr);
        CloseHandle(hChildStd_ERR_Rd);
        CloseHandle(hChildStd_ERR_Wr);
        return -1;
    }
    
    PROCESS_INFORMATION piProcInfo;
    STARTUPINFOA siStartInfo;
    
    ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));
    ZeroMemory(&siStartInfo, sizeof(STARTUPINFOA));
    
    siStartInfo.cb = sizeof(STARTUPINFOA);
    siStartInfo.hStdError = hChildStd_ERR_Wr;
    siStartInfo.hStdOutput = hChildStd_OUT_Wr;
    siStartInfo.dwFlags |= STARTF_USESTDHANDLES;
    
    // Utwórz proces
    BOOL bSuccess = CreateProcessA(
        NULL,
        const_cast<char*>(commandLine.c_str()),
        NULL,
        NULL,
        TRUE,
        0,
        NULL,
        NULL,
        &siStartInfo,
        &piProcInfo
    );
    
    if (!bSuccess) {
        error = "Nie można utworzyć procesu";
        CloseHandle(hChildStd_OUT_Rd);
        CloseHandle(hChildStd_OUT_Wr);
        CloseHandle(hChildStd_ERR_Rd);
        CloseHandle(hChildStd_ERR_Wr);
        return -1;
    }
    
    // Zamknij końce zapisu pipe
    CloseHandle(hChildStd_OUT_Wr);
    CloseHandle(hChildStd_ERR_Wr);
    
    // Odczytaj output
    DWORD dwRead;
    CHAR chBuf[4096];
    output = "";
    error = "";
    
    // Odczytaj stdout
    while (true) {
        bSuccess = ReadFile(hChildStd_OUT_Rd, chBuf, 4096, &dwRead, NULL);
        if (!bSuccess || dwRead == 0) break;
        output.append(chBuf, dwRead);
    }
    
    // Odczytaj stderr
    while (true) {
        bSuccess = ReadFile(hChildStd_ERR_Rd, chBuf, 4096, &dwRead, NULL);
        if (!bSuccess || dwRead == 0) break;
        error.append(chBuf, dwRead);
    }
    
    // Czekaj na zakończenie procesu
    WaitForSingleObject(piProcInfo.hProcess, INFINITE);
    
    DWORD exitCode;
    GetExitCodeProcess(piProcInfo.hProcess, &exitCode);
    
    CloseHandle(piProcInfo.hProcess);
    CloseHandle(piProcInfo.hThread);
    CloseHandle(hChildStd_OUT_Rd);
    CloseHandle(hChildStd_ERR_Rd);
    
    return exitCode;
}

// Funkcja do wykonywania procesu ze strumieniami (stdin/stdout)
int ExecuteProcessWithStreams(
    const std::string& exePath,
    const std::vector<std::string>& args,
    const std::vector<uint8_t>& inputData,
    std::vector<uint8_t>& outputData,
    std::string& error
) {
    HANDLE hChildStd_IN_Rd = NULL;
    HANDLE hChildStd_IN_Wr = NULL;
    HANDLE hChildStd_OUT_Rd = NULL;
    HANDLE hChildStd_OUT_Wr = NULL;
    HANDLE hChildStd_ERR_Rd = NULL;
    HANDLE hChildStd_ERR_Wr = NULL;
    
    SECURITY_ATTRIBUTES saAttr;
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;
    
    // Utwórz pipe dla stdin
    if (!CreatePipe(&hChildStd_IN_Rd, &hChildStd_IN_Wr, &saAttr, 0)) {
        error = "Nie można utworzyć pipe dla stdin";
        return -1;
    }
    if (!SetHandleInformation(hChildStd_IN_Wr, HANDLE_FLAG_INHERIT, 0)) {
        error = "Nie można ustawić informacji dla stdin";
        CloseHandle(hChildStd_IN_Rd);
        CloseHandle(hChildStd_IN_Wr);
        return -1;
    }
    
    // Utwórz pipe dla stdout
    if (!CreatePipe(&hChildStd_OUT_Rd, &hChildStd_OUT_Wr, &saAttr, 0)) {
        error = "Nie można utworzyć pipe dla stdout";
        CloseHandle(hChildStd_IN_Rd);
        CloseHandle(hChildStd_IN_Wr);
        return -1;
    }
    if (!SetHandleInformation(hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0)) {
        error = "Nie można ustawić informacji dla stdout";
        CloseHandle(hChildStd_IN_Rd);
        CloseHandle(hChildStd_IN_Wr);
        CloseHandle(hChildStd_OUT_Rd);
        CloseHandle(hChildStd_OUT_Wr);
        return -1;
    }
    
    // Utwórz pipe dla stderr
    if (!CreatePipe(&hChildStd_ERR_Rd, &hChildStd_ERR_Wr, &saAttr, 0)) {
        error = "Nie można utworzyć pipe dla stderr";
        CloseHandle(hChildStd_IN_Rd);
        CloseHandle(hChildStd_IN_Wr);
        CloseHandle(hChildStd_OUT_Rd);
        CloseHandle(hChildStd_OUT_Wr);
        return -1;
    }
    if (!SetHandleInformation(hChildStd_ERR_Rd, HANDLE_FLAG_INHERIT, 0)) {
        error = "Nie można ustawić informacji dla stderr";
        CloseHandle(hChildStd_IN_Rd);
        CloseHandle(hChildStd_IN_Wr);
        CloseHandle(hChildStd_OUT_Rd);
        CloseHandle(hChildStd_OUT_Wr);
        CloseHandle(hChildStd_ERR_Rd);
        CloseHandle(hChildStd_ERR_Wr);
        return -1;
    }
    
    PROCESS_INFORMATION piProcInfo;
    STARTUPINFOA siStartInfo;
    
    ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));
    ZeroMemory(&siStartInfo, sizeof(STARTUPINFOA));
    
    siStartInfo.cb = sizeof(STARTUPINFOA);
    siStartInfo.hStdInput = hChildStd_IN_Rd;
    siStartInfo.hStdOutput = hChildStd_OUT_Wr;
    siStartInfo.hStdError = hChildStd_ERR_Wr;
    siStartInfo.dwFlags |= STARTF_USESTDHANDLES;
    
    std::string commandLine = "\"" + exePath + "\"";
    for (const auto& arg : args) {
        commandLine += " \"" + arg + "\"";
    }
    commandLine += " --stream";
    
    BOOL bSuccess = CreateProcessA(
        NULL,
        const_cast<char*>(commandLine.c_str()),
        NULL,
        NULL,
        TRUE,
        0,
        NULL,
        NULL,
        &siStartInfo,
        &piProcInfo
    );
    
    if (!bSuccess) {
        error = "Nie można utworzyć procesu";
        CloseHandle(hChildStd_IN_Rd);
        CloseHandle(hChildStd_IN_Wr);
        CloseHandle(hChildStd_OUT_Rd);
        CloseHandle(hChildStd_OUT_Wr);
        CloseHandle(hChildStd_ERR_Rd);
        CloseHandle(hChildStd_ERR_Wr);
        return -1;
    }
    
    // Zamknij końce odczytu w procesie rodzica
    CloseHandle(hChildStd_IN_Rd);
    CloseHandle(hChildStd_OUT_Wr);
    CloseHandle(hChildStd_ERR_Wr);
    
    // Zapisz dane wejściowe do stdin
    DWORD dwWritten;
    if (inputData.size() > 0) {
        if (!WriteFile(hChildStd_IN_Wr, inputData.data(), inputData.size(), &dwWritten, NULL)) {
            error = "Nie można zapisać danych do stdin";
            CloseHandle(piProcInfo.hProcess);
            CloseHandle(piProcInfo.hThread);
            CloseHandle(hChildStd_IN_Wr);
            CloseHandle(hChildStd_OUT_Rd);
            CloseHandle(hChildStd_ERR_Rd);
            return -1;
        }
    }
    CloseHandle(hChildStd_IN_Wr);
    
    // Odczytaj dane wyjściowe z stdout
    DWORD dwRead;
    CHAR chBuf[4096];
    outputData.clear();
    
    while (true) {
        bSuccess = ReadFile(hChildStd_OUT_Rd, chBuf, 4096, &dwRead, NULL);
        if (!bSuccess || dwRead == 0) break;
        outputData.insert(outputData.end(), chBuf, chBuf + dwRead);
    }
    
    // Odczytaj błędy z stderr
    error = "";
    while (true) {
        bSuccess = ReadFile(hChildStd_ERR_Rd, chBuf, 4096, &dwRead, NULL);
        if (!bSuccess || dwRead == 0) break;
        error.append(chBuf, dwRead);
    }
    
    // Czekaj na zakończenie procesu
    WaitForSingleObject(piProcInfo.hProcess, INFINITE);
    
    DWORD exitCode;
    GetExitCodeProcess(piProcInfo.hProcess, &exitCode);
    
    CloseHandle(piProcInfo.hProcess);
    CloseHandle(piProcInfo.hThread);
    CloseHandle(hChildStd_OUT_Rd);
    CloseHandle(hChildStd_ERR_Rd);
    
    return exitCode;
}
