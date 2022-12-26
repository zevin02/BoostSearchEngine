#pragma once
#include<iostream>
#include<string>
#include<ctime>
using namespace std;
#define NORMAL  1
#define WARNING 2
#define DEBUG   3
#define FATAL   4
#define LOG(LEVEL,MESSAGE) log(#LEVEL,MESSAGE,__FILE__,__LINE__)
void log(string level,string messages,string file,int line)
{
    cout<<"["<<level<<"]"<<"["<<time(nullptr)<<"]"<<"["<<messages<<"]"<<"["<<file<<" : "<<line<<"]"<<endl;
}