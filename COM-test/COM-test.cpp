#include <windows.h>
#include <iostream>

using namespace std;

void ConfigureSerialPort(HANDLE hSerial) {
    DCB dcbSerialParams = { 0 };
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if (!GetCommState(hSerial, &dcbSerialParams)) {
        cerr << "Ошибка получения параметров COM-порта\n";
        return;
    }

    dcbSerialParams.BaudRate = CBR_9600;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;

    if (!SetCommState(hSerial, &dcbSerialParams)) {
        cerr << "Ошибка установки параметров COM-порта\n";
    }   

    COMMTIMEOUTS timeouts = { 0 };
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;
    SetCommTimeouts(hSerial, &timeouts);
}

void SendData(HANDLE hSerial, const char* data) {
    DWORD bytesWritten;
    WriteFile(hSerial, data, strlen(data), &bytesWritten, NULL);
    cout << "Отправлено: " << data << endl;
}

void ReceiveData(HANDLE hSerial) {
    char buffer[256] = { 0 };
    DWORD bytesRead;

    BOOL result = ReadFile(hSerial, buffer, sizeof(buffer) - 1, &bytesRead, NULL);

    if (result && bytesRead > 0) {
        buffer[bytesRead] = '\0';
        cout << "Принято: " << buffer << endl;
    }
    else {
        cerr << "Ошибка приема данных\n";
    }
}

int main() {
    setlocale(LC_ALL, "Russian");

    int portNumber;
    cout << "Введите номер COM-порта (например, 1 для COM1): ";
    cin >> portNumber;

    if (portNumber < 1 || portNumber > 256) {
        cerr << "Недопустимый номер порта\n";
        system("pause");
        return 1;
    }
    wchar_t portName[20];
    swprintf_s(portName, L"\\\\.\\COM%d", portNumber);

    HANDLE hSerial = CreateFileW(portName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (hSerial == INVALID_HANDLE_VALUE) {
        cerr << "Ошибка открытия COM-порта " << portNumber << endl;
        system("pause");
        return 1;
    }

    ConfigureSerialPort(hSerial);

    const char* message = "Loopback test!";
    SendData(hSerial, message);
    ReceiveData(hSerial);

    CloseHandle(hSerial);
    system("pause");
    return 0;
}

