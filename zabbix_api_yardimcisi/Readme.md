# Zabbix Api Yardımcısı

## Neyi amaçlıyorum

**Bu projenin amacı, Zabbix API'sini kullanarak program geliştirmek isteyen veya Zabbix ile yeni tanışmış olan kullanıcılara yardımcı olmaktır.** 
**Proje ayrıca sürekli olarak Zabbix API'sini kullanan kullanıcılara da destek sağlamayı amaçlamaktadır. Tekrar eden kodları sürekli olarak yazmak yerine,** 
**hazır yapılar sunarak zaman tasarrufu sağlamayı hedeflemektedir. Böylece tüm API anahtar kelimelerini hatırlamak veya sürekli hatırlayabileceğinize güvenmek zorunda kalmazsınız.**

## Genel yapı
### Frontend
  - Kullanıcı dostu olması açısından ve terminalde çalışabilmesi hedeflendiği için cpp dili ile bir panellerden oluşan bir arayüz yapılmıştır seçenekler seçebildiğiniz ve seçenekler arasında ok tuşları ile hareket edebildiğiniz bir yapı.
### API sorguları
  - Zabbix API ile haberleştirilen kodlar go dili ile yazılmış cpp içerisinde argümanlarla çalışabililen scriptler olarak kullanılmıştır. Amaç daha sonradan bir web arayüzü yapılmak istenirse aynı dosyalar aynı parametrelerle kullanılabilir durumdadır.
### Veritabanı
- Veritabanımız PostgreSQL kullanılarak oluşturulmuştur, boyutu küçük olsa da tercih edilen bir seçenektir. Panellerde kullanılacak veriler bu veritabanında saklanacaktır. Veritabanıyla ilgili kodlar models klasörü içerisinde bulunmaktadır.
#### Veri Toplama
- **Zabbix API Verileri:** Zabbix API dokümantasyonunu tarayarak, mevcut olan methodlar ve method parametrelerini içeren verileri toplarız.
- **Brute-Force Yöntemiyle Alınan Veriler:** Şu anda sadece çıktı parametreleri için argümanları almak amacıyla oluşturulan brute-force yöntemiyle elde edilen veriler depolanır. Bu, mevcut argümanları tespit etmek için bir kod parçası kullanarak gerçekleştirilir.

## İşleyiş
- Zabbix API Yardımcısı, kullanıcının istediği hostlara ait bilgileri hangi methodlar, parametreler ve parametre argümanlarını kullanılarak görüntülemek istediğini sırasıyla sorarak başlar. Kullanıcı seçenekleri belirledikten sonra, bu seçenekleri kullanarak çalışan programlardan oluşan bir shell script dosyası oluşturulur.
- Bu shell script dosyasını çalıştırdığınızda, seçtiğiniz özelliklere bağlı olarak istenilen çıktıyı elde edebilirsiniz. Böylece, tekrar tekrar aynı işlemi yazma zorunluluğundan kurtulmuş olursunuz. Her çalıştırdığınızda, seçtiğiniz özelliklere göre **ANLIK** bir çıktı alırsınız.
- Bu şekilde, hazır scriptler elde ederek Zabbix API'sini daha verimli bir şekilde kullanmayı amaçlamaktadır. Kullanıcılar, API çağrılarıyla ilgili detaylara dikkat etmek zorunda kalmadan, hızlı ve kolay bir şekilde istedikleri sonuçlara ulaşabilirler.
- Şuan ekli olmasada çıktı türleri olarak parametreli bir şekilde isterseniz csv isterseniz json tarzında çıktılar alınabilmesi sağlanacaktır. Default olarak okuması kolay olacak bir biçimde tablo olarak görüntülenir.

** !!! Şuanda sadece tüm methodlar (tek tek bir liste olarak eklenecek) ,4 parametre hostid itemid output limit kendi içerisindeki argümanlarıyla işlenmiş durumdadır.** 

## Nasıl kullanılır
Öncellikle kurulumun bir çok aşamasını otomatikleştirebilmek için make firstMake komutu ile ilerleyeceğiz.
make firstMake komutunda bulunmayan 2 temel şeyimiz vardır.
- [Postgresql kurulumu](https://www.digitalocean.com/community/tutorials/how-to-install-postgresql-on-ubuntu-20-04-quickstart)
- [Go kurulumu ](https://www.digitalocean.com/community/tutorials/how-to-install-go-on-ubuntu-20-04)
- Makefike kurulumu

Kurulumlarının yapılması gerekir
  
```
make --version

Çıktı:
GNU Make 4.2.1
Built for x86_64-pc-linux-gnu
Copyright (C) 1988-2016 Free Software Foundation, Inc.
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.
```
Buna benzer bir çıktı alamıyorsanız öncellikle make komutunu kullanılabilir hale getirmek gerekir
```
sudo apt install make
```

Komutunu çalıştırınız.
Daha sonrasında 

### Panel ile kullanım
```
make firstMake
make
./run_program/panel
```
komutlarını çalıştırırak uygulamayı kullanabiliriz ürettiğimiz sh dosyaları output_scripts içerisinde listelenecektir.

### get_items programı ile kullanım
- Bu şekildeki bir kullanımda her zaman ilk parametre method:X parametresi olmalıdır. Diğer parametrelerin sırası önemli değildir.
          
```
./run_program/get_items hostid:"10084" itemids:"45509,44056" output:"description,status" # 10084 hostid ye sahip hostuna ait 45509,44056 itemid lerine ait description ve status özellikleri ile getirir
./run_program/get_items hostid:"" itemids:"" output:"name,description,lastvalue,status" # var olan tüm hostlara ait tüm itemleri sadece name ,description ,lastvalue ve status özellikleri ile getirir
./run_program/get_items hostid:"" itemids:"" output:"" # var olan tüm hostlara ait tüm itemleri tüm output parametreleriyle getirir
./run_program/get_items method:host.get output:hostid,host,name,status

# tüm get yöntemleri için çlaışır durumdadır (Bazı get mothadları için özel durumlar olabilir. ) (Kendi sitesinde var olan bilgiler yakın zamanda güncellenmiş olup. Kendileri bazı hataları gidermişler. Değişen herşeyi kontrol etme şansım ne yazıkki bulunmuyor şuanlık.)
(Panellerde veya daha sonradan oluşturacağım sql tablolarından doğru parametreleri alabilirsiniz) ()

./run_program/get_items method:trigger.get output:description hostid:10084

./run_program/get_items method:history.get hostid:10084 limit:12 itemids:23662

./run_program/get_items method:trigger.get output:event_name limit:10
... gibi örnekleri çoğaltmak mümkündür
```

### Resimlerle adım adım bir Panel kullanım örneği
            ****EKLENECEK****

# Geliştiriciler için notlar
- Bu aşamayı programın geliştirmek istediğim adımlarını, aşamalarını yazarak ilerleyeceğim.
  
## Buglar
- Eğer terminal boyutu belirli miktarın altında olursa seçenekler tam gözükmeyebiliyor. **(Kütüphane kaynaklı problem)**
- En alttaki seçenekte "Devam" yazmasa bile seçtiğinizde Sanki devam seçilmiş gibi davranıyor **(YAPILDI)**

## Problemler
- /var/log/zabbix_api_yardimcisi/all.log içerisinde veriler tutulur fakat get_items içerisinde var olan errorler doğru şekilde yazdırılamıyor. 
  
## Eklenecek/Değiştirilecek şeyler
### Postgresql ile ilgili eklenecek şeyler
- İlk çalıştırmada bu programa özel bir postgresql kullanıcısı ve diğer lazım olan yapıları ekleyecek kodlar eklenecek. **(YAPILDI)** 
- Şuanda item_alt_basliklar.txt de depolanan veri postgresql kullanılarak depolanacak. **(YAPILDI)**
- Veritabanını, program her çalıştırıldığında güncellenecek şekilde bir kodda eklenecek. **(KISMEN YAPILDI)**
- Config dosyası eklenecek. **(YAPILDI)**
- Diğer parametreler ve parametrelere ait argümanlar farklı paneller olarak eklenecek. **(YAPILDI)** **(EFEKTİF DEĞİL)**
- Error handling işlemleri postgresql içerisinde yada /var/log dosyası içersinde oluşturulacak bir dosya içersinde saklanmalıdır. **(YAPILDI)** PANEL İÇİN SORUNSUZ GET_İTEMS İÇİN PROBLEMLİ
  
## Değiştirilecek şeyler
- Buglar düzeltiecek.
- Şuanda host_listeleme ve get_item farklı 2 dosya olarak bulunuyor. Bu sistem değiştirilip tek bir koda parametre verilecek şekilde değiştirilecek get_itemsin içindeki yapı buna uygun dizayn edildi. Diğer methodları ekleme noktası için öncelikle yapılması gerekilen bir şeydir. (**YAPILDI)**
- Çıktı formatı değiştirme get_itemsin içersindeki yapı farklı türde output vermememe izin verecek kütüphane ve fonksiyonlar kullanırak doğru bir şekilde dizayn edildi. **(YAPILDI)**
