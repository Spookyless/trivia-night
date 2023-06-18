#include "Trivia.h"
#include <iostream>
#include <curl/curl.h>
#include <lamarrr/chalk.h>
#include <aelliixx/inquirer.h>
#include <random>
#include "include/os_dependent.h"

size_t writeFn(void *content, size_t size, size_t nmemb, std::string *output) {
    size_t totalSize = size * nmemb;
    output->append(static_cast<char *>(content), totalSize);
    return totalSize;
}

void prepareCurl(CURL *curl) {
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
}

Trivia::Trivia() {
    fetchCategories();
}

std::vector<std::string> Trivia::difficulties = {"Any", "Easy", "Medium", "Hard"};

bool Trivia::fetchCategories() {
    const std::string reqUrl = API_BASE + API_QUESTION_CATEGORY;

    CURL *curl = curl_easy_init();
    std::string response;

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, reqUrl.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFn);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        prepareCurl(curl);

        CURLcode result = curl_easy_perform(curl);

        if (result == CURLE_OK) {
            long httpCode;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);

            _categories = std::move(json::parse(response));

            curl_easy_cleanup(curl);
            return true;
        } else {
            std::cerr << "Request failure: " << curl_easy_strerror(result) << std::endl;
        }
    } else {
        std::cerr << "Failed to initialize curl library" << std::endl;
    }

    curl_easy_cleanup(curl);
    return false;
}

std::string Trivia::getCurrentCategoryName() const {
    return _currentCategoryName;
}

bool Trivia::fetchCurrentCategoryInfo() {
    if (_currentCategoryId == -1) {
        return false;
    }

    const std::string reqUrl = API_BASE + API_QUESTION_COUNT + "?category=" + std::to_string(_currentCategoryId);

    CURL *curl = curl_easy_init();
    std::string response;

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, reqUrl.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFn);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        prepareCurl(curl);

        CURLcode result = curl_easy_perform(curl);

        if (result == CURLE_OK) {
            long httpCode;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);

            _currentCategoryInfo = std::move(json::parse(response));

            curl_easy_cleanup(curl);
            return true;
        } else {
            std::cerr << "Request failure: " << curl_easy_strerror(result) << std::endl;
        }
    } else {
        std::cerr << "Failed to initialize curl library" << std::endl;
    }

    curl_easy_cleanup(curl);
    return false;
}

void Trivia::setCurrentCategory(const std::string &categoryName) {
    if (_categories == nullptr) {
        return;
    }

    if (categoryName == "All") {
        _currentCategoryId = -1;
        _currentCategoryName = "All";

        resetCurrentCategoryInfo();
        return;
    }

    for (auto &el: _categories["trivia_categories"]) {
        if (el["name"].template get<std::string>() == categoryName) {
            _currentCategoryId = el["id"].template get<int>();
            _currentCategoryName = categoryName;
            break;
        }
    }

    fetchCurrentCategoryInfo();
}

void Trivia::getAllCategories(std::vector<std::string> &v) const {
    if (_categories == nullptr) {
        return;
    }

    for (auto &el: _categories["trivia_categories"]) {
        v.push_back(el["name"].template get<std::string>());
    }
}

bool Trivia::getCurrentCategoryCount(std::vector<int> &v) const {
    if (_categories == nullptr || _currentCategoryInfo == nullptr || _currentCategoryId == -1) {
        return false;
    }

    v.push_back(_currentCategoryInfo["category_question_count"]["total_question_count"].template get<int>());
    v.push_back(_currentCategoryInfo["category_question_count"]["total_easy_question_count"].template get<int>());
    v.push_back(_currentCategoryInfo["category_question_count"]["total_medium_question_count"].template get<int>());
    v.push_back(_currentCategoryInfo["category_question_count"]["total_hard_question_count"].template get<int>());

    return true;
}

void Trivia::resetCurrentCategoryInfo() {
    _currentCategoryInfo.clear();
    _currentCategoryInfo = nullptr;
}

void Trivia::setCurrentDifficulty(const std::string &difficultyName) {
    _currentDifficultyName = difficultyName;
}

std::string Trivia::getCurrentDifficultyName() const {
    return _currentDifficultyName;
}

void Trivia::setCurrentQuestionAmount(int questionAmount) {
    if (questionAmount < 1 || questionAmount > 50) {
        return;
    }

    _currentQuestionAmount = questionAmount;
}

int Trivia::getCurrentQuestionAmount() const {
    return _currentQuestionAmount;
}

