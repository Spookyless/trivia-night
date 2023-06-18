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

    void getAllCategories(std::vector<std::string>& v) const;
    void setCurrentCategory(const std::string& categoryName);
    std::string getCurrentCategoryName() const;
    bool getCurrentCategoryCount(std::vector<int>& v) const;
    void setCurrentDifficulty(const std::string& difficultyName);
    std::string getCurrentDifficultyName() const;
    void setCurrentQuestionAmount(int questionAmount);
    int getCurrentQuestionAmount() const;
    void printCurrentGameParameters() const;

private:
    bool fetchCategories();
    bool fetchCurrentCategoryInfo();
    void resetCurrentCategoryInfo();

public:
    static std::vector<std::string> difficulties;

private:
    json _categories = nullptr;
    json _currentCategoryInfo = nullptr;
    int _currentCategoryId = -1;
    std::string _currentCategoryName = "Any";
    std::string _currentDifficultyName = Trivia::difficulties[0];
    int _currentQuestionAmount = 20;
};

#endif //TRIVIA_NIGHT_TRIVIA_H
