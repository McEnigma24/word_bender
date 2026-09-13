// #include <bitset>
#include <boost/dynamic_bitset.hpp>
#include "json.hpp" // external lib

#include "__preprocessor__.h"

#include <unordered_map>
#include <map>
#include <string>
#include <optional>
#include <set>
#include <cmath>


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

class Permutator
{
    const std::vector<char>& input_elements;
    const size_t N;

    // std::bitset<N> flags;
    // boost::dynamic_bitset<char> flags;
    boost::dynamic_bitset<> flags;
    std::string accumulated_elements;

    public:
    Permutator(const std::vector<char>& input_elements)
        : input_elements(input_elements)
        , N(input_elements.size())
        , flags(N)
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
                        // bodyPermutations(output, (i + 1), depth - 1); -> does not get all combinations
                        // bodyPermutations(output, (i + 1) % N, depth - 1); -> does not get all combinations

                        bodyPermutations(output, 0, depth - 1);
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
        }
    }
};


class GameState
{
    struct xyCoord
    {
        i8 x;
        i8 y;

        bool operator==(const xyCoord& other)
        {
            return ((this->x == other.x) && (this->y == other.y));
        }
        bool operator!=(const xyCoord& other)
        {
            return not this->operator==(other);
        }

        void operator=(const xyCoord& other)
        {
            this->x = other.x;
            this->y = other.y;
        }
    };

    struct WordPositionOnAGameboard
    {
        xyCoord start;
        xyCoord end;

        std::string letters;

        bool isWordTopDown() const
        {
            return start.x == end.x;
        }

