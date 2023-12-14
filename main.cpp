#include "Observer.h"
struct people{
    int age=0;
    double ll=0.0;
};
void printValue(people pp){
    std::cout << pp.age<<"  "<<pp.ll << std::endl;
}

void printValueBB(people pp){
    std::cout << pp.age<<"  "<<pp.ll << "BB"<< std::endl;
}

struct mat{
    std::string type="mat";
    int age=0;
};
void printValueAA(mat pp){
    std::cout << pp.age<<"  "<<pp.type << std::endl;
}


int main() {
    EventManager::getInstance().registerHandler<people>(std::bind(printValue, std::placeholders::_1));
    EventManager::getInstance().registerHandler<people>(std::bind(printValueBB, std::placeholders::_1));
    EventManager::getInstance().registerHandler<mat>(std::bind(printValueAA, std::placeholders::_1));
    people ljj;
    ljj.age =18;
    ljj.ll =100;
    EventManager::getInstance().post(ljj);

    mat m;
    m.type = "Mat";
    m.age = 1000;
    EventManager::getInstance().post(m);

    ljj.age =28;
    ljj.ll =200;
    EventManager::getInstance().post(ljj);

    EventManager::getInstance().unregisterHandler<people>();

    ljj.age =38;
    ljj.ll =300;
    EventManager::getInstance().post(ljj);
    return 0;
}

