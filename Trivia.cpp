#include "Trivia.h"
#include <iostream>
#include <curl/curl.h>
#include <lamarrr/chalk.h>

size_t writeFn(void* content, size_t size, size_t nmemb, std::string* output) {
    size_t totalSize = size * nmemb;
    output->append(static_cast<char*>(content), totalSize);
    return totalSize;
}

void prepareCurl(CURL* curl) {
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
}

Trivia::Trivia() {
    fetchCategories();
}

std::vector<std::string> Trivia::difficulties = {"Any", "Easy", "Medium", "Hard"};

bool Trivia::fetchCategories() {
    const std::string reqUrl = API_BASE + API_QUESTION_CATEGORY;

    std::cout << reqUrl << std::endl;

    CURL* curl = curl_easy_init();
    std::string response;

    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, reqUrl.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFn);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        prepareCurl(curl);

        CURLcode result = curl_easy_perform(curl);

        if(result == CURLE_OK) {
            long httpCode;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
            std::clog << "Response code: " << httpCode << std::endl;
            std::clog << "Response body: " << response << std::endl;

            _categories = std::move(json::parse(response));

            curl_easy_cleanup(curl);
            return true;
        } else {
            std::cerr << "Request failure: " << curl_easy_strerror(result) << std::endl;
        }

        std::clog << "Result code: " << result << std::endl;
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
    if(_currentCategoryId == -1) {
        return false;
    }

    const std::string reqUrl = API_BASE + API_QUESTION_COUNT + "?category=" + std::to_string(_currentCategoryId);

    std::cout << reqUrl << std::endl;

    CURL* curl = curl_easy_init();
    std::string response;

    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, reqUrl.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFn);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        prepareCurl(curl);

        CURLcode result = curl_easy_perform(curl);

        if(result == CURLE_OK) {
            long httpCode;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
            std::clog << "Response code: " << httpCode << std::endl;
            std::clog << "Response body: " << response << std::endl;

            _currentCategoryInfo = std::move(json::parse(response));

            curl_easy_cleanup(curl);
            return true;
        } else {
            std::cerr << "Request failure: " << curl_easy_strerror(result) << std::endl;
        }

        std::clog << "Result code: " << result << std::endl;
    } else {
        std::cerr << "Failed to initialize curl library" << std::endl;
    }

    curl_easy_cleanup(curl);
    return false;
}

void Trivia::setCurrentCategory(const std::string& categoryName) {
    if(_categories == nullptr) {
        return;
    }

    if(categoryName == "All") {
        _currentCategoryId = -1;
        _currentCategoryName = "All";

        resetCurrentCategoryInfo();
        return;
    }

    for(auto &el: _categories["trivia_categories"]) {
        if(el["name"].template get<std::string>() == categoryName) {
            _currentCategoryId = el["id"].template get<int>();
            _currentCategoryName = categoryName;
            break;
        }
    }

    fetchCurrentCategoryInfo();
}

void Trivia::getAllCategories(std::vector<std::string>& v) const {
    if(_categories == nullptr) {
        return;
    }

    for(auto &el: _categories["trivia_categories"]) {
        v.push_back(el["name"].template get<std::string>());
    }
}

bool Trivia::getCurrentCategoryCount(std::vector<int> &v) const {
    if(_categories == nullptr || _currentCategoryInfo == nullptr || _currentCategoryId == -1) {
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
    if(questionAmount < 1 || questionAmount > 50) {
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

    if(currentCategoryCountSuccessful) {
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
    const chalk::ForegroundColor* fgc = &chalk::fg::Cyan;

    if(difficultyName == "Easy") { fgc = &chalk::fg::BrightGreen;}
    else if(difficultyName == "Medium") { fgc = &chalk::fg::BrightYellow;}
    else if(difficultyName == "Hard") { fgc = &chalk::fg::BrightRed;}

    std::cout << "Current difficulty: " << (*fgc)(difficultyName) << std::endl;

    std::cout << "Amount of questions: " << chalk::fg::Cyan(std::to_string(getCurrentQuestionAmount())) << std::endl;
}


