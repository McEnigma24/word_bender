#include "__preprocessor__.h"

#include <unordered_map>
#include <map>
#include <string>
#include <optional>
#include <bitset>

typedef std::unordered_map<std::string, char> check_map_t;
// typedef std::map<std::string, char> check_map_t;

std::unordered_map<char, i8> letter_values = {
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

constexpr i8 gameboard_height = 15;
constexpr i8 gameboard_width = 15;
constexpr i8 max_number_of_user_letters = 7;

struct cell
{
    char current_letter = 0;
    i8 multiplyer_word = 1;
    i8 multiplyer_letter = 1;

    bool fleshlyAdded = false;

    cell(const i8 mult_word = 1, const i8 mult_letter = 1)
    : multiplyer_word(mult_word)
    , multiplyer_letter(mult_letter)
    {
    }
};
#define c(...) cell{__VA_ARGS__}

typedef std::array<std::array<cell, gameboard_width>, gameboard_height> gameboard_t;

template<size_t N>
class Permutator
{
    const std::vector<char>& input_elements;

    std::bitset<N> flags;
    std::string accumulated_elements;

    public:
    Permutator(const std::vector<char>& input_elements)
        : input_elements(input_elements)
    {
        flags.reset();
        accumulated_elements.reserve(N);
    }

    std::vector<std::string> getPermutations(int depth)
    {
        std::vector<std::string> ret;

        bodyPermutations(ret, 0, depth);

        return ret;
    }

    void bodyPermutations(std::vector<std::string>& output, int i_start, int depth)
    {
        if(depth > 0)
        {
            for(int i = i_start; i < N; i++)
            {
                if(not flags.test(i))
                {
                    flags.set(i); // block
                    accumulated_elements.push_back(input_elements[i]);
                    {
                        bodyPermutations(output, (i + 1) % N, depth - 1);
                    }
                    accumulated_elements.pop_back();
                    flags.reset(i); // un-block
                }
            }
        }
        else
        {
            // we need to have a separete container that keep the order of added input_elements
            output.push_back(accumulated_elements);

            line("hrere");
        }
    }
};

#if 0
class GameState
{
    struct xyCoord
    {
        i8 x;
        i8 y;
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

        void generateLetters(const gameboard_t &pMap)
        {
            letters = "";

            if(isWordLeftRight())
            {
                const i8 y = start.y;

                for(i8 x = start.x; x <= end.x; x++)
                {
                    letters += (std::string)pMap[y][x].current_letter;
                }
            }
            else if(isWordTopDown())
            {
                const i8 x = start.x;

                for(i8 y = start.y; y <= end.y; y++)
                {
                    letters += (std::string)pMap[y][x].current_letter;
                }
            }
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

    std::optional<i8> returnClampedCoordToBoard_Y(const i8 y, const i8 modifier)
    {
        i8 result = y + modifier;

        if(0 <= result && result < gameboard_height) return y;

        return std::nullopt;
    }

    std::optional<i8> returnClampedCoordToBoard_X(const i8 x, const i8 modifier)
    {
        i8 result = x + modifier;

        if(0 <= result && result < gameboard_width) return x;

        return std::nullopt;
    }

    std::vector<WordPositionOnAGameboard> getNewlyCreatedWords(const gameboard_t &pMap, const WordPositionOnAGameboard addedLetters)
    {
        std::vector<WordPositionOnAGameboard> ret;
        ret.push_back(addedLetters); // inserting the added word just by itself



        // check if it touches any other letters //

        // +1 and -1      clamped to GameBoardDimentions

        if(addedLetters.isWordLeftRight())
        {
            i8 x_min = addedLetters.start.x;
            i8 x_max = addedLetters.end.x;

            for(i8 x = x_min; x < x_max; x++)
            {
                i8 y = addedLetters.start.y;



                // checking if cell -1 or +1 on Y is occupied with old letter
                // x, y

                auto top_y    = returnClampedCoordToBoard_Y(y, -1);
                auto bottom_y = returnClampedCoordToBoard_Y(y, +1);

                const bool top_present_and_not_null       = (top_y && (pMap[top_y.value()]   [x].current_letter != 0));
                const bool bottom_present_and_not_null = (bottom_y && (pMap[bottom_y.value()][x].current_letter != 0));

                // both are present -> its for sure not counted as new word
                if (top_present_and_not_null && bottom_present_and_not_null)
                {
                    continue;
                }

                if (top_present_and_not_null || bottom_present_and_not_null) // we got ourself w new word
                {
                    if (top_present_and_not_null)
                    {
                        WordPositionOnAGameboard new_word;

                        // going UP so now we know only the end //

                        new_word.end.x = x;
                        new_word.end.y = y;

                        // we got UP as far as the word goes AND as far as board goes

                        while(true)
                        {
                            y--;

                            if(not (0 <= y)) break;
                            if(not (pMap[y][x].current_letter != 0)) break;
                        }

                        // y_max //

                        new_word.start.x = x;
                        new_word.start.y = y;

                        new_word.generateLetters(pMap);

                        ret.push_back(new_word);
                    }

                    if (bottom_present_and_not_null)
                    {
                        WordPositionOnAGameboard new_word;

                        // going DOWN so now we know only the start //

                        new_word.start.x = x;
                        new_word.start.y = y;

                        // we got DOWN as far are the word goes AND as far as board goes

                        while(true)
                        {
                            y++;

                            if(not (y < gameboard_height)) break;
                            if(not (pMap[y][x].current_letter != 0)) break;
                        }

                        // y_max //

                        new_word.end.x = x;
                        new_word.end.y = y;

                        new_word.generateLetters(pMap);

                        ret.push_back(new_word);
                    }
                }
            }
        }
        else if (addedLetters.isWordTopDown())
        {
            i8 y_min = addedLetters.start.y;
            i8 y_max = addedLetters.end.y;

            for(i8 y = y_min; y < y_max; y++)
            {
                i8 x = addedLetters.start.x;



                // checking if cell -1 or +1 on Y is occupied with old letter
                // x, y

                auto left_x  = returnClampedCoordToBoard_X(x, -1);
                auto right_x = returnClampedCoordToBoard_X(x, +1);

                const bool left_present_and_not_null   = (left_y && (pMap[y] [left_y.value()].current_letter != 0));
                const bool right_present_and_not_null = (right_y && (pMap[y][right_y.value()].current_letter != 0));

                // both are present -> its for sure not counted as new word
                if (left_present_and_not_null && right_present_and_not_null)
                {
                    continue;
                }

                if (left_present_and_not_null || right_present_and_not_null) // we got ourself w new word
                {
                    if (left_present_and_not_null)
                    {
                        WordPositionOnAGameboard new_word;

                        // going LEFT so now we know only the end //

                        new_word.end.x = x;
                        new_word.end.y = y;

                        // we got LEFT as far as the word goes AND as far as board goes

                        while(true)
                        {
                            x--;

                            if(not (0 <= x)) break;
                            if(not (pMap[y][x].current_letter != 0)) break;
                        }

                        // x_max //

                        new_word.start.x = x;
                        new_word.start.y = y;

                        new_word.generateLetters(pMap);

                        ret.push_back(new_word);
                    }

                    if (right_present_and_not_null)
                    {
                        WordPositionOnAGameboard new_word;

                        // going RIGHT so now we know only the start //

                        new_word.start.x = x;
                        new_word.start.y = y;

                        // we got RIGHT as far are the word goes AND as far as board goes

                        while(true)
                        {
                            x++;

                            if(not (x < gameboard_width)) break;
                            if(not (pMap[y][x].current_letter != 0)) break;
                        }

                        // x_max //

                        new_word.end.x = x;
                        new_word.end.y = y;

                        new_word.generateLetters(pMap);

                        ret.push_back(new_word);
                    }
                }
            }
        }
        else
        {
            CRASH_LOG("fuck !!!");
        }

        return ret;
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

        i16 sum = 0;
        for(const auto& word : getNewlyCreatedWords(pMap))
        {
            i16 singleWordScore = 0;

            i16 wholeWordMultiplyer = 1;

            if(word.isWordLeftRight())
            {
                const i8 y = start.y;

                for(i8 x = start.x; x <= end.x; x++)
                {
                    auto& cell = pMap[y][x];

                    if(cell.current_letter == 0) CRASH_LOG("FUCK !!!");

                    singleWordScore += (cell.multiplyer_letter * letter_values[cell.current_letter]);

                    if(1 < cell.multiplyer_word)
                    {
                        wholeWordMultiplyer *= cell.multiplyer_word;
                    }
                }
            }
            else if(word.isWordTopDown())
            {
                const i8 x = start.x;

                for(i8 y = start.y; y <= end.y; y++)
                {
                    auto& cell = pMap[y][x];

                    if(cell.current_letter == 0) CRASH_LOG("FUCK !!!");

                    singleWordScore += (cell.multiplyer_letter * letter_values[cell.current_letter]);

                    if(1 < cell.multiplyer_word)
                    {
                        wholeWordMultiplyer *= cell.multiplyer_word;
                    }
                }
            }
            else { CRASH_LOG("fuck !!!"); }



            sum += singleWordScore * wholeWordMultiplyer;

            // another word //
        }

        return sum;
    }



    std::vector<char> = { 0 }; // max_number_of_user_letters

    // identyfikujemy wszystkie miejsca zaczepu -> pola przez które może przechodzić albo do których można dołączyć litery
    // przedstawimy je jako paski przed i po
    //
    // że tutaj może się kończyć, tutaj może się zaczynać albo tutaj może być tak po prostu
    // te paski będziemy rozszeżać w zależności od tego jak długie słowo chcemy zmieścić -> zaczynamy od 2 kończymy na 7

    // dokładamy kompletnie losowo, po prostu wszystkie kombinacje przechodzimy i kolejność ma znaczenia -> czyli chyba tylko silnia jedno przejście -> jakby 7, 6, 5, 4, i bez powtarzania tej samej litery, czyli właśnie maleje ilość dostępnych

    //


    void goingOverAllPossibleCombinations()
    {
        // generate it combination and then put letters in allowed places //

        for(int stencilSize = 2; stencilSize <= 7; stencilSize++)
        {


            // recursive shit -> stencil will have from 2 to 7 possible combinations -> we have to use this recursive jumping and locking input_elements
        }
    }

};
#endif








#ifdef BUILD_EXECUTABLE
int main(int argc, char* argv[])
{
    time_stamp("main starting");

    std::vector<char> input_elements{'a', 'b', 'c'};
    Permutator<3> permute(input_elements);
    for(const auto& permutation : permute.getPermutations(3))
    {
        var(permutation);
    }
    time_stamp("ready");

    for(const auto& permutation : permute.getPermutations(2))
    {
        var(permutation);
    }
    time_stamp("ready");

    return 0;
}
#endif