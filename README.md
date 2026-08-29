# ScorpionTransfer

**ScorpionTransfer**, C++ ve Windows Sockets (Winsock2) kullanılarak geliştirilmiş, yerel ağ (LAN) üzerindeki cihazları otomatik tespit eden ve cihazlar arasında TCP protokolü üzerinden doğrudan dosya transferi sağlayan hafif bir konsol uygulamasıdır.

## Özellikler

* **Otomatik Cihaz Keşfi:** UDP Broadcast (Port 5000) kullanarak aynı ağdaki aktif cihazları otomatik olarak bulur.
* **Doğrudan Dosya Transferi:** TCP Sockets (Port 5001) üzerinden dosyaları internete ihtiyaç duymadan, yerel ağ hızında aktarır.
* **Dahili Dosya Sunucusu:** Arka planda çalışan bir `std::thread` sayesinde gelen dosya transferi isteklerini dinler ve dosyaları otomatik kaydeder.
* **Konsol Yönetim Paneli:** Ağ taramasını durdurup kayıtlı cihazları listeleme ve dosya gönderme komutları içerir.

## Gereksinimler

* **İşletim Sistemi:** Windows (Winsock API bağımlılığı nedeniyle)
* **Derleyici:** C++11 veya üzeri destekleyen bir C++ derleyicisi (MSVC, MinGW vb.)
* **Kütüphaneler:** `ws2_32.lib` (Windows Sockets kütüphanesi)

## Derleme

### Visual Studio Kullanarak:
1. Projeyi Visual Studio üzerinde bir C++ Console Application olarak açın.
2. Kod içerisindeki `#pragma comment(lib, "ws2_32.lib")` direktifi kütüphaneyi otomatik bağlayacaktır.
3. Projeyi **Release** veya **Debug** modunda derleyip çalıştırabilirsiniz.

### MinGW (g++) Kullanarak:
Komut satırından derlemek için `ws2_32` kütüphanesini bağlamanız gerekir:
```bash
g++ main.cpp -o ScorpionTransfer.exe -lws2_32
