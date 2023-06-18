#include <iostream>
#include "Trivia.h"
#include <lamarrr/chalk.h>
#include <aelliixx/inquirer.h>

int main() {
    Trivia trivia;

    std::cout << chalk::fmt::Bold("Hello") << std::endl;

    trivia.fetchCategories();

//    system("cls");

    std::cout << "Welcome to da game!" << std::endl;

    while(true) {
        auto menu_inquirer = alx::Inquirer();

        menu_inquirer.add_question({"menu", "What do you want to do?",
                                    {"Start a new game", "Game settings", "Quit"}});
        menu_inquirer.ask();

        std::string menu_answer = menu_inquirer.answer("menu");

        if(menu_answer == "Quit") {
            return 0;
        }

        if(menu_answer == "Game settings") {
//            system("cls");

            while(true) {
                std::cout << "Current category: " << chalk::fg::Cyan(trivia.getCurrentCategoryName()) << std::endl;
                std::cout << std::endl;

                auto settings_inquirer = alx::Inquirer();

                settings_inquirer.add_question({"settings", "Choose a parameter to change:",
                                                { "Change category", "Change difficulty", "Adjust amount", "Return"}});
                settings_inquirer.ask();

                std::string settings_answer = settings_inquirer.answer("settings");

                if(settings_answer == "Return") {
                    break;
                }

                if(settings_answer == "Change category") {
//                    system("cls");

                    auto category_inquirer = alx::Inquirer();

                    std::vector<std::string> category_options;
                    category_options.emplace_back("All");
                    trivia.getAllCategories(category_options);

                    category_inquirer.add_question({"category", "Choose the category:", category_options});
                    category_inquirer.ask();

                    std::string category_answer = category_inquirer.answer("category");

                    trivia.setCurrentCategory(category_answer);
                    trivia.fetchCurrentCategoryInfo();
                }
            }
        }
    }

    return -1;
}