        bool isWordLeftRight() const
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
                    letters += pMap[y][x].current_letter;
                }
            }
            else if(isWordTopDown())
            {
                const i8 x = start.x;

                for(i8 y = start.y; y <= end.y; y++)
                {
                    letters += pMap[y][x].current_letter;
                }
            }
        }

        void operator=(const WordPositionOnAGameboard& other)
        {
            this->start = other.start;
            this->end = other.end;
            this->letters = other.letters;
        }
    };


    check_map_t check_map;

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

    std::vector<char> availableLetters;

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


        // initialize letters on map with JSON input //


    }


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

                const bool left_present_and_not_null   = (left_x && (pMap[y][left_x.value()].current_letter != 0));
                const bool right_present_and_not_null = (right_x && (pMap[y][right_x.value()].current_letter != 0));

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
            FATAL_ERROR("invalid word orientation");
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
        for(const auto& word : getNewlyCreatedWords(pMap, addedLetters))
        {
            i16 singleWordScore = 0;

            i16 wholeWordMultiplyer = 1;

            if(word.isWordLeftRight())
            {
                const i8 y = word.start.y;

                for(i8 x = word.start.x; x <= word.end.x; x++)
                {
                    auto& cell = pMap[y][x];

                    if(cell.current_letter == 0) FATAL_ERROR("empty cell in horizontal word");

                    singleWordScore += (cell.multiplyer_letter * letter_values[cell.current_letter]);

                    if(1 < cell.multiplyer_word)
                    {
                        wholeWordMultiplyer *= cell.multiplyer_word;
                    }
                }
            }
            else if(word.isWordTopDown())
            {
                const i8 x = word.start.x;

                for(i8 y = word.start.y; y <= word.end.y; y++)
                {
                    auto& cell = pMap[y][x];

                    if(cell.current_letter == 0) FATAL_ERROR("empty cell in vertical word");

                    singleWordScore += (cell.multiplyer_letter * letter_values[cell.current_letter]);

                    if(1 < cell.multiplyer_word)
                    {
                        wholeWordMultiplyer *= cell.multiplyer_word;
                    }
                }
            }
            else { FATAL_ERROR("invalid word orientation"); }



            sum += singleWordScore * wholeWordMultiplyer;

            // another word //
        }

        return sum;
    }

    // identyfikujemy wszystkie miejsca zaczepu -> pola przez które może przechodzić albo do których można dołączyć litery
    // przedstawimy je jako paski przed i po
    //
    // że tutaj może się kończyć, tutaj może się zaczynać albo tutaj może być tak po prostu
    // te paski będziemy rozszeżać w zależności od tego jak długie słowo chcemy zmieścić -> zaczynamy od 2 kończymy na 7

    // dokładamy kompletnie losowo, po prostu wszystkie kombinacje przechodzimy i kolejność ma znaczenia -> czyli chyba tylko silnia jedno przejście -> jakby 7, 6, 5, 4, i bez powtarzania tej samej litery, czyli właśnie maleje ilość dostępnych

    std::vector<xyCoord> getAllPositionsToCheck(const gameboard_t &pMap)
    {
        std::set<xyCoord> uniqueCoords;

        // going to every cell with letter and goint around it adding all empty cells

        const auto max_Y = pMap.size();
        const auto max_X = pMap[0].size();

        for(int y=0; y < max_Y; y++) for(int x=0; x < max_X; x++)
        {
            const auto& cell = pMap[y][x];

            if(cell.letter != 0)
            {
                // teraz idziemy na około niej góra-dół-lewo-prawo

                for(int yy=y-1; yy < y+1; yy++) for(int xx=x-1; xx < x+1; xx++)
                {
                    if(not (0 < yy && yy < gameboard_height)) continue;
                    if(not (0 < xx && xx < gameboard_width)) continue;
                    if(yy == y && xx == x) continue;
                    if(std::abs(yy) == 1 && std::abs(xx) == 1) continue;

                    const auto& check_cell = pMap[yy][xx];

                    if(check_cell.letter == 0)
                    {
                        // we add empty cells

                        uniqueCoords.insert(xyCoord(xx, yy));
                    }
                }
            }
        }
    }

    void goingOverAllPossibleCombinations()
    {
        const std::vector<xyCoord> allPositionsToCheck = getAllPositionsToCheck(mGameboard);

        Permutator permute(availableLetters);
        int maxStencilSize = std::min((int)availableLetters.size(), (int)7);


        // best move so far //
        WordPositionOnAGameboard currentBestWord;
        int currentBestEvaluation = 0;

        for(int stencilSize = 1; stencilSize <= maxStencilSize; stencilSize++)
        {
            int distance = stencilSize - 1;

            for(const auto& pos : allPositionsToCheck)
            {
                for(const auto& letters : permute.getPermutations(stencilSize))
                {
                    // moving stencil across the position -> LEFT to RIGHT //
                    {
                        // COPY of gameboard //
                        auto gameboardCopy = mGameboard;



                        const int y = pos.y;
                        int x = std::clamp(pos.x - distance, 0, gameboard_width);
                        int x_end = std::clamp(pos.x, 0, gameboard_width);
                        int letters_index = 0;

                        WordPositionOnAGameboard placedWord;
                        placedWord.start.x = x;
                        placedWord.start.y = y;

                        for(;(x <= x_end) && (x < gameboard_width); x++)
                        {
                            // -> now lets place the letters //

                            if(gameboardCopy[y][x].letter != 0) // cell occupied //
                            {
                                x_end++;
                                continue;
                            }
                            else
                            {
                                const auto letter = letters[letters_index ++];

                                gameboardCopy[y][x].letter = letter;
                                placedWord.letters += letter;
                            }
                        }

                        placedWord.end.x = x;
                        placedWord.end.y = y;

                        // now lets evaluate it //

                        auto value = evaluateProposedWordAlredyPutIntoGameBoard(gameboardCopy, placedWord);

                        if(currentBestEvaluation < value)
                        {
                            currentBestEvaluation = value;
                            currentBestWord = placedWord;
                        }
                    }

                    // moving stencil across the position ->  UP  to DOWN //
                    {
                        // COPY of gameboard //
                        auto gameboardCopy = mGameboard;



                        const int x = pos.x;
                        int y = std::clamp(pos.y - distance, 0, gameboard_height);
                        int y_end = std::clamp(pos.y, 0, gameboard_height);
                        int letters_index = 0;

                        WordPositionOnAGameboard placedWord;
                        placedWord.start.x = x;
                        placedWord.start.y = y;

                        for(;(y <= y_end) && (y < gameboard_height); y++)
                        {
                            // -> now lets place the letters //

                            if(gameboardCopy[y][x].letter != 0) // cell occupied //
                            {
                                y_end++;
                                continue;
                            }
                            else
                            {
                                const auto letter = letters[letters_index ++];

                                gameboardCopy[y][x].letter = letter;
                                placedWord.letters += letter;
                            }
                        }

                        placedWord.end.x = x;
                        placedWord.end.y = y;

                        // now lets evaluate it added word //

                        auto value = evaluateProposedWordAlredyPutIntoGameBoard(gameboardCopy, placedWord);

                        if(currentBestEvaluation < value)
                        {
                            currentBestEvaluation = value;
                            currentBestWord = placedWord;
                        }
                    }
                }
            }
        }

        line("Found it");
        varr(currentBestEvaluation.start.x);
        var(currentBestEvaluation.start.y);
        varr(currentBestEvaluation.end.x);
        var(currentBestEvaluation.end.y);
        var(currentBestEvaluation.letters);
    }
};






