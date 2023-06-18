#ifndef TRIVIA_NIGHT_TRIVIA_H
#define TRIVIA_NIGHT_TRIVIA_H

#include <string>
#include <nlohmann/json.hpp>

const std::string API_BASE = "https://opentdb.com/";
const std::string  API_QUESTION = "api.php";
const std::string  API_QUESTION_CATEGORY = "api_category.php";
const std::string  API_QUESTION_COUNT = "api_count.php";

using json = nlohmann::json;

class Trivia {
public:
    Trivia();
    ~Trivia() = default;

    void getAllCategories(std::vector<std::string>& v);
    void setCurrentCategory(const std::string& categoryName);
    std::string getCurrentCategoryName();
    bool getCurrentCategoryCount(std::vector<int>& v);

private:
    bool fetchCategories();
    bool fetchCurrentCategoryInfo();
    void resetCurrentCategoryInfo();

private:
    json _categories = nullptr;
    int _currentCategoryId = -1;
    std::string _currentCategoryName;
    json _currentCategoryInfo = nullptr;
};

#endif //TRIVIA_NIGHT_TRIVIA_H
