#include <iostream>
#include "Trivia.h"
#include <lamarrr/chalk.h>
#include <aelliixx/inquirer.h>
#include <cstdlib>

#ifdef __linux__
#define CLEAR "clear"
#else
#define CLEAR "cls"
#endif

int main() {
    Trivia trivia;

    while(true) {
        system(CLEAR);

        std::cout << chalk::fg::BrightCyan("Welcome to Trivia Night!") << std::endl;
        std::cout << std::endl;
        std::cout << std::endl;

        auto menuInquirer = alx::Inquirer();

        menuInquirer.add_question({"menu", "What do you want to do?",
                                   {"Start a new game", "Game settings", "Quit"}});
        menuInquirer.ask();

        std::string menuAnswer = menuInquirer.answer("menu");

        if(menuAnswer == "Start a new game") {
            system(CLEAR);

            auto startGameInquirer = alx::Inquirer();
            startGameInquirer.add_question({"startGame", "Are we good to go?", alx::Type::yesNo});

            std::cout << "Starting game with following parameters:" << std::endl;
            std::cout << std::endl;

            trivia.printCurrentGameParameters();

            std::cout << std::endl;

            startGameInquirer.ask();

            std::string startGameAnswer = startGameInquirer.answer("startGame");

            if(startGameAnswer == "yes") {
                // TODO: Start game
            }
        } else if(menuAnswer == "Game settings") {
            while(true) {
                system(CLEAR);

                trivia.printCurrentGameParameters();
                std::cout << std::endl;

                auto settingsInquirer = alx::Inquirer();

                settingsInquirer.add_question({"settings", "Choose a parameter to change:",
                                               { "Change category", "Change difficulty", "Adjust amount", "Return"}});
                settingsInquirer.ask();

                std::string settingsAnswer = settingsInquirer.answer("settings");

                if(settingsAnswer == "Return") {
                    break;
                }

                if(settingsAnswer == "Change category") {
                    auto categoryInquirer = alx::Inquirer();

                    std::vector<std::string> categoryOptions;
                    categoryOptions.emplace_back("All");
                    trivia.getAllCategories(categoryOptions);

                    categoryInquirer.add_question({"category", "Choose the category:", categoryOptions});
                    categoryInquirer.ask();

                    std::string categoryAnswer = categoryInquirer.answer("category");

                    trivia.setCurrentCategory(categoryAnswer);
                } else if(settingsAnswer == "Change difficulty") {
                    auto difficultyInquirer = alx::Inquirer();

                    difficultyInquirer.add_question({"difficulty", "Choose the difficulty:", Trivia::difficulties});
                    difficultyInquirer.ask();

                    std::string difficultyAnswer = difficultyInquirer.answer("difficulty");

                    trivia.setCurrentDifficulty(difficultyAnswer);
                } else if(settingsAnswer == "Adjust amount") {
                    auto amountInquirer = alx::Inquirer();

                    amountInquirer.add_question({"amount", "Enter the number of questions (1 - 50):", alx::Type::integer});
                    amountInquirer.ask();

                    int amountAnswer = stoi(amountInquirer.answer("amount"));

                    trivia.setCurrentQuestionAmount(amountAnswer);
                }
            }
        } else {
            return 0;
        }
    }

    // You should NEVER exit the main function through here
    return 1;
}