int main()
{
    // Parse JSON from file
    std::ifstream file("input/gameState.json");
    if (!file.is_open())
    {
        std::cerr << "Error: Could not open file input/test.json" << std::endl;
        return 1;
    }

    nlohmann::json json_data;
    file >> json_data;
    file.close();

    // Display all values
    std::cout << "\n=== JSON Content ===" << std::endl;
    std::cout << json_data.dump(4) << std::endl;

    std::cout << "\n=== Parsed Values ===" << std::endl;

    /*
    if (json_data.contains("Messages") && json_data["Messages"].is_array())
    {
        for (size_t i = 0; i < json_data["Messages"].size(); ++i)
        {
            const auto& message = json_data["Messages"][i];

            std::cout << "\n--- Message " << i << " ---" << std::endl;

            if (message.contains("tytle")) { std::cout << "Title: " << message["tytle"].get<std::string>() << std::endl; }

            // Display var_a
            if (message.contains("var_a"))
            {
                const auto& var_a = message["var_a"];
                std::cout << "\nvar_a:" << std::endl;
                if (var_a.contains("type")) std::cout << "  type: " << var_a["type"].get<std::string>() << std::endl;
                if (var_a.contains("required")) std::cout << "  required: " << var_a["required"].get<bool>() << std::endl;
                if (var_a.contains("range"))
                {
                    std::cout << "  range: [" << var_a["range"][0].get<int>() << ", " << var_a["range"][1].get<int>() << "]" << std::endl;
                }
                if (var_a.contains("default")) std::cout << "  default: " << var_a["default"].get<int>() << std::endl;
            }

            // Display var_b
            if (message.contains("var_b"))
            {
                const auto& var_b = message["var_b"];
                std::cout << "\nvar_b:" << std::endl;
                if (var_b.contains("type")) std::cout << "  type: " << var_b["type"].get<std::string>() << std::endl;
                if (var_b.contains("required")) std::cout << "  required: " << var_b["required"].get<bool>() << std::endl;
                if (var_b.contains("range"))
                {
                    std::cout << "  range: [" << var_b["range"][0].get<int>() << ", " << var_b["range"][1].get<int>() << "]" << std::endl;
                }
                if (var_b.contains("default")) std::cout << "  default: " << var_b["default"].get<int>() << std::endl;
            }
        }
    }
    */

    return 0;
}


// #ifdef BUILD_EXECUTABLE
#ifdef 0
int main(int argc, char* argv[])
{
    time_stamp("main starting");

    std::vector<char> input_elements{'a', 'b', 'c', 'd', 'e'};



    Permutator permute(input_elements);
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

    for(const auto& permutation : permute.getPermutations(1))
    {
        var(permutation);
    }
    time_stamp("ready");

    return 0;
}
#endif