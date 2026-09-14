// #include <bitset>
#include <boost/dynamic_bitset.hpp>
#include "json.hpp" // external lib

#include "__preprocessor__.h"

#include <cctype>
#include <unordered_map>
#include <map>
#include <string>
#include <optional>
#include <set>
#include <cmath>
#include <filesystem>


typedef std::unordered_map<std::string, char> check_map_t;
// typedef std::map<std::string, char> check_map_t;

std::unordered_map<std::string, i8> letter_values = {
    {"a", 1},
    {"ą", 5},
    {"b", 3},
    {"c", 2},
    {"ć", 6},
    {"d", 2},
    {"e", 1},
    {"ę", 5},
    {"f", 4},
    {"g", 3},
    {"h", 3},
    {"i", 1},
    {"j", 3},
    {"k", 2},
    {"l", 2},
    {"ł", 3},
    {"m", 2},
    {"n", 1},
    {"ń", 7},
    {"o", 1},
    {"ó", 5},
    {"p", 2},
    {"r", 1},
    {"s", 1},
    {"ś", 5},
    {"t", 2},
    {"u", 3},
    {"w", 1},
    {"y", 2},
    {"z", 1},
    {"ź", 7},
    {"ż", 5}
};

size_t utf8CharLen(unsigned char first_byte)
{
    if ((first_byte & 0x80) == 0) return 1;
    if ((first_byte & 0xE0) == 0xC0) return 2;
    if ((first_byte & 0xF0) == 0xE0) return 3;
    if ((first_byte & 0xF8) == 0xF0) return 4;
    return 1;
}

std::string firstUtf8Letter(const std::string& s)
{
    if (s.empty()) return "";
    return s.substr(0, utf8CharLen(static_cast<unsigned char>(s[0])));
}

std::string normalizeBoardLetter(const std::string& s)
{
    std::string out = firstUtf8Letter(s);
    if (out.size() == 1)
        out[0] = static_cast<char>(std::tolower(static_cast<unsigned char>(out[0])));
    return out;
}

bool isLetterInAllowedSet(const std::string& letter)
{
    return letter_values.find(letter) != letter_values.end();
}

bool isAllowedSet(const std::string& word)
{
    for (size_t i = 0; i < word.size();)
    {
        const std::string letter = word.substr(i, utf8CharLen(static_cast<unsigned char>(word[i])));
        if(not isLetterInAllowedSet(letter))
        {
            return false;
        }
        i += letter.size();
    }
    return true;
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

void loadWordsFromDictFile(check_map_t& check_map, const std::string& filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Error: Could not open the file: " << filename << "\n";
        return;
    }

    std::string line;
    while (std::getline(file, line))
    {
        for (const std::string& part : getWordsFromLine(line))
        {
            check_map[part] = 0;
        }
    }
}

check_map_t buildCheckMap(const std::string& input_directory)
{
    check_map_t check_map;

    namespace fs = std::filesystem;
    const fs::path dir(input_directory);

    if(not fs::is_directory(dir))
    {
        std::cerr << "Error: dictionary directory does not exist: " << input_directory << "\n";
        return {};
    }

    for(const auto& entry : fs::directory_iterator(dir))
    {
        if(not entry.is_regular_file()) continue;

        const std::string filename = entry.path().filename().string();
        if(filename.compare(0, 5, "dict_") != 0) continue;
        if(filename.size() <= 9 || filename.substr(filename.size() - 4) != ".txt") continue;

        loadWordsFromDictFile(check_map, entry.path().string());
    }

    return check_map;
}

constexpr i8 gameboard_height = 15;
constexpr i8 gameboard_width = 15;
constexpr i8 max_number_of_user_letters = 7;

struct cell
{
    std::string current_letter;
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

