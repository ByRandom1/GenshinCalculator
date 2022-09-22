//
// Created by 陈劭彬 on 2022/8/10.
//

#ifndef GENSHINCALCULATOR_DEPLOYMENT_H
#define GENSHINCALCULATOR_DEPLOYMENT_H

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include "Condition.h"
#include "Character.h"
#include "Weapon.h"
#include "Artifact.h"

using namespace std;

extern bool debug;
extern string filter_type;
extern ofstream outfile;
#define max_up_num_per_base 4
#define max_attribute_num_per_pos 3
#define artifact_2_2_max_entry_bonus 2

struct Config
{
    Condition *condition;
    string react_type;//扩散（风+水火雷冰），结晶（岩+水火雷冰），绽放（草水+火雷），激化（草雷），燃烧（草火），蒸发（水火），融化（火冰），冻结（水冰），感电（雷水），超载（雷火），超导（雷冰）
    string ele_attach_type;
    int max_entry_all;
    vector<bool> useful_attributes;
    Character* teammate_1;
    Character* teammate_2;
    Character* teammate_3;
    string teammate_all;
    string team_weapon_artifact;

    Config(Condition *condition_, string react_type_, string ele_attach_type_, int max_entry_all_, bool if_life_useful, bool if_atk_useful, bool if_def_useful, bool if_mastery_useful,
           bool if_recharge_useful, bool if_critrate_useful, bool if_critdam_useful, bool if_damplus_useful, bool if_heal_useful, bool if_shield_useful, Character* teammate_1_,
           Character* teammate_2_, Character* teammate_3_, string team_weapon_artifact_)
    {
        condition = condition_;
        react_type = react_type_;
        ele_attach_type = ele_attach_type_;
        max_entry_all = max_entry_all_;
        useful_attributes.push_back(if_life_useful);
        useful_attributes.push_back(if_atk_useful);
        useful_attributes.push_back(if_def_useful);
        useful_attributes.push_back(true);//额外倍率
        useful_attributes.push_back(if_mastery_useful);
        useful_attributes.push_back(if_recharge_useful);
        useful_attributes.push_back(if_critrate_useful);
        useful_attributes.push_back(if_critdam_useful);
        useful_attributes.push_back(if_damplus_useful);
        useful_attributes.push_back(true);//抗性削弱
        useful_attributes.push_back(true);//防御削弱
        useful_attributes.push_back(true);//防御无视
        useful_attributes.push_back(if_heal_useful);
        useful_attributes.push_back(if_shield_useful);
        teammate_1 = teammate_1_;
        teammate_2 = teammate_2_;
        teammate_3 = teammate_3_;
        teammate_all = teammate_1_->name + teammate_2_->name + teammate_3_->name;
        team_weapon_artifact = team_weapon_artifact_;
    }
};

struct attribute
{
    string name;
    double percentage;
    int entry_num;
    double value_per_entry;
    bool useful;

    attribute(string name_,
              double percentage_,
              int entry_num_,
              double value_per_entry_,
              bool useful_)
    {
        name = name_;
        percentage = percentage_;
        entry_num = entry_num_;
        value_per_entry = value_per_entry_;
        useful = useful_;
    }
};

class Deployment
{
public:
    Character *c_point;
    Weapon *w_point;
    Artifact *suit1;
    Artifact *suit2;
    string a_main3;
    string a_main4;
    string a_main5;
    Config *config;

    int base_life;
    int base_atk;
    int base_def;
    double base_skillrate;
    vector<attribute *> data_list;
    double damage;

    Deployment(Character *c_point_,
               Weapon *w_point_,
               Artifact *suit1_,
               Artifact *suit2_,
               string a_main3_,
               string a_main4_,
               string a_main5_,
               Config *config_);

    ~Deployment();

    //配置数值初始化
    int init_check_data();

    bool add_percentage(string type_, double value_, string source);

    string check_disabled_combinations();//在main中实现

    void check_weapon_special(bool &valid);//在main中实现

    void check_artifact_special(bool &valid);//在main中实现

    void modify_useful_attribute();//在main中实现

    void get_team_data();//在main中实现

    bool check_recharge_requirement();//在main中实现

    //单人最佳伤害(副词条)计算
    void get_convert_value(double &life, double &atk, double &def, double &mastery, double &recharge, double &critrate, double &critdam, double &damplus);//在main中实现

    void get_extra_rate_value(double life, double atk, double def, double mastery, double recharge, double critrate, double critdam, double damplus, double &extrarate);//在main中实现

    void get_react_value(double mastery, double &extrarate, double &growrate);//在main中实现

    void cal_damage_entry_num();

    //输出到文件
    void out();//在main中实现
};

#endif //GENSHINCALCULATOR_DEPLOYMENT_H
