#include "panel.hpp"
#include "utils.hpp"


void zabbix_Api_I_O::Informations_app()
{
secim_paneli(string(""),string("") + "Bu program, Zabbix API'sini kullanarak erisilebilecek verilere hizli erisim saglamak icin tasarlanmistir.\n"
     + "Bir dizi adimla kullaniciya secimler yaptirir.\n\n"
     + "Adim 1: Host Secimi\n"
     + "Bu adimda, görüntülemek istediginiz verilere ait olan hangi hostlari secmek istediginizi belirtmeniz gerekmektedir.\n\n"
     + "Adim 2: Method Secimi\n"
     + "Bu adimda, hangi yöntemlere ait verileri görüntülemek istediginizi secmeniz gerekmektedir.\n\n"
     + "Adim 3: Item Özellikleri Secimi\n"
     + "Bu adimda, sectiginiz itemlere ait hangi özelliklere bakmak istediginizi belirtmeniz gerekmektedir. Ayrica, bu verileri kaydetmek istediginizde hangi özelliklere bakip kaydetmek istediginiz de sorulur.\n\n"
     + "Adim 4: Item Listesi\n"
     + "Bu adimda, sectiginiz cikti parametrelerine göre itemler listelenir.\n\n"
     + "Adim 5: Özellik Secimi\n"
     + "Bu adimda, sectiginiz itemlerin hangi özelliklerini kaydetmek istediginizi belirtmeniz gerekmektedir.\n\n");
}

string zabbix_Api_I_O::get_panel_hosts()
{
    cout << "hostlari host id ,host , status ,name ve isletim sistemi olarak görüntülüyorsunuz: \n";
    string test = executeCommand("./run_program/get_items method:host.get output:hostid,host,name,status");
    vector<string> host_id = secim_paneli(test,string("hostlari host id ,host , status ,name başliklarinda olarak görüntülüyorsunuz:\n "));
    // host id yi yazdir
    string host_id_str = " hostid:";
    host_id_str = convert_argument_string(host_id,host_id_str);
    return host_id_str;
}

vector<string> zabbix_Api_I_O::get_panel_methods()
{
    return(secim_paneli(string("items\n") + "triger\n" ,string("Hangi tür verileri görmek istersiniz\n")));
}

string zabbix_Api_I_O::get_output_parameters()
{
    string contents = "";
    std::ifstream file("item_alt_basliklar.txt");
    if (!file.is_open()) {
        std::cout << "Dosya acilamadi." << std::endl;
        exit(EXIT_FAILURE);
    }
    std::stringstream buffer;
    buffer << file.rdbuf(); // Dosyanin tamamini buffer'a oku
    contents = contents + buffer.str();
    file.close();
    return contents;
}

// string("itemleri kaydetmek icin output parametresi özelliklerini seciniz \n"
string zabbix_Api_I_O::get_panel_output_parameters(string baslik)
{
    string output_str = " output:";
    string contents = get_output_parameters();
    this->output_parametresi = secim_paneli(contents,baslik);
    output_str = convert_argument_string(this->output_parametresi,output_str);
    return output_str;
}

void zabbix_Api_I_O::create_sh_script(string script_info)
{
    string sh_name;
    cout << "scripti kaydetmek istediginiz dosya adini giriniz sonuna .sh eklenecektir: ";
    cin >> sh_name;
    executeCommand(string ("echo ") + "'" + "./run_program/get_items" + script_info +"'" + " > " +"./output_scripts/"+sh_name +".sh");
}

void zabbix_Api_I_O::view_hosts()
{
    string host_id_str = get_panel_hosts();
    vector<string> get_values_type = get_panel_methods();
    
    std::string contents = "";
    for (int i = 0; i < get_values_type.size(); i++)
    {
        cout << get_values_type[i] << endl;
        if(get_values_type[i] == "items")
        {
            string output_str ;
            
            output_str = get_panel_output_parameters(string("itemleri görüntülemek icin output parametresi özelliklerini seciniz \n"));
            cout << "./run_program/get_items" + host_id_str + output_str << "\n";
            string test2 = executeCommand("./run_program/get_items" + host_id_str + output_str );
            // cout << test2 << endl;
            
            vector<string> isim = secim_paneli(test2,string("hazir bir script olarak kaydetmek istediginiz itemleri seciniz bir\n") );
            string itemids = " itemids:";
            itemids = convert_argument_string(isim,itemids);
            
            string test3 = executeCommand("./run_program/get_items" + host_id_str + output_str + itemids );
            cout << test3 << endl;
            
            output_str = get_panel_output_parameters(string("itemleri kaydetmek icin output parametresi özelliklerini seciniz \n"));
            create_sh_script(host_id_str + output_str + itemids);
        }
    }
}

zabbix_Api_I_O::zabbix_Api_I_O()
{
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