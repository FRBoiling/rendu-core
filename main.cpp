#include <iostream>

float calc_final(float base,float ratio){
    return base*(1.0+ratio);
}

void print_message(std::string &name ,const std::string &event,int cur_year,float cur_count){
    std::cout << "  " <<cur_year<<"年,"<<name<<" "<< event<<"月工资数:" << cur_count<<"\n";
}

const float ratio =  0.1;
const float jump_ratio = 0.25;
const int base_year = 2016;
const int now_year = 2022;
const std::string name = "小花";

int main(int, char**) {
    float base_count = 18000;
    int year = 6;
    float cur_count = base_count;
    int cur_year = base_year;


    std::cout << "按照不跳涨幅"<<ratio*100 <<"%,跳槽涨幅"<<jump_ratio*100 <<"%算!\n";
    // print_message(name,"小花离开了糖果",cur_year,cur_count);
    
    // for (int i=0;i<year;++i)
    // {
    //     cur_count = calc_final(cur_count,ratio);
    //    print_message()
    //     cur_year++;
    //     // if (cur_year%2 == 0&& cur_year< now_year) 
    //     if (cur_year == now_year) 
    //     {
    //         std::cout <<cur_year<<"年,跳槽了\n";
    //         cur_count = calc_final(cur_count,jump_ratio);
    //     }
    //     print_message(cur_year,cur_count);
    // }

    // std::cout <<"今年是"<<cur_year<<"年\n";
    // print_message(cur_year,cur_count);

    std::cout << "Hello, 雷森破!\n";
}
