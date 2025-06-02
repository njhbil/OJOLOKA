#include <iostream>
#include <fstream>
using namespace std;

struct penumpang
{
    string nama;
    string id_penumpang;
    string alamat;
    string noTelepon;
    string password;
};

struct kurir
{
    string id_kurir;
    string nama;
    string password;
    string statusKurir;
    string lokasiSekarang;
};

struct makanan
{
    string id_makanan;
    string namaMakanan;
    float harga;
};

struct graphLokasi
{
    int destination;
    float jarak;
    graphLokasi *next;
};

struct lokasi
{
    string id_lokasi;
    string alamat;
    graphLokasi *V;
};

struct pesananPenumpangNode
{
    string id_pesanan;
    penumpang *penumpang;
    kurir *kurir;
    float totalHarga;
};

struct pesananMakanan
{
    string id_pesanan;
    makanan *makanan;
    kurir *kurir;
    float totalHarga;
};

struct TreeNodeMakanan
{
    makanan data;
    TreeNodeMakanan *left;
    TreeNodeMakanan *right;
};

struct makananTree
{
    TreeNodeMakanan *root;
    int size;
};

// Order management structures for merchants
struct merchantOrder
{
    string id_pesanan;
    string id_penumpang;
    string nama_penumpang;
    string id_makanan;
    string nama_makanan;
    float harga;
    string status; // "pending", "ready", "completed"
    string kurir_id;
    string created_time;
};

struct orderNode
{
    merchantOrder data;
    orderNode *next;
};

struct orderQueue
{
    orderNode *front;
    orderNode *rear;
    int size;
};

struct menuStack
{
    makanan *items;
    int top;
    int capacity;
};

// Struct untuk merchant
struct merchant
{
    string namaMerchant;
    string idMerchant;
    string password;
    makananTree treeMakanan;
    orderQueue orders;
    menuStack popularMenu;
};

// History structures for Stack implementation
struct historyItem
{
    string id_transaksi;
    string jenis; // "makanan" atau "penumpang"
    string tanggal;
    string waktu;
    string lokasi_asal;
    string lokasi_tujuan;
    string detail_pesanan;
    float total_biaya;
    string status; // "selesai", "dibatalkan"
    string kurir_id;
    string kurir_nama;
    int rating;
};

struct historyNode
{
    historyItem data;
    historyNode *next;
};

struct historyStack
{
    historyNode *top;
    int size;
};

// Function declarations for order management
void initOrderQueue(orderQueue *queue);
void initMenuStack(menuStack *stack);
void enqueueOrder(orderQueue *queue, merchantOrder order);
void displayOrders(orderQueue *queue);
void updateOrderStatus(orderQueue *queue, string orderId, string newStatus);
void tambahMakananBaru();
void editHargaMakanan();

// History Stack function declarations - LIFO implementation
void initHistoryStack(historyStack *stack);
void pushHistory(historyStack *stack, historyItem item);
void tampilkanHistory(historyStack *stack);
int findPenumpangIndex(string id_penumpang);
int findKurirIndex(string id_kurir);
void tambahHistoryPenumpang(string id_penumpang, historyItem item);
void tambahHistoryKurir(string id_kurir, historyItem item);

const int TABLE_SIZE = 100;
merchant hashTableMerchant[TABLE_SIZE];
bool isInitializedMerchant[TABLE_SIZE] = {false};

void initializeMakananTree(makananTree *tree)
{
    tree->root = nullptr;
    tree->size = 0;
}

TreeNodeMakanan *createMakananNode(makanan data)
{
    TreeNodeMakanan *newNode = new TreeNodeMakanan;
    newNode->data = data;
    newNode->left = nullptr;
    newNode->right = nullptr;
    return newNode;
}

void addMakananToTree(makananTree *tree, makanan data)
{
    TreeNodeMakanan *newNode = createMakananNode(data);
    if (tree->root == nullptr)
    {
        tree->root = newNode;
    }
    else
    {
        TreeNodeMakanan *current = tree->root;
        TreeNodeMakanan *parent = nullptr;

        while (true)
        {
            parent = current;
            if (data.id_makanan < current->data.id_makanan)
            {
                current = current->left;
                if (current == nullptr)
                {
                    parent->left = newNode;
                    break;
                }
            }
            else
            {
                current = current->right;
                if (current == nullptr)
                {
                    parent->right = newNode;
                    break;
                }
            }
        }
    }
    tree->size++;
}

void tambahRuteKeGraphLokasi(lokasi *loc, int destination, float jarak)
{
    graphLokasi *newEdge = new graphLokasi;
    newEdge->destination = destination;
    newEdge->jarak = jarak;
    newEdge->next = loc->V;
    loc->V = newEdge;
}

void loadMerchantData()
{
    ifstream file("merchant.txt");
    if (!file.is_open())
    {
        cout << "Error: Tidak dapat membuka file merchant.txt!" << endl;
        return;
    }

    string line;
    while (getline(file, line))
    {
        size_t pos1 = line.find(',');
        size_t pos2 = line.find(',', pos1 + 1);
        size_t pos3 = line.find(',', pos2 + 1);
        size_t pos4 = line.find(',', pos3 + 1);
        size_t pos5 = line.find(',', pos4 + 1);

        if (pos1 == string::npos || pos2 == string::npos ||
            pos3 == string::npos || pos4 == string::npos || pos5 == string::npos)
        {
            cout << "Warning: Format data tidak sesuai: " << line << endl;
            continue;
        }

        string idMerchant = line.substr(0, pos1);
        string namaMerchant = line.substr(pos1 + 1, pos2 - pos1 - 1);
        string passwordMerchant = line.substr(pos2 + 1, pos3 - pos2 - 1);
        string idMakanan = line.substr(pos3 + 1, pos4 - pos3 - 1);
        string namaMakanan = line.substr(pos4 + 1, pos5 - pos4 - 1);
        string hargaMakanan = line.substr(pos5 + 1);

        if (idMerchant.empty() || namaMerchant.empty() || passwordMerchant.empty() ||
            idMakanan.empty() || namaMakanan.empty() || hargaMakanan.empty())
        {
            cout << "Warning: Data tidak lengkap: " << line << endl;
            continue;
        }

        int hashIndex = stoi(idMerchant) % TABLE_SIZE; // Inisialisasi merchant jika belum ada
        if (!isInitializedMerchant[hashIndex])
        {
            isInitializedMerchant[hashIndex] = true;
            hashTableMerchant[hashIndex].idMerchant = idMerchant;
            hashTableMerchant[hashIndex].namaMerchant = namaMerchant;
            hashTableMerchant[hashIndex].password = passwordMerchant;
            initializeMakananTree(&hashTableMerchant[hashIndex].treeMakanan);
            initOrderQueue(&hashTableMerchant[hashIndex].orders);
            initMenuStack(&hashTableMerchant[hashIndex].popularMenu);
        }

        // Tambah makanan ke tree merchant
        makanan newMakanan;
        newMakanan.id_makanan = idMakanan;
        newMakanan.namaMakanan = namaMakanan;
        newMakanan.harga = stof(hargaMakanan);

        addMakananToTree(&hashTableMerchant[hashIndex].treeMakanan, newMakanan);
    }
    file.close();
    cout << "Data merchant berhasil dimuat!" << endl;
}

void loadDataLokasitoGraph()
{
    ifstream file("lokasi.txt");
    if (!file.is_open())
    {
        cout << "Error: Tidak dapat membuka file lokasi.txt!" << endl;
        return;
    }

    string line;
    while (getline(file, line))
    {
        size_t pos1 = line.find(',');
        size_t pos2 = line.find(',', pos1 + 1);

        if (pos1 == string::npos || pos2 == string::npos)
        {
            cout << "Warning: Format data tidak sesuai: " << line << endl;
            continue;
        }

        string idLokasi = line.substr(0, pos1);
        string alamatLokasi = line.substr(pos1 + 1, pos2 - pos1 - 1);

        if (idLokasi.empty() || alamatLokasi.empty())
        {
            cout << "Warning: Data tidak lengkap: " << line << endl;
            continue;
        }

        lokasi newLokasi;
        newLokasi.id_lokasi = idLokasi;
        newLokasi.alamat = alamatLokasi;
        newLokasi.V = nullptr;

        // Tambah rute ke graph
        int destination;
        float jarak;
        while (file >> destination >> jarak)
        {
            tambahRuteKeGraphLokasi(&newLokasi, destination, jarak);
        }
    }
    file.close();
    cout << "Data lokasi berhasil dimuat!" << endl;
}

// Struct untuk queue kurir
struct QueueNode
{
    kurir *data;
    QueueNode *next;
};

struct KurirQueue
{
    QueueNode *front;
    QueueNode *rear;
    int size;
};

// Struct untuk stack menu
struct StackMenuNode
{
    makanan data;
    StackMenuNode *next;
};

struct MenuStack
{
    StackMenuNode *top;
    int size;
};

// Struct untuk pesanan aktif
struct PesananAktif
{
    string id_pesanan;
    string jenis; // "makanan" atau "penumpang"
    penumpang *customer;
    kurir *kurirTerpilih;
    string lokasi_asal;
    string lokasi_tujuan;
    float totalBiaya;
    string status; // "menunggu", "dalam_perjalanan", "selesai"
    bool konfirmasi_customer;
    bool konfirmasi_kurir;
    string feedback;
    int rating;
};

// Global arrays untuk data
const int MAX_PENUMPANG = 1000;
const int MAX_KURIR = 500;
const int MAX_LOKASI = 100;
const int MAX_PESANAN = 2000;

penumpang dataPenumpang[MAX_PENUMPANG];
kurir dataKurir[MAX_KURIR];
lokasi dataLokasi[MAX_LOKASI];
PesananAktif dataPesanan[MAX_PESANAN];

int jumlahPenumpang = 0;
int jumlahKurir = 0;
int jumlahLokasi = 0;
int jumlahPesanan = 0;

// History Stack arrays - LIFO (Last In First Out)
historyStack historyPenumpang[MAX_PENUMPANG];
historyStack historyKurir[MAX_KURIR];
bool historyInitialized[MAX_PENUMPANG + MAX_KURIR] = {false};

// Session variables
penumpang *currentPenumpang = nullptr;
kurir *currentKurir = nullptr;
merchant *currentMerchant = nullptr;

// ===== FUNGSI LOAD DATA =====

void loadPenumpangData()
{
    ifstream file("penumpang.txt");
    if (!file.is_open())
    {
        cout << "Warning: File penumpang.txt tidak ditemukan!" << endl;
        return;
    }

    string line;
    while (getline(file, line) && jumlahPenumpang < MAX_PENUMPANG)
    {
        size_t pos1 = line.find(',');
        size_t pos2 = line.find(',', pos1 + 1);
        size_t pos3 = line.find(',', pos2 + 1);
        size_t pos4 = line.find(',', pos3 + 1);

        if (pos1 != string::npos && pos2 != string::npos &&
            pos3 != string::npos && pos4 != string::npos)
        {

            dataPenumpang[jumlahPenumpang].id_penumpang = line.substr(0, pos1);
            dataPenumpang[jumlahPenumpang].nama = line.substr(pos1 + 1, pos2 - pos1 - 1);
            dataPenumpang[jumlahPenumpang].alamat = line.substr(pos2 + 1, pos3 - pos2 - 1);
            dataPenumpang[jumlahPenumpang].noTelepon = line.substr(pos3 + 1, pos4 - pos3 - 1);
            dataPenumpang[jumlahPenumpang].password = line.substr(pos4 + 1);
            jumlahPenumpang++;
        }
    }
    file.close();
    cout << "Data penumpang berhasil dimuat: " << jumlahPenumpang << " data" << endl;
}

void loadKurirData()
{
    ifstream file("kurir.txt");
    if (!file.is_open())
    {
        cout << "Warning: File kurir.txt tidak ditemukan!" << endl;
        return;
    }

    string line;
    while (getline(file, line) && jumlahKurir < MAX_KURIR)
    {
        size_t pos1 = line.find(',');
        size_t pos2 = line.find(',', pos1 + 1);
        size_t pos3 = line.find(',', pos2 + 1);
        size_t pos4 = line.find(',', pos3 + 1);

        if (pos1 != string::npos && pos2 != string::npos &&
            pos3 != string::npos && pos4 != string::npos)
        {

            dataKurir[jumlahKurir].id_kurir = line.substr(0, pos1);
            dataKurir[jumlahKurir].nama = line.substr(pos1 + 1, pos2 - pos1 - 1);
            dataKurir[jumlahKurir].password = line.substr(pos2 + 1, pos3 - pos2 - 1);
            dataKurir[jumlahKurir].statusKurir = line.substr(pos3 + 1, pos4 - pos3 - 1);
            dataKurir[jumlahKurir].lokasiSekarang = line.substr(pos4 + 1);
            jumlahKurir++;
        }
    }
    file.close();
    cout << "Data kurir berhasil dimuat: " << jumlahKurir << " data" << endl;
}

void loadLokasiData()
{
    ifstream file("lokasi.txt");
    if (!file.is_open())
    {
        cout << "Warning: File lokasi.txt tidak ditemukan!" << endl;
        return;
    }

    string line;
    while (getline(file, line) && jumlahLokasi < MAX_LOKASI)
    {
        size_t pos1 = line.find(',');
        if (pos1 != string::npos)
        {
            dataLokasi[jumlahLokasi].id_lokasi = line.substr(0, pos1);
            dataLokasi[jumlahLokasi].alamat = line.substr(pos1 + 1);
            dataLokasi[jumlahLokasi].V = nullptr;
            jumlahLokasi++;
        }
    }
    file.close();

    // Load graph connections
    ifstream graphFile("rute.txt");
    if (graphFile.is_open())
    {
        string graphLine;
        while (getline(graphFile, graphLine))
        {
            size_t pos1 = graphLine.find(',');
            size_t pos2 = graphLine.find(',', pos1 + 1);

            if (pos1 != string::npos && pos2 != string::npos)
            {
                int src = stoi(graphLine.substr(0, pos1));
                int dest = stoi(graphLine.substr(pos1 + 1, pos2 - pos1 - 1));
                float jarak = stof(graphLine.substr(pos2 + 1));

                if (src < jumlahLokasi && dest < jumlahLokasi)
                {
                    tambahRuteKeGraphLokasi(&dataLokasi[src], dest, jarak);
                }
            }
        }
        graphFile.close();
    }
    cout << "Data lokasi dan rute berhasil dimuat: " << jumlahLokasi << " lokasi" << endl;
}

