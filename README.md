# SISOP 2019 : Modul 4 [Soal Shift] Kelompok D8

## #Soal 1

Semua nama file dan folder harus terenkripsi
Enkripsi yang Atta inginkan sangat sederhana, yaitu Caesar cipher. Namun, Kusuma mengatakan enkripsi tersebut sangat mudah dipecahkan. Dia menyarankan untuk character list diekspansi,tidak hanya alfabet, dan diacak. Berikut character list yang dipakai:
```
qE1~ YMUR2"`hNIdPzi%^t@(Ao:=CQ,nx4S[7mHFye#aT6+v)DfKL$r?bkOGB>}!9_wV']jcp5JZ&Xl|\8s;g<{3.u*W-0
```

### Pembuatan


## #Soal 2

Semua file video yang tersimpan secara terpecah-pecah (splitted) harus secara otomatis tergabung (joined) dan diletakkan dalam folder “Videos”
Urutan operasi dari kebutuhan ini adalah:
* Tepat saat sebelum file system di-mount
  * Secara otomatis folder “Videos” terbuat di root directory file system
  * Misal ada sekumpulan file pecahan video bernama “computer.mkv.000”, “computer.mkv.001”, “computer.mkv.002”, dst. Maka secara otomatis file pecahan tersebut akan di-join menjadi file video “computer.mkv”
  * Untuk mempermudah kalian, dipastikan hanya video file saja yang terpecah menjadi beberapa file. File pecahan tersebut dijamin terletak di root folder fuse
  * Karena mungkin file video sangat banyak sehingga mungkin saja saat menggabungkan file video, file system akan membutuhkan waktu yang lama untuk sukses ter-mount. Maka pastikan saat akan menggabungkan file pecahan video, file system akan membuat 1 thread/proses(fork) baru yang dikhususkan untuk menggabungkan file video tersebut
  * Pindahkan seluruh file video yang sudah ter-join ke dalam folder “Videos”
  * Jangan tampilkan file pecahan di direktori manapun
* Tepat saat file system akan di-unmount
  * Hapus semua file video yang berada di folder “Videos”, tapi jangan hapus file pecahan yang terdapat di root directory file system
  * Hapus folder “Videos” 

### Pembuatan


## #Soal 3

Sebelum diterapkannya file system ini, Atta pernah diserang oleh hacker LAPTOP_RUSAK yang menanamkan user bernama “chipset” dan “ic_controller” serta group “rusak” yang tidak bisa dihapus. Karena paranoid, Atta menerapkan aturan pada file system ini untuk menghapus “file bahaya” yang memiliki spesifikasi:
  * Owner Name 	: ‘chipset’ atau ‘ic_controller’
  * Group Name	: ‘rusak’
  * Tidak dapat dibaca
Jika ditemukan file dengan spesifikasi tersebut ketika membuka direktori, Atta akan menyimpan nama file, group ID, owner ID, dan waktu terakhir diakses dalam file “filemiris.txt” (format waktu bebas, namun harus memiliki jam menit detik dan tanggal) lalu menghapus “file bahaya” tersebut untuk mencegah serangan lanjutan dari LAPTOP_RUSAK.

### Pembuatan

## #Soal 4

Pada folder YOUTUBER, setiap membuat folder permission foldernya akan otomatis menjadi 750. Juga ketika membuat file permissionnya akan otomatis menjadi 640 dan ekstensi filenya akan bertambah “.iz1”. File berekstensi “.iz1” tidak bisa diubah permissionnya dan memunculkan error bertuliskan “File ekstensi iz1 tidak boleh diubah permissionnya.”

### Pembuatan


## #Soal 5

Ketika mengedit suatu file dan melakukan save, maka akan terbuat folder baru bernama Backup kemudian hasil dari save tersebut akan disimpan pada backup dengan nama namafile_[timestamp].ekstensi. Dan ketika file asli dihapus, maka akan dibuat folder bernama RecycleBin, kemudian file yang dihapus beserta semua backup dari file yang dihapus tersebut (jika ada) di zip dengan nama namafile_deleted_[timestamp].zip dan ditaruh ke dalam folder RecycleBin (file asli dan backup terhapus). Dengan format [timestamp] adalah yyyy-MM-dd_HH:mm:ss

### Pembuatan
* Menggunakan fungsi write dan unlink
  Write berfungsi untuk:
      * Membuat folder `Backup` jika belum ada
      * Meletakkan salinan file yang diupdate kedalam folder `Backup`
      * Menyimmpan file perubahan

```c
static int xmp_write(const char *path, const char *buf, size_t size,
             off_t offset, struct fuse_file_info *fi)
{
    int fd;
    int res;
    char fpath[1000], fpathbaru[1000], folderbaru[1000], fpathtmp[1000];
    char nameenc[1000], name_tmp[1000];
    
    char folder[] = "BACKUP";
    time_t now = time(NULL);
    struct tm wkt_now;
    struct stat st;
    pid_t pid;

    sprintf(nameenc,"%s",path);
    encrypt(nameenc);
    strcpy(name_tmp, nameenc);
    printf("NAME ENC%s\n", nameenc);
    sprintf(fpath, "%s/%s",dirpath,nameenc);
    printf("FPATH %s\n", fpath);

    fd = open(fpath, O_WRONLY);
    if (fd == -1)
        return -errno;
    res = pwrite(fd, buf, size, offset);
    if (res == -1)
        return -errno;

    close(fd);

    decrypt(nameenc);
    wkt_now = *localtime(&now);
    char new[1000], newpath[1000];
    sprintf(new, "%s_%d-%d-%d_%d:%d:%d.ekstensi", nameenc, wkt_now.tm_year+1900, wkt_now.tm_mon, wkt_now.tm_mday, wkt_now.tm_hour, wkt_now.tm_min, wkt_now.tm_sec);
    printf("NEW NAME %s\n", new);
    encrypt(new);

    sprintf(fpathbaru, "%s/%s", dirpath, new);
    sprintf(fpathtmp, "%s", fpath);
    
    encrypt(folder);
    sprintf(folderbaru, "%s/%s", dirpath, folder);
    if(stat(folderbaru, &st)<0){
        mkdir(folderbaru, 0755);
        printf("BUAT FOLDER %s\n", folderbaru);
    }
    printf("\nfpathbaru %s \n\nFOLDER BARU %s\n", fpathbaru, folderbaru);
    if((pid=fork())==0){
        sprintf(newpath, "%s/%s", folderbaru, new);
        char *argv[] = {"cp", fpathtmp, newpath, NULL};
        if((execv("/bin/cp", argv))<0){
            printf("GAGAL CP %s\n", folderbaru);
        }
    }else{
        printf("\nSETELAH CP ke %s\n", folderbaru);
    }

    return res;
}
```
    Unlink berfungsi untuk:
        * Membuat folder `RecycleBin` jika belum ada
        * Mengkompress file dan file backup menjadi zip dan dimasukkan ke folder `RecycleBin`
        * Menghapus file asli
```c
static int xmp_unlink(const char *path)
{
    int res;
    char fpath[1000];
    char nameenc[1000], folder[] = "RecycleBin", *folderbaru = NULL;

    struct stat st;
    sprintf(nameenc,"%s",path);
    encrypt(nameenc);
    sprintf(fpath, "%s%s",dirpath,nameenc);


    encrypt(folder);
    sprintf(folderbaru, "%s/%s", dirpath, folder);
    if (stat(folderbaru, &st) < 0)
    {
        mkdir(folderbaru, 0755);
        printf("BUAT FOLDER %s\n", folderbaru);
    }

    res = unlink(fpath);
    if (res == -1)
        return -errno;

    return 0;
}
```
