#include "utils.hpp"

vector<string> adv_tokenizer(string s, char del)
{
    string tmp;
    stringstream ss(s);
    vector<string> words;

    while (getline(ss, tmp, del))
    {
        // tmp içerisinde olan tüm del karakterlerini kaldıralım
        tmp.erase(remove(tmp.begin(), tmp.end(), del), tmp.end());
        if(tmp == "")
            continue;
        words.push_back(tmp);
    }
    return words;
}

std::string executeCommand(const std::string &command)
{
    std::array<char, 128> buffer;
    std::string result;
    FILE *pipe = popen(command.c_str(), "r");

    if (!pipe)
    {
        throw std::runtime_error("Komut çalistirilamadi.");
    }

    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr)
    {
        result += buffer.data();
    }

    pclose(pipe);
    return result;
}


std::string convert_argument_string(vector<string> arg, string arg_key)
{
    std::string arg_str = arg_key;
    for (size_t i = 0; i < arg.size(); i++)
    {
        arg_str += arg[i] + ((i < arg.size() - 1) ? "," : "");
    }
    return arg_str;
}

std::vector<std::string> secim_paneli(std::string data,std::string baslik) {
    std::vector<std::string> all = adv_tokenizer(data, '\n');
    std::vector<std::string> secenekler;
    secenekler.insert(secenekler.end(), all.begin() , all.end());
    secenekler.push_back("Devam");
    int secenekler_size = secenekler.size();
    int secim = 0;
    int key;
    int sayfa_sayisi = (secenekler_size % 45 == 0) ? secenekler_size / 45 : (secenekler_size / 45) + 1;
    int mevcut_sayfa = 1;

    std::vector<char> c(secenekler_size, ' ');
    std::vector<std::string> secilen_secenekler;

    initscr();
    keypad(stdscr, TRUE);
    bool control = true;

    while (control) {
        clear();
        printw("\t%s\n", baslik.c_str());
        
        int baslangic_index = (mevcut_sayfa - 1) * 45;
        int bitis_index = std::min(mevcut_sayfa * 45, secenekler_size);
        
        for (int i = baslangic_index; i < bitis_index; i++) {
            printw("%s[%c] %d. %s\n", (i == secim ? ">" : " "), c[i], i + 1, secenekler[i].c_str());
        }

        printw("----------------------------\n");
        printw("Sayfa: %d/%d\n", mevcut_sayfa, sayfa_sayisi);
        printw("Seciminizi yapin (İleri: Sag Yön Tusu | Geri: Sol Yön Tusu): ");
        refresh();

        key = getch();
        switch (key) {
            case KEY_UP:
                secim = (secim - 1 < 0) ? bitis_index - 1 : secim - 1;
                break;
            case KEY_DOWN:
                secim = (secim + 1 >= bitis_index) ? baslangic_index : secim + 1;
                break;
            case KEY_RIGHT:
                mevcut_sayfa = (mevcut_sayfa + 1 > sayfa_sayisi) ? 1 : mevcut_sayfa + 1;
                secim = (mevcut_sayfa - 1) * 45;
                break;
            case KEY_LEFT:
                mevcut_sayfa = (mevcut_sayfa - 1 < 1) ? sayfa_sayisi : mevcut_sayfa - 1;
                secim = (mevcut_sayfa - 1) * 45;
                break;
            case 10:
                clear();
                if (secim == bitis_index - 1 && mevcut_sayfa == sayfa_sayisi) {
                    control = false;
                    break;
                }
                printw("Secenek %d'i sectiniz.\n", secim + 1);
                c[secim] = (c[secim] == 'X') ? ' ' : 'X';
                refresh();
                if (std::find(secilen_secenekler.begin(), secilen_secenekler.end(), secenekler[secim]) == secilen_secenekler.end()) {
                    secilen_secenekler.push_back(adv_tokenizer(secenekler[secim], ' ')[0]);
                } else {
                    secilen_secenekler.erase(std::find(secilen_secenekler.begin(), secilen_secenekler.end(), secenekler[secim]));
                }
                break;
        }
    }

    endwin();
    return secilen_secenekler;
}

std::string panel_user_input(string baslik)
{
    std::string input_text;
    initscr();
    keypad(stdscr, TRUE);
    bool control = true;
    int key;
    while (control)
    {
        clear();
        printw("\t%s\n", baslik.c_str());
        printw("----------------------------\n");
        printw("Girdinizi girin: %s", input_text.c_str());
        refresh();
        key = getch();
        switch (key)
        {
        case 10:
            control = false;
            break;
        case KEY_BACKSPACE:
            input_text = input_text.substr(0, input_text.size() - 1);
            break;
        default:
            input_text += key;
            break;
        }
    }
    endwin();
    return input_text;
}


void writelog(string fileName, string functionName ,string id, string extraMessage, string error)
{
    auto now = std::chrono::system_clock::now();
    std::time_t timestamp = std::chrono::system_clock::to_time_t(now);
    std::string timestampStr = std::ctime(&timestamp);
    timestampStr.pop_back(); // Ctime'dan gelen son karakteri (\n) kaldır
    
    int fd = open("shared_file2.txt", O_RDWR);
    flock(fd, LOCK_EX);

    // Dosyayı okuma işlemi
    std::string data;
    std::ofstream file;

    file.open("shared_file2.txt", std::ios::out | std::ios::app);
    if (!file) {
        std::cerr << "Dosya açılamadı." << std::endl;
        flock(fd, LOCK_UN);
        close(fd);
        return;
    }
    file << "fileName: " << fileName << " functionName: " << functionName << " id: " << id << " extraMessage: " << extraMessage << " error: " << error << std::endl;

    if (!file) {
        std::cerr << "Dosyaya yazı yazılamadı." << std::endl;
    }
    // Kilidi serbest bırakma ve dosyayı kapatma
    flock(fd, LOCK_UN);
    close(fd);
}
