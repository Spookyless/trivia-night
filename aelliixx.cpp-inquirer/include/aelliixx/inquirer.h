// Copyright © 2023 Donatas Mockus
// https://github.com/aelliixx/cpp-inquirer/
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
// documentation files (the “Software”), to deal in the Software without restriction, including without limitation 
// the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and
// to permit persons to whom the Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or substantial portions of
// the Software.
//
// THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//
// Modified by Spookyless


#ifndef CPP_INQUIRER_SRC_INQUIRER_H
#define CPP_INQUIRER_SRC_INQUIRER_H

#include <iostream>
#include <unordered_map>
#include <string>
#include <utility>
#include <vector>
#include <regex>
#include <variant>
#include <cassert>
#include <lamarrr/chalk.h>

#if __linux__
#else
#include <conio.h>
#include <windows.h>

#define WK_DOWN 80
#define WK_UP 72
#define WK_LEFT 75
#define WK_RIGHT 77
#endif

namespace alx {
    class Inquirer;

    enum class Type {
        text = 0,
        integer = 1,
        decimal = 2,
        yesNo = 3,
        confirm = 4,
        options = 5
    };

    struct Question {
        Question(const Question &q) {
            key = q.key;
            question = q.question;
            answer = q.answer;
            type = q.type;

            if (type == Type::options)
                options = q.options;
        }

        Question(Question &&q) noexcept {
            key = std::move(q.key);
            question = std::move(q.question);
            answer = std::move(q.answer);
            type = q.type;

            if (type == Type::options)
                options = std::move(q.options);
        }

        Question(std::string key, std::string question, Type type = Type::text)
                : key(std::move(key)),
                  question(std::move(question)),
                  type(type) {}

        Question(std::string key, std::string question, std::vector<std::string> options)
                : key(std::move(key)),
                  question(std::move(question)),
                  type(Type::options),
                  options(std::move(options)) {
            assert(!this->options.empty() && "Must have one or more options");
        }

        ~Question() = default;

    private:
        friend Inquirer;
        std::string key;
        std::string question;
        std::string answer;
        Type type;
        std::vector<std::string> options;
    };

    class Inquirer {
        std::vector<std::pair<std::string, Question>> m_questions;
        std::string m_title;
    public:
        Inquirer() = default;

        explicit Inquirer(std::string title) : m_title(std::move(title)) {}

        ~Inquirer() = default;

        void add_question(const Question &question) {
            m_questions.emplace_back(question.key, question);
        }

        void ask() {
            if (!m_title.empty()) {
                std::cout << chalk::fg::Cyan(">") << m_title << '\n';
            }

            for (auto &question: m_questions) {
                auto &q = question.second;

                auto printQuestion = [&](const std::string &append = "") {
                    std::cout << chalk::fg::Cyan("?") << " " << chalk::fmt::Bold(q.question) << " " << append;
                };

                auto takeInput = [](std::string &destination) {
                    std::cout << chalk::fg::Cyan.esc() << chalk::fg::Cyan.rep();
                    std::getline(std::cin, destination);
                    std::cout << chalk::fg::Cyan.esc() << chalk::fg::Cyan.terminator();
                };

                switch (q.type) {
                    case Type::confirm: {
                        printQuestion("(y/N) ");
                        std::string answer;
                        takeInput(answer);
                        while (!(answer == "y" || answer == "Y" || answer == "n" || answer == "N")) {
                            erase_lines(2);
                            printQuestion("(y/N) ");
                            takeInput(answer);
                        }
                        q.answer = answer;
                        break;
                    }
                    case Type::text:
                        printQuestion();
                        takeInput(q.answer);
                        break;
                    case Type::integer: {
                        printQuestion();
                        std::string answer;
                        takeInput(answer);
                        while (!is_integer(answer)) {
                            erase_lines(2);
                            printQuestion();
                            takeInput(answer);
                        }
                        q.answer = answer;
                        break;
                    }
                    case Type::decimal: {
                        printQuestion();
                        std::string answer;
                        takeInput(answer);
                        while (!is_decimal(answer)) {
                            erase_lines(2);
                            printQuestion();
                            takeInput(answer);
                        }
                        q.answer = answer;
                        break;
                    }
                    case Type::yesNo: {
                        const std::string yes = chalk::fg::Cyan("yes") + " " + "no" + "\n";
                        const std::string no = std::string("yes") + " " + chalk::fg::Cyan("no") + "\n";

                        std::cout << std::flush;
                        printQuestion(yes);

                        int key;
                        bool position = true;

                        while (true) {
                            key = getch();
                            if (key == 89 || key == 121 || key == 37) {
                                position = true;
                                erase_lines(2);
                                printQuestion(yes);
                            } else if (key == 78 || key == 110 || key == 39) {
                                position = false;
                                erase_lines(2);
                                printQuestion(no);
                            }

                            if (key == 13) {
                                q.answer = position ? "yes" : "no";
                                break;
                            }
                        }
                        break;
                    }
                    case Type::options: {
                        int key;
                        unsigned int selectedIndex = 0;
                        auto printOptions = [&]() {
                            std::cout << '\n';
                            for (int i = 0; i < q.options.size(); ++i) {
                                if (i == selectedIndex) {
                                    std::cout << chalk::fg::Cyan("> " + q.options.at(i)) + "\n";
                                } else {
                                    std::cout << "  " << q.options.at(i) << "\n";
                                }
                            }
                        };

                        printQuestion();
                        printOptions();

                        while (true) {
                            key = getch();
                            if (key == 66 || key == 's') {
                                selectedIndex = wrap_int((int) selectedIndex + 1, 0, q.options.size() - 1);
                                erase_lines(q.options.size() + 2);
                                printQuestion();
                                printOptions();
//                            } else if (key == 65 || key == VK_UP || key == WK_UP) {
                            } else if (key == 65 || key == 'w') {
                                selectedIndex = wrap_int((int) selectedIndex - 1, 0, q.options.size() - 1);
                                erase_lines(q.options.size() + 2);
                                printQuestion();
                                printOptions();
                            }
                            if (key == 13 || key == 'F') {
                                q.answer = q.options.at(selectedIndex);
                                erase_lines(q.options.size() + 2);
                                printQuestion(chalk::fg::Cyan(q.options.at(selectedIndex)) + "\n");
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        }

        void print_questions() const {
            for (const auto &q: m_questions) {
                std::cout << q.second.question << " " << static_cast<int>(q.second.type) << '\n';
            }
        }

        void print_answers() const {
            for (const auto &q: m_questions)
                std::cout << q.second.question << ": " << q.second.answer << '\n';
        }

        [[nodiscard]] std::string answer(const std::string &key) const {
            for (const auto &question: m_questions)
                if (question.first == key)
                    return question.second.answer;
            return "";
        }

    private:
        static void erase_lines(unsigned int count = 1) {
            if (count == 0)
                return;

            std::cout << "\x1b[2K"; // Delete current line
            for (int i = 1; i < count; ++i) {
                std::cout << "\x1b[1A" // Move cursor one line up
                          << "\x1b[2K"; // Delete current line
            }
            std::cout << '\r';
        }

        static bool is_integer(const std::string &string) {
            if (string.empty() || ((!isdigit(string[0])) && (string[0] != '-') && (string[0] != '+'))) return false;
            char *p;
            strtol(string.c_str(), &p, 10);
            return (*p == 0);
        }

        static bool is_decimal(const std::string &string) {
            if (string.empty() || ((!isdigit(string[0])) && (string[0] != '-') && (string[0] != '+'))) return false;
            char *p;
            strtod(string.c_str(), &p);
            return (*p == 0);
        }

        static unsigned int wrap_int(int k, unsigned int lowerBound, unsigned int upperBound) {
            unsigned int rangeSize = upperBound - lowerBound + 1;

            int wrap = (k - (int) lowerBound) % (int) rangeSize + (int) lowerBound;

            if (wrap < 0) {
                wrap += (int) rangeSize;
            }

            return wrap;
        }

        // This function should return the keystroke without allowing it to echo on screen
        static int getch() {
            int c;

            system("stty raw");    // Raw input - wait for only a single keystroke
            system("stty -echo");  // Echo off
            c = getchar();
            system("stty cooked"); // Cooked input - reset
            system("stty echo");   // Echo on - Reset

            return c;
        }

    };

}
#endif //CPP_INQUIRER_SRC_INQUIRER_H
