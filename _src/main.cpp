#include "__preprocessor__.h"


#include <unordered_map>
#include <map>
#include <string>

typedef std::unordered_map<std::string, char> check_map_t;
// typedef std::map<std::string, char> check_map_t;

std::unordered_map<char, u8> letter_values = {
    {'a', 1},   // 1
    {'ą', 5},   // 2
    {'b', 3},   // 3
    {'c', 2},   // 4
    {'ć', 6},   // 5
    {'d', 2},   // 6
    {'e', 1},   // 7
    {'ę', 5},   // 8
    {'f', 4},   // 9
    {'g', 3},   // 10
    {'h', 3},   // 11
    {'i', 1},   // 12
    {'j', 3},   // 13
    {'k', 2},   // 14
    {'l', 2},   // 15
    {'ł', 3},   // 16
    {'m', 2},   // 17
    {'n', 1},   // 18
    {'ń', 7},   // 19
    {'o', 1},   // 20
    {'ó', 5},   // 21
    {'p', 2},   // 22
    {'r', 1},   // 23
    {'s', 1},   // 24
    {'ś', 5},   // 25
    {'t', 2},   // 26
    {'u', 3},   // 27
    {'w', 1},   // 28
    {'y', 2},   // 29
    {'z', 1},   // 30
    {'ź', 7},   // 31
    {'ż', 5}    // 32
};

bool isAllowedSet(const std::string& word)
{
    for (char c : word)
    {
        // c = std::tolower(static_cast<unsigned char>(c));   // not allowing capital letters

        if (letter_values.find(c) == letter_values.end())
        {
            return false; // Character not in allowed set
        }
    }
    return true; // All characters are in the allowed set
}