        bool operator==(const xyCoord& other) const
        {
            return ((this->x == other.x) && (this->y == other.y));
        }
        bool operator!=(const xyCoord& other) const
        {
            return not this->operator==(other);
        }
        bool operator<(const xyCoord& other) const
        {
            return (y < other.y) || (y == other.y && x < other.x);
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

        bool isThisCordPresentInWord(const xyCoord& coord) const
        {
            if(isWordLeftRight())
            {
                return ((start.x <= coord.x) && (coord.x <= end.x))         && ((start.y == coord.y) && (coord.y == end.y));
            }
            else if(isWordTopDown())
            {
                return ((start.y <= coord.y) && (coord.y <= end.y))         && ((start.x == coord.x) && (coord.x == end.x));
            }
            else { FATAL_ERROR("fuuuck"); }
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


    public:
    GameState()
    {
        time_stamp("Starting GameState constructor");
        {

            check_map = buildCheckMap("input");

        }
        time_stamp("check_map - DONE");

        var(check_map.size());
        // for(auto& [key, value] : check_map) std::cout << key << " : " << (int)value << std::endl;



        // initialize letters on map with JSON input //

        // Parse JSON from file
        std::ifstream file("input/gameState.json");
        if (!file.is_open())
        {
            std::cerr << "Error: Could not open file input/gameState.json" << std::endl;
            return;
        }

        nlohmann::json json_data;
        file >> json_data;
        file.close();

        // Display all values
        // std::cout << "\n=== JSON Content ===" << std::endl;
        // std::cout << json_data.dump(4) << std::endl;

        // std::cout << "\n=== Parsed Values ===" << std::endl;



        if (json_data.contains("user_letters") && json_data["user_letters"].is_array())
        {
            availableLetters.clear();

            for (const auto& letter_json : json_data["user_letters"])
            {
                if (!letter_json.is_string())
                    continue;

                const std::string letter = letter_json.get<std::string>();
                if (letter.empty())
                    continue;

                const std::string normalized = normalizeBoardLetter(letter);
                if (normalized.size() == 1)
                    availableLetters.push_back(normalized[0]);
            }
        }

        if (json_data.contains("board") && json_data["board"].is_array())
        {
            const auto& board_json = json_data["board"];
            if (board_json.size() != gameboard_height) FATAL_ERROR("board height mismatch");

            for (size_t y = 0; y < board_json.size(); y++)
            {
                if (!board_json[y].is_array() || board_json[y].size() != gameboard_width) FATAL_ERROR("board row width mismatch");

                for (size_t x = 0; x < board_json[y].size(); x++)
                {
                    std::string letter;
                    if (board_json[y][x].is_string())
                    {
                        const std::string s = board_json[y][x].get<std::string>();
                        if (!s.empty() && s != " ")
                            letter = normalizeBoardLetter(s);
                    }
                    // tylko litera — mnożniki zostają z domyślnej planszy

                    mGameboard[y][x].current_letter = letter;
                }
            }
        }
    }


    #define pp(x) cout << x << " ";
    #define p(x) cout << x << "\n";

    void printGameBoard(const WordPositionOnAGameboard& currentBestWord)
    {
        auto copyGameboard = mGameboard;

        const int max_Y = static_cast<int>(copyGameboard.size());
        const int max_X = static_cast<int>(copyGameboard[0].size());

        int letter_index = 0;

        for(int y=-1; y < max_Y + 1; y++)
        {
            for(int x=-1; x < max_X + 1; x++)
            {
                if(y == -1 && x == -1) { pp("  "); continue; }
                if(y == -1 && x == max_X) { pp("  "); continue; }
                if(y == max_Y && x == -1) { pp("  "); continue; }
                if(y == max_Y && x == max_X) { pp("  "); continue; }

                if(y == -1)     { char c = 65 + x; pp(c); continue; }
                if(y == max_Y)  { char c = 65 + x; pp(c); continue; }

                if(x == -1)     { int val = y+1; if(val<10) pp(""); pp(y+1); continue; }
                if(x == max_X)  { int val = y+1; pp(y+1); continue; }

                const auto& letter = copyGameboard[y][x].current_letter;
                if(not letter.empty())
                {
                    pp(letter);
                    if(currentBestWord.isThisCordPresentInWord(xyCoord(x, y)))
                    {
                        letter_index += static_cast<int>(utf8CharLen(static_cast<unsigned char>(currentBestWord.letters[letter_index])));
                    }
                }
                else
                {
                    if(currentBestWord.isThisCordPresentInWord(xyCoord(x, y)))
                    {
                        const std::string placed_letter = currentBestWord.letters.substr(
                            letter_index,
                            utf8CharLen(static_cast<unsigned char>(currentBestWord.letters[letter_index])));
                        letter_index += static_cast<int>(placed_letter.size());

                        if (placed_letter.size() == 1)
                        {
                            pp(static_cast<char>(std::toupper(static_cast<unsigned char>(placed_letter[0]))));
                        }
                        else
                        {
                            pp(placed_letter);
                        }
                    }
                    else
                    {
                        pp(" ");
                    }
                }
            }
            p("");
        }
    }

    bool isWordPresentInDict(const std::string& word) const
    {
        return check_map.contains(word);
    }

    // pojedyncza litera nie tworzy słowa - sprawdzamy tylko sekwencje od 2 liter w górę
    bool isSequenceLegal(const std::string& sequence)
    {
        return (sequence.size() < 2) || isWordPresentInDict(sequence);
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

                if(letter.empty())
                {
                    // nothing OR word ended //

                    if(word_started) // word was started and now it ends //
                    {
                        word_started = false; // word ended


                        // got to check the current_word that accumulated //

                        if(not isSequenceLegal(current_word))
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

            if(word_started && not isSequenceLegal(current_word))
                return false;
        }

        // top -> bottom //
        for(int x=0; x<max_X; x++)
        {
            std::string current_word = "";
            bool word_started = false;

            for(int y=0; y<max_Y; y++)
            {
                const auto& letter = pMap[y][x].current_letter;

                if(letter.empty())
                {
                    // nothing OR word ended //

                    if(word_started) // word was started and now it ends //
                    {
                        word_started = false; // word ended


                        // got to check the current_word that accumulated //

                        if(not isSequenceLegal(current_word))
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

            if(word_started && not isSequenceLegal(current_word))
                return false;
        }

        return true;
    }

    std::optional<i8> returnClampedCoordToBoard_Y(const i8 y, const i8 modifier)
    {
        i8 result = y + modifier;

        if(0 <= result && result < gameboard_height) return result;

        return std::nullopt;
    }

    std::optional<i8> returnClampedCoordToBoard_X(const i8 x, const i8 modifier)
    {
        i8 result = x + modifier;

        if(0 <= result && result < gameboard_width) return result;

        return std::nullopt;
    }

    static std::string wordPositionKey(const WordPositionOnAGameboard& word)
    {
        return std::to_string(word.start.x) + "," + std::to_string(word.start.y)
             + "-" + std::to_string(word.end.x) + "," + std::to_string(word.end.y);
    }

    std::optional<WordPositionOnAGameboard> getPerpendicularWordAt(
        const gameboard_t& pMap, const i8 x, const i8 y, const bool main_is_horizontal) const
    {
        if(pMap[y][x].current_letter.empty()) return std::nullopt;

        WordPositionOnAGameboard word;

        if(main_is_horizontal)
        {
            i8 y_min = y;
            while((0 < y_min) && not pMap[y_min - 1][x].current_letter.empty()) y_min--;

            i8 y_max = y;
            while((y_max < gameboard_height - 1) && not pMap[y_max + 1][x].current_letter.empty()) y_max++;

            if(y_min == y_max) return std::nullopt;

            word.start.x = x;
            word.start.y = y_min;
            word.end.x = x;
            word.end.y = y_max;
        }
        else // main word is vertical
        {
            i8 x_min = x;
            while((0 < x_min) && not pMap[y][x_min - 1].current_letter.empty()) x_min--;

            i8 x_max = x;
            while((x_max < gameboard_width - 1) && not pMap[y][x_max + 1].current_letter.empty()) x_max++;

            if(x_min == x_max) return std::nullopt;

            word.start.x = x_min;
            word.start.y = y;
            word.end.x = x_max;
            word.end.y = y;
        }

        word.generateLetters(pMap);
        return word;
    }

    static i8 effectiveMultiplier(const i8 multiplier)
    {
        return (multiplier < 2) ? 1 : multiplier;
    }

    bool shouldScoreCrossWord(const gameboard_t& pMap, const WordPositionOnAGameboard& word) const
    {
        // Score a perpendicular word only when this turn placed at least one of its letters,
        // and the letters that were already on the board did not already form a legal word.

        bool has_fresh = false;
        std::string old_subword;

        if(word.isWordLeftRight())
        {
            const i8 y = word.start.y;

            for(i8 x = word.start.x; x <= word.end.x; x++)
            {
                if(pMap[y][x].fleshlyAdded) has_fresh = true;
                else old_subword += pMap[y][x].current_letter;
            }
        }
        else if(word.isWordTopDown())
        {
            const i8 x = word.start.x;

            for(i8 y = word.start.y; y <= word.end.y; y++)
            {
                if(pMap[y][x].fleshlyAdded) has_fresh = true;
                else old_subword += pMap[y][x].current_letter;
            }
        }
        else
        {
            return false;
        }

        if(not has_fresh) return false;
        if(old_subword.size() < 2) return true;

        return not isWordPresentInDict(old_subword);
    }

    i16 scoreWordOnMap(gameboard_t& pMap, const WordPositionOnAGameboard& word)
    {
        i16 singleWordScore = 0;
        i16 wholeWordMultiplyer = 1;

        if(word.isWordLeftRight())
        {
            const i8 y = word.start.y;

            for(i8 x = word.start.x; x <= word.end.x; x++)
            {
                auto& cell = pMap[y][x];

                if(cell.current_letter.empty()) FATAL_ERROR("empty cell in horizontal word");

                const i8 letter_multiplier = cell.fleshlyAdded
                    ? effectiveMultiplier(cell.multiplyer_letter)
                    : 1;
                singleWordScore += (letter_multiplier * letter_values.at(cell.current_letter));

                if(cell.fleshlyAdded)
                {
                    const i8 word_multiplier = effectiveMultiplier(cell.multiplyer_word);
                    if(word_multiplier > 1) wholeWordMultiplyer *= word_multiplier;
                }

                if(not cell.fleshlyAdded)
                {
                    cell.multiplyer_letter = 1;
                    cell.multiplyer_word = 1;
                }
            }
        }
        else if(word.isWordTopDown())
        {
            const i8 x = word.start.x;

            for(i8 y = word.start.y; y <= word.end.y; y++)
            {
                auto& cell = pMap[y][x];

                if(cell.current_letter.empty()) FATAL_ERROR("empty cell in vertical word");

                const i8 letter_multiplier = cell.fleshlyAdded
                    ? effectiveMultiplier(cell.multiplyer_letter)
                    : 1;
                singleWordScore += (letter_multiplier * letter_values.at(cell.current_letter));

                if(cell.fleshlyAdded)
                {
                    const i8 word_multiplier = effectiveMultiplier(cell.multiplyer_word);
                    if(word_multiplier > 1) wholeWordMultiplyer *= word_multiplier;
                }

                if(not cell.fleshlyAdded)
                {
                    cell.multiplyer_letter = 1;
                    cell.multiplyer_word = 1;
                }
            }
        }
        else { FATAL_ERROR("invalid word orientation"); }

        return singleWordScore * wholeWordMultiplyer;
    }

    std::vector<WordPositionOnAGameboard> getNewlyCreatedWords(const gameboard_t &pMap, const WordPositionOnAGameboard addedLetters)
    {
        std::vector<WordPositionOnAGameboard> ret;
        ret.push_back(addedLetters);

        const bool main_is_horizontal = addedLetters.isWordLeftRight();
        if(not main_is_horizontal && not addedLetters.isWordTopDown())
        {
            FATAL_ERROR("invalid word orientation");
        }

        std::set<std::string> seen_words;
        seen_words.insert(wordPositionKey(addedLetters));

        if(main_is_horizontal)
        {
            for(i8 x = addedLetters.start.x; x <= addedLetters.end.x; x++)
            {
                const i8 y = addedLetters.start.y;
                const auto cross_word = getPerpendicularWordAt(pMap, x, y, true);
                if(not cross_word) continue;
                if(not shouldScoreCrossWord(pMap, cross_word.value())) continue;

                const auto key = wordPositionKey(cross_word.value());
                if(seen_words.contains(key)) continue;

                seen_words.insert(key);
                ret.push_back(cross_word.value());
            }
        }
        else
        {
            for(i8 y = addedLetters.start.y; y <= addedLetters.end.y; y++)
            {
                const i8 x = addedLetters.start.x;
                const auto cross_word = getPerpendicularWordAt(pMap, x, y, false);
                if(not cross_word) continue;
                if(not shouldScoreCrossWord(pMap, cross_word.value())) continue;

                const auto key = wordPositionKey(cross_word.value());
                if(seen_words.contains(key)) continue;

                seen_words.insert(key);
                ret.push_back(cross_word.value());
            }
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

        gameboard_t scoringMap = pMap;

        i16 sum = 0;
        for(const auto& word : getNewlyCreatedWords(scoringMap, addedLetters))
        {
            sum += scoreWordOnMap(scoringMap, word);
        }

        return sum;
    }



    std::vector<xyCoord> getAllPositionsToCheck(const gameboard_t &pMap)
    {
        std::set<xyCoord> uniqueCoords;

        // going to every cell with letter and goint around it adding all empty cells

        const auto max_Y = pMap.size();
        const auto max_X = pMap[0].size();

        bool wholeMapIsEmpty = true;

        for(int y=0; y < max_Y; y++) for(int x=0; x < max_X; x++)
        {
            const auto& cell = pMap[y][x];

            if(not cell.current_letter.empty())
            {
                wholeMapIsEmpty = false;
                // teraz idziemy na około niej góra-dół-lewo-prawo

                for(int yy=y-1; yy <= y+1; yy++) for(int xx=x-1; xx <= x+1; xx++)
                {
                    if(not (0 <= yy && yy < gameboard_height)) continue;
                    if(not (0 <= xx && xx < gameboard_width)) continue;
                    if(yy == y && xx == x) continue;
                    if(std::abs(yy - y) == 1 && std::abs(xx - x) == 1) continue;

                    const auto& check_cell = pMap[yy][xx];

                    if(check_cell.current_letter.empty())
                    {
                        // we add empty cells

                        uniqueCoords.insert(xyCoord{static_cast<i8>(xx), static_cast<i8>(yy)});
                    }
                }
            }
        }

        if(wholeMapIsEmpty)
        {
            uniqueCoords.insert(xyCoord{static_cast<i8>(7), static_cast<i8>(7)}); // if empty we add the middle one
        }

        return std::vector<xyCoord>(uniqueCoords.begin(), uniqueCoords.end());
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
                        int x = std::clamp(static_cast<int>(pos.x) - distance, 0, static_cast<int>(gameboard_width) - 1);
                        int x_end = std::clamp(static_cast<int>(pos.x), 0, static_cast<int>(gameboard_width) - 1);
                        int letters_index = 0;

                        WordPositionOnAGameboard placedWord;
                        placedWord.start.x = x;
                        placedWord.start.y = y;

                        for(;(x <= x_end) && (x < gameboard_width); x++)
                        {
                            // -> now lets place the letters //

                            if(not gameboardCopy[y][x].current_letter.empty()) // cell occupied //
                            {
                                x_end++;
                                placedWord.letters += gameboardCopy[y][x].current_letter;
                                continue;
                            }
                            else
                            {
                                const auto letter = letters[letters_index ++];

                                gameboardCopy[y][x].current_letter = std::string(1, letter);
                                gameboardCopy[y][x].fleshlyAdded = true;
                                placedWord.letters += letter;
                            }
                        }

                        placedWord.end.x = x - 1;
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
                        int y = std::clamp(static_cast<int>(pos.y) - distance, 0, static_cast<int>(gameboard_height) - 1);
                        int y_end = std::clamp(static_cast<int>(pos.y), 0, static_cast<int>(gameboard_height) - 1);
                        int letters_index = 0;

                        WordPositionOnAGameboard placedWord;
                        placedWord.start.x = x;
                        placedWord.start.y = y;

                        for(;(y <= y_end) && (y < gameboard_height); y++)
                        {
                            // -> now lets place the letters //

                            if(not gameboardCopy[y][x].current_letter.empty()) // cell occupied //
                            {
                                y_end++;
                                placedWord.letters += gameboardCopy[y][x].current_letter;
                                continue;
                            }
                            else
                            {
                                const auto letter = letters[letters_index ++];

                                gameboardCopy[y][x].current_letter = std::string(1, letter);
                                gameboardCopy[y][x].fleshlyAdded = true;
                                placedWord.letters += letter;
                            }
                        }

                        placedWord.end.x = x;
                        placedWord.end.y = y - 1;

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

        if(currentBestEvaluation <= 0)
        {
            p("No valid move found.");
            return;
        }

        line("Found it");
        varr((int)currentBestWord.start.x);
        var((int)currentBestWord.start.y);
        varr((int)currentBestWord.end.x);
        var((int)currentBestWord.end.y);

        var((int)currentBestEvaluation);

        var(currentBestWord.letters);

        printGameBoard(currentBestWord);
    }
};



#ifdef BUILD_EXECUTABLE
int main(int argc, char* argv[])
{
    time_stamp("main starting");

    GameState game;
    game.goingOverAllPossibleCombinations();

    return 0;
}
#endif