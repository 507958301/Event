#include "EventManger.h"
struct people{
    virtual ~people(){}
    int age=0;
    double ll=0.0;
};

struct blackPoeple: public people
{
    std::string color ="black";
    /* data */
};

void printValue(people pp)
{
    std::cout << pp.age<<"  "<<pp.ll << std::endl;
}

void printValueBB(const people& pp)
{
    const blackPoeple& blackpoeple = *static_cast<const blackPoeple*>(&pp);
    std::cout << blackpoeple.age<<"  "<<blackpoeple.ll << "BB" << blackpoeple.color<<std::endl;
}

struct mat{
    std::string type="mat";
    int age=0;
};
void printValueAA(mat pp){
    std::cout << pp.age<<"  "<<pp.type << std::endl;
}


int main() 
{
    // auto handler = EventManager::getInstance().subsrcibeToEvent<people>(std::bind(printValue, std::placeholders::_1));
    auto handler = EventManager::getInstance().subsrcibeToEvent<people>(std::bind(printValueBB, std::placeholders::_1));
    EventManager::getInstance().subsrcibeToEvent<mat>(std::bind(printValueAA, std::placeholders::_1));
    people ljj;
    ljj.age =18;
    ljj.ll =100;
    EventManager::getInstance().postEvent(ljj);

    mat m;
    m.type = "Mat";
    m.age = 1000;
    EventManager::getInstance().postEvent(m);

    ljj.age =28;
    ljj.ll =200;
    EventManager::getInstance().postEvent(ljj);

    EventManager::getInstance().unSubscribeToEvent<people>(handler);

    ljj.age =38;
    ljj.ll =300;
    EventManager::getInstance().postEvent(ljj);
    return 0;
}

