#ifndef TRIVIA_NIGHT_GAMERESULTS_H
#define TRIVIA_NIGHT_GAMERESULTS_H

#include <string>

struct GameResults {
    int totalQuestions;
    int correctAnswers;
    int wrongAnswers;
    float correctness;
    std::string category;
    std::string difficulty;
    int status;
};

#endif //TRIVIA_NIGHT_GAMERESULTS_H
