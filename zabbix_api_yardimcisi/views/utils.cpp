#include "utils.hpp"

vector<string> adv_tokenizer(string s, char del)
{
    string tmp;
    stringstream ss(s);
    vector<string> words;

    while (getline(ss, tmp, del))
    {
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
    for (int i = 0; i < arg.size(); i++)
    {
        arg_str += arg[i] + ((i < arg.size() - 1) ? "," : "");
        std::cout << arg[i] << std::endl;
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
                if (secim == bitis_index - 1) {
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
