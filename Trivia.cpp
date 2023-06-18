#include "Trivia.h"
#include <iostream>
#include <curl/curl.h>

size_t writeFn(void* content, size_t size, size_t nmemb, std::string* output) {
    size_t totalSize = size * nmemb;
    output->append(static_cast<char*>(content), totalSize);
    return totalSize;
}

void prepareCurl(CURL* curl) {
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
}

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

void Trivia::setCurrentCategory(int categoryId) {
    if(_categories == nullptr) {
        return;
    }

    if(categoryId == -1) {
        _currentCategoryId = -1;
        return;
    }

    for(auto &el: _categories["trivia_categories"]) {
        if(el["id"].template get<int>() == categoryId) {
            _currentCategoryId = categoryId;
            return;
        }
    }
}

std::string Trivia::getCurrentCategoryName() {
    std::string categoryName;

    if(_currentCategoryId == -1) {
        categoryName = "All";
    }

    for(auto &el: _categories["trivia_categories"]) {
        if(el["id"].template get<int>() == _currentCategoryId) {
            categoryName = el["name"].template get<std::string>();
            break;
        }
    }

    return categoryName;
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

            _currentCategoryCount = std::move(json::parse(response));

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
        return;
    }

    for(auto &el: _categories["trivia_categories"]) {
        if(el["name"].template get<std::string>() == categoryName) {
            _currentCategoryId = el["id"].template get<int>();
            break;
        }
    }
}

void Trivia::getAllCategories(std::vector<std::string>& v) {
    if(_categories == nullptr) {
        return;
    }

    for(auto &el: _categories["trivia_categories"]) {
        v.push_back(el["name"].template get<std::string>());
    }
}
