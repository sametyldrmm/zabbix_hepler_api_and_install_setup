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
    string host_id_str = " hostids:";
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

void zabbix_Api_I_O::create_sh_script()
{
    string sh_name;
    cout << "scripti kaydetmek istediginiz dosya adini giriniz sonuna .sh eklenecektir: ";
    cin >> sh_name;
    executeCommand(string("echo ") + "'" + "#!/bin/bash\n" + "'" + " >> " + "./output_scripts/" + sh_name + ".sh");

    for ( auto method : this->methods)
    {
        cout << string("echo ") + "'" + this->path_get_items + this->executeCommandsMap[method] + "'" + " >> " + "./output_scripts/" + sh_name + ".sh" << endl;
        executeCommand(string("echo ") + "'" + this->path_get_items + this->executeCommandsMap[method] + "'" + " >> " + "./output_scripts/" + sh_name + ".sh");
    }
}

void zabbix_Api_I_O::view_hosts()
{
    this->host_id_str = get_panel_hosts();
}

void zabbix_Api_I_O::viewAndGetMethods()
{
    this->methods = get_panel_methods();
}

void zabbix_Api_I_O::viewAndGetMethodParameters(string method)
{
    vector<string> secilecek_method_parametreleri = GetAllColumnValues(method, "parameter");
    // bu parametrelerden hangilerini seçeceğimizi seçelim
    vector<string> secilecen_method_parametreleri = secim_paneli(join_rows(secilecek_method_parametreleri),method + string("methoduna göre secmeniz gereken parametreler \n"));
    // seçilen parametreleri map öğemize ekleyelim
    this->method_parametresi[method] = secilecen_method_parametreleri;
}

void zabbix_Api_I_O::viewAndGetMethodsParametersArguments(string method)
{
    vector<string> secilecen_method_parametreleri = this->method_parametresi[method];
    string method_execute_str = " method:" + method + ".get";
    for ( auto temp_str : secilecen_method_parametreleri)
    {
        string type = this->GetColumnValue(method, "parameter", temp_str.c_str(), "type");
        // bu noktadan sonra type türün göre fonksiyon çağırılacak ve parametre argümanları alınacak burası için düşündüğüm şeylerden ilki boolen değerler için
        if(type == "boolean")
        {
           vector<string> temp = secim_paneli("true\nfalse", "sectiginiz parametre icin boolean bir deger girlilmesi gerekmektedir \n");
            temp_str = " "+ temp_str + ":" + temp[0];
        }
        else if(type == "string/array" )
        {
            temp_str = panel_user_input(" sectiginiz parametre icin string bir deger girlilmesi gerekmektedir \n Panell daha sonra gelistirilecektir \n suan icin string giriniz \n Kontroller eksiktir lütfen düzgün deger giriniz \n ','degerlerinizi karakteri ile ayiriniz \n");
            temp_str = " " + temp_str + ":" + temp_str;
        }
        else if(type == "integer")
        {
            int temp = atoi( panel_user_input(" sectiginiz parametre icin integer bir deger girlilmesi gerekmektedir \n Kontroller eksiktir lütfen düzgün deger giriniz \n").c_str());
            temp_str = " " + temp_str + ":" + to_string(temp);
        }
        else if(type == "array of objects")
        {
            temp_str = panel_user_input(" sectiginiz parametre array of objects türündedir \n daha yapilmadi \n");
            temp_str = " " + temp_str + ":" + temp_str;
        }
        else if(type == "query")
        {
            temp_str = panel_user_input(" sectiginiz parametre icin string bir deger girlilmesi gerekmektedir \n Panell daha sonra gelistirilecektir \n suan icin string giriniz \n Kontroller eksiktir lütfen düzgün deger giriniz \n ','degerlerinizi karakteri ile ayiriniz \n");
            temp_str = " " + temp_str + ":" + temp_str;
        }
        else if(type == "object")
        {
            string temp_str_key = panel_user_input(" sectiginiz parametre icin string bir deger çifti girlilmesi gerekmektedir \n key giriniz \n");
            string temp_str_value = panel_user_input(" sectiginiz parametre icin string bir deger çifti girlilmesi gerekmektedir \n value giriniz \n");
            temp_str = " " + temp_str + ":" + temp_str_key + ":" + temp_str_value;
        }
        else if(type == "tags")
        {
            temp_str = panel_user_input(" sectiginiz parametre tags türündedir \n daha yapilmadi \n");
        }
        else
        {
            cout << "hata" << endl;
            return;
        }
        method_execute_str += temp_str;
    }
    this->executeCommandsMap[method] = method_execute_str;
}

void zabbix_Api_I_O::viewAndGetOutputParameters(string method)
{
    string executubleOutputParameters = get_panel_output_parameters(string("denenenene\n"),method);
    this->executeCommandsMap[method] += executubleOutputParameters;
    cout << executeCommandsMap[method] << endl;
}

string zabbix_Api_I_O::get_method_items(string method)
{
    string secilebilecek_itemler = executeCommand(this->path_get_items + this->executeCommandsMap[method]);
    return secilebilecek_itemler;
}

void zabbix_Api_I_O::viewAndGetItems(string method)
{
    string secilebilecek_itemler = get_method_items(method);
    vector<string> secilen_items = secim_paneli(secilebilecek_itemler, "suan icin sadece item methodu icin secim yapmaninizin önemi vadir. secilebilecek itemler \n");
    if(secilen_items.size() > 0 && method == "item")
    {
        this->executeCommandsMap[method] += convert_argument_string(secilen_items, " itemids:");
    }
}

zabbix_Api_I_O::zabbix_Api_I_O()
{
    try
    {
        this->Informations_app();
        this->view_hosts(); // host seç
        this->viewAndGetMethods(); // method seç
        for (auto method : this->methods)
        {
            this->viewAndGetMethodParameters(method); // method parametrelerini seç
            this->viewAndGetMethodsParametersArguments(method); // method parametrlerine uygun argüman parametrelerini seç
            this->viewAndGetOutputParameters(method); // method parametrlerine uygun output parametrelerini seç 
            this->viewAndGetItems(method); // itemleri seç// normalde bir önceki fonksiyonda olması gerekir şuan için özel bir konumdadır sadece item için çalışır
        }
        // this->executeCommandsConstrucor(); // script içeriğini hazırla
        this->create_sh_script(); // scripti kaydet        
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        writelog("","","","",e.what());
    }
    
}

zabbix_Api_I_O::~zabbix_Api_I_O()
{

}

int main()
{
    zabbix_Api_I_O zabbix_Api_I_O;
    
    return 0;
}
