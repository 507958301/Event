//#include "widget.h"

//#include <QApplication>

//int main(int argc, char *argv[])
//{
//    QApplication a(argc, argv);
//    Widget w;
//    w.show();
//    return a.exec();
//}
/*
* 关键代码：在目标类中增加一个ArrayList来存放观察者们。
*/


//#include "Observer.h"
//int main() {
//    MyStruct s;
//    std::cout << typeid(s).name() << std::endl;
//    return 0;
//}

struct people{
    int age=0;
    double ll=0.0;
};

#include "Observer.h"
void printValue(people pp){
    std::cout << typeid(pp).name() << std::endl;
    std::cout << pp.age<<"  "<<pp.ll << std::endl;
}

int main() {
    Person<people> alice;
    people pp;
    alice.registered(pp,std::bind(printValue, std::placeholders::_1));
    people ljj;
    ljj.age =18;
    ljj.ll =100;
    alice.post(ljj);
}
