
# Zabbix Kurulumu Öncesi Tavsiyeler

- Öncellikle kendi bilgisayarınıza kurmanız önerilmez. İleride yapacağınız konfigürasyonlar dolayısıyla bazı sistemleriniz çalışmayabilir.
- Önerilen yöntem, bir sanal bilgisayara Ubuntu 22.04 Server kurulumu yapmak ve ardından Zabbix kurulumuna devam etmektir.
- Sunucumuzun network ayarını bridge yapmamız gerekir.

## Zabbix Kurulum Öncesi Static IP Ayarlama

Zabbix dökümanlarında açık bir şekilde static IP yapılandırılmasının yapılması tavsiye edilmektedir. Aşağıdaki adımları izleyerek static IP ayarlaması yapabilirsiniz:

1. İlk olarak, [bu adresten](https://github.com/sametyldrmm/zabbix_heplers_api_and_install_setup.git) indirdiğiniz dosyaları sunucunuza yükleyin.
2. Terminal üzerinden `scp` komutunu kullanarak dosyaları sunucunuza kopyalayabilirsiniz. Örneğin:
```
scp auto_static_ip_config_build.sh SUNUCU_KULLANICI@sunucu_ip:SUNUCU_DOSYA_YOLU # komutu ile indirdiğiniz dosyayı sunucunuza yükleyiniz.
```
Burada "SUNUCU_KULLANICI" kısmını sunucunuzdaki kullanıcı adıyla, "sunucu_ip" kısmını sunucunuzun IP adresiyle ve "SUNUCU_DOSYA_YOLU" kısmını dosyanın sunucudaki hedef yolunu belirleyerek değiştirmelisiniz.

Burada "SUNUCU_DOSYA_YOLU" kısmını gerçek dosya yolunuza göre değiştirmelisiniz.

3. Sunucuda dosyaları kopyaladıktan sonra, terminal üzerinden dosya konumuna gidin:

4. Aşağıdaki komutu kullanarak static IP ayarlama betiğini çalıştırın:
```
    sudo bash auto_static_ip_config_build.sh
``` 
Bu komut çalıştırıldığında, size static IP olarak belirlemek istediğiniz değeri girmenizi isteyecektir (örneğin, "192.168.1.68/24").

Not: Eğer kendi evinizin ağını kullanıyorsanız, mevcut IP'nizin son değerini rasgele bir biçimde değiştirmeniz genellikle yeterli olacaktır.
#### Eklenecek (28 Haziran)
- Eğer daha önceden ip yapılandırması yaptıysanız ve artık farklı bir ağda çalışmanız gerekiyorsa elimizdeki script işe yaramaz. Bunun önüne geçebilmek için defaulta dönme scripti ekleyeceğim.
### Static IP Ayarlamak için Network Manager Kullanma

Burada, Network Manager aracılığıyla static IP ayarlama işlemi de yapabilirsiniz. Bu yöntem için ayrıntılı adımları [bu linkten](https://linux.die.net/man/1/nmcli) inceleyebilirsiniz.

# Zabbix Kurulumu

- Bu bölümde, Zabbix kurulumunu gerçekleştirmek için bir sh dosyası kullanacağız. Bu dosya, [Zabbix indirme](https://www.zabbix.com/download?zabbix=6.4&os_distribution=ubuntu&os_version=22.04&components=server_frontend_agent&db=pgsql&ws=nginx) web sitesindeki komutları içermektedir.
- Buradaki steden tek farkı php8.1 yerine php8.2 yükleniyor olmasıdır. Eğer apache kurulumu yapacaksanız php8.1 tavsiye edilendir. Ngnix kurduğumuz için 8.2 kurmak hata çıkmaması için tercih edilmesi gerekendir.
- Postgresql kurulumu yapılacaktır.

**Not:** Bu dosya sadece işi hızlandırmak için kullanılmaktadır. Daha fazla özelleştirme yapmak isterseniz, dosyalar içerisindeki ilgili yerleri değiştirmeniz gerekmektedir.

1. İlk olarak, `script.sh` dosyasını [buradan](https://github.com/sametyldrmm/zabbix_heplers_api_and_install_setup.git) indirin veya sunucunuza kopyalayın.
2. Terminal üzerinden dosyanın bulunduğu konuma gidin.
3. Aşağıdaki komutu kullanarak Zabbix kurulumunu başlatın:
``` 
sudo bash script.sh
``` 
Bu komutla, Zabbix'in kurulumunun bir kısmı gerçekleştirilecektir.

4. Kurulum tamamlandıktan sonra, yapılandırma dosyalarını düzenlemek için aşağıdaki adımları takip edin:

- `/etc/zabbix/zabbix_server.conf` dosyasında `DBPassword` değerini istediğiniz bir parola ile değiştirin.
- `/etc/zabbix/nginx.conf` dosyasında `listen` ve `server_name` değerlerini istediğiniz şekilde düzenleyin. Örneğin, `listen 8080;` ve `server_name example.com;` gibi.

**Not:** server_name önemli bir veri değildir daha sonrasında çoğunlukla sunucu ip si üzerinden bağlanma işlemi yapılacak.

5. Son olarak, aşağıdaki komutu kullanarak Zabbix bileşenlerini etkinleştirin ve yeniden başlatın:

``` 
systemctl restart zabbix-server zabbix-agent nginx php8.2-fpm
systemctl enable zabbix-server zabbix-agent nginx php8.2-fpm
``` 
Bu adımları izleyerek Zabbix kurulumunu gerçekleştirebilirsiniz. 