void Trivia::printCurrentGameParameters() const {
    std::cout << "Current category: " << chalk::fg::Cyan(getCurrentCategoryName());

    std::vector<int> currentCategoryCount;
    bool currentCategoryCountSuccessful = getCurrentCategoryCount(currentCategoryCount);

    if (currentCategoryCountSuccessful) {
        std::cout << " - " << \
                    currentCategoryCount[0] << " questions " << "(" << \
                    chalk::fg::BrightGreen(std::to_string(currentCategoryCount[1])) << "/" << \
                    chalk::fg::BrightYellow(std::to_string(currentCategoryCount[2])) << "/" << \
                    chalk::fg::BrightRed(std::to_string(currentCategoryCount[3])) << ")" << \
                    std::endl;
    } else {
        std::cout << std::endl;
    }

    std::string difficultyName = getCurrentDifficultyName();
    std::cout << "Current difficulty: " << colorDifficulty(difficultyName) << std::endl;

    std::cout << "Amount of questions: " << chalk::fg::Cyan(std::to_string(getCurrentQuestionAmount())) << std::endl;
}

void Trivia::startGame(GameResults &gameResult) {
    int fetchResult = fetchQuestions();

    gameResult.status = fetchResult;
    gameResult.difficulty = _currentDifficultyName;
    gameResult.category = _currentCategoryName;

    if(fetchResult != 0) {
        return;
    }

    int totalQuestions = 0;
    for(auto &el: _questions["results"]) {
        ++totalQuestions;
    }

    int correctAnswers = 0;
    int currentQuestion = 1;
    for(auto &el: _questions["results"]) {
        std::string correctAnswer = el["correct_answer"].template get<std::string>();

        std::vector<std::string> answers;
        answers.push_back(correctAnswer);

        for(auto &eel: el["incorrect_answers"]) {
            answers.push_back(eel.template get<std::string>());
        }

        auto rng = std::default_random_engine {};
        std::shuffle(answers.begin(), answers.end(), rng );

        auto gameQuestionInquirer = alx::Inquirer();
        gameQuestionInquirer.add_question({"question", el["question"], answers});

        system(CLEAR);

        std::cout << "Question " << chalk::fg::BrightWhite(std::to_string(currentQuestion) + "/" + std::to_string(totalQuestions)) << std::endl;
        std::cout << std::endl;
        std::cout << chalk::fg::BrightWhite(el["category"].template get<std::string>()) << " (" << Trivia::colorDifficulty(el["difficulty"].template get<std::string>()) << ")" << std::endl;
        std::cout << std::endl;
        std::cout << std::endl;

        gameQuestionInquirer.ask();

        std::string gameQuestionAnswer = gameQuestionInquirer.answer("question");

        if(gameQuestionAnswer == correctAnswer) {
            std::cout << chalk::fg::BrightGreen("Correct!") << std::endl;

            ++correctAnswers;
        } else {
            std::cout << chalk::fg::BrightRed("That doesn't seem right :(") << std::endl;
            std::cout << "The correct answer was: " << chalk::fg::BrightGreen(correctAnswer) << std::endl;
        }

        std::cout << std::endl;
        std::cout << "(press any button to continue)" << std::endl;

        OSDEP::getch();

        ++currentQuestion;
    }

    gameResult.correctAnswers = correctAnswers;
    gameResult.totalQuestions = totalQuestions;
    gameResult.wrongAnswers = totalQuestions - correctAnswers;
    gameResult.correctness = (float)correctAnswers / (float)totalQuestions;

    resetQuestions();
}

int Trivia::fetchQuestions() {
    std::string reqUrl = API_BASE + API_QUESTION;
    reqUrl += "?amount=" + std::to_string(getCurrentQuestionAmount());
    reqUrl += "&type=multiple";

    if (_currentCategoryId != -1) {
        reqUrl = reqUrl + "&category=" + std::to_string(_currentCategoryId);
    }

    if (_currentDifficultyName != difficulties[0]) {
        std::string cdn = _currentDifficultyName;
        std::transform(cdn.begin(), cdn.end(), cdn.begin(), ::tolower);
        reqUrl += "&difficulty" + cdn;
    }

    CURL *curl = curl_easy_init();
    std::string response;

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, reqUrl.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFn);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        prepareCurl(curl);

        CURLcode result = curl_easy_perform(curl);

        if (result == CURLE_OK) {
            long httpCode;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);

            _questions = std::move(json::parse(response));

            curl_easy_cleanup(curl);
            return _questions["response_code"].template get<int>();
        } else {
            std::cerr << "Request failure: " << curl_easy_strerror(result) << std::endl;
        }
    } else {
        std::cerr << "Failed to initialize curl library" << std::endl;
    }

    curl_easy_cleanup(curl);
    return false;
}

void Trivia::resetQuestions() {
    _questions.clear();
    _questions = nullptr;
}

std::string Trivia::colorDifficulty(const std::string &difficultyName) {
    const chalk::ForegroundColor *fgc = &chalk::fg::Cyan;

    if (difficultyName == "Easy" || difficultyName == "easy") { fgc = &chalk::fg::BrightGreen; }
    else if (difficultyName == "Medium" || difficultyName == "medium") { fgc = &chalk::fg::BrightYellow; }
    else if (difficultyName == "Hard" || difficultyName == "hard") { fgc = &chalk::fg::BrightRed; }

    return (*fgc)(difficultyName);
}