// Hash function for merchant hash table
int hashFunction(string key)
{
    int hash = 0;
    for (char c : key)
    {
        hash += c;
    }
    return hash % TABLE_SIZE;
}

// ===== FUNGSI QUEUE KURIR =====

void initKurirQueue(KurirQueue *queue)
{
    queue->front = nullptr;
    queue->rear = nullptr;
    queue->size = 0;
}

void enqueueKurir(KurirQueue *queue, kurir *kurirData)
{
    QueueNode *newNode = new QueueNode();
    newNode->data = kurirData;
    newNode->next = nullptr;

    if (queue->rear == nullptr)
    {
        queue->front = queue->rear = newNode;
    }
    else
    {
        queue->rear->next = newNode;
        queue->rear = newNode;
    }
    queue->size++;
}

kurir *dequeueKurir(KurirQueue *queue)
{
    if (queue->front == nullptr)
        return nullptr;

    QueueNode *temp = queue->front;
    kurir *kurirData = temp->data;
    queue->front = queue->front->next;

    if (queue->front == nullptr)
    {
        queue->rear = nullptr;
    }

    delete temp;
    queue->size--;
    return kurirData;
}

kurir *cariKurirTersedia(string lokasiPenumpang)
{
    for (int i = 0; i < jumlahKurir; i++)
    {
        if (dataKurir[i].statusKurir == "tersedia" &&
            dataKurir[i].lokasiSekarang == lokasiPenumpang)
        {
            return &dataKurir[i];
        }
    }

    // Jika tidak ada kurir di lokasi yang sama, cari yang tersedia
    for (int i = 0; i < jumlahKurir; i++)
    {
        if (dataKurir[i].statusKurir == "tersedia")
        {
            return &dataKurir[i];
        }
    }
    return nullptr;
}

// ===== FUNGSI STACK MENU =====

void initMenuStack(MenuStack *stack)
{
    stack->top = nullptr;
    stack->size = 0;
}

void pushMenu(MenuStack *stack, makanan data)
{
    StackMenuNode *newNode = new StackMenuNode();
    newNode->data = data;
    newNode->next = stack->top;
    stack->top = newNode;
    stack->size++;
}

makanan popMenu(MenuStack *stack)
{
    if (stack->top == nullptr)
    {
        makanan empty = {"", "", 0};
        return empty;
    }

    StackMenuNode *temp = stack->top;
    makanan data = temp->data;
    stack->top = stack->top->next;
    delete temp;
    stack->size--;
    return data;
}

void tampilkanMenuStack(MenuStack *stack)
{
    if (stack->top == nullptr)
    {
        cout << "Menu kosong!" << endl;
        return;
    }

    cout << "=== MENU MAKANAN ===" << endl;
    StackMenuNode *current = stack->top;
    int no = 1;
    while (current != nullptr)
    {
        cout << no << ". " << current->data.namaMakanan
             << " - Rp" << current->data.harga
             << " (ID: " << current->data.id_makanan << ")" << endl;
        current = current->next;
        no++;
    }
}

// ===== FUNGSI TREE MAKANAN =====

TreeNodeMakanan *cariMakanan(TreeNodeMakanan *root, string id_makanan)
{
    if (root == nullptr || root->data.id_makanan == id_makanan)
    {
        return root;
    }

    if (id_makanan < root->data.id_makanan)
    {
        return cariMakanan(root->left, id_makanan);
    }

    return cariMakanan(root->right, id_makanan);
}

void tampilkanMenuTree(TreeNodeMakanan *root)
{
    if (root != nullptr)
    {
        tampilkanMenuTree(root->left);
        cout << "ID: " << root->data.id_makanan
             << ", Nama: " << root->data.namaMakanan
             << ", Harga: Rp" << root->data.harga << endl;
        tampilkanMenuTree(root->right);
    }
}

void tampilkanMenuMerchant(string idMerchant)
{
    int hashIndex = stoi(idMerchant) % TABLE_SIZE;

    if (isInitializedMerchant[hashIndex] &&
        hashTableMerchant[hashIndex].idMerchant == idMerchant)
    {

        cout << "=== Menu " << hashTableMerchant[hashIndex].namaMerchant << " ===" << endl;
        tampilkanMenuTree(hashTableMerchant[hashIndex].treeMakanan.root);
    }
    else
    {
        cout << "Merchant tidak ditemukan!" << endl;
    }
}

makanan *cariMakananDiMerchant(string idMerchant, string idMakanan)
{
    int hashIndex = stoi(idMerchant) % TABLE_SIZE;

    if (isInitializedMerchant[hashIndex] &&
        hashTableMerchant[hashIndex].idMerchant == idMerchant)
    {

        TreeNodeMakanan *found = cariMakanan(
            hashTableMerchant[hashIndex].treeMakanan.root,
            idMakanan);

        if (found != nullptr)
        {
            return &(found->data);
        }
    }
    return nullptr;
}

// ===== FUNGSI LOGIN DAN REGISTER =====

penumpang *loginPenumpang(string id, string password)
{
    for (int i = 0; i < jumlahPenumpang; i++)
    {
        if (dataPenumpang[i].id_penumpang == id &&
            dataPenumpang[i].password == password)
        {
            return &dataPenumpang[i];
        }
    }
    return nullptr;
}

kurir *loginKurir(string id, string password)
{
    for (int i = 0; i < jumlahKurir; i++)
    {
        if (dataKurir[i].id_kurir == id &&
            dataKurir[i].password == password)
        {
            return &dataKurir[i];
        }
    }
    return nullptr;
}

merchant *loginMerchant(string id, string password)
{
    int hashIndex = stoi(id) % TABLE_SIZE;
    if (isInitializedMerchant[hashIndex] &&
        hashTableMerchant[hashIndex].idMerchant == id &&
        hashTableMerchant[hashIndex].password == password)
    {
        return &hashTableMerchant[hashIndex];
    }
    return nullptr;
}

void registerPenumpang()
{
    if (jumlahPenumpang >= MAX_PENUMPANG)
    {
        cout << "\n+==============================================================+" << endl;
        cout << "|                      DATABASE PENUH                         |" << endl;
        cout << "|              Tidak dapat menambah penumpang baru             |" << endl;
        cout << "+==============================================================+" << endl;
        return;
    }

    cout << "\n+==============================================================+" << endl;
    cout << "|                   REGISTER PENUMPANG                        |" << endl;
    cout << "|              Bergabunglah dengan komunitas OJOLOKA!         |" << endl;
    cout << "+==============================================================+" << endl;

    string tempId, tempNama, tempAlamat, tempTelepon, tempPassword;

    // Input with validation
    bool idValid = false;
    while (!idValid)
    {
        cout << "\n[ID] ID Penumpang (format: USR001): ";
        cin >> tempId;

        // Check if ID already exists
        bool idExists = false;
        for (int i = 0; i < jumlahPenumpang; i++)
        {
            if (dataPenumpang[i].id_penumpang == tempId)
            {
                idExists = true;
                break;
            }
        }

        if (idExists)
        {
            cout << "[X] ID sudah digunakan! Gunakan ID yang berbeda." << endl;
        }
        else if (tempId.length() < 3)
        {
            cout << "[X] ID terlalu pendek! Minimal 3 karakter." << endl;
        }
        else
        {
            idValid = true;
        }
    }

    cout << "[NAMA] Nama Lengkap: ";
    cin.ignore();
    getline(cin, tempNama);

    while (tempNama.empty())
    {
        cout << "[X] Nama tidak boleh kosong! Masukkan nama: ";
        getline(cin, tempNama);
    }

    cout << "[ALAMAT] Alamat Lengkap: ";
    getline(cin, tempAlamat);

    while (tempAlamat.empty())
    {
        cout << "[X] Alamat tidak boleh kosong! Masukkan alamat: ";
        getline(cin, tempAlamat);
    }

    bool teleponValid = false;
    while (!teleponValid)
    {
        cout << "[TELP] No Telepon (08xxxxxxxxxx): ";
        cin >> tempTelepon;

        if (tempTelepon.length() >= 10 && tempTelepon.length() <= 15 && tempTelepon.substr(0, 2) == "08")
        {
            teleponValid = true;
        }
        else
        {
            cout << "[X] Format telepon tidak valid! Contoh: 08123456789" << endl;
        }
    }

    bool passwordValid = false;
    while (!passwordValid)
    {
        cout << "[PASS] Password (minimal 6 karakter): ";
        cin >> tempPassword;

        if (tempPassword.length() >= 6)
        {
            passwordValid = true;
        }
        else
        {
            cout << "[X] Password terlalu pendek! Minimal 6 karakter." << endl;
        }
    }

    // Assign validated data
    dataPenumpang[jumlahPenumpang].id_penumpang = tempId;
    dataPenumpang[jumlahPenumpang].nama = tempNama;
    dataPenumpang[jumlahPenumpang].alamat = tempAlamat;
    dataPenumpang[jumlahPenumpang].noTelepon = tempTelepon;
    dataPenumpang[jumlahPenumpang].password = tempPassword;

    jumlahPenumpang++;

    cout << "\n+==============================================================+" << endl;
    cout << "|                   PENDAFTARAN BERHASIL!                     |" << endl;
    cout << "+==============================================================+" << endl;
    cout << "| [OK] Selamat datang di OJOLOKA, " << tempNama << "!" << endl;
    cout << "| [ID] ID Anda: " << tempId << endl;
    cout << "| [TELP] Telepon: " << tempTelepon << endl;
    cout << "| [ALAMAT] Alamat: " << tempAlamat << endl;
    cout << "+==============================================================+" << endl;
    cout << "| TIPS:                                                        |" << endl;
    cout << "| * Gunakan ID dan password untuk login                       |" << endl;
    cout << "| * Pastikan saldo mencukupi sebelum memesan                  |" << endl;
    cout << "| * Berikan rating dan feedback untuk kurir                   |" << endl;
    cout << "+==============================================================+" << endl;

    cout << "\n>> Tekan Enter untuk kembali ke menu utama...";
    cin.ignore();
    cin.get();
}

void registerKurir()
{
    if (jumlahKurir >= MAX_KURIR)
    {
        cout << "\n+==============================================================+" << endl;
        cout << "|                      DATABASE PENUH!                        |" << endl;
        cout << "+==============================================================+" << endl;
        cout << "| [X] Maaf, kapasitas kurir sudah mencapai batas maksimum.    |" << endl;
        cout << "| [!] Silakan hubungi admin untuk informasi lebih lanjut.     |" << endl;
        cout << "+==============================================================+" << endl;

        cout << "\n>> Tekan Enter untuk kembali ke menu utama...";
        cin.ignore();
        cin.get();
        return;
    }
    cout << "\n+==============================================================+" << endl;
    cout << "|                     REGISTER KURIR                          |" << endl;
    cout << "+==============================================================+" << endl;
    cout << "| [+] Bergabunglah dengan tim kurir OJOLOKA!                  |" << endl;
    cout << "| [$] Dapatkan penghasilan dengan mengantar pesanan.          |" << endl;
    cout << "+==============================================================+" << endl;

    string tempId, tempNama, tempPassword, tempLokasi;
    bool idValid = false, namaValid = false, passwordValid = false, lokasiValid = false; // Validate ID Kurir
    while (!idValid)
    {
        cout << "\n[ID] Masukkan ID Kurir (unik): ";
        cin >> tempId;

        // Check if ID already exists
        bool idExists = false;
        for (int i = 0; i < jumlahKurir; i++)
        {
            if (dataKurir[i].id_kurir == tempId)
            {
                idExists = true;
                break;
            }
        }

        if (idExists)
        {
            cout << "[X] ID Kurir sudah terdaftar! Silakan gunakan ID lain." << endl;
        }
        else if (tempId.empty())
        {
            cout << "[X] ID Kurir tidak boleh kosong!" << endl;
        }
        else
        {
            idValid = true;
        }
    }

    // Validate Nama
    cin.ignore(); // Clear buffer
    while (!namaValid)
    {
        cout << "[NAMA] Masukkan Nama Lengkap: ";
        getline(cin, tempNama);

        if (tempNama.empty())
        {
            cout << "[X] Nama tidak boleh kosong!" << endl;
        }
        else if (tempNama.length() < 2)
        {
            cout << "[X] Nama terlalu pendek! Minimal 2 karakter." << endl;
        }
        else
        {
            namaValid = true;
        }
    }

    // Validate Password
    while (!passwordValid)
    {
        cout << "[PASS] Masukkan Password (minimal 6 karakter): ";
        cin >> tempPassword;

        if (tempPassword.length() >= 6)
        {
            passwordValid = true;
        }
        else
        {
            cout << "[X] Password terlalu pendek! Minimal 6 karakter." << endl;
        }
    }

    // Validate Lokasi
    cin.ignore(); // Clear buffer
    while (!lokasiValid)
    {
        cout << "[LOC] Masukkan Lokasi Saat Ini: ";
        getline(cin, tempLokasi);

        if (tempLokasi.empty())
        {
            cout << "[X] Lokasi tidak boleh kosong!" << endl;
        }
        else
        {
            lokasiValid = true;
        }
    }

    // Assign validated data
    dataKurir[jumlahKurir].id_kurir = tempId;
    dataKurir[jumlahKurir].nama = tempNama;
    dataKurir[jumlahKurir].password = tempPassword;
    dataKurir[jumlahKurir].lokasiSekarang = tempLokasi;
    dataKurir[jumlahKurir].statusKurir = "tersedia";

    jumlahKurir++;
    cout << "\n+==============================================================+" << endl;
    cout << "|                   PENDAFTARAN BERHASIL!                     |" << endl;
    cout << "+==============================================================+" << endl;
    cout << "| [OK] Selamat datang di tim kurir OJOLOKA, " << tempNama << "!" << endl;
    cout << "| [ID] ID Kurir: " << tempId << endl;
    cout << "| [LOC] Lokasi: " << tempLokasi << endl;
    cout << "| [STATUS] Status: Tersedia untuk menerima pesanan            |" << endl;
    cout << "+==============================================================+" << endl;
    cout << "| TIPS UNTUK KURIR:                                           |" << endl;
    cout << "| * Login dengan ID dan password untuk mulai bekerja          |" << endl;
    cout << "| * Pastikan handphone selalu aktif saat bertugas             |" << endl;
    cout << "| * Berikan pelayanan terbaik untuk rating tinggi             |" << endl;
    cout << "| * Update lokasi secara berkala                              |" << endl;
    cout << "+==============================================================+" << endl;

    cout << "\n>> Tekan Enter untuk kembali ke menu utama...";
    cin.ignore();
    cin.get();
}

