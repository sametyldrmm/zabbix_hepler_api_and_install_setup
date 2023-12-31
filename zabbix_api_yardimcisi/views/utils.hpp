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
#include <ncurses.h> 
#include <chrono>
#include <fcntl.h>
#include <sys/file.h>
#include <unistd.h>

using namespace std;
// split
vector<string> adv_tokenizer(string s, char del);

// komut çalıştırma
std::string executeCommand(const std::string &command);

std::string convert_argument_string(vector<string> arg, string arg_key);

vector<string> secim_paneli(string data,std::string baslik);

std::string panel_user_input(string baslik);

void writelog(string fileName, string functionName ,string id, string extraMessage, string error);
