# Voxelization 3D Object Program (Octree-based)

> Tugas Kecil 2 IF2211 Strategi Algoritma

## Deskripsi Singkat

Program ini melakukan voxelization terhadap model 3D berbasis struktur data **octree**. Program membaca file `.obj` dari folder `test/input`, kemudian membagi ruang menjadi voxel berdasarkan parameter **max depth**, dan menyimpan hasilnya ke folder `test/output`.

Program juga menyediakan visualisasi objek menggunakan SFML.

---

## Requirement

- CMake >= 3.10
- C++17 compatible compiler
- SFML >= 2.5 dan maksimal 2.x (tidak mendukung versi 3.x atau 1.x)

### Tested on

- GCC 13.3.0 (Linux / WSL)
- MinGW GCC 15.2.0 (Windows)

---

## Quick Start (Recommended)

Gunakan executable yang sudah disediakan.

```bash
cd bin
```

**Linux / WSL:**

```bash
./voxelizer <namafile>.obj <maxdepth>

# Contoh:
./voxelizer teapot.obj 4
```

**Windows (PowerShell):**

```powershell
.\voxelizer.exe <namafile>.obj <maxdepth>

# Contoh:
.\voxelizer.exe cow.obj 6
```

---

## Cara Kompilasi (Opsional)

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

Output akan berada di folder `build/`.

---

## Cara Menjalankan (Hasil Kompilasi Sendiri)

```bash
cd build
```

**Linux / WSL:**

```bash
./voxelizer <namafile>.obj <maxdepth>
```

**Windows (PowerShell):**

```powershell
.\voxelizer.exe <namafile>.obj <maxdepth>
```

---

## Catatan

- File `.obj` harus berada di folder `test/input/`
- Output voxel akan disimpan di `test/output/`
- Program harus dijalankan dari folder `bin/` atau `build/` agar path relatif bekerja dengan benar
- Hanya menerima file `.obj` dengan mesh berbentuk **segitiga (triangular faces)**; face dengan lebih dari 3 vertex tidak didukung

---

## Author

- 13524053 Muhammad Haris Putra Sulastianto
- 13524110 Jennifer Khang

Teknik Informatika, Institut Teknologi Bandung — 2026