void registerMerchant()
{
    cout << "\n+==============================================================+" << endl;
    cout << "|                    REGISTER MERCHANT                        |" << endl;
    cout << "+==============================================================+" << endl;
    cout << "| [+] Mulai bisnis kuliner Anda bersama OJOLOKA!              |" << endl;
    cout << "| [GROW] Jangkau lebih banyak pelanggan dengan platform kami. |" << endl;
    cout << "+==============================================================+" << endl;

    string idMerchant, namaMerchant, password;
    bool idValid = false, namaValid = false, passwordValid = false;

    // Validate ID Merchant
    while (!idValid)
    {
        cout << "\n[ID] Masukkan ID Merchant (unik): ";
        cin >> idMerchant;

        if (idMerchant.empty())
        {
            cout << "[X] ID Merchant tidak boleh kosong!" << endl;
            continue;
        }

        // Check if merchant ID already exists
        int hashIndex = hashFunction(idMerchant);
        if (isInitializedMerchant[hashIndex] && hashTableMerchant[hashIndex].idMerchant == idMerchant)
        {
            cout << "[X] ID Merchant sudah terdaftar! Silakan gunakan ID lain." << endl;
        }
        else
        {
            idValid = true;
        }
    } // Validate Nama Merchant
    cin.ignore(); // Clear buffer
    while (!namaValid)
    {
        cout << "[NAMA] Masukkan Nama Merchant/Restoran: ";
        getline(cin, namaMerchant);

        if (namaMerchant.empty())
        {
            cout << "[X] Nama merchant tidak boleh kosong!" << endl;
        }
        else if (namaMerchant.length() < 2)
        {
            cout << "[X] Nama merchant terlalu pendek! Minimal 2 karakter." << endl;
        }
        else
        {
            namaValid = true;
        }
    }

    // Validate Password
    while (!passwordValid)
    {
        cout << "[PASS] Masukkan Password (minimal 6 karakter): ";
        cin >> password;

        if (password.length() >= 6)
        {
            passwordValid = true;
        }
        else
        {
            cout << "[X] Password terlalu pendek! Minimal 6 karakter." << endl;
        }
    }

    // Initialize new merchant
    int hashIndex = hashFunction(idMerchant);
    hashTableMerchant[hashIndex].idMerchant = idMerchant;
    hashTableMerchant[hashIndex].namaMerchant = namaMerchant;
    hashTableMerchant[hashIndex].password = password;

    // Initialize structures
    initializeMakananTree(&hashTableMerchant[hashIndex].treeMakanan);
    initOrderQueue(&hashTableMerchant[hashIndex].orders);
    initMenuStack(&hashTableMerchant[hashIndex].popularMenu);

    isInitializedMerchant[hashIndex] = true;
    cout << "\n+==============================================================+" << endl;
    cout << "|                   PENDAFTARAN BERHASIL!                     |" << endl;
    cout << "+==============================================================+" << endl;
    cout << "| [OK] Selamat datang di OJOLOKA, " << namaMerchant << "!" << endl;
    cout << "| [ID] ID Merchant: " << idMerchant << endl;
    cout << "| [STATUS] Status: Siap menerima pesanan                      |" << endl;
    cout << "+==============================================================+" << endl;
    cout << "| TIPS UNTUK MERCHANT:                                        |" << endl;
    cout << "| * Login untuk mengelola menu dan pesanan                    |" << endl;
    cout << "| * Tambahkan menu makanan yang menarik                       |" << endl;
    cout << "| * Kelola pesanan dengan cepat dan efisien                   |" << endl;
    cout << "| * Berikan kualitas makanan terbaik                          |" << endl;
    cout << "+==============================================================+" << endl;

    // Option to add initial food items
    cout << "\n[MENU] Ingin menambahkan menu makanan sekarang? (y/n): ";
    char addFood;
    cin >> addFood;

    if (addFood == 'y' || addFood == 'Y')
    {
        int jumlahMakanan;
        cout << "📝 Berapa banyak makanan yang ingin ditambahkan? ";
        cin >> jumlahMakanan;
        if (jumlahMakanan <= 0)
        {
            cout << "[!] Tidak ada makanan yang ditambahkan." << endl;
        }
        else
        {
            for (int i = 0; i < jumlahMakanan; i++)
            {
                cout << "\n+-------------------------------------------------------------+" << endl;
                cout << "|                    MAKANAN KE-" << (i + 1) << "                         |" << endl;
                cout << "+-------------------------------------------------------------+" << endl;

                makanan newMakanan;
                bool makananValid = false;

                // Validate ID Makanan
                while (!makananValid)
                {
                    cout << "[ID] ID Makanan: ";
                    cin >> newMakanan.id_makanan;

                    if (newMakanan.id_makanan.empty())
                    {
                        cout << "[X] ID Makanan tidak boleh kosong!" << endl;
                    }
                    else
                    {
                        makananValid = true;
                    }
                }

                // Validate Nama Makanan
                cout << "[NAMA] Nama Makanan: ";
                cin.ignore();
                getline(cin, newMakanan.namaMakanan);
                while (newMakanan.namaMakanan.empty())
                {
                    cout << "[X] Nama makanan tidak boleh kosong!" << endl;
                    cout << "[NAMA] Nama Makanan: ";
                    getline(cin, newMakanan.namaMakanan);
                }

                // Validate Harga
                while (true)
                {
                    cout << "[HARGA] Harga: Rp";
                    cin >> newMakanan.harga;
                    if (newMakanan.harga <= 0)
                    {
                        cout << "[X] Harga harus lebih dari 0!" << endl;
                    }
                    else
                    {
                        break;
                    }
                }

                addMakananToTree(&hashTableMerchant[hashIndex].treeMakanan, newMakanan);
                cout << "[OK] " << newMakanan.namaMakanan << " berhasil ditambahkan!" << endl;
            }
        }
    }

    cout << "\n[DONE] Setup merchant selesai! Silakan login untuk mulai berjualan." << endl;
    cout << "\n>> Tekan Enter untuk kembali ke menu utama...";
    cin.ignore();
    cin.get();
}

// ===== FUNGSI EDIT HARGA MAKANAN =====
void editHargaMakanan()
{
    cout << "\n+============================================================+" << endl;
    cout << "|                [EDIT] EDIT HARGA MAKANAN                  |" << endl;
    cout << "+============================================================+" << endl;

    if (currentMerchant->treeMakanan.root == nullptr)
    {
        cout << "[X] Menu makanan kosong! Tambahkan makanan terlebih dahulu." << endl;
        cout << "\nTekan Enter untuk kembali...";
        cin.ignore();
        cin.get();
        return;
    }

    // Display current menu
    cout << "\n[MENU] Daftar Makanan Saat Ini:" << endl;
    cout << "+-----------------------------------------------------------+" << endl;
    tampilkanMenuTree(currentMerchant->treeMakanan.root);
    cout << "+-----------------------------------------------------------+" << endl;

    string idMakanan;
    cout << "\n[ID] Masukkan ID makanan yang ingin diedit: ";
    cin >> idMakanan;

    // Search for the food item
    TreeNodeMakanan *makananNode = cariMakanan(currentMerchant->treeMakanan.root, idMakanan);
    if (makananNode == nullptr)
    {
        cout << "[X] Makanan dengan ID '" << idMakanan << "' tidak ditemukan!" << endl;
        cout << "\nTekan Enter untuk kembali...";
        cin.ignore();
        cin.get();
        return;
    }

    // Display current information
    cout << "\n+-----------------------------------------------------------+" << endl;
    cout << "|                  INFORMASI MAKANAN SAAT INI              |" << endl;
    cout << "+-----------------------------------------------------------+" << endl;
    cout << "| [ID] ID Makanan: " << makananNode->data.id_makanan << endl;
    cout << "| [NAMA] Nama: " << makananNode->data.namaMakanan << endl;
    cout << "| [HARGA] Harga Saat Ini: Rp" << makananNode->data.harga << endl;
    cout << "+-----------------------------------------------------------+" << endl;

    float hargaBaru;
    bool hargaValid = false;

    while (!hargaValid)
    {
        cout << "\n[NEW] Masukkan harga baru (Rp): ";
        cin >> hargaBaru;

        if (hargaBaru <= 0)
        {
            cout << "[X] Harga harus lebih dari 0!" << endl;
        }
        else
        {
            hargaValid = true;
        }
    }

    // Confirmation
    cout << "\n+-----------------------------------------------------------+" << endl;
    cout << "|                    KONFIRMASI PERUBAHAN                  |" << endl;
    cout << "+-----------------------------------------------------------+" << endl;
    cout << "| [OLD] Harga Lama: Rp" << makananNode->data.harga << endl;
    cout << "| [NEW] Harga Baru: Rp" << hargaBaru << endl;
    cout << "+-----------------------------------------------------------+" << endl;
    cout << "[CONFIRM] Konfirmasi perubahan harga? (y/n): ";
    char konfirmasi;
    cin >> konfirmasi;

    if (konfirmasi == 'y' || konfirmasi == 'Y')
    {
        float hargaLama = makananNode->data.harga;
        makananNode->data.harga = hargaBaru;

        cout << "\n+============================================================+" << endl;
        cout << "|                [SUCCESS] HARGA BERHASIL DIUPDATE!         |" << endl;
        cout << "+============================================================+" << endl;
        cout << "| [FOOD] " << makananNode->data.namaMakanan << endl;
        cout << "| [OLD] Harga Lama: Rp" << hargaLama << endl;
        cout << "| [NEW] Harga Baru: Rp" << hargaBaru << endl;
        cout << "+============================================================+" << endl;
        cout << "| [TIP] Perubahan harga akan berlaku untuk pesanan baru     |" << endl;
        cout << "+============================================================+" << endl;
    }
    else
    {
        cout << "\n[CANCEL] Perubahan harga dibatalkan!" << endl;
    }

    cout << "\nTekan Enter untuk kembali...";
    cin.ignore();
    cin.get();
}

// ===== FUNGSI HAPUS MAKANAN =====
TreeNodeMakanan *hapusMakananDariTree(TreeNodeMakanan *root, string idMakanan, bool &berhasil)
{
    if (root == nullptr)
    {
        berhasil = false;
        return root;
    }

    if (idMakanan < root->data.id_makanan)
    {
        root->left = hapusMakananDariTree(root->left, idMakanan, berhasil);
    }
    else if (idMakanan > root->data.id_makanan)
    {
        root->right = hapusMakananDariTree(root->right, idMakanan, berhasil);
    }
    else
    {
        // Node yang akan dihapus ditemukan
        berhasil = true;

        // Case 1: Node tidak memiliki anak
        if (root->left == nullptr && root->right == nullptr)
        {
            delete root;
            return nullptr;
        }
        // Case 2: Node memiliki satu anak
        else if (root->left == nullptr)
        {
            TreeNodeMakanan *temp = root->right;
            delete root;
            return temp;
        }
        else if (root->right == nullptr)
        {
            TreeNodeMakanan *temp = root->left;
            delete root;
            return temp;
        }
        // Case 3: Node memiliki dua anak
        else
        {
            // Cari successor (node terkecil di subtree kanan)
            TreeNodeMakanan *successor = root->right;
            while (successor->left != nullptr)
            {
                successor = successor->left;
            }

            // Copy data successor ke node yang akan dihapus
            root->data = successor->data;

            // Hapus successor
            bool tempBerhasil = false;
            root->right = hapusMakananDariTree(root->right, successor->data.id_makanan, tempBerhasil);
        }
    }
    return root;
}

