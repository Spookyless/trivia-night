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
    Trivia() = default;
    ~Trivia() = default;

    bool fetchCategories();
    void getAllCategories(std::vector<std::string>& v);
    void setCurrentCategory(int categoryId);
    void setCurrentCategory(const std::string& categoryName);
    std::string getCurrentCategoryName();
    bool fetchCurrentCategoryInfo();
private:
    json _categories = nullptr;
    int _currentCategoryId = -1;
    json _currentCategoryCount = nullptr;
};

#endif //TRIVIA_NIGHT_TRIVIA_H
