//
// Created by boil on 2023/3/10.
//

#include <iostream>

class Base{
public:
    virtual Base *GetBaseType() { return this; }
    Base *GetClassType() { return this; }
};

class Son:public Base{
public:
    Base *GetBaseType() override { return Base::GetBaseType(); }
};

int main() {
    Base base;
    Son son;
    const std::type_info &base_Info = typeid(base);
    const std::type_info &son_Info = typeid(son);
    std::cout << base_Info.name() << std::endl;

    std::cout << son_Info.name() << std::endl;

    std::cout << "hello world" << std::endl;
}