std::vector<std::string> get_words_from_line(const std::string& line)
{
    std::vector<std::string> words;
    std::string word;
    std::size_t pos = 0;
    std::size_t next_pos = line.find(',');
    while (next_pos != std::string::npos)
    {
        word = line.substr(pos, next_pos - pos);
        // eliminate leading and trailing spaces
        word.erase(word.begin(), std::find_if(word.begin(), word.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
        word.erase(std::find_if(word.rbegin(), word.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), word.end());

        if(not isAllowedSet(word))
        {
            pos = next_pos + 1;
            next_pos = line.find(',', pos);
            continue;
        }
        words.push_back(word);
        pos = next_pos + 1;
        next_pos = line.find(',', pos);
    }

    // add the last part
    word = line.substr(pos);
    // eliminate leading and trailing spaces
    word.erase(word.begin(), std::find_if(word.begin(), word.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
    word.erase(std::find_if(word.rbegin(), word.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), word.end());

    if(not isAllowedSet(word))
    {
        return words;
    }
    words.push_back(word);
    return words;
}

check_map_t build_check_map(const std::string& filename)
{
    check_map_t check_map;

    // 1. Create an input file stream object
    std::ifstream file(filename);

    // 2. Check if the file opened successfully
    if (!file.is_open())
    {
        std::cerr << "Error: Could not open the file.\n";
        return {};
    }

    std::string line;

    // 3. Read the file line by line until EOF (End Of File)
    while (std::getline(file, line))
    {
        // Process the line (here we just print it)

        // std::cout << line << '\n';

        // split to different strings by',' and eliminate ' '
        std::vector<std::string> parts = get_words_from_line(line);

        for (const std::string& part : parts)
        {
            check_map[part] = 0;
        }
    }

    // 4. Close the file (optional, as the ifstream destructor does this automatically)
    file.close();

    return check_map;
}



class GameState
{
    struct cell
    {
        char current_letter = 0;
        u8 multiplyer_word = 0;
        u8 multiplyer_letter = 0;

        cell(const u8 mult_word = 0, const u8 mult_letter = 0)
            : multiplyer_word(mult_word)
            , multiplyer_letter(mult_letter)
        {
        }
    };

    #define c(...) cell{__VA_ARGS__}
    std::array<std::array<cell, 15>, 15> map = {{

        { c(3, 0), c(    ), c(    ), c(0, 2), c(    ),           c(    ), c(    ), c(3, 0), c(    ), c(    ),         c(    ), c(0, 2), c(    ), c(    ), c(3, 0) },
        { c(    ), c(2, 0), c(    ), c(    ), c(    ),           c(0, 3), c(    ), c(    ), c(    ), c(0, 3),         c(    ), c(    ), c(    ), c(2, 0), c(    ) },
        { c(    ), c(    ), c(2, 0), c(    ), c(    ),           c(    ), c(0, 2), c(    ), c(0, 2), c(    ),         c(    ), c(    ), c(2, 0), c(    ), c(    ) },
        { c(0, 2), c(    ), c(    ), c(2, 0), c(    ),           c(    ), c(    ), c(0, 2), c(    ), c(    ),         c(    ), c(2, 0), c(    ), c(    ), c(0, 2) },
        { c(    ), c(    ), c(    ), c(    ), c(2, 0),           c(    ), c(    ), c(    ), c(    ), c(    ),         c(2, 0), c(    ), c(    ), c(    ), c(    ) },

        { c(    ), c(0, 3), c(    ), c(    ), c(    ),           c(0, 3), c(    ), c(    ), c(    ), c(0, 3),         c(    ), c(    ), c(    ), c(0, 3), c(    ) },
        { c(    ), c(    ), c(0, 2), c(    ), c(    ),           c(    ), c(0, 2), c(    ), c(0, 2), c(    ),         c(    ), c(    ), c(0, 2), c(    ), c(    ) },
        { c(3, 0), c(    ), c(    ), c(0, 2), c(    ),           c(    ), c(    ), c(2, 0), c(    ), c(    ),         c(    ), c(0, 2), c(    ), c(    ), c(3, 0) },
        { c(    ), c(    ), c(0, 2), c(    ), c(    ),           c(    ), c(0, 2), c(    ), c(0, 2), c(    ),         c(    ), c(    ), c(0, 2), c(    ), c(    ) },
        { c(    ), c(0, 3), c(    ), c(    ), c(    ),           c(0, 3), c(    ), c(    ), c(    ), c(0, 3),         c(    ), c(    ), c(    ), c(0, 3), c(    ) },

        { c(    ), c(    ), c(    ), c(    ), c(2, 0),           c(    ), c(    ), c(    ), c(    ), c(    ),         c(2, 0), c(    ), c(    ), c(    ), c(    ) },
        { c(0, 2), c(    ), c(    ), c(2, 0), c(    ),           c(    ), c(    ), c(0, 2), c(    ), c(    ),         c(    ), c(2, 0), c(    ), c(    ), c(0, 2) },
        { c(    ), c(    ), c(2, 0), c(    ), c(    ),           c(    ), c(0, 2), c(    ), c(0, 2), c(    ),         c(    ), c(    ), c(2, 0), c(    ), c(    ) },
        { c(    ), c(2, 0), c(    ), c(    ), c(    ),           c(0, 3), c(    ), c(    ), c(    ), c(0, 3),         c(    ), c(    ), c(    ), c(2, 0), c(    ) },
        { c(3, 0), c(    ), c(    ), c(0, 2), c(    ),           c(    ), c(    ), c(3, 0), c(    ), c(    ),         c(    ), c(0, 2), c(    ), c(    ), c(3, 0) }

    }};

    std::vector<std::string> current_words;

    void regenerate_current_words()
    {
        // scan whole map //


    }
};









#ifdef BUILD_EXECUTABLE
int main(int argc, char* argv[])
{
    time_stamp("It just works");

    auto check_map = build_check_map("input/dict.txt");
    // auto check_map = build_check_map("input/test.txt");


    var(check_map.size());
    // for(auto& [key, value] : check_map) std::cout << key << " : " << (int)value << std::endl;

    time_stamp("check_map - DONE");


















    return 0;
}
#endif