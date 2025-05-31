# OJOLOKA - Sistem Transportasi & Delivery Platform

## 📋 Deskripsi Proyek

OJOLOKA adalah sistem aplikasi transportasi dan delivery makanan yang dibuat menggunakan bahasa pemrograman C++. Aplikasi ini menerapkan berbagai struktur data dan algoritma untuk mengelola layanan antar-jemput penumpang dan pengiriman makanan dengan efisien.

## ✨ Fitur Utama

### 👥 Untuk Penumpang
- ✅ Registrasi dan login penumpang
- 🍔 Pemesanan makanan dari berbagai merchant
- 🚗 Layanan antar-jemput dengan perhitungan jarak optimal
- 📦 Tracking pesanan real-time
- ⭐ Rating dan feedback untuk kurir
- 🛒 Shopping cart untuk multiple items

### 🏍️ Untuk Kurir
- ✅ Registrasi dan login kurir
- 📍 Update lokasi dan status ketersediaan
- 📋 Manajemen pesanan aktif
- ✅ Konfirmasi pickup dan delivery
- 💰 Tracking pendapatan

### 🏪 Untuk Merchant
- ✅ Registrasi dan login merchant
- 🍕 Manajemen menu makanan (CRUD operations)
- 📊 Kelola pesanan masuk
- 📈 Statistik penjualan
- ⚡ Update status pesanan

## 🔧 Struktur Data yang Digunakan

- **Hash Table**: Penyimpanan data merchant dengan akses O(1)
- **Binary Search Tree**: Organisasi menu makanan untuk pencarian efisien
- **Queue**: Manajemen antrian pesanan merchant
- **Stack**: Menu populer dan operasi undo/redo
- **Graph dengan Adjacency List**: Representasi jaringan lokasi
- **Array**: Penyimpanan data penumpang dan kurir

## 🧮 Algoritma yang Diimplementasikan

- **Dijkstra's Algorithm**: Pencarian jarak terpendek antar lokasi
- **Binary Search**: Pencarian makanan dalam menu
- **Bubble Sort**: Pengurutan merchant berdasarkan jarak
- **Tree Traversal**: In-order traversal untuk display menu
- **Hash Function**: Distribusi data merchant

## 📁 Struktur File

```
OJOLOKA/
├── main.cpp              # File utama aplikasi
├── main.exe              # Executable program
├── ojoloka.exe           # Alternative executable
├── penumpang.txt         # Database penumpang
├── kurir.txt             # Database kurir
├── merchant.txt          # Database merchant & menu
├── lokasi.txt            # Database lokasi
├── rute.txt              # Database rute antar lokasi
└── README.md             # Dokumentasi proyek
```

## 🚀 Instalasi dan Penggunaan

### Prasyarat
- Compiler C++ (GCC, MinGW, atau MSVC)
- Sistem operasi Windows/Linux/macOS

### Langkah Instalasi

1. **Clone atau download repository**
   ```bash
   git clone <repository-url>
   cd OJOLOKA
   ```

2. **Kompilasi program**
   ```bash
   g++ -o ojoloka main.cpp
   ```

3. **Jalankan aplikasi**
   ```bash
   ./ojoloka        # Linux/macOS
   ojoloka.exe      # Windows
   ```

### Penggunaan

1. **Jalankan aplikasi** dan pilih jenis pengguna:
   - Login sebagai Penumpang (menu 1)
   - Login sebagai Kurir (menu 2)  
   - Login sebagai Merchant (menu 3)

2. **Registrasi** jika belum memiliki akun (menu 4-6)

3. **Ikuti menu** yang tersedia sesuai dengan peran pengguna

## 📊 Data Sample

### Format Data Penumpang
```
ID,Nama,Alamat,NoTelepon,Password
P001,Ahmad Rizki,Jl. Sudirman No. 123,081234567890,pass123
```

### Format Data Kurir
```
ID,Nama,Password,Status,LokasiSekarang
K001,Driver Joko,pass123,tersedia,Sudirman
```

### Format Data Merchant
```
IDMerchant,NamaMerchant,Password,IDMakanan,NamaMakanan,Harga
0,Warung Padang Sudirman,pass123,M001,Rendang,25000
```

### Format Data Lokasi
```
ID,NamaLokasi
0,Sudirman Plaza
1,Thamrin City
```

### Format Data Rute
```
LokasiAsal,LokasiTujuan,Jarak
0,1,2.5
1,0,2.5
```

## 🎯 Contoh Penggunaan

### Login Penumpang
```
ID Penumpang: P001
Password: pass123
```

### Pesan Makanan
1. Pilih lokasi saat ini
2. Lihat daftar merchant terdekat (diurutkan berdasarkan jarak)
3. Pilih merchant dan menu makanan
4. Tambah ke keranjang
5. Konfirmasi pesanan

### Login Kurir
```
ID Kurir: K001  
Password: pass123
```

## 🏗️ Arsitektur Sistem

```
┌─────────────────────────────────────────────────────────────┐
│                    OJOLOKA SYSTEM                           │
├─────────────────────────────────────────────────────────────┤
│  User Interface Layer                                       │
│  ├── Login/Register Menu                                    │
│  ├── Penumpang Menu                                         │
│  ├── Kurir Menu                                             │
│  └── Merchant Menu                                          │
├─────────────────────────────────────────────────────────────┤
│  Business Logic Layer                                       │
│  ├── Order Management                                       │
│  ├── Location Services (Dijkstra)                          │
│  ├── User Authentication                                    │
│  └── Payment Calculation                                    │
├─────────────────────────────────────────────────────────────┤
│  Data Structure Layer                                       │
│  ├── Hash Table (Merchants)                                │
│  ├── BST (Food Menu)                                       │
│  ├── Queue (Orders)                                        │
│  ├── Stack (Popular Menu)                                  │
│  └── Graph (Locations)                                     │
├─────────────────────────────────────────────────────────────┤
│  Data Persistence Layer                                     │
│  ├── penumpang.txt                                         │
│  ├── kurir.txt                                             │
│  ├── merchant.txt                                          │
│  ├── lokasi.txt                                            │
│  └── rute.txt                                              │
└─────────────────────────────────────────────────────────────┘
```

## 🔍 Fitur Teknis Detail

### Hash Table Implementation
- **Size**: 100 slots
- **Collision Handling**: Linear probing
- **Hash Function**: Sum of ASCII values mod table size
- **Load Factor**: Dynamic based on merchant count

### Binary Search Tree (Menu)
- **Key**: ID Makanan (string)
- **Operations**: Insert, Search, Delete, In-order traversal
- **Time Complexity**: O(log n) average case

### Dijkstra's Algorithm
- **Purpose**: Mencari jarak terpendek antar lokasi
- **Implementation**: Adjacent list representation
- **Time Complexity**: O(V²) dengan array implementation

### Queue (Order Management)
- **Type**: FIFO queue dengan linked list
- **Operations**: Enqueue, Dequeue, Display
- **Use Case**: Antrian pesanan merchant

## 📈 Kompleksitas Algoritma

| Operasi | Struktur Data | Time Complexity | Space Complexity |
|---------|---------------|-----------------|------------------|
| Login | Linear Search | O(n) | O(1) |
| Cari Merchant | Hash Table | O(1) avg | O(1) |
| Cari Menu | BST | O(log n) avg | O(1) |
| Shortest Path | Dijkstra | O(V²) | O(V) |
| Sort Merchant | Bubble Sort | O(n²) | O(1) |

## 🐛 Troubleshooting

### Error: "File tidak ditemukan"
**Solusi**: Pastikan semua file .txt berada di direktori yang sama dengan executable

### Error: "Merchant tidak ditemukan"
**Solusi**: Periksa format ID merchant (harus berupa angka)

### Error: "Lokasi tidak terjangkau"
**Solusi**: Pastikan ada rute yang menghubungkan lokasi asal dan tujuan di rute.txt

## 🚧 Pengembangan Selanjutnya

- [ ] Implementasi database SQLite
- [ ] GUI menggunakan Qt atau GTK
- [ ] Web API untuk mobile app
- [ ] Real-time tracking menggunakan GPS
- [ ] Payment gateway integration
- [ ] Machine learning untuk rekomendasi

## 👨‍💻 Tim Pengembang

- **Developer**: [Nama Developer]
- **Institution**: [Nama Institusi]
- **Course**: Struktur Data dan Algoritma
- **Year**: 2024

## 📄 Lisensi

Project ini dilisensikan di bawah [MIT License](LICENSE).

## 🤝 Kontribusi

Kontribusi sangat diterima! Silakan buat pull request atau buka issue untuk saran dan perbaikan.

## 📞 Kontak

Untuk pertanyaan atau bantuan:
- Email: [email@domain.com]
- GitHub: [username]

---

**OJOLOKA** - *Connecting People Through Technology* 🚀
