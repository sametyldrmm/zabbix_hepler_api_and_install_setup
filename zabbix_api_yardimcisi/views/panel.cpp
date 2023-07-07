#include "panel.hpp"
#include "utils.hpp"

void zabbix_Api_I_O::Informations_app()
{
    secim_paneli(string(""), string("") + "Bu program, Zabbix API'sini kullanarak erisilebilecek verilere hizli erisim saglamak icin tasarlanmistir.\n" + "Bir dizi adimla kullaniciya secimler yaptirir.\n\n" + "Adim 1: Host Secimi\n" + "Bu adimda, görüntülemek istediginiz verilere ait olan hangi hostlari secmek istediginizi belirtmeniz gerekmektedir.\n\n" + "Adim 2: Method Secimi\n" + "Bu adimda, hangi yöntemlere ait verileri görüntülemek istediginizi secmeniz gerekmektedir.\n\n" + "Adim 3: Item Özellikleri Secimi\n" + "Bu adimda, sectiginiz itemlere ait hangi özelliklere bakmak istediginizi belirtmeniz gerekmektedir. Ayrica, bu verileri kaydetmek istediginizde hangi özelliklere bakip kaydetmek istediginiz de sorulur.\n\n" + "Adim 4: Item Listesi\n" + "Bu adimda, sectiginiz cikti parametrelerine göre itemler listelenir.\n\n" + "Adim 5: Özellik Secimi\n" + "Bu adimda, sectiginiz itemlerin hangi özelliklerini kaydetmek istediginizi belirtmeniz gerekmektedir.\n\n");
}

string zabbix_Api_I_O::get_panel_hosts()
{
    cout << "hostlari host id ,host , status ,name ve isletim sistemi olarak görüntülüyorsunuz: \n";
    string test = executeCommand("./run_program/get_items method:host.get output:hostid,host,name,status");
    vector<string> host_id = secim_paneli(test, string("hostlari host id ,host , status ,name başliklarinda olarak görüntülüyorsunuz:\n "));
    // host id yi yazdir
    string host_id_str = " hostid:";
    host_id_str = convert_argument_string(host_id, host_id_str);
    return host_id_str;
}

string join_rows(vector<string> &rows)
{
    string joined_rows = "";
    for (auto data : rows)
    {
        joined_rows += data + "\n";
    }
    return joined_rows;
}

vector<string> zabbix_Api_I_O::get_panel_methods()
{
    vector<string> data = GetAllColumnValues("all_methods", "method");
    // DB_table_data.DbLoadTableData("all_methods");
    return (secim_paneli(join_rows(data), string("Hangi tür verileri görmek istersiniz\n")));
}

string zabbix_Api_I_O::get_output_parameters(string method)
{
    string content;
    content = executeCommand(string("") + "./run_program/get_items" + " method:" + method + ".get " + "limit:1");
    // contentin içerisinden '\n' den sonrasını kaldırmalıyız
    content = content.substr(0, content.find('\n'));
    // tüm boşluk karakterlerini \n karakterine çevirelim
    replace(content.begin(), content.end(), ' ', '\n');
    return (content);
}

// string("itemleri kaydetmek icin output parametresi özelliklerini seciniz \n"
string zabbix_Api_I_O::get_panel_output_parameters(string baslik, string method)
{
    string output_str = " output:";
    string contents = get_output_parameters(method);
    // cout << contents << endl;
    this->output_parametresi = secim_paneli(contents, baslik);
    output_str = convert_argument_string(this->output_parametresi, output_str);
    return output_str;
}

void zabbix_Api_I_O::create_sh_script(string script_info)
{
    string sh_name;
    cout << "scripti kaydetmek istediginiz dosya adini giriniz sonuna .sh eklenecektir: ";
    cin >> sh_name;

    executeCommand(string("echo ") + "'" + "#!/bin/bash\n" + "'" + " >> " + "./output_scripts/" + sh_name + ".sh");
    executeCommand(string("echo ") + "'" + "./run_program/get_items" + script_info + "'" + " >> " + "./output_scripts/" + sh_name + ".sh");
}

void zabbix_Api_I_O::view_hosts()
{
    string host_id_str = get_panel_hosts();
    vector<string> get_values_method = get_panel_methods();

    std::string contents = "";
    for (int i = 0; i < get_values_method.size(); i++)
    {
        // cout << get_values_method[i] << endl;
        // tabo adım get_values_method
        // tabloada 2 sütun var method ve type
        // get_values_method[i] methodu seçiyor
        // type değerini alıcaz
        // Databasemanager ı kullanarak alalım

        // ilk başta aldığımız methodun parametrelerini seçelim
        vector<string> secilecek_method_parametreleri = GetAllColumnValues(get_values_method[i], "parameter");
        // bu parametrelerden hangilerini seçeceğimizi seçelim
        vector<string> secilecen_method_parametreleri = secim_paneli(join_rows(secilecek_method_parametreleri),get_values_method[i] + string("methoduna göre secmeniz gereken parametreler \n"));
        for ( auto temp_str : secilecen_method_parametreleri)
        {
            string type = this->GetColumnValue(get_values_method[i], "parameter", temp_str.c_str(), "type");
            // bu noktadan sonra type türün göre fonksiyon çağırılacak ve parametre argümanları alınacak burası için düşündüğüm şeylerden ilki boolen değerler için
            if(type == "boolean")
            {
                
            }
            if(type == "string/array" )
            {

            }
            if(type == "integer")
            {

            }
            if(type == "array of objects")
            {

            }
            if(type == "query")
            {

            }
            if(type == "object")
            {

            }
        }

        string output_str;
        // burada güzel bir sistem oturtmalıyız method param
        output_str = get_panel_output_parameters(string("itemleri görüntülemek icin output parametresi özelliklerini seciniz \n"), get_values_method[i]);
        cout << "./run_program/get_items" + host_id_str + output_str << "\n";
        string test2 = executeCommand(string() + "./run_program/get_items" + " method:" + get_values_method[i] + ".get" + host_id_str + output_str);
        // cout << test2 << endl;

        vector<string> isim = secim_paneli(test2, string("hazir bir script olarak kaydetmek istediginiz itemleri seciniz bir\n"));
        string itemids = " itemids:";
        itemids = convert_argument_string(isim, itemids);

        string test3 = executeCommand(string() + "./run_program/get_items" + " method:" + get_values_method[i] + host_id_str + output_str + itemids);
        cout << test3 << endl;

        output_str = get_panel_output_parameters(string("itemleri kaydetmek icin output parametresi özelliklerini seciniz \n"), get_values_method[i]);
        create_sh_script(string() + " method:" + get_values_method[i] + ".get" + host_id_str + output_str + itemids);
    }
}

zabbix_Api_I_O::zabbix_Api_I_O()
{
    // string type = this->GetColumnValue("item", "type", "boolean", "parameter");
    // SelectRows("item", "type", "boolean");
    // cout << type<<"\n";
    // GetTableNames();

    this->Informations_app();
    this->view_hosts();
}

zabbix_Api_I_O::~zabbix_Api_I_O()
{
}

int main()
{

    zabbix_Api_I_O zabbix_Api_I_O;
    
    return 0;
}

// 1 host seç
// 2 method seç
// 3 method parametrelerini seç
// 4 method parametrlerine uygun output parametrelerini seç
// 5 itemleri seç
// 6 method parametrelerini seç
// 7 method parametrlerine uygun output parametrelerini seç
// 8 scripti kaydet