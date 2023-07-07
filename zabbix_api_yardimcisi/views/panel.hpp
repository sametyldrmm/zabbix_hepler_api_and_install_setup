#include <iostream>
#include <fstream>
#include <ostream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <set>
#include <queue>
#include <stack>
#include <list>
#include <iterator>
#include <algorithm>
#include "../models/get_db2.cpp"

using namespace std;

class zabbix_Api_I_O : public DatabaseManager
{
private:
    vector<string> Env;
    vector<string> User_I_first_split;
    vector<string> output_parametresi;
public:
    void view_hosts();
    void Informations_app();
    string get_panel_hosts();
    vector<string>  get_panel_methods();
    string get_output_parameters(string method);
    string get_panel_output_parameters(string baslik,string method);
    void create_sh_script(string script_info);
    
    zabbix_Api_I_O();
    ~zabbix_Api_I_O();
};

