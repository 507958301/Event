#include "Observer.h"
struct people{
    int age=0;
    double ll=0.0;
};
void printValue(people pp){
    std::cout << pp.age<<"  "<<pp.ll << std::endl;
}


int main() {
    Person::getInstance().registerHandler<people>(std::bind(printValue, std::placeholders::_1));
    people ljj;
    ljj.age =18;
    ljj.ll =100;
    Person::getInstance().post(ljj);

    ljj.age =28;
    ljj.ll =200;
    Person::getInstance().post(ljj);

    Person::getInstance().unregisterHandler<people>();

    ljj.age =38;
    ljj.ll =300;
    Person::getInstance().post(ljj);
    return 0;
}

