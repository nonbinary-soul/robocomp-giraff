#ifndef GLOBALS_H
#define GLOBALS_H

#include <opencv2/opencv.hpp>
#include <random>
#include <string>
#include <algorithm>
#include <cctype>

class Globals {
public:
    // variables
    static const int res_x;
    static const int res_y;
    static const float fact_x;
    static const float fact_y;
    static const float interpolation_speed;
    static const float progress_increment;

    // colors
    static const cv::Scalar WHITE;

    /**
     * @brief Generates a random integer within a specified range.
     *
     * This function generates a random integer using the C++ standard random library:
     * - It uses `std::random_device` to seed the random number generator.
     * - It uses `std::mt19937` (a Mersenne Twister engine) to generate high-quality random numbers.
     * - A `std::uniform_int_distribution` is used to ensure the number is uniformly distributed within the specified range.
     *
     * Example:
     *   - `randInt(1, 10)` will return a random integer between 1 and 10 (inclusive).
     *
     * @param min The minimum value (inclusive) of the generated random integer.
     * @param max The maximum value (inclusive) of the generated random integer.
     * @return A random integer between `min` and `max`, inclusive.
     */
    static int randInt(int min, int max);

    /**
     * @brief Converts a string to snake_case format.
     *
     * This function converts a given string to `snake_case` by:
     * - Converting all uppercase letters to lowercase.
     * - Inserting underscores (`_`) before each uppercase letter that is converted to lowercase.
     *
     * It handles the conversion safely by ensuring the characters are cast to `unsigned char` before passing them to `std::tolower` and `std::isupper`.
     * This avoids narrowing conversion issues that may arise with signed `char` types.
     *
     * @param str The input string to be converted to snake_case.
     * @return A new string converted to snake_case format.
     */
    static std::string to_snake_case(const std::string &str);
};

#endif //GLOBALS_H