void hapusMakanan()
{
    cout << "\n+============================================================+" << endl;
    cout << "|                [DELETE] HAPUS MAKANAN                     |" << endl;
    cout << "+============================================================+" << endl;

    if (currentMerchant->treeMakanan.root == nullptr)
    {
        cout << "[X] Menu makanan kosong! Tidak ada makanan untuk dihapus." << endl;
        cout << "\nTekan Enter untuk kembali...";
        cin.ignore();
        cin.get();
        return;
    }

    // Display current menu
    cout << "\n[MENU] Daftar Makanan Saat Ini:" << endl;
    cout << "+-----------------------------------------------------------+" << endl;
    tampilkanMenuTree(currentMerchant->treeMakanan.root);
    cout << "+-----------------------------------------------------------+" << endl;

    string idMakanan;
    cout << "\n[ID] Masukkan ID makanan yang ingin dihapus: ";
    cin >> idMakanan;

    // Search for the food item first
    TreeNodeMakanan *makananNode = cariMakanan(currentMerchant->treeMakanan.root, idMakanan);
    if (makananNode == nullptr)
    {
        cout << "[X] Makanan dengan ID '" << idMakanan << "' tidak ditemukan!" << endl;
        cout << "\nTekan Enter untuk kembali...";
        cin.ignore();
        cin.get();
        return;
    }

    // Display item to be deleted
    cout << "\n+-----------------------------------------------------------+" << endl;
    cout << "|                MAKANAN YANG AKAN DIHAPUS                 |" << endl;
    cout << "+-----------------------------------------------------------+" << endl;
    cout << "| [ID] ID Makanan: " << makananNode->data.id_makanan << endl;
    cout << "| [NAMA] Nama: " << makananNode->data.namaMakanan << endl;
    cout << "| [HARGA] Harga: Rp" << makananNode->data.harga << endl;
    cout << "+-----------------------------------------------------------+" << endl;

    // Double confirmation for safety
    cout << "\n[WARNING] PERINGATAN: Makanan yang dihapus tidak dapat dikembalikan!" << endl;
    cout << "[CONFIRM] Apakah Anda yakin ingin menghapus makanan ini? (y/n): ";
    char konfirmasi1;
    cin >> konfirmasi1;

    if (konfirmasi1 != 'y' && konfirmasi1 != 'Y')
    {
        cout << "\n[CANCEL] Penghapusan makanan dibatalkan!" << endl;
        cout << "\nTekan Enter untuk kembali...";
        cin.ignore();
        cin.get();
        return;
    }

    cout << "[FINAL] Konfirmasi terakhir - ketik 'HAPUS' untuk melanjutkan: ";
    string konfirmasiFinal;
    cin >> konfirmasiFinal;

    if (konfirmasiFinal != "HAPUS")
    {
        cout << "\n[CANCEL] Penghapusan makanan dibatalkan!" << endl;
        cout << "\nTekan Enter untuk kembali...";
        cin.ignore();
        cin.get();
        return;
    }

    // Store data for display before deletion
    string namaMakanan = makananNode->data.namaMakanan;
    float hargaMakanan = makananNode->data.harga;

    // Perform deletion
    bool berhasil = false;
    currentMerchant->treeMakanan.root = hapusMakananDariTree(
        currentMerchant->treeMakanan.root,
        idMakanan,
        berhasil);

    if (berhasil)
    {
        currentMerchant->treeMakanan.size--;

        cout << "\n+============================================================+" << endl;
        cout << "|                [SUCCESS] MAKANAN BERHASIL DIHAPUS!        |" << endl;
        cout << "+============================================================+" << endl;
        cout << "| [DELETED] " << namaMakanan << endl;
        cout << "| [ID] ID: " << idMakanan << endl;
        cout << "| [PRICE] Harga: Rp" << hargaMakanan << endl;
        cout << "+============================================================+" << endl;
        cout << "| [INFO] Total makanan tersisa: " << currentMerchant->treeMakanan.size << endl;
        cout << "+============================================================+" << endl;
    }
    else
    {
        cout << "\n[X] Gagal menghapus makanan!" << endl;
    }
    cout << "\nTekan Enter untuk kembali...";
    cin.ignore();
    cin.get();
}

// ===== FUNGSI PENCARIAN LOKASI TERDEKAT =====

void tampilkanLokasi()
{
    cout << "=== DAFTAR LOKASI ===" << endl;
    for (int i = 0; i < jumlahLokasi; i++)
    {
        cout << i << ". " << dataLokasi[i].alamat << endl;
    }
}

// Implementasi Dijkstra untuk mencari jarak terpendek
float hitungJarakTerpendek(int lokasiAsal, int lokasiTujuan)
{
    if (lokasiAsal == lokasiTujuan)
        return 0;

    // Array untuk menyimpan jarak terpendek
    float jarak[MAX_LOKASI];
    bool visited[MAX_LOKASI];

    // Inisialisasi
    for (int i = 0; i < jumlahLokasi; i++)
    {
        jarak[i] = 999999; // Infinity
        visited[i] = false;
    }

    jarak[lokasiAsal] = 0;

    // Algoritma Dijkstra
    for (int count = 0; count < jumlahLokasi - 1; count++)
    {
        // Cari node dengan jarak minimum yang belum dikunjungi
        int u = -1;
        for (int v = 0; v < jumlahLokasi; v++)
        {
            if (!visited[v] && (u == -1 || jarak[v] < jarak[u]))
                u = v;
        }

        if (u == -1 || jarak[u] == 999999)
            break;

        visited[u] = true;

        // Update jarak ke tetangga
        graphLokasi *current = dataLokasi[u].V;
        while (current != nullptr)
        {
            int v = current->destination;
            if (!visited[v] && jarak[u] + current->jarak < jarak[v])
            {
                jarak[v] = jarak[u] + current->jarak;
            }
            current = current->next;
        }
    }

    return jarak[lokasiTujuan];
}

// Fungsi lama untuk kompatibilitas
float hitungJarakTerdekat(int lokasiAsal, int lokasiTujuan)
{
    return hitungJarakTerpendek(lokasiAsal, lokasiTujuan);
}

// ===== FUNGSI PEMESANAN =====

// Cart item structure for multiple item ordering
struct cartItem
{
    makanan food;
    int quantity;
    float subtotal;
};

void pesanMakanan()
{
    if (currentPenumpang == nullptr)
    {
        cout << "Anda harus login terlebih dahulu!" << endl;
        return;
    }

    cout << "\n+============================================================+" << endl;
    cout << "|                    [FOOD] PESAN MAKANAN                   |" << endl;
    cout << "+============================================================+" << endl;

    // Step 1: Get customer location first
    cout << "\n+============================================================+" << endl;
    cout << "|                [LOC] LOKASI ANDA SAAT INI                 |" << endl;
    cout << "+============================================================+" << endl;
    tampilkanLokasi();

    int lokasiPengiriman;
    cout << "\n> Masukkan lokasi Anda saat ini (nomor): ";
    cin >> lokasiPengiriman;

    if (lokasiPengiriman < 0 || lokasiPengiriman >= jumlahLokasi)
    {
        cout << "[X] Lokasi tidak valid!" << endl;
        return;
    }
    cout << "\n[MAP] Lokasi Anda: " << dataLokasi[lokasiPengiriman].alamat << endl;
    cout << "[FINDING] Mencari SEMUA merchant dan menghitung jarak optimal..." << endl;

    // Step 2: Calculate distances and sort merchants by proximity
    struct MerchantJarak
    {
        int merchantIndex;
        string merchantId;
        string namaMerchant;
        int lokasiId;
        string namaLokasi;
        float jarak;
        string statusJarak; // "Langsung" atau "Via rute"
    };

    MerchantJarak merchantTerdekat[TABLE_SIZE];
    int jumlahMerchantDitemukan = 0;

    // Find ALL merchants and calculate shortest path distances
    cout << "[CALC] Menghitung jarak terpendek ke semua merchant..." << endl;
    for (int i = 0; i < TABLE_SIZE; i++)
    {
        if (isInitializedMerchant[i])
        {
            int merchantLokasiId = stoi(hashTableMerchant[i].idMerchant);
            if (merchantLokasiId >= 0 && merchantLokasiId < jumlahLokasi)
            {
                // Gunakan algoritma Dijkstra untuk mendapatkan jarak terpendek
                float jarak = hitungJarakTerpendek(lokasiPengiriman, merchantLokasiId);

                // Cek apakah ada koneksi langsung
                string statusJarak = "Via rute";
                graphLokasi *current = dataLokasi[lokasiPengiriman].V;
                while (current != nullptr)
                {
                    if (current->destination == merchantLokasiId)
                    {
                        statusJarak = "Langsung";
                        break;
                    }
                    current = current->next;
                }

                // Tambahkan SEMUA merchant, bahkan yang jaraknya 999999
                if (jumlahMerchantDitemukan < TABLE_SIZE)
                {
                    merchantTerdekat[jumlahMerchantDitemukan].merchantIndex = i;
                    merchantTerdekat[jumlahMerchantDitemukan].merchantId = hashTableMerchant[i].idMerchant;
                    merchantTerdekat[jumlahMerchantDitemukan].namaMerchant = hashTableMerchant[i].namaMerchant;
                    merchantTerdekat[jumlahMerchantDitemukan].lokasiId = merchantLokasiId;
                    merchantTerdekat[jumlahMerchantDitemukan].namaLokasi = dataLokasi[merchantLokasiId].alamat;
                    merchantTerdekat[jumlahMerchantDitemukan].jarak = jarak;
                    merchantTerdekat[jumlahMerchantDitemukan].statusJarak = statusJarak;
                    jumlahMerchantDitemukan++;
                }
            }
        }
    }
    if (jumlahMerchantDitemukan == 0)
    {
        cout << "\n+============================================================+" << endl;
        cout << "| [EMPTY] Tidak ada merchant terdaftar dalam sistem!        |" << endl;
        cout << "| [INFO] Silakan hubungi admin untuk menambah merchant.     |" << endl;
        cout << "+============================================================+" << endl;
        return;
    }

    // Step 3: Sort merchants by distance using bubble sort (nearest first)
    for (int i = 0; i < jumlahMerchantDitemukan - 1; i++)
    {
        for (int j = 0; j < jumlahMerchantDitemukan - i - 1; j++)
        {
            if (merchantTerdekat[j].jarak > merchantTerdekat[j + 1].jarak)
            {
                MerchantJarak temp = merchantTerdekat[j];
                merchantTerdekat[j] = merchantTerdekat[j + 1];
                merchantTerdekat[j + 1] = temp;
            }
        }
    } // Step 4: Display sorted merchants by distance
    cout << "\n+============================================================+" << endl;
    cout << "|             [ALL MERCHANTS] SEMUA TOKO TERSEDIA           |" << endl;
    cout << "+============================================================+" << endl;
    cout << "| [ALGORITHM] Menggunakan Dijkstra untuk jarak terpendek    |" << endl;
    cout << "| [FOUND] Total merchant ditemukan: " << jumlahMerchantDitemukan << endl;
    cout << "| [SORTED] Diurutkan berdasarkan jarak optimal               |" << endl;
    cout << "+============================================================+" << endl;

    // TAMPILKAN SEMUA MERCHANT YANG DITEMUKAN
    int maxTampil = jumlahMerchantDitemukan;
    float estimasiWaktu, ongkosKirim;
    for (int i = 0; i < maxTampil; i++)
    {
        // Hitung estimasi dan ongkir berdasarkan jarak

        string statusKetersediaan;

        if (merchantTerdekat[i].jarak == 999999)
        {
            estimasiWaktu = 0;
            ongkosKirim = 0;
            statusKetersediaan = "TIDAK TERJANGKAU";
        }
        else
        {
            estimasiWaktu = merchantTerdekat[i].jarak * 2.5;
            ongkosKirim = merchantTerdekat[i].jarak * 2000;
            statusKetersediaan = "TERSEDIA";
        }

        cout << "+-----------------------------------------------------------+" << endl;
        cout << "| " << (i + 1) << ". [ID: " << merchantTerdekat[i].merchantId << "] "
             << merchantTerdekat[i].namaMerchant << endl;
        cout << "| [LOC] " << merchantTerdekat[i].namaLokasi << endl;

        if (merchantTerdekat[i].jarak == 999999)
        {
            cout << "| [DIST] Jarak: TIDAK TERHUBUNG" << endl;
            cout << "| [STATUS] " << statusKetersediaan << endl;
            cout << "| [INFO] Lokasi tidak dapat dijangkau dari posisi Anda" << endl;
        }
        else
        {
            cout << "| [DIST] Jarak: " << merchantTerdekat[i].jarak << " km ("
                 << merchantTerdekat[i].statusJarak << ")" << endl;
            cout << "| [TIME] Estimasi: " << (int)estimasiWaktu << " menit" << endl;
            cout << "| [DELIVERY] Ongkir: Rp" << ongkosKirim << endl;
            cout << "| [STATUS] " << statusKetersediaan << endl;
        }
    }
    cout << "+-----------------------------------------------------------+" << endl;
    cout << "| [INFO] Menampilkan SEMUA " << jumlahMerchantDitemukan << " merchant" << endl;
    cout << "| [TIP] Pilih merchant dengan status 'TERSEDIA' untuk pesan |" << endl;
    cout << "+============================================================+" << endl; // Step 5: Let customer choose from sorted merchant list
    string idMerchant;
    cout << "\n> Pilih merchant berdasarkan ID: ";
    cin >> idMerchant;

    // Verify merchant exists
    int hashIndex = stoi(idMerchant) % TABLE_SIZE;
    if (!isInitializedMerchant[hashIndex] || hashTableMerchant[hashIndex].idMerchant != idMerchant)
    {
        cout << "[X] Merchant tidak ditemukan!" << endl;
        return;
    }

    // Check if merchant is reachable by finding it in our sorted list
    bool merchantTerjangkau = false;
    float jarakKeMerchant = 999999;
    for (int i = 0; i < jumlahMerchantDitemukan; i++)
    {
        if (merchantTerdekat[i].merchantId == idMerchant)
        {
            jarakKeMerchant = merchantTerdekat[i].jarak;
            if (jarakKeMerchant != 999999)
            {
                merchantTerjangkau = true;
            }
            break;
        }
    }

    if (!merchantTerjangkau)
    {
        cout << "\n+============================================================+" << endl;
        cout << "|                  [WARNING] MERCHANT TIDAK TERJANGKAU      |" << endl;
        cout << "+============================================================+" << endl;
        cout << "| [ERROR] Merchant yang Anda pilih tidak dapat dijangkau    |" << endl;
        cout << "| [REASON] Tidak ada rute yang menghubungkan lokasi Anda    |" << endl;
        cout << "|          dengan lokasi merchant tersebut                  |" << endl;
        cout << "| [SOLUTION] Silakan pilih merchant dengan status 'TERSEDIA'|" << endl;
        cout << "+============================================================+" << endl;
        cout << "\n[BACK] Kembali ke daftar merchant? (y/n): ";
        char back;
        cin >> back;
        if (back == 'y' || back == 'Y')
        {
            pesanMakanan(); // Restart the function
        }
        return;
    }

    cout << "\n[OK] Merchant dapat dijangkau! Jarak: " << jarakKeMerchant << " km" << endl;

    // Shopping cart for multiple items
    cartItem cart[50]; // Maximum 50 items in cart
    int cartSize = 0;
    float totalHargaMakanan = 0;

    char lanjutBelanja = 'y';
    while (lanjutBelanja == 'y' || lanjutBelanja == 'Y')
    {
        cout << "\n+============================================================+" << endl;
        cout << "|                  [MENU] DAFTAR MAKANAN                    |" << endl;
        cout << "|               " << hashTableMerchant[hashIndex].namaMerchant << endl;
        cout << "+============================================================+" << endl;
        tampilkanMenuMerchant(idMerchant);

        string idMakanan;
        cout << "\n> Pilih makanan (ID): ";
        cin >> idMakanan;

        makanan *makananDipilih = cariMakananDiMerchant(idMerchant, idMakanan);
        if (makananDipilih == nullptr)
        {
            cout << "[X] Makanan tidak ditemukan!" << endl;
            continue;
        }

        int quantity;
        cout << "> Jumlah pesanan: ";
        cin >> quantity;

        if (quantity <= 0)
        {
            cout << "[X] Jumlah harus lebih dari 0!" << endl;
            continue;
        }

        // Add to cart
        cart[cartSize].food = *makananDipilih;
        cart[cartSize].quantity = quantity;
        cart[cartSize].subtotal = makananDipilih->harga * quantity;
        totalHargaMakanan += cart[cartSize].subtotal;
        cartSize++;

        cout << "\n[OK] " << quantity << "x " << makananDipilih->namaMakanan
             << " ditambahkan ke keranjang!" << endl;
        cout << "[PRICE] Subtotal: Rp" << cart[cartSize - 1].subtotal << endl;

        // Show current cart
        cout << "\n+-----------------------------------------------------------+" << endl;
        cout << "|                   [CART] KERANJANG ANDA                  |" << endl;
        cout << "+-----------------------------------------------------------+" << endl;
        for (int i = 0; i < cartSize; i++)
        {
            cout << "| " << (i + 1) << ". " << cart[i].quantity << "x "
                 << cart[i].food.namaMakanan << " - Rp" << cart[i].subtotal << endl;
        }
        cout << "+-----------------------------------------------------------+" << endl;
        cout << "| [SUBTOTAL] Total Makanan: Rp" << totalHargaMakanan << endl;
        cout << "+-----------------------------------------------------------+" << endl;

        if (cartSize >= 50)
        {
            cout << "[!] Keranjang penuh! Maksimal 50 item." << endl;
            break;
        }

        cout << "\n[SHOP] Tambah makanan lain? (y/n): ";
        cin >> lanjutBelanja;
    }

    if (cartSize == 0)
    {
        cout << "[X] Keranjang kosong! Pesanan dibatalkan." << endl;
        return;
    }

    // Find available courier
    kurir *kurirTerpilih = cariKurirTersedia(dataLokasi[lokasiPengiriman].alamat);
    if (kurirTerpilih == nullptr)
    {
        cout << "[X] Tidak ada kurir tersedia saat ini!" << endl;
        cout << "[!] Silakan coba lagi nanti." << endl;
        return;
    }

    // Calculate delivery cost and total
    float biayaLayanan = 2000; // Service fee
    float total = totalHargaMakanan + ongkosKirim + biayaLayanan;

    // Display detailed receipt
    cout << "\n+============================================================+" << endl;
    cout << "|                    [RECEIPT] DETAIL PESANAN               |" << endl;
    cout << "+============================================================+" << endl;
    cout << "| [MERCHANT] " << hashTableMerchant[hashIndex].namaMerchant << endl;
    cout << "| [CUSTOMER] " << currentPenumpang->nama << endl;
    cout << "| [PHONE] " << currentPenumpang->noTelepon << endl;
    cout << "| [DELIVERY] " << dataLokasi[lokasiPengiriman].alamat << endl;
    cout << "| [COURIER] " << kurirTerpilih->nama << " (ID: " << kurirTerpilih->id_kurir << ")" << endl;
    cout << "+-----------------------------------------------------------+" << endl;
    cout << "|                      ITEM PESANAN                        |" << endl;
    cout << "+-----------------------------------------------------------+" << endl;

    for (int i = 0; i < cartSize; i++)
    {
        cout << "| " << (i + 1) << ". " << cart[i].quantity << "x "
             << cart[i].food.namaMakanan << endl;
        cout << "|    @Rp" << cart[i].food.harga << " = Rp" << cart[i].subtotal << endl;
    }

    cout << "+-----------------------------------------------------------+" << endl;
    cout << "|                     RINCIAN BIAYA                        |" << endl;
    cout << "+-----------------------------------------------------------+" << endl;
    cout << "| [FOOD] Subtotal Makanan      : Rp" << totalHargaMakanan << endl;
    cout << "| [DELIVERY] Ongkos Kirim      : Rp" << ongkosKirim << endl;
    cout << "| [SERVICE] Biaya Layanan      : Rp" << biayaLayanan << endl;
    cout << "+-----------------------------------------------------------+" << endl;
    cout << "| [TOTAL] TOTAL PEMBAYARAN     : Rp" << total << endl;
    cout << "+============================================================+" << endl; // Purchase confirmation
    cout << "\n[CONFIRM] Konfirmasi pemesanan? (y/n): ";
    char konfirmasi;
    cin >> konfirmasi;

    if (konfirmasi != 'y' && konfirmasi != 'Y')
    {
        cout << "\n[CANCEL] Pesanan dibatalkan!" << endl;
        cout << "[!] Terima kasih telah menggunakan OJOLOKA." << endl;
        return;
    }

    // Create order
    if (jumlahPesanan < MAX_PESANAN)
    {
        string orderId = "PSN" + to_string(jumlahPesanan + 1);
        dataPesanan[jumlahPesanan].id_pesanan = orderId;
        dataPesanan[jumlahPesanan].jenis = "makanan";
        dataPesanan[jumlahPesanan].customer = currentPenumpang;
        dataPesanan[jumlahPesanan].kurirTerpilih = kurirTerpilih;
        dataPesanan[jumlahPesanan].lokasi_tujuan = dataLokasi[lokasiPengiriman].alamat;
        dataPesanan[jumlahPesanan].totalBiaya = total;
        dataPesanan[jumlahPesanan].status = "menunggu";
        dataPesanan[jumlahPesanan].konfirmasi_customer = false;
        dataPesanan[jumlahPesanan].konfirmasi_kurir = false;

        // Add to merchant order queue
        merchantOrder newOrder;
        newOrder.id_pesanan = orderId;
        newOrder.id_penumpang = currentPenumpang->id_penumpang;
        newOrder.nama_penumpang = currentPenumpang->nama;

        // For simplicity, use first item as main order (in real system, would handle multiple items)
        newOrder.id_makanan = cart[0].food.id_makanan;
        newOrder.nama_makanan = cart[0].food.namaMakanan;
        if (cartSize > 1)
        {
            newOrder.nama_makanan += " + " + to_string(cartSize - 1) + " item lainnya";
        }
        newOrder.harga = total;
        newOrder.status = "pending";
        newOrder.kurir_id = kurirTerpilih->id_kurir;
        newOrder.created_time = "now"; // In real system, would use actual timestamp

        enqueueOrder(&hashTableMerchant[hashIndex].orders, newOrder);

        kurirTerpilih->statusKurir = "sibuk";
        jumlahPesanan++; // Add to history stack for passenger (LIFO - Last In First Out)
        historyItem newHistory;
        newHistory.id_transaksi = orderId;
        newHistory.jenis = "makanan";
        newHistory.tanggal = "2025-06-02"; // Current date
        newHistory.waktu = "12:00";        // Placeholder time
        newHistory.lokasi_asal = hashTableMerchant[hashIndex].namaMerchant;
        newHistory.lokasi_tujuan = dataLokasi[lokasiPengiriman].alamat;
        newHistory.detail_pesanan = newOrder.nama_makanan;
        newHistory.total_biaya = total;
        newHistory.status = "menunggu";
        newHistory.kurir_id = kurirTerpilih->id_kurir;
        newHistory.kurir_nama = kurirTerpilih->nama;
        newHistory.rating = 0;

        // Push to global history stack using Stack (LIFO) implementation
        tambahHistoryPenumpang(currentPenumpang->id_penumpang, newHistory);

        // Also add to courier history
        tambahHistoryKurir(kurirTerpilih->id_kurir, newHistory);

        cout << "\n+============================================================+" << endl;
        cout << "|                  [SUCCESS] PESANAN BERHASIL!              |" << endl;
        cout << "+============================================================+" << endl;
        cout << "| [ORDER ID] " << orderId << endl;
        cout << "| [STATUS] Menunggu konfirmasi merchant                     |" << endl;
        cout << "| [ESTIMATE] Estimasi waktu: 20-30 menit                    |" << endl;
        cout << "+============================================================+" << endl;
        cout << "| [TRACK] Anda dapat melacak pesanan di menu 'Lihat Pesanan'|" << endl;
        cout << "| [PAY] Pembayaran: Cash on Delivery (COD)                  |" << endl;
        cout << "+============================================================+" << endl;
    }
    else
    {
        cout << "[X] Sistem penuh! Tidak dapat membuat pesanan baru." << endl;
    }
}

void antarJemput()
{
    if (currentPenumpang == nullptr)
    {
        cout << "Anda harus login terlebih dahulu!" << endl;
        return;
    }

    cout << "=== ANTAR JEMPUT ===" << endl;
    cout << "Lokasi asal:" << endl;
    tampilkanLokasi();
    int lokasiAsal;
    cout << "Pilih lokasi asal (nomor): ";
    cin >> lokasiAsal;

    cout << "Lokasi tujuan:" << endl;
    tampilkanLokasi();
    int lokasiTujuan;
    cout << "Pilih lokasi tujuan (nomor): ";
    cin >> lokasiTujuan;

    float jarak = hitungJarakTerdekat(lokasiAsal, lokasiTujuan);
    float tarif = jarak * 2000; // Rp 2000 per km

    kurir *kurirTerpilih = cariKurirTersedia(dataLokasi[lokasiAsal].alamat);
    if (kurirTerpilih == nullptr)
    {
        cout << "Tidak ada kurir tersedia saat ini!" << endl;
        return;
    }

    cout << "Jarak: " << jarak << " km" << endl;
    cout << "Tarif: Rp" << tarif << endl;
    cout << "Kurir: " << kurirTerpilih->nama << endl;

    char konfirmasi;
    cout << "Lanjutkan pesanan? (y/n): ";
    cin >> konfirmasi;

    if (konfirmasi == 'y' || konfirmasi == 'Y')
    {
        if (jumlahPesanan < MAX_PESANAN)
        {
            dataPesanan[jumlahPesanan].id_pesanan = "PSN" + to_string(jumlahPesanan + 1);
            dataPesanan[jumlahPesanan].jenis = "penumpang";
            dataPesanan[jumlahPesanan].customer = currentPenumpang;
            dataPesanan[jumlahPesanan].kurirTerpilih = kurirTerpilih;
            dataPesanan[jumlahPesanan].lokasi_asal = dataLokasi[lokasiAsal].alamat;
            dataPesanan[jumlahPesanan].lokasi_tujuan = dataLokasi[lokasiTujuan].alamat;
            dataPesanan[jumlahPesanan].totalBiaya = tarif;
            dataPesanan[jumlahPesanan].status = "menunggu";
            dataPesanan[jumlahPesanan].konfirmasi_customer = false;
            dataPesanan[jumlahPesanan].konfirmasi_kurir = false;
            kurirTerpilih->statusKurir = "sibuk";
            jumlahPesanan++;

            // Add to history stack for both passenger and courier (LIFO - Last In First Out)
            historyItem newHistory;
            newHistory.id_transaksi = dataPesanan[jumlahPesanan - 1].id_pesanan;
            newHistory.jenis = "penumpang";
            newHistory.tanggal = "2025-06-02"; // Current date
            newHistory.waktu = "12:00";        // Placeholder time
            newHistory.lokasi_asal = dataLokasi[lokasiAsal].alamat;
            newHistory.lokasi_tujuan = dataLokasi[lokasiTujuan].alamat;
            newHistory.detail_pesanan = "Antar Jemput - " + to_string(jarak) + " km";
            newHistory.total_biaya = tarif;
            newHistory.status = "menunggu";
            newHistory.kurir_id = kurirTerpilih->id_kurir;
            newHistory.kurir_nama = kurirTerpilih->nama;
            newHistory.rating = 0;

            // Push to global history stacks using Stack (LIFO) implementation
            tambahHistoryPenumpang(currentPenumpang->id_penumpang, newHistory);
            tambahHistoryKurir(kurirTerpilih->id_kurir, newHistory);

            cout << "Pesanan berhasil dibuat!" << endl;
            cout << "ID Pesanan: " << dataPesanan[jumlahPesanan - 1].id_pesanan << endl;
        }
    }
}

// ===== FUNGSI REKURSI UNTUK TOTAL HARGA =====
float hitungTotalHargaRekursif(float hargaPokok, float ongkir, int level)
{
    if (level == 0)
        return hargaPokok;
    return hitungTotalHargaRekursif(hargaPokok, ongkir, level - 1) + (ongkir / level);
}

// ===== FUNGSI MENU UTAMA =====

void menuPenumpang()
{
    int pilihan;
    do
    {
        cout << "\n+====================================+" << endl;
        cout << "|         MENU PENUMPANG             |" << endl;
        cout << "+====================================+" << endl;
        cout << "| 1. [FOOD] Pesan Makanan           |" << endl;
        cout << "| 2. [RIDE] Antar Jemput             |" << endl;
        cout << "| 3. [LIST] Lihat Pesanan Aktif      |" << endl;
        cout << "| 4. [CONF] Konfirmasi Pesanan       |" << endl;
        cout << "| 5. [HIST] Riwayat Pesanan (Stack)  |" << endl;
        cout << "| 6. [EXIT] Logout                   |" << endl;
        cout << "+====================================+" << endl;
        cout << "> Pilihan Anda: ";
        cin >> pilihan;

        if (pilihan == 1)
        {
            pesanMakanan();
        }
        else if (pilihan == 2)
        {
            antarJemput();
        }
        else if (pilihan == 3)
        {
            // Tampilkan pesanan aktif
            cout << "\n+============================================================+" << endl;
            cout << "|                    PESANAN AKTIF                           |" << endl;
            cout << "+============================================================+" << endl;
            bool adaPesanan = false;
            for (int i = 0; i < jumlahPesanan; i++)
            {
                if (dataPesanan[i].customer->id_penumpang == currentPenumpang->id_penumpang)
                {
                    cout << "+-----------------------------------------------------------+" << endl;
                    cout << "| [ID] ID Pesanan: " << dataPesanan[i].id_pesanan << endl;
                    cout << "| [STAT] Status: " << dataPesanan[i].status << endl;
                    cout << "| [COST] Total Biaya: Rp" << dataPesanan[i].totalBiaya << endl;
                    cout << "| [KURR] Kurir: " << dataPesanan[i].kurirTerpilih->nama
                         << " (ID: " << dataPesanan[i].kurirTerpilih->id_kurir << ")" << endl;
                    cout << "+-----------------------------------------------------------+" << endl;
                    adaPesanan = true;
                }
            }
            if (!adaPesanan)
            {
                cout << "[X] Tidak ada pesanan aktif saat ini." << endl;
            }
        }
        else if (pilihan == 4)
        {
            // Konfirmasi pesanan
            cout << "\n+============================================================+" << endl;
            cout << "|                   KONFIRMASI PESANAN                       |" << endl;
            cout << "+============================================================+" << endl;
            cout << "> Masukkan ID pesanan untuk konfirmasi: ";
            string idPesanan;
            cin >> idPesanan;

            bool pesananDitemukan = false;
            for (int i = 0; i < jumlahPesanan; i++)
            {
                if (dataPesanan[i].id_pesanan == idPesanan &&
                    dataPesanan[i].customer->id_penumpang == currentPenumpang->id_penumpang)
                {
                    dataPesanan[i].konfirmasi_customer = true;
                    dataPesanan[i].status = "selesai";
                    dataPesanan[i].kurirTerpilih->statusKurir = "tersedia";
                    cout << "\n[OK] Pesanan dikonfirmasi selesai!" << endl;

                    cout << "[RATE] Berikan rating (1-5): ";
                    cin >> dataPesanan[i].rating;
                    cout << "[FEED] Feedback: ";
                    cin.ignore();
                    getline(cin, dataPesanan[i].feedback);
                    cout << "[THX] Terima kasih atas feedback Anda!" << endl;

                    // Update history status to completed with rating (Stack LIFO implementation)
                    int passengerIndex = findPenumpangIndex(currentPenumpang->id_penumpang);
                    int courierIndex = findKurirIndex(dataPesanan[i].kurirTerpilih->id_kurir);

                    // Update history in both passenger and courier stacks
                    if (passengerIndex != -1)
                    {
                        historyNode *current = historyPenumpang[passengerIndex].top;
                        while (current != nullptr)
                        {
                            if (current->data.id_transaksi == idPesanan)
                            {
                                current->data.status = "selesai";
                                current->data.rating = dataPesanan[i].rating;
                                break;
                            }
                            current = current->next;
                        }
                    }

                    if (courierIndex != -1)
                    {
                        historyNode *current = historyKurir[courierIndex].top;
                        while (current != nullptr)
                        {
                            if (current->data.id_transaksi == idPesanan)
                            {
                                current->data.status = "selesai";
                                current->data.rating = dataPesanan[i].rating;
                                break;
                            }
                            current = current->next;
                        }
                    }

                    pesananDitemukan = true;
                    break;
                }
            }
            if (!pesananDitemukan)
            {
                cout << "[X] ID pesanan tidak ditemukan atau bukan milik Anda!" << endl;
            }
        }
        else if (pilihan == 5)
        {
            // View history using Stack (LIFO)
            int index = findPenumpangIndex(currentPenumpang->id_penumpang);
            if (index != -1)
            {
                tampilkanHistory(&historyPenumpang[index]);
            }
            else
            {
                cout << "\n[X] Error: Data penumpang tidak ditemukan!" << endl;
            }
            cout << "\n[BACK] Tekan Enter untuk kembali...";
            cin.ignore();
            cin.get();
        }
        else if (pilihan == 6)
        {
            currentPenumpang = nullptr;
            cout << "\n[EXIT] Logout berhasil! Terima kasih telah menggunakan OJOLOKA!" << endl;
        }
        else
        {
            cout << "[X] Pilihan tidak valid! Silakan pilih 1-6." << endl;
        }
    } while (pilihan != 6);
}

void menuKurir()
{
    int pilihan;
    do
    {
        cout << "\n+====================================+" << endl;
        cout << "|           MENU KURIR               |" << endl;
        cout << "+====================================+" << endl;
        cout << "| 1. [LIST] Lihat Pesanan Aktif      |" << endl;
        cout << "| 2. [PICK] Konfirmasi Lokasi Pickup |" << endl;
        cout << "| 3. [DEST] Konfirmasi Lokasi Tujuan |" << endl;
        cout << "| 4. [STAT] Update Status Kurir      |" << endl;
        cout << "| 5. [HIST] Riwayat Pesanan (Stack)  |" << endl;
        cout << "| 6. [EXIT] Logout                   |" << endl;
        cout << "+====================================+" << endl;
        cout << "> Pilihan Anda: ";
        cin >> pilihan;
        if (pilihan == 1)
        {
            cout << "\n+============================================================+" << endl;
            cout << "|                    PESANAN AKTIF                           |" << endl;
            cout << "+============================================================+" << endl;

            bool adaPesanan = false;
            for (int i = 0; i < jumlahPesanan; i++)
            {
                if (dataPesanan[i].kurirTerpilih->id_kurir == currentKurir->id_kurir &&
                    dataPesanan[i].status != "selesai")
                {
                    cout << "+-----------------------------------------------------------+" << endl;
                    cout << "| [ID] ID Pesanan: " << dataPesanan[i].id_pesanan << endl;
                    cout << "| [CUST] Customer: " << dataPesanan[i].customer->nama << endl;
                    cout << "| [PHONE] Telepon: " << dataPesanan[i].customer->noTelepon << endl;
                    cout << "| [TYPE] Jenis: " << dataPesanan[i].jenis << endl;
                    cout << "| [DEST] Tujuan: " << dataPesanan[i].lokasi_tujuan << endl;
                    cout << "| [COST] Total: Rp" << dataPesanan[i].totalBiaya << endl;
                    cout << "| [STAT] Status: " << dataPesanan[i].status << endl;
                    cout << "| [OK-K] Konfirmasi Kurir: " << (dataPesanan[i].konfirmasi_kurir ? "Ya" : "Belum") << endl;
                    cout << "| [OK-C] Konfirmasi Customer: " << (dataPesanan[i].konfirmasi_customer ? "Ya" : "Belum") << endl;
                    cout << "+-----------------------------------------------------------+" << endl;
                    adaPesanan = true;
                }
            }

            if (!adaPesanan)
            {
                cout << "[X] Tidak ada pesanan aktif saat ini." << endl;
            }

            cout << "\nTekan Enter untuk kembali...";
            cin.ignore();
            cin.get();
        }
        else if (pilihan == 2)
        {
            cout << "\n+============================================================+" << endl;
            cout << "|              KONFIRMASI LOKASI PICKUP                     |" << endl;
            cout << "+============================================================+" << endl;

            // Show available orders for pickup
            cout << "\n[LIST] Pesanan siap untuk pickup:" << endl;
            bool adaPesananPickup = false;

            for (int i = 0; i < jumlahPesanan; i++)
            {
                if (dataPesanan[i].kurirTerpilih->id_kurir == currentKurir->id_kurir &&
                    dataPesanan[i].status == "menunggu" && !dataPesanan[i].konfirmasi_kurir)
                {
                    cout << "* ID: " << dataPesanan[i].id_pesanan
                         << " | Customer: " << dataPesanan[i].customer->nama
                         << " | Jenis: " << dataPesanan[i].jenis << endl;
                    adaPesananPickup = true;
                }
            }

            if (!adaPesananPickup)
            {
                cout << "[X] Tidak ada pesanan yang menunggu pickup." << endl;
                break;
            }

            cout << "\n> Masukkan ID pesanan untuk konfirmasi pickup: ";
            string idPesanan;
            cin >> idPesanan;

            bool pesananDitemukan = false;
            for (int i = 0; i < jumlahPesanan; i++)
            {
                if (dataPesanan[i].id_pesanan == idPesanan &&
                    dataPesanan[i].kurirTerpilih->id_kurir == currentKurir->id_kurir)
                { // Location confirmation process
                    cout << "\n[MAP] Lokasi pickup: " << currentKurir->lokasiSekarang << endl;
                    cout << "[LOC] Apakah Anda sudah berada di lokasi pickup? (y/n): ";
                    char konfirmasi;
                    cin >> konfirmasi;

                    if (konfirmasi == 'y' || konfirmasi == 'Y')
                    {
                        dataPesanan[i].konfirmasi_kurir = true;
                        dataPesanan[i].status = "di_pickup";
                        currentKurir->statusKurir = "sibuk";

                        cout << "\n[OK] Konfirmasi lokasi pickup berhasil!" << endl;
                        cout << "[STAT] Status pesanan diupdate: " << dataPesanan[i].status << endl;
                        cout << "[KURR] Status kurir: " << currentKurir->statusKurir << endl;

                        // Update location if it's food delivery
                        if (dataPesanan[i].jenis == "makanan")
                        {
                            cout << "[FOOD] Mengambil pesanan makanan dari merchant..." << endl;
                            cout << "[TIME] Estimasi waktu pickup: 5-10 menit" << endl;
                        }
                    }
                    else
                    {
                        cout << "[X] Silakan menuju ke lokasi pickup terlebih dahulu." << endl;
                        cout << "[GPS] Gunakan GPS untuk navigasi ke: " << currentKurir->lokasiSekarang << endl;
                    }
                    pesananDitemukan = true;
                    break;
                }
            }
            if (!pesananDitemukan)
            {
                cout << "[X] ID pesanan tidak ditemukan atau bukan tanggung jawab Anda!" << endl;
            }
            cout << "\nTekan Enter untuk kembali...";
            cin.ignore();
            cin.get();
        }
        else if (pilihan == 3)
        {
            cout << "\n+============================================================+" << endl;
            cout << "|              KONFIRMASI LOKASI TUJUAN                     |" << endl;
            cout << "+============================================================+" << endl;

            // Show orders ready for delivery
            cout << "\n[DELIV] Pesanan siap untuk diantar:" << endl;
            bool adaPesananDelivery = false;

            for (int i = 0; i < jumlahPesanan; i++)
            {
                if (dataPesanan[i].kurirTerpilih->id_kurir == currentKurir->id_kurir &&
                    (dataPesanan[i].status == "di_pickup" || dataPesanan[i].status == "dalam_perjalanan"))
                {
                    cout << "* ID: " << dataPesanan[i].id_pesanan
                         << " | Customer: " << dataPesanan[i].customer->nama
                         << " | Tujuan: " << dataPesanan[i].lokasi_tujuan
                         << " | Status: " << dataPesanan[i].status << endl;
                    adaPesananDelivery = true;
                }
            }

            if (!adaPesananDelivery)
            {
                cout << "[X] Tidak ada pesanan yang siap untuk diantar." << endl;
                break;
            }

            cout << "\n> Masukkan ID pesanan untuk konfirmasi tiba di tujuan: ";
            string idPesanan;
            cin >> idPesanan;

            bool pesananDitemukan = false;
            for (int i = 0; i < jumlahPesanan; i++)
            {
                if (dataPesanan[i].id_pesanan == idPesanan &&
                    dataPesanan[i].kurirTerpilih->id_kurir == currentKurir->id_kurir)
                { // Location confirmation process
                    cout << "\n[DEST] Lokasi tujuan: " << dataPesanan[i].lokasi_tujuan << endl;
                    cout << "[LOC] Apakah Anda sudah tiba di lokasi tujuan? (y/n): ";
                    char konfirmasi;
                    cin >> konfirmasi;

                    if (konfirmasi == 'y' || konfirmasi == 'Y')
                    {
                        dataPesanan[i].status = "dalam_perjalanan";
                        cout << "\n[OK] Konfirmasi lokasi tujuan berhasil!" << endl;
                        cout << "[STAT] Status pesanan: " << dataPesanan[i].status << endl;

                        // Delivery confirmation
                        cout << "[DELIV] Apakah pesanan sudah diserahkan kepada customer? (y/n): ";
                        char serahkan;
                        cin >> serahkan;

                        if (serahkan == 'y' || serahkan == 'Y')
                        {
                            dataPesanan[i].status = "diantar";
                            cout << "[OK] Pesanan berhasil diserahkan!" << endl;
                            cout << "[WAIT] Menunggu konfirmasi dari customer..." << endl;
                        }
                        else
                        {
                            cout << "[X] Pesanan belum diserahkan. Status tetap dalam perjalanan." << endl;
                        }
                    }
                    else
                    {
                        cout << "[X] Silakan menuju ke lokasi tujuan terlebih dahulu." << endl;
                        cout << "[GPS] Gunakan GPS untuk navigasi ke: " << dataPesanan[i].lokasi_tujuan << endl;
                    }
                    pesananDitemukan = true;
                    break;
                }
            }
            if (!pesananDitemukan)
            {
                cout << "[X] ID pesanan tidak ditemukan atau bukan tanggung jawab Anda!" << endl;
            }

            cout << "\nTekan Enter untuk kembali...";
            cin.ignore();
            cin.get();
        }
        else if (pilihan == 4)
        {
            cout << "\n+====================================+" << endl;
            cout << "|        UPDATE STATUS KURIR         |" << endl;
            cout << "+====================================+" << endl;
            cout << "| 1. [OK] Tersedia                   |" << endl;
            cout << "| 2. [X] Sibuk                       |" << endl;
            cout << "| 3. [OFF] Off Duty                  |" << endl;
            cout << "+====================================+" << endl;
            cout << "> Pilih status: ";
            int status;
            cin >> status;

            string statusBaru;
            if (status == 1)
            {
                statusBaru = "tersedia";
            }
            else if (status == 2)
            {
                statusBaru = "sibuk";
            }
            else if (status == 3)
            {
                statusBaru = "off_duty";
            }
            else
            {
                cout << "[X] Pilihan tidak valid!" << endl;
            }

            if (status >= 1 && status <= 3)
            {
                currentKurir->statusKurir = statusBaru;
                cout << "[UPDT] Status kurir berhasil diupdate menjadi: " << currentKurir->statusKurir << endl;

                // Show current location
                cout << "[LOC] Lokasi saat ini: " << currentKurir->lokasiSekarang << endl;

                // Allow location update if available
                if (statusBaru == "tersedia")
                {
                    cout << "[MAP] Ingin update lokasi Anda? (y/n): ";
                    char updateLoc;
                    cin >> updateLoc;

                    if (updateLoc == 'y' || updateLoc == 'Y')
                    {
                        cout << "\nDaftar lokasi tersedia:" << endl;
                        tampilkanLokasi();
                        cout << "> Pilih nomor lokasi baru: ";
                        int lokasiIndex;
                        cin >> lokasiIndex;

                        if (lokasiIndex >= 0 && lokasiIndex < jumlahLokasi)
                        {
                            currentKurir->lokasiSekarang = dataLokasi[lokasiIndex].alamat;
                            cout << "[OK] Lokasi berhasil diupdate ke: " << currentKurir->lokasiSekarang << endl;
                        }
                        else
                        {
                            cout << "[X] Nomor lokasi tidak valid!" << endl;
                        }
                    }
                }
            }
        }
        else if (pilihan == 5)
        {
            // View history using Stack (LIFO)
            int index = findKurirIndex(currentKurir->id_kurir);
            if (index != -1)
            {
                tampilkanHistory(&historyKurir[index]);
            }
            else
            {
                cout << "\n[X] Error: Data kurir tidak ditemukan!" << endl;
            }
            cout << "\n[BACK] Tekan Enter untuk kembali...";
            cin.ignore();
            cin.get();
        }
        else if (pilihan == 6)
        {
            currentKurir = nullptr;
            cout << "\n[EXIT] Logout berhasil! Terima kasih telah menggunakan OJOLOKA!" << endl;
        }
        else
        {
            cout << "[X] Pilihan tidak valid! Silakan pilih 1-6." << endl;
        }
    } while (pilihan != 6);
}

void menuMerchant()
{
    int pilihan;
    do
    {
        cout << "\n+============================================================+" << endl;
        cout << "|                   [SHOP] MENU MERCHANT                    |" << endl;
        cout << "|                 " << currentMerchant->namaMerchant << endl;
        cout << "+============================================================+" << endl;
        cout << "| 1. [MENU] Lihat Menu Makanan                              |" << endl;
        cout << "| 2. [ADD] Tambah Makanan Baru                              |" << endl;
        cout << "| 3. [EDIT] Edit Harga Makanan                              |" << endl;
        cout << "| 4. [DEL] Hapus Makanan                                    |" << endl;
        cout << "| 5. [ORDER] Kelola Pesanan                                 |" << endl;
        cout << "| 6. [CONF] Konfirmasi Pesanan Siap                         |" << endl;
        cout << "| 7. [POP] Menu Populer (Stack)                             |" << endl;
        cout << "| 8. [STAT] Statistik Pesanan                               |" << endl;
        cout << "| 9. [EXIT] Logout                                          |" << endl;
        cout << "+============================================================+" << endl;
        cout << "> Pilihan Anda: ";
        cin >> pilihan;
        if (pilihan == 1)
        {
            cout << "\n+============================================================+" << endl;
            cout << "|                   [FOOD] MENU MAKANAN                     |" << endl;
            cout << "+============================================================+" << endl;
            tampilkanMenuMerchant(currentMerchant->idMerchant);
            cout << "\n[BACK] Tekan Enter untuk kembali...";
            cin.ignore();
            cin.get();
        }
        else if (pilihan == 2)
        {
            tambahMakananBaru();
        }
        else if (pilihan == 3)
        {
            editHargaMakanan();
        }
        else if (pilihan == 4)
        {
            hapusMakanan();
        }
        else if (pilihan == 5)
        {
            cout << "\n+============================================================+" << endl;
            cout << "|                   [ORDER] KELOLA PESANAN                  |" << endl;
            cout << "+============================================================+" << endl;
            displayOrders(&currentMerchant->orders);
            cout << "\n[BACK] Tekan Enter untuk kembali...";
            cin.ignore();
            cin.get();
        }
        else if (pilihan == 6)
        {
            cout << "\n+============================================================+" << endl;
            cout << "|                [CONF] KONFIRMASI PESANAN SIAP             |" << endl;
            cout << "+============================================================+" << endl;

            displayOrders(&currentMerchant->orders);

            if (currentMerchant->orders.size > 0)
            {
                string orderId;
                cout << "\n[ID] Masukkan ID pesanan yang siap diambil: ";
                cin >> orderId;

                cout << "\n[STATUS] Pilih status:" << endl;
                cout << "+-----------------------------------------------------------+" << endl;
                cout << "| 1. [READY] Ready for pickup                              |" << endl;
                cout << "| 2. [DONE] Completed                                      |" << endl;
                cout << "+-----------------------------------------------------------+" << endl;

                int statusChoice;
                cout << "> Pilihan: ";
                cin >> statusChoice;

                string newStatus;
                if (statusChoice == 1)
                {
                    newStatus = "ready";
                    cout << "\n[OK] Pesanan ditandai siap untuk pickup!" << endl;
                }
                else if (statusChoice == 2)
                {
                    newStatus = "completed";
                    cout << "\n[DONE] Pesanan ditandai selesai!" << endl;
                }
                else
                {
                    cout << "\n[X] Pilihan tidak valid!" << endl;
                    continue;
                }

                updateOrderStatus(&currentMerchant->orders, orderId, newStatus);
            }
            else
            {
                cout << "\n[EMPTY] Tidak ada pesanan yang perlu dikonfirmasi." << endl;
            }

            cout << "\n[BACK] Tekan Enter untuk kembali...";
            cin.ignore();
            cin.get();
        }
        else if (pilihan == 7)
        {
            cout << "\n+============================================================+" << endl;
            cout << "|                   [POP] MENU POPULER                      |" << endl;
            cout << "+============================================================+" << endl;

            if (currentMerchant->popularMenu.top >= 0)
            {
                cout << "\n[STAR] Menu Populer (berdasarkan pesanan terbanyak):" << endl;
                cout << "+-----------------------------------------------------------+" << endl;
                for (int i = currentMerchant->popularMenu.top; i >= 0; i--)
                {
                    cout << "| " << (currentMerchant->popularMenu.top - i + 1) << ". "
                         << currentMerchant->popularMenu.items[i].namaMakanan
                         << " - Rp" << currentMerchant->popularMenu.items[i].harga << endl;
                }
                cout << "+-----------------------------------------------------------+" << endl;
            }
            else
            {
                cout << "\n[EMPTY] Belum ada menu populer." << endl;
                cout << "[TIP] Tip: Tambahkan makanan ke menu populer saat menambah makanan baru!" << endl;
            }

            cout << "\n[BACK] Tekan Enter untuk kembali...";
            cin.ignore();
            cin.get();
        }
        else if (pilihan == 8)
        {
            cout << "\n+============================================================+" << endl;
            cout << "|                   [STAT] STATISTIK PESANAN                |" << endl;
            cout << "+============================================================+" << endl;

            int totalPesanan = currentMerchant->orders.size;
            int pendingCount = 0, readyCount = 0, completedCount = 0;

            orderNode *current = currentMerchant->orders.front;
            while (current != nullptr)
            {
                if (current->data.status == "pending")
                    pendingCount++;
                else if (current->data.status == "ready")
                    readyCount++;
                else if (current->data.status == "completed")
                    completedCount++;
                current = current->next;
            }

            cout << "\n+-----------------------------------------------------------+" << endl;
            cout << "|                    RINGKASAN PESANAN                      |" << endl;
            cout << "+-----------------------------------------------------------+" << endl;
            cout << "| [TOTAL] Total Pesanan: " << totalPesanan << endl;
            cout << "| [WAIT] Pending: " << pendingCount << endl;
            cout << "| [READY] Ready: " << readyCount << endl;
            cout << "| [DONE] Completed: " << completedCount << endl;
            cout << "+-----------------------------------------------------------+" << endl;

            cout << "\n[BACK] Tekan Enter untuk kembali...";
            cin.ignore();
            cin.get();
        }
        else if (pilihan == 9)
        {
            currentMerchant = nullptr;
            cout << "\n+============================================================+" << endl;
            cout << "|                   [LOGOUT] LOGOUT BERHASIL                |" << endl;
            cout << "|            Terima kasih telah menggunakan OJOLOKA!        |" << endl;
            cout << "|                 Sampai jumpa lagi!                        |" << endl;
            cout << "+============================================================+" << endl;
        }
        else
        {
            cout << "\n[X] Pilihan tidak valid!" << endl;
            cout << "[!] Silakan pilih angka 1-9." << endl;
        }
    } while (pilihan != 9);
}

// ===== FUNGSI ORDER MANAGEMENT =====

void initOrderQueue(orderQueue *queue)
{
    queue->front = nullptr;
    queue->rear = nullptr;
    queue->size = 0;
}

void initMenuStack(menuStack *stack)
{
    stack->items = new makanan[20];
    stack->top = -1;
    stack->capacity = 20;
}

void enqueueOrder(orderQueue *queue, merchantOrder order)
{
    orderNode *newNode = new orderNode();
    newNode->data = order;
    newNode->next = nullptr;

    if (queue->rear == nullptr)
    {
        queue->front = queue->rear = newNode;
    }
    else
    {
        queue->rear->next = newNode;
        queue->rear = newNode;
    }
    queue->size++;
}

merchantOrder dequeueOrder(orderQueue *queue)
{
    merchantOrder empty = {"", "", "", "", "", 0, "", "", ""};
    if (queue->front == nullptr)
    {
        return empty;
    }

    orderNode *temp = queue->front;
    merchantOrder data = temp->data;
    queue->front = queue->front->next;

    if (queue->front == nullptr)
    {
        queue->rear = nullptr;
    }

    delete temp;
    queue->size--;
    return data;
}

void pushPopularMenu(menuStack *stack, makanan item)
{
    if (stack->top < stack->capacity - 1)
    {
        stack->top++;
        stack->items[stack->top] = item;
    }
}

makanan popPopularMenu(menuStack *stack)
{
    makanan empty = {"", "", 0};
    if (stack->top >= 0)
    {
        makanan item = stack->items[stack->top];
        stack->top--;
        return item;
    }
    return empty;
}

// ===== HISTORY STACK FUNCTIONS - LIFO IMPLEMENTATION =====

void initHistoryStack(historyStack *stack)
{
    stack->top = nullptr;
    stack->size = 0;
}

void pushHistory(historyStack *stack, historyItem item)
{
    historyNode *newNode = new historyNode();
    newNode->data = item;
    newNode->next = stack->top;
    stack->top = newNode;
    stack->size++;
}

void tampilkanHistory(historyStack *stack)
{
    if (stack->top == nullptr)
    {
        cout << "\n+============================================================+" << endl;
        cout << "|                    [EMPTY] RIWAYAT KOSONG                 |" << endl;
        cout << "|                Belum ada transaksi yang selesai           |" << endl;
        cout << "+============================================================+" << endl;
        return;
    }

    cout << "\n+============================================================+" << endl;
    cout << "|                     [HISTORY] RIWAYAT PESANAN             |" << endl;
    cout << "+============================================================+" << endl;

    historyNode *current = stack->top;
    int nomor = 1;

    while (current != nullptr)
    {
        cout << "+-----------------------------------------------------------+" << endl;
        cout << "| [" << nomor << "] ID: " << current->data.id_transaksi << endl;
        cout << "| [JENIS] " << current->data.jenis << endl;
        cout << "| [TANGGAL] " << current->data.tanggal << " " << current->data.waktu << endl;
        cout << "| [RUTE] " << current->data.lokasi_asal << endl;
        cout << "|        -> " << current->data.lokasi_tujuan << endl;
        cout << "| [PESANAN] " << current->data.detail_pesanan << endl;
        cout << "| [BIAYA] Rp" << current->data.total_biaya << endl;
        cout << "| [STATUS] " << current->data.status << endl;
        if (!current->data.kurir_nama.empty())
        {
            cout << "| [KURIR] " << current->data.kurir_nama << " (" << current->data.kurir_id << ")" << endl;
        }
        if (current->data.rating > 0)
        {
            cout << "| [RATING] " << current->data.rating << "/5 bintang" << endl;
        }
        cout << "+-----------------------------------------------------------+" << endl;

        current = current->next;
        nomor++;
    }

    cout << "\n[INFO] Total transaksi: " << stack->size << " (ditampilkan dari yang terbaru)" << endl;
}

int findPenumpangIndex(string id_penumpang)
{
    for (int i = 0; i < jumlahPenumpang; i++)
    {
        if (dataPenumpang[i].id_penumpang == id_penumpang)
        {
            return i;
        }
    }
    return -1;
}

int findKurirIndex(string id_kurir)
{
    for (int i = 0; i < jumlahKurir; i++)
    {
        if (dataKurir[i].id_kurir == id_kurir)
        {
            return i;
        }
    }
    return -1;
}

void tambahHistoryPenumpang(string id_penumpang, historyItem item)
{
    int index = findPenumpangIndex(id_penumpang);
    if (index != -1)
    {
        // Initialize history stack if not already done
        if (!historyInitialized[index])
        {
            initHistoryStack(&historyPenumpang[index]);
            historyInitialized[index] = true;
        }

        // Push to stack (LIFO - Last In First Out)
        pushHistory(&historyPenumpang[index], item);
    }
}

void tambahHistoryKurir(string id_kurir, historyItem item)
{
    int index = findKurirIndex(id_kurir);
    if (index != -1)
    {
        // Initialize history stack if not already done
        int stackIndex = MAX_PENUMPANG + index;
        if (!historyInitialized[stackIndex])
        {
            initHistoryStack(&historyKurir[index]);
            historyInitialized[stackIndex] = true;
        }

        // Push to stack (LIFO - Last In First Out)
        pushHistory(&historyKurir[index], item);
    }
}

int main()
{
    // Load semua data dari file
    loadPenumpangData();
    loadKurirData();
    loadLokasiData();
    loadMerchantData();
    int pilihan;
    do
    {
        cout << "\n+==============================================================+" << endl;
        cout << "|                       SISTEM OJOLOKA                        |" << endl;
        cout << "|              Platform Transportasi & Delivery               |" << endl;
        cout << "+==============================================================+" << endl;
        cout << "|                         LOGIN MENU                          |" << endl;
        cout << "|  1. [USER] Login Penumpang                                  |" << endl;
        cout << "|  2. [RIDER] Login Kurir                                     |" << endl;
        cout << "|  3. [SHOP] Login Merchant                                   |" << endl;
        cout << "+==============================================================+" << endl;
        cout << "|                       REGISTER MENU                         |" << endl;
        cout << "|  4. [REG-U] Register Penumpang                              |" << endl;
        cout << "|  5. [REG-R] Register Kurir                                  |" << endl;
        cout << "|  6. [REG-S] Register Merchant                               |" << endl;
        cout << "+==============================================================+" << endl;
        cout << "|  7. [EXIT] Keluar                                           |" << endl;
        cout << "+==============================================================+" << endl;
        cout << "> Pilihan Anda: ";
        cin >> pilihan;
        if (pilihan == 1)
        {
            cout << "\n+==============================================================+" << endl;
            cout << "|                    [USER] LOGIN PENUMPANG                   |" << endl;
            cout << "+==============================================================+" << endl;

            string id, password;
            cout << "[ID] ID Penumpang: ";
            cin >> id;
            cout << "[PASS] Password: ";
            cin >> password;

            currentPenumpang = loginPenumpang(id, password);
            if (currentPenumpang != nullptr)
            {
                cout << "\n[OK] Login berhasil!" << endl;
                cout << "[WELCOME] Selamat datang, " << currentPenumpang->nama << "!" << endl;
                cout << "[PHONE] Telepon: " << currentPenumpang->noTelepon << endl;
                cout << "[HOME] Alamat: " << currentPenumpang->alamat << endl;
                menuPenumpang();
            }
            else
            {
                cout << "\n[X] Login gagal!" << endl;
                cout << "[!] Periksa kembali ID dan password Anda." << endl;
            }
        }
        else if (pilihan == 2)
        {
            cout << "\n+==============================================================+" << endl;
            cout << "|                      [RIDER] LOGIN KURIR                    |" << endl;
            cout << "+==============================================================+" << endl;

            string id, password;
            cout << "[ID] ID Kurir: ";
            cin >> id;
            cout << "[PASS] Password: ";
            cin >> password;

            currentKurir = loginKurir(id, password);
            if (currentKurir != nullptr)
            {
                cout << "\n[OK] Login berhasil!" << endl;
                cout << "[WELCOME] Selamat datang, " << currentKurir->nama << "!" << endl;
                cout << "[LOC] Lokasi saat ini: " << currentKurir->lokasiSekarang << endl;
                cout << "[STAT] Status: " << currentKurir->statusKurir << endl;
                menuKurir();
            }
            else
            {
                cout << "\n[X] Login gagal!" << endl;
                cout << "[!] Periksa kembali ID dan password Anda." << endl;
            }
        }
        else if (pilihan == 3)
        {
            cout << "\n+==============================================================+" << endl;
            cout << "|                     [SHOP] LOGIN MERCHANT                   |" << endl;
            cout << "+==============================================================+" << endl;

            string id, password;
            cout << "[ID] ID Merchant: ";
            cin >> id;
            cout << "[PASS] Password: ";
            cin >> password;

            currentMerchant = loginMerchant(id, password);
            if (currentMerchant != nullptr)
            {
                cout << "\n[OK] Login berhasil!" << endl;
                cout << "[WELCOME] Selamat datang, " << currentMerchant->namaMerchant << "!" << endl;
                cout << "[ID] ID Merchant: " << currentMerchant->idMerchant << endl;
                menuMerchant();
            }
            else
            {
                cout << "\n[X] Login gagal!" << endl;
                cout << "[!] Periksa kembali ID dan password Anda." << endl;
            }
        }
        else if (pilihan == 4)
        {
            registerPenumpang();
        }
        else if (pilihan == 5)
        {
            registerKurir();
        }
        else if (pilihan == 6)
        {
            registerMerchant();
        }
        else if (pilihan == 7)
        {
            cout << "\n+============================================================+" << endl;
            cout << "|                   [BYE] TERIMA KASIH!                     |" << endl;
            cout << "|              Terima kasih telah menggunakan               |" << endl;
            cout << "|                     SISTEM OJOLOKA                        |" << endl;
            cout << "|                 Sampai jumpa lagi!                        |" << endl;
            cout << "+============================================================+" << endl;
        }
        else
        {
            cout << "\n[X] Pilihan tidak valid!" << endl;
            cout << "[!] Silakan pilih angka 1-7." << endl;
        }
    } while (pilihan != 7);

    return 0;
}

// ===== IMPLEMENTASI FUNGSI YANG BELUM DIBUAT =====

void tambahMakananBaru()
{
    cout << "\n+============================================================+" << endl;
    cout << "|                [ADD] TAMBAH MAKANAN BARU                  |" << endl;
    cout << "+============================================================+" << endl;

    if (currentMerchant == nullptr)
    {
        cout << "[X] Error: Merchant tidak ditemukan!" << endl;
        return;
    }

    makanan newMakanan;
    bool makananValid = false;

    // Validate ID Makanan
    while (!makananValid)
    {
        cout << "[ID] ID Makanan: ";
        cin >> newMakanan.id_makanan;

        if (newMakanan.id_makanan.empty())
        {
            cout << "[X] ID Makanan tidak boleh kosong!" << endl;
        }
        else
        {
            // Check if ID already exists
            TreeNodeMakanan *existing = cariMakanan(currentMerchant->treeMakanan.root, newMakanan.id_makanan);
            if (existing != nullptr)
            {
                cout << "[X] ID Makanan sudah ada! Gunakan ID yang berbeda." << endl;
            }
            else
            {
                makananValid = true;
            }
        }
    }

    // Validate Nama Makanan
    cout << "[NAMA] Nama Makanan: ";
    cin.ignore();
    getline(cin, newMakanan.namaMakanan);
    while (newMakanan.namaMakanan.empty())
    {
        cout << "[X] Nama makanan tidak boleh kosong!" << endl;
        cout << "[NAMA] Nama Makanan: ";
        getline(cin, newMakanan.namaMakanan);
    }

    // Validate Harga
    while (true)
    {
        cout << "[HARGA] Harga: Rp";
        cin >> newMakanan.harga;
        if (newMakanan.harga <= 0)
        {
            cout << "[X] Harga harus lebih dari 0!" << endl;
        }
        else
        {
            break;
        }
    }

    // Confirmation
    cout << "\n+-----------------------------------------------------------+" << endl;
    cout << "|                    KONFIRMASI MAKANAN BARU                |" << endl;
    cout << "+-----------------------------------------------------------+" << endl;
    cout << "| [ID] ID: " << newMakanan.id_makanan << endl;
    cout << "| [NAMA] Nama: " << newMakanan.namaMakanan << endl;
    cout << "| [HARGA] Harga: Rp" << newMakanan.harga << endl;
    cout << "+-----------------------------------------------------------+" << endl;
    cout << "[CONFIRM] Tambahkan makanan ini? (y/n): ";
    char konfirmasi;
    cin >> konfirmasi;

    if (konfirmasi == 'y' || konfirmasi == 'Y')
    {
        // Add to tree
        addMakananToTree(&currentMerchant->treeMakanan, newMakanan);

        // Add to popular menu stack (merchant can decide later which items are popular)
        pushPopularMenu(&currentMerchant->popularMenu, newMakanan);

        cout << "\n+============================================================+" << endl;
        cout << "|                [SUCCESS] MAKANAN BERHASIL DITAMBAHKAN!    |" << endl;
        cout << "+============================================================+" << endl;
        cout << "| [ADDED] " << newMakanan.namaMakanan << endl;
        cout << "| [ID] ID: " << newMakanan.id_makanan << endl;
        cout << "| [PRICE] Harga: Rp" << newMakanan.harga << endl;
        cout << "+============================================================+" << endl;
        cout << "| [INFO] Total makanan: " << currentMerchant->treeMakanan.size << endl;
        cout << "| [TIP] Makanan otomatis ditambahkan ke menu populer        |" << endl;
        cout << "+============================================================+" << endl;
    }
    else
    {
        cout << "\n[CANCEL] Penambahan makanan dibatalkan!" << endl;
    }

    cout << "\n[BACK] Tekan Enter untuk kembali...";
    cin.ignore();
    cin.get();
}

void displayOrders(orderQueue *queue)
{
    cout << "\n+============================================================+" << endl;
    cout << "|                   [ORDER] DAFTAR PESANAN                  |" << endl;
    cout << "+============================================================+" << endl;

    if (queue->front == nullptr)
    {
        cout << "| [EMPTY] Tidak ada pesanan saat ini                        |" << endl;
        cout << "+============================================================+" << endl;
        return;
    }

    orderNode *current = queue->front;
    int count = 1;

    while (current != nullptr)
    {
        cout << "+-----------------------------------------------------------+" << endl;
        cout << "| " << count << ". [ORDER ID] " << current->data.id_pesanan << endl;
        cout << "| [CUSTOMER] " << current->data.nama_penumpang
             << " (ID: " << current->data.id_penumpang << ")" << endl;
        cout << "| [FOOD] " << current->data.nama_makanan << endl;
        cout << "| [PRICE] Rp" << current->data.harga << endl;
        cout << "| [STATUS] " << current->data.status << endl;
        cout << "| [COURIER] " << current->data.kurir_id << endl;
        cout << "| [TIME] " << current->data.created_time << endl;

        // Status indicator
        if (current->data.status == "pending")
        {
            cout << "| [ACTION] >> PERLU DIPROSES <<" << endl;
        }
        else if (current->data.status == "ready")
        {
            cout << "| [ACTION] >> SIAP DIAMBIL KURIR <<" << endl;
        }
        else if (current->data.status == "completed")
        {
            cout << "| [ACTION] >> PESANAN SELESAI <<" << endl;
        }

        current = current->next;
        count++;
    }

    cout << "+-----------------------------------------------------------+" << endl;
    cout << "| [TOTAL] Total Pesanan: " << queue->size << endl;
    cout << "+============================================================+" << endl;

    // Show status summary
    int pendingCount = 0, readyCount = 0, completedCount = 0;
    current = queue->front;

    while (current != nullptr)
    {
        if (current->data.status == "pending")
            pendingCount++;
        else if (current->data.status == "ready")
            readyCount++;
        else if (current->data.status == "completed")
            completedCount++;
        current = current->next;
    }

    cout << "\n[SUMMARY] Pending: " << pendingCount
         << " | Ready: " << readyCount
         << " | Completed: " << completedCount << endl;
}

void updateOrderStatus(orderQueue *queue, string orderId, string newStatus)
{
    if (queue->front == nullptr)
    {
        cout << "[X] Tidak ada pesanan untuk diupdate!" << endl;
        return;
    }

    orderNode *current = queue->front;
    bool found = false;

    while (current != nullptr)
    {
        if (current->data.id_pesanan == orderId)
        {
            string oldStatus = current->data.status;
            current->data.status = newStatus;
            found = true;

            cout << "\n+============================================================+" << endl;
            cout << "|               [UPDATE] STATUS BERHASIL DIUPDATE!          |" << endl;
            cout << "+============================================================+" << endl;
            cout << "| [ORDER ID] " << orderId << endl;
            cout << "| [CUSTOMER] " << current->data.nama_penumpang << endl;
            cout << "| [FOOD] " << current->data.nama_makanan << endl;
            cout << "| [OLD STATUS] " << oldStatus << endl;
            cout << "| [NEW STATUS] " << newStatus << endl;
            cout << "+============================================================+" << endl;

            // Give appropriate message based on new status
            if (newStatus == "ready")
            {
                cout << "| [NOTIF] Kurir akan segera mengambil pesanan               |" << endl;
                cout << "| [ACTION] Siapkan pesanan untuk pickup                     |" << endl;
            }
            else if (newStatus == "completed")
            {
                cout << "| [NOTIF] Pesanan telah selesai                             |" << endl;
                cout << "| [SUCCESS] Terima kasih atas pelayanan yang baik!          |" << endl;
            }
            cout << "+============================================================+" << endl;
            break;
        }
        current = current->next;
    }

    if (!found)
    {
        cout << "\n[X] Pesanan dengan ID '" << orderId << "' tidak ditemukan!" << endl;
        cout << "[TIP] Periksa kembali ID pesanan yang dimasukkan." << endl;
    }
}