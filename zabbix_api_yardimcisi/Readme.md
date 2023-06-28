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

## İşleyiş
- Zabbix API Yardımcısı, kullanıcının istediği hostlara ait bilgileri hangi methodlar, parametreler ve parametre argümanlarını kullanılarak görüntülemek istediğini sırasıyla sorarak başlar. Kullanıcı seçenekleri belirledikten sonra, bu seçenekleri kullanarak çalışan programlardan oluşan bir shell script dosyası oluşturulur.
- Bu shell script dosyasını çalıştırdığınızda, seçtiğiniz özelliklere bağlı olarak istenilen çıktıyı elde edebilirsiniz. Böylece, tekrar tekrar aynı işlemi yazma zorunluluğundan kurtulmuş olursunuz. Her çalıştırdığınızda, seçtiğiniz özelliklere göre **ANLIK** bir çıktı alırsınız.
- Bu şekilde, hazır scriptler elde ederek Zabbix API'sini daha verimli bir şekilde kullanmayı amaçlamaktadır. Kullanıcılar, API çağrılarıyla ilgili detaylara dikkat etmek zorunda kalmadan, hızlı ve kolay bir şekilde istedikleri sonuçlara ulaşabilirler.
- Şuan ekli olmasada çıktı türleri olarak parametreli bir şekilde isterseniz csv isterseniz json tarzında çıktılar alınabilmesi sağlanacaktır. Default olarak okuması kolay olacak bir biçimde tablo olarak görüntülenir.

**!!! Şuanda sadece 1 method(get_item) ,3 parametresi (hostid itemid output) kendi içerisindeki argümanlarıyla işlenmiş konumdadır.** 

## Nasıl kullanılır
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
```
make
./run_program/panel
```
komutlarını çalıştırırak uygulamayı kullanabiliriz ürettiğimiz sh dosyaları output_scripts içerisinde listelenecektir.

Ayrıca panel kullanmadan api istek scriptlerini kullanmak istersek.
```
./run_program/hosts_listele # tüm var olan hostları host id ,host , status ,name ve isletim sistemi olrak görüntüler 

./run_program/get_items hostid:"10084" itemids:"45509,44056" output:"description,status" # 10084 hostid ye sahip hostuna ait 45509,44056 itemid lerine ait description ve status özellikleri ile getirir
./run_program/get_items hostid:"" itemids:"" output:"name,description,lastvalue,status" # var olan tüm hostlara ait tüm itemleri sadece name ,description ,lastvalue ve status özellikleri ile getirir
./run_program/get_items hostid:"" itemids:"" output:"" # var olan tüm hostlara ait tüm itemleri tüm output parametreleriyle getirir
```

### Resimlerle adım adım bir kullanım örnek
            ****EKLENECEK****
## Örnek kullanım alanına bağlı olarak kod yazımı ve hazır parse göre kullanım örneği
            ****EKLENECEK****
## Buglar
- Eğer terminal boyutu belirli miktarın altında olursa seçenekler tam gözükmeyebiliyor. (Kütüphane kaynaklı problem)
- En alttaki seçenekte "Devam" yazmasa bile seçtiğinizde Sanki devam seçilmiş gibi davranıyor -Çözüldü

## Eklenecek/Değiştirilecek şeyler
### Postgresql ile ilgili eklenecek şeyler
- İlk çalıştırmada bu programa özel bir postgresql kullanıcısı ve diğer lazım olan yapıları ekleyecek kodlar eklenecek.
- Bir postgresql yapısı içerisinde (config dosayasında postgresqle ait kullanılması gerekilen bilgiler için bloklar eklenecek) verileri düzenli aralıklarla çalıştırılıp eklenmesini sağlayacak kod blokları eklenecek. (crontab ı düzenleyecek kod blokları eklenecek)
- Şuanda item_alt_basliklar.txt de depolanan veri postgresql kullanılarak depolanacak. Program her çalıştırıldığında güncellenecek şekilde bir kodda eklenecek.
- Config dosyası eklenecek.
- Diğer parametreler ve parametrelere ait argümanlar farklı paneller olarak eklenecek.
- Error handling işlemleri postgresql içerisinde yada /var/log dosyası içersinde oluşturulacak bir dosya içersinde saklanmalıdır.
- Bir branch oluşturulup sadece en son çalışan verisyonun githubda görüntülenmesi sağlanacak.
  
## Değiştirilecek şeyler
- Buglar düzeltiecek.
- Şuanda host_listeleme ve get_item farklı 2 dosya olarak bulunuyor. Bu sistem değiştirilip tek bir koda parametre verilecek şekilde değiştirilecek get_itemsin içindeki yapı buna uygun dizayn edildi. Diğer methodları ekleme noktası için öncelikle yapılması gerekilen bir şeydir. (YAPILDI FAKAT PANELDE DEĞİŞİKLİK YAPILMADI PANELDE DEĞİŞİKLİK YAPILNCA SİLİNECEK)
- Çıktı formatı değiştirme get_itemsin içersindeki yapı farklı türde output vermememe izin verecek kütüphane ve fonksiyonlar kullanırak doğru bir şekilde dizayn edildi. (PLAN YAPILDI)

