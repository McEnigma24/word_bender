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

bool isLetterInAllowedSet(const char letter)
{
    // c = std::tolower(static_cast<unsigned char>(c));   // not allowing capital letters  -> we could allow it if we want to
    return letter_values.find(letter) != letter_values.end();
}

bool isAllowedSet(const std::string& word)
{
    for (const char c : word)
    {
        if(not isLetterInAllowedSet(c))
        {
            return false;
        }
    }
    return true; // All characters are present in the allowed set
}

std::vector<std::string> getWordsFromLine(const std::string& line)
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

check_map_t buildCheckMap(const std::string& filename)
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
        std::vector<std::string> parts = getWordsFromLine(line);

        for (const std::string& part : parts)
        {
            check_map[part] = 0;
        }
    }

    // 4. Close the file (optional, as the ifstream destructor does this automatically)
    file.close();

    return check_map;
}

constexpr u8 gameboard_height = 15;
constexpr u8 gameboard_width = 15;
constexpr u8 max_number_of_user_letters = 7;

struct cell
{
    char current_letter = 0;
    u8 multiplyer_word = 1;
    u8 multiplyer_letter = 1;

    cell(const u8 mult_word = 1, const u8 mult_letter = 1)
    : multiplyer_word(mult_word)
    , multiplyer_letter(mult_letter)
    {
    }
};
#define c(...) cell{__VA_ARGS__}

typedef std::array<std::array<cell, gameboard_width>, gameboard_height> gameboard_t;



class GameState
{
    struct xyCoord
    {
        u8 x;
        u8 y;
    }

    struct WordPositionOnAGameboard
    {
        xyCoord start;
        xyCoord end;

        std::string letters;

        bool isWordTopDown()
        {
            return start.x == end.x;
        }

        bool isWordLeftRight()
        {
            return start.y == end.y;
        }
    };


    check_map_t check_map;

    GameState()
    {
        time_stamp("Starting GameState constructor");
        {

            check_map = buildCheckMap("input/dict.txt");
            // auto check_map = buildCheckMap("input/test.txt");

        }
        time_stamp("check_map - DONE");

        var(check_map.size());
        // for(auto& [key, value] : check_map) std::cout << key << " : " << (int)value << std::endl;
    }

    gameboard_t mGameboard = {{

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

    bool isWordPresentInDict(const std::string& word)
    {
        return check_map.contains(word);
    }

    bool checkIfWordsAreLegal(const gameboard_t& pMap)
    {
        const auto max_Y = pMap.size();
        const auto max_X = pMap[0].size();

        // left -> right //
        for(int y=0; y<max_Y; y++)
        {
            std::string current_word = "";
            bool word_started = false;

            for(int x=0; x<max_X; x++)
            {
                const auto& letter = pMap[y][x].current_letter;

                if(letter == 0)
                {
                    // nothing OR word ended //

                    if(word_started) // word was started and now it ends //
                    {
                        word_started = false; // word ended


                        // got to check the current_word that accumulated //

                        if(not isWordPresentInDict(current_word))
                        {
                            return false;
                        }

                        // checking word //
                        current_word = "";
                    }
                }
                else
                {
                    word_started = true; // got first letter or continuing word
                    current_word += letter;
                }
            }
        }

        // top -> bottom //
        for(int x=0; x<max_X; x++)
        {
            std::string current_word = "";
            bool word_started = false;

            for(int y=0; y<max_Y; y++)
            {
                const auto& letter = pMap[y][x].current_letter;

                if(letter == 0)
                {
                    // nothing OR word ended //

                    if(word_started) // word was started and now it ends //
                    {
                        word_started = false; // word ended


                        // got to check the current_word that accumulated //

                        if(not isWordPresentInDict(current_word))
                        {
                            return false;
                        }

                        // checking word //
                        current_word = "";
                    }
                }
                else
                {
                    word_started = true; // got first letter or continuing word
                    current_word += letter;
                }
            }
        }
    }


    std::vector<WordPositionOnAGameboard> getNewlyCreatedWords(const gameboard_t &pMap, const WordPositionOnAGameboard addedLetters)
    {
        // check if it touches any other letters //

        //
    }

    // jak sprawdzamy to od razu wsadzamy do mapy -> tworzymy kopię mapy za każdym razem kiedy podrzucamy nowe słowo do sprawdzenia
    u16 evaluateProposedWordAlredyPutIntoGameBoard(const gameboard_t &pMap, const WordPositionOnAGameboard addedLetters)
    {
        // check letters
        // check any formed word is real
        // check if it fits on a board -> checked when placing the word on board

        if(not checkIfWordsAreLegal(pMap))
        {
            return 0;
        }

        // calculate bonuses //

        // 1. added word bonus - check if it adds to other word and count that too
        // 2. additionaly created words bonuses



        // lista nowych słów -> to co dodaliśmy + to co dotworzyliśmy z już istniejacych
        // je trzeba obliczyć osobno i zsumować

        //
    }






    std::vector<char> = { 0 }; // max_number_of_user_letters

    // identyfikujemy wszystkie miejsca zaczepu -> pola przez które może przechodzić albo do których można dołączyć wyraz
    // przedstawimy je jako paski przed i po
    //
    // że tutaj może się kończyć, tutaj może się zaczynać albo tutaj może być tak po prostu
    // te paski będziemy rozszeżać w zależności od tego jak długie słowo chcemy zmieścić -> zaczynamy od 2 kończymy na 7

    // dokładamy kompletnie losowo, po prostu wszystkie kombinacje przechodzimy i kolejność ma znaczenia -> czyli chyba tylko silnia jedno przejście -> jakby 7, 6, 5, 4, i bez powtarzania tej samej litery, czyli właśnie maleje ilość dostępnych

    //

};









#ifdef BUILD_EXECUTABLE
int main(int argc, char* argv[])
{
    time_stamp("main starting");























    return 0;
}
#endif