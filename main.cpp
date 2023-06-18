#include <iostream>
#include "Trivia.h"
#include <lamarrr/chalk.h>
#include <aelliixx/inquirer.h>
#include <cstdlib>
#include "include/os_dependent.h"

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
                GameResults results;

                trivia.startGame(results);

                system(CLEAR);

                if(results.status != 0) {
                    std::cout << "Server couldn't process your game request" << std::endl;
                    std::cout << "Try adjusting your game settings" << std::endl;
                    std::cout << std::endl;
                    std::cout << "(press any button to return to main menu)" << std::endl;

                    OSDEP::getch();
                    continue;
                }

                std::cout << "That's all!" << std::endl;
                std::cout << std::endl;
                std::cout << "Here are your results:" << std::endl;
                std::cout << "  " << "You played \'" << chalk::fg::Cyan(results.category) << "\' on \'" << Trivia::colorDifficulty(results.difficulty) << "\' difficulty" << std::endl;
                std::cout << "  " << "You got " << chalk::fg::BrightWhite(std::to_string(results.correctAnswers) + "/" + std::to_string(results.totalQuestions)) << " right" << std::endl;
                std::cout << "  " << "That's " << std::setprecision(2) << results.correctness * 100 << "% correctness!" << std::endl;
                std::cout << std::endl;
                std::cout << "(press any button to return to main menu)" << std::endl;

                OSDEP::getch();
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
