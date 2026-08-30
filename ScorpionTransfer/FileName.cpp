#include <iostream>
#include <cstring>
#include <fstream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <conio.h>
#include <cstdlib>
#include <thread>
#include <string>

#pragma comment(lib, "ws2_32.lib")

void kesifYayiniYap(SOCKET sock, sockaddr_in broadcastAdress)
{
    const char* message = "SCORPION_DISCOVER";
    while (true)
    {
        sendto(sock, message, strlen(message), 0,
            (sockaddr*)&broadcastAdress, sizeof(broadcastAdress));

        std::this_thread::sleep_for(std::chrono::seconds(3));
    }
}

void dosyaServer(SOCKET fileServer)
{
    while (true)
    {
        SOCKET incoming = accept(
            fileServer,
            nullptr,
            nullptr
        );

        if (incoming == INVALID_SOCKET)
        {
            break;
        }

        int isimUzunlugu = 0;

        int alinan = recv(
            incoming,
            (char*)&isimUzunlugu,
            sizeof(isimUzunlugu),
            0
        );

        if (alinan != sizeof(isimUzunlugu))
        {
            closesocket(incoming);
            continue;
        }

        std::string dosyaAdi(isimUzunlugu, '\0');

        int toplam = 0;

        for (; toplam < isimUzunlugu;)
        {
            int sonuc = recv(
                incoming,
                &dosyaAdi[0] + toplam,
                isimUzunlugu - toplam,
                0
            );

            if (sonuc <= 0)
            {
                break;
            }

            toplam += sonuc;
        }

        if (toplam != isimUzunlugu)
        {
            closesocket(incoming);
            continue;
        }

        long long dosyaBoyutu = 0;

        alinan = recv(
            incoming,
            (char*)&dosyaBoyutu,
            sizeof(dosyaBoyutu),
            0
        );

        if (alinan != sizeof(dosyaBoyutu))
        {
            closesocket(incoming);
            continue;
        }

        std::ofstream dosya(
            dosyaAdi,
            std::ios::binary
        );

        if (!dosya)
        {
            closesocket(incoming);
            continue;
        }

        char buffer[4096];
        long long toplamAlinan = 0;

        for (; toplamAlinan < dosyaBoyutu;)
        {
            int alinacak = static_cast<int>(
                min(
                    static_cast<long long>(sizeof(buffer)),
                    dosyaBoyutu - toplamAlinan
                )
                );

            int sonuc = recv(
                incoming,
                buffer,
                alinacak,
                0
            );

            if (sonuc <= 0)
            {
                break;
            }

            dosya.write(buffer, sonuc);
            toplamAlinan += sonuc;
        }

        dosya.close();
        closesocket(incoming);
    }
}

int main() {
	WSADATA wsaData;

	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (result != 0) {
		std::cout << "winsock baslamadi: " << result << std::endl;
	}

	std::cout << "winsock baslatildi" << std::endl;

	SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (sock == INVALID_SOCKET) {
		std::cout << "socket olusturulamadi: " << WSAGetLastError() << std::endl;
		WSACleanup();
		return 1;
	}

    std::string liste[100];
    int listeSayisi = 0;
    std::string isim[100];

    std::string ipKayit;
    std::string panel;

	BOOL broadcast = TRUE;

	setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (const char*)&broadcast, sizeof(broadcast));

	sockaddr_in localAdress;

	localAdress.sin_family = AF_INET;
	localAdress.sin_port = htons(5000);
	localAdress.sin_addr.s_addr = INADDR_ANY;

	if (bind(sock, (sockaddr*)&localAdress, sizeof(localAdress)) == SOCKET_ERROR) {
		std::cout << "bind basarisiz: " << WSAGetLastError() << std::endl;
		closesocket(sock);
		WSACleanup();
		return 1;
	}

	std::cout << "bind basarili" << std::endl;
	u_long mode = 1;
	ioctlsocket(sock, FIONBIO, &mode);

	sockaddr_in broadcastAdress;
    SOCKET fileServer;

    fileServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (fileServer == INVALID_SOCKET)
    {
        std::cout << "5001 socket olusturulamadi: "
            << WSAGetLastError() << std::endl;

        closesocket(sock);
        WSACleanup();
        return 1;
    }

    sockaddr_in fileAddress{};

    fileAddress.sin_family = AF_INET;
    fileAddress.sin_port = htons(5001);
    fileAddress.sin_addr.s_addr = INADDR_ANY;

    if (bind(
        fileServer,
        (sockaddr*)&fileAddress,
        sizeof(fileAddress)) == SOCKET_ERROR)
    {
        std::cout << "5001 bind basarisiz: "
            << WSAGetLastError() << std::endl;

        closesocket(fileServer);
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    if (listen(fileServer, SOMAXCONN) == SOCKET_ERROR)
    {
        std::cout << "5001 listen basarisiz: "
            << WSAGetLastError() << std::endl;

        closesocket(fileServer);
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    std::cout << "5001 dosya server hazir." << std::endl;

    std::thread serverThread(dosyaServer, fileServer);
    serverThread.detach();

	broadcastAdress.sin_family = AF_INET;
	broadcastAdress.sin_port = htons(5000);
	broadcastAdress.sin_addr.s_addr = INADDR_BROADCAST;


	const char* message = "SCORPION_DISCOVER";

    std::cout << "3 saniyede bir LAN taramasi baslatildi...\n";
    std::thread yayinThread(kesifYayiniYap, sock, broadcastAdress);
    yayinThread.detach();

	sendto(
		sock,
		message,
		strlen(message),
		0,
		(sockaddr*)&broadcastAdress,
		sizeof(broadcastAdress)
	);

	std::string dosya;

	std::cout << "LAN taraniyor \n";

	char buffer[1024];

    while (true)
    {
        if (_kbhit())
        {
            if (_getch() == 13)
            {
				system("cls");
                std::cout << "\nTarama durduruldu." << std::endl;

                while (true)
                {
                    std::cout << "> ";
					std::cin >> panel;

                    if (panel == "kayit")
                    {
                        std::cout << "Kayitli IP adresleri:" << std::endl;

                        for (int i = 0; i < listeSayisi; i++)
                        {
                            std::cout << i + 1 << ". isim: " << isim[i] << " ip: " << liste[i] << std::endl;
                        }
                    }
                    else if (panel == "cikis")
                    {
                        std::cout << "Cikmak istediginize emin misiniz (y/n): ";
                        std::cin >> panel;

                        if (panel == "y")
                        {
                            break;
                        }
                    }
                    else if (panel == "cls") {

                        system("cls");
                    }
                    else if (panel == "dosya")
                    {
                        std::string hedefIsim;
                        std::string dosyaYolu;
                        std::string dosyaAdi;

                        std::cout << "Gonderilecek kisinin ismi: ";
                        std::cin >> hedefIsim;

                        int hedefIndex = -1;

                        for (int i = 0; i < listeSayisi; i++)
                        {
                            if (isim[i] == hedefIsim)
                            {
                                hedefIndex = i;
                                break;
                            }
                        }

                        if (hedefIndex == -1)
                        {
                            std::cout << "Bu isimde kayitli cihaz bulunamadi." << std::endl;
                            continue;
                        }

                        std::cout << "Hedef: " << isim[hedefIndex] << std::endl;
                        std::cout << "IP: " << liste[hedefIndex] << std::endl;

                        std::cout << "Dosya yolunu surukleyip birakin: ";
                        std::cin >> std::ws;
                        std::getline(std::cin, dosyaYolu);

                        if (dosyaYolu.size() >= 2 &&
                            dosyaYolu.front() == '"' &&
                            dosyaYolu.back() == '"')
                        {
                            dosyaYolu = dosyaYolu.substr(1, dosyaYolu.size() - 2);
                        }

                        std::cout << "Dosya adi: ";
                        std::cin >> dosyaAdi;

                        std::cout << "\nDosya aciliyor..." << std::endl;

                        std::ifstream dosyaOku(dosyaYolu, std::ios::binary);

                        if (!dosyaOku)
                        {
                            std::cout << "Dosya acilamadi!" << std::endl;
                            continue;
                        }

                        dosyaOku.seekg(0, std::ios::end);
                        long long dosyaBoyutu = dosyaOku.tellg();
                        dosyaOku.seekg(0, std::ios::beg);

                        std::cout << "Dosya boyutu: " << dosyaBoyutu << " byte" << std::endl;

                        SOCKET fileSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

                        if (fileSocket == INVALID_SOCKET)
                        {
                            std::cout << "TCP socket olusturulamadi: "
                                << WSAGetLastError() << std::endl;

                            continue;
                        }

                        sockaddr_in hedef{};

                        hedef.sin_family = AF_INET;
                        hedef.sin_port = htons(5001);

                        inet_pton(
                            AF_INET,
                            liste[hedefIndex].c_str(),
                            &hedef.sin_addr
                        );

                        std::cout << "Baglaniliyor..." << std::endl;

                        if (connect(
                            fileSocket,
                            (sockaddr*)&hedef,
                            sizeof(hedef)) == SOCKET_ERROR)
                        {
                            std::cout << "Baglanti basarisiz: "
                                << WSAGetLastError() << std::endl;

                            closesocket(fileSocket);
                            continue;
                        }

                        std::cout << "Baglandi!" << std::endl;



                        int isimUzunlugu = static_cast<int>(dosyaAdi.size());

                        send(fileSocket,(char*)&isimUzunlugu,sizeof(isimUzunlugu),0);

                        send(fileSocket,dosyaAdi.c_str(),isimUzunlugu, 0);

                        send(fileSocket,(char*)&dosyaBoyutu,sizeof(dosyaBoyutu),0);


                        char dosyaBuffer[4096];

                        for (long long toplamGonderilen = 0;
                            toplamGonderilen < dosyaBoyutu;)
                        {
                            dosyaOku.read(
                                dosyaBuffer,
                                sizeof(dosyaBuffer)
                            );

                            std::streamsize okunan = dosyaOku.gcount();

                            if (okunan <= 0)
                                break;

                            int gonderilenToplam = 0;

                            for (; gonderilenToplam < okunan;)
                            {
                                int gonderilen = send(
                                    fileSocket,
                                    dosyaBuffer + gonderilenToplam,
                                    static_cast<int>(okunan - gonderilenToplam),
                                    0
                                );

                                if (gonderilen == SOCKET_ERROR)
                                {
                                    std::cout << "Dosya gonderilirken hata: "
                                        << WSAGetLastError() << std::endl;

                                    closesocket(fileSocket);
                                    dosyaOku.close();
                                    break;
                                }

                                gonderilenToplam += gonderilen;
                            }

                            toplamGonderilen += gonderilenToplam;
                        }

                        dosyaOku.close();
                        closesocket(fileSocket);

                        std::cout << "Dosya gonderildi!" << std::endl;
                    }
                    else
                    {
                        std::cout << "Bilinmeyen komut." << std::endl;
                    }

                }

                break;
            }
        }

        sockaddr_in senderAdress{};
        int senderAdressSize = sizeof(senderAdress);

        int received = recvfrom(
            sock,
            buffer,
            sizeof(buffer) - 1,
            0,
            (sockaddr*)&senderAdress,
            &senderAdressSize
        );

        if (received == SOCKET_ERROR)
        {
            if (WSAGetLastError() == WSAEWOULDBLOCK)
            {
                continue;
            }

            break;
        }

        buffer[received] = '\0';

        if (strcmp(buffer, "SCORPION_DISCOVER") == 0)
        {
            char ip[INET_ADDRSTRLEN];

            InetNtopA(
                AF_INET,
                &senderAdress.sin_addr,
                ip,
                INET_ADDRSTRLEN
            );

            if (ipKayit != ip && listeSayisi < 100)
            {
                liste[listeSayisi] = ip;
				std::cout << "Lutfen bu IP icin bir isim giriniz: ";
				std::cin >> isim[listeSayisi];
                listeSayisi++;

                ipKayit = ip;

                std::cout << "Bulundu: " << ip << std::endl;
            }

            const char* response = "SCORPION_HERE";

            sendto(
                sock,
                response,
                static_cast<int>(strlen(response)),
                0,
                (sockaddr*)&senderAdress,
                senderAdressSize
            );

            
        }
    }

	closesocket(sock);

	WSACleanup();

	return 0;
}