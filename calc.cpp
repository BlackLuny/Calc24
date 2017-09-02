//
//  main.cpp
//  Calc24
//
//  Created by BlackK on 2016/9/30.
//  Copyright © 2016年 BlackK. All rights reserved.
//

#include <iostream>
#include <stack>
#include <vector>
#include <list>
#include <math.h>
#define MAX_LENGTH 256
using namespace std;
typedef struct t_item{
    double num;
    bool used;
}*PItem,Item;

typedef bool (*PFN_SIGN)(double &a,double &b,double &r);
//符号结构体 支持自定义运算
typedef struct t_sign{
    PFN_SIGN pfn;   //符号运算方法
    const char *sign;    //符号标示
    bool canChange;//是否满足交换律
    //char level;    //优先级
}*PSign,Sign;
//记录运算过程的结构体
typedef struct t_record{
    int a;         //数字序号或者代号
    int b;         //数字序号或者代号
    PSign psign;  //符号
    int fname;  //代号 代表中间量 用n+1,n+1 ...表示 n指的是给定的数字个数
}*PRecord,Record;
//保存运算状态的结构体
typedef struct t_state{
    vector<Item> nums;     //所有的参与运算的数字，包括给定的和中间过程产生的
    vector<Record>   RecordVect;  //记录的运算过程
}*PState,State;
template <typename T>
vector<T> &operator+(vector<T> &v1,vector<T> const &v2){
    v1.insert(v1.end(),v2.begin(),v2.end());
    return v1;
}
/*************************************************
 Function: printRecord
 Description: 根据record的内容，生成相应的计算表达式
 Calls: printRecord
 Called By: EnumSituation,Calc
 Input: record:记录计算过程的vector,v:用做计算的数字以及运算过程产生的数字，n：给定的数字个数，goal:目标数值，firstLayer:是否是第一层，用于把最大的括号去掉
 Return: char* 生成的计算数学表达式
 *************************************************/
char* printRecord(const vector<Record> &record,const vector<Item> &v,const int n,const int goal,bool firstLayer)
{
    char *s1 = NULL ,*s2 = NULL,*s3 = NULL;
    for (int i = 0; i<record.size(); i++) {
        if(record[i].fname==goal){
            if(record[i].a>=n){
                //如果记录中的一个数是中间数值，将这个数字的计算表达式生成出来
                s1 = printRecord(record, v, n, (int)record[i].a,false);
            }else{
                //如果记录中的一个数是给定的数字，直接输出这个数字
                s1 = (char*)malloc(MAX_LENGTH);
                memset(s1,0,MAX_LENGTH);
                sprintf(s1,"%d", (int)v[record[i].a].num);
            }
            //另外一个数字
            if(record[i].b>=n){
                s2 = printRecord(record, v, n, record[i].b,false);
            }
            else{
                s2 = (char*)malloc(MAX_LENGTH);
                memset(s2,0,MAX_LENGTH);
                sprintf(s2,"%d", (int)v[record[i].b].num);
            }
            //将两个数字合成一个表达式如 s1 = "3" ,s2 = "9" 则 s3 = (3@9)  s1="(3+2)" s2 = “(4+2)” 则 s3=((3+2)@(4+2))  @代表运算符
            s3 = (char*)malloc(strlen(s1)+strlen(s2)+1+strlen(record[i].psign->sign)+2);
            memset(s3,0,strlen(s1)+strlen(s2)+1+strlen(record[i].psign->sign)+2);
            //如果是第一层，就不用加括号
            if(!firstLayer)
                strcat(s3,"(");
            strcat(s3,s1);
            strcat(s3,record[i].psign->sign);
            strcat(s3,s2);
            if(!firstLayer)
                strcat(s3,")");
            free(s1);
            free(s2);
            break;

        }
        
    }
    return s3;
}
/*************************************************
 Function: ifexistend
 Description: s是否在ans中存在 存在返回true 不存在返回false
 Calls: NULL
 Called By: EnumSituation
 Input: ans:保存结果的vector,s:待查找的字符串
 Return: true s已经在ans中纯在，false s在ans中并不存在
 *************************************************/
bool ifexistend(const vector<char*> &ans,const char *s)
{
    for(int i = 0;i<ans.size();i++){
        if(strcmp(ans[i], s) ==0)
            return true;
    }
    return false;
}
/*************************************************
 Function: EnumSituation
 Description: 枚举每种有效的结果的所有满足交换律的情况，并检查是否已经在ans中存在，存在返回true，否则返回false
 Calls: printRecord，ifexistend，EnumSituation
 Called By: Calc
 Input: ans:保存结果的vector,record:当前计算过程的记录,v:用做计算的数字以及运算过程产生的数字,n：给定的数字个数，goal:目标数值，deep:当前遍历的深度
 Return: true 本次表达式是重复表达式，false 不是重复的表达式
 *************************************************/
bool EnumSituation(const vector<char*> &ans,const vector<Record> &record,const vector<Item> &v,const int &n,const int &goal,const int &deep)
{
    bool bexistend = false;
    for (int i = deep; i<record.size(); i++) {
        if(deep==record.size() - 1) //最后一层
        {   int a,b;
            a = record[i].a;
            b = record[i].b;
            char *temp = NULL;
            vector<Record> temp_record = record;
            temp = printRecord(temp_record,v,n,goal,true);
            if(ifexistend(ans, temp))
                return true;
            //尝试交换两个数字
            if(a!=b){
                temp_record[i].a = b;
                temp_record[i].b = a;
                temp = NULL;
                temp = printRecord(temp_record,v,n,goal,true);
                if(ifexistend(ans, temp))
                    return true;
            }
        }else{
            //如果是满足交换律的运算
            if(record[i].psign->canChange){
                    vector<Record> temp_record = record;
                    int a,b;
                    a = record[i].a;
                    b = record[i].b;
                //不交换两个数字
                    bexistend = EnumSituation(ans, temp_record, v, n, goal, deep + 1);
                if(bexistend==true)
                    return true;
                //尝试交换两个数字
                if(a!=b){
                    temp_record[i].a = b;
                    temp_record[i].b = a;
                    bexistend = EnumSituation(ans, temp_record, v, n, goal, deep + 1);
                    if(bexistend==true)
                        return true;
                }

            }else{
                bexistend = EnumSituation(ans, record, v, n, goal, deep + 1);
            }

        }
    }
    return bexistend;
}

/*************************************************
 Function: success
 Description: 判断是否等于目标值
 Calls: NULL
 Called By: Calc
 Input: v:用做计算的数字以及运算过程产生的数字,goal:目标数值
 Return: true 已经使用完所有数字且表达式的值是goal，false 有没有用完的数字或者表达式的值不等于goal
 *************************************************/
bool success(const vector<Item> &v,const double &goal)
{
    //除了最后的答案，所有给定的数字和中间量都必须用到
    for(int i = 0;i<v.size()-1;i++){
        if(v[i].used==false)
            return false;
    }
    if(fabs((v[v.size()-1].num - goal))<=1e-10)
        return true;
    else
        return false;
}
/*************************************************
 Function: Calc
 Description: 枚举给定数字和符号集的所有值，所有组合情况
 Calls: success，printRecord，EnumSituation
 Called By: calcMN
 Input: nums:给定的计算数字,n：给定的数字个数，goal:目标数值，Signs:定义的符号集
 Return: vector<char*> 本所有满足的表达式
 *************************************************/
vector<char*> Calc(const vector<Item>& nums,const int &n,const double &goal, vector<Sign> &Signs)
{
    vector<char*> ans;
    stack<State> st; //创建DFS的栈
    State temp_s;
    Item temp_item;
    temp_s.nums = nums;
    temp_s.RecordVect = vector<Record>();
    st.push(temp_s);
    while(!st.empty()){
        temp_s = st.top(); //弹出当前情况进行计算
        st.pop();
        int cur_n = (int)temp_s.nums.size();
        //如果当前满足要求
        if(success(temp_s.nums, goal)){
            //判断是否是重复解
            if(EnumSituation(ans, temp_s.RecordVect, temp_s.nums, n, cur_n - 1,0)==false)
            {
                char *s = printRecord(temp_s.RecordVect,temp_s.nums,n,cur_n - 1,true);
                //cout<<s<<endl;
                ans.push_back(s);
            }
        }else{
            //遍历当前情况所有可用数字（包括给定的和生成的中间量 a b 是序号），找其中的两个数字进行某种运算，
            for(int a = 0;a<cur_n-1;a++){
                if(temp_s.nums[a].used)
                    continue;
                for(int b = a + 1;b<cur_n;b++){
                    //如果选择的两个数字都没有被使用
                    if(temp_s.nums[a].used==false&&temp_s.nums[b].used==false){
                        double num1 = temp_s.nums[a].num;
                        double num2 = temp_s.nums[b].num;
                        //遍历每种运算过程
                        for (int i = 0; i<Signs.size(); i++) {
                            double temp_result_1 = 0;
                            if((*Signs[i].pfn)(num1,num2,temp_result_1)){
                                //记录本次运算过程，保存产生的结果，产生新状态，并入栈
                                temp_item.num = temp_result_1;
                                temp_item.used = false;
                                State newState;
                                Record temp_Record;
                                newState.nums = temp_s.nums;
                                newState.nums[b].used = true;
                                newState.nums[a].used = true;
                                newState.nums.push_back(temp_item);
                                temp_Record.a = a;
                                temp_Record.b = b;
                                temp_Record.psign = &Signs[i];
                                temp_Record.fname = cur_n;  //数字代号，用下标表示
                                newState.RecordVect = temp_s.RecordVect;
                                newState.RecordVect.push_back(temp_Record);
                                st.push(newState);
                            }
                            double temp_result_2 = 0;
                            //交换两个数字并执行相同的运算，如果结果不同，产生新的状态
                            if(Signs[i].canChange==false&&(*Signs[i].pfn)(num2,num1,temp_result_2)){
                                if(temp_result_2!=temp_result_1){
                                    temp_item.num = temp_result_2;
                                    temp_item.used = false;
                                    State newState;
                                    Record temp_Record;
                                    newState.nums = temp_s.nums;
                                    newState.nums[b].used = true;
                                    newState.nums[a].used = true;
                                    newState.nums.push_back(temp_item);
                                    temp_Record.a = b;
                                    temp_Record.b = a;
                                    temp_Record.psign = &Signs[i];
                                    temp_Record.fname = cur_n;
                                    newState.RecordVect = temp_s.RecordVect;
                                    newState.RecordVect.push_back(temp_Record);
                                    st.push(newState);
                                }
                            }

                        }

                    }
               }
            }
        }
        
    }
    return ans;
}
bool sign_add(double &a,double &b,double &r)
{
    r = a + b;
    return true;
}
bool sign_sub(double &a,double &b,double &r)
{
    r = a - b;
    return true;
}
bool sign_mul(double &a,double &b,double &r)
{
    r = a * b;
    return true;
}
bool sign_div(double &a,double &b,double &r)
{
    if (b!=0) {
        r = a / b;
        return  true;
    }else
        return false;
}
//测试这个函数
/*************************************************
 Function: calcMN
 Description:计算给定数字与符号运算的所有组合，使之等于goal
 Calls: Calc
 Called By: NULL
 Input: nums:给定的计算数字,goal:目标数值
 Return: vector<char*> 本所有满足的表达式
 *************************************************/
vector<char*> calcMN(vector<double>&v_nums,double goal)
{
    //定义的运算符号集
    vector<Sign> signs;
    Sign temp;
    temp.sign = "*";
    temp.canChange = true;
    temp.pfn = sign_mul;
    signs.push_back(temp);
    temp.sign = "/";
    temp.canChange = false;
    temp.pfn = sign_div;
    signs.push_back(temp);
    temp.sign = "+";
    temp.canChange = true;
    temp.pfn = sign_add;
    signs.push_back(temp);
    temp.sign = "-";
    temp.canChange = false;
    temp.pfn = sign_sub;
    signs.push_back(temp);
    //待计算的数字
    vector<Item> nums;
    for (int i = 0; i<v_nums.size(); i++) {
        Item x;
        x.used = false;
        x.num = v_nums[i];
        nums.push_back(x);
    }
    vector<char*> ans = Calc(nums,(int)nums.size(),goal,signs);
    return ans;
}

int main(int argc, const char * argv[]) {

    vector<double> num = {3,5,2,8};
    vector<char*> ans = calcMN(num,24);
    for (int i = 0; i<ans.size(); i++) {
        cout<<ans[i]<<endl;
    }
    return 0;
}

