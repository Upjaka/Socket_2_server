#include <iostream>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <WinSock2.h>

#pragma comment(lib, "Ws2_32.lib")

const int PORT = 12000;
const int MAX_BUFFER_SIZE = 1024;

int main() {
    WSADATA wsaData;
    int iResult;

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        std::cerr << "WSAStartup failed: " << iResult << std::endl;
        return 1;
    }

    // Create a UDP socket
    SOCKET serverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Error creating socket: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    // Bind the socket to the port
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    iResult = bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
    if (iResult == SOCKET_ERROR) {
        std::cerr << "Bind failed with error: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Server is running and listening for incoming UDP packets..." << std::endl;

    srand(static_cast<unsigned int>(time(nullptr)));

    while (true) {
        char buffer[MAX_BUFFER_SIZE];
        sockaddr_in clientAddr;
        int clientAddrSize = sizeof(clientAddr);

        // Receive data from client
        int bytesReceived = recvfrom(serverSocket, buffer, MAX_BUFFER_SIZE, 0, (sockaddr*)&clientAddr, &clientAddrSize);
        if (bytesReceived == SOCKET_ERROR) {
            std::cerr << "recvfrom failed with error: " << WSAGetLastError() << std::endl;
            closesocket(serverSocket);
            WSACleanup();
            return 1;
        }

        // Process received data
        if (rand() % 10 >= 4) {
            for (int i = 0; i < bytesReceived; ++i) {
                buffer[i] = toupper(buffer[i]);
            }
            // Send modified data back to client
            int bytesSent = sendto(serverSocket, buffer, bytesReceived, 0, (sockaddr*)&clientAddr, clientAddrSize);
            if (bytesSent == SOCKET_ERROR) {
                std::cerr << "sendto failed with error: " << WSAGetLastError() << std::endl;
                closesocket(serverSocket);
                WSACleanup();
                return 1;
            }
        }
    }

    closesocket(serverSocket);
    WSACleanup();
    return 0;
}
