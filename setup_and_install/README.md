# zabbix_hepler_api_and_install_setup

## Zabix kurulum öncesi static ip ayarlama

scp auto_static_ip_config_build.sh samet@192.168.1.68:/home/samet komutu ile indirdiğiniz dosyayı sunucunuza yükleyiniz.
scp komutu ile detaylı bilgiye https://linux.die.net/man/1/scp sitesinden ulaşabilirsiniz.

Eğer sunucu ip nizi bilmiyorsanız 
    ip a | grep -A2 "$Device" | grep "inet " | awk '{print($2)} 

komutu ile bakabilirsiniz

Eğer kendi evinizin ağını kullanıyorsanız sadece var olan ip nizin son değerini rasgele bir biçimde değiştirmeniz muhtemelen yeterli olacaktır.

Kullanabileceğiniz tüm ip aralığını öğrenmek isterseniz İP subnet calculator ü inceleyebilrsiniz.
https://www.calculator.net/ip-subnet-calculator.html?cclass=any&csubnet=30&cip=85.107.65.191&ctype=ipv4&printit=0&x=79&y=33

Kulanım:
    scp auto_static_ip_config_build.sh samet@192.168.1.68:/home/samet komutu ile indirdiğiniz dosyayı sunucunuza yükleyiniz.
    sudo bash auto_static_ip_config_build.sh
    # sizden static ip olarak belirlemek istediğiniz değeri girmenizi isteyecektir ( örnek 192.168.1.68/24)
    
### Static ip ayarlamak için Network Manager kullanma