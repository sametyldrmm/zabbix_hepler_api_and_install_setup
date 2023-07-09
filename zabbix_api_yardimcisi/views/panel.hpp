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
    vector<string> methods;
    string host_id_str;
    map<string, vector<string>> method_parametresi;
    map<string, vector<string>> method_output_parameters;
    map<string, vector<string>> method_select_items;
    map<string, string> executeCommandsMap;
    string path_get_items = "./run_program/get_items ";
public:
    void view_hosts();
    void viewAndGetMethods();
    void Informations_app();
    void viewAndGetMethodParameters(string method);
    void viewAndGetMethodsParametersArguments(string method);
    void viewAndGetOutputParameters(string method);
    void viewAndGetItems(string method);

    string get_panel_hosts();
    vector<string>  get_panel_methods();
    string get_method_items(string method);
    string get_output_parameters(string method);
    string get_panel_output_parameters(string baslik,string method);
    void create_sh_script();
    
    zabbix_Api_I_O();
    ~zabbix_Api_I_O();
};

