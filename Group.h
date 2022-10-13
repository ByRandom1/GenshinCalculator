//
// Created by 陈劭彬 on 2022/10/11.
//

#ifndef GENSHINCALCULATOR_GROUP_H
#define GENSHINCALCULATOR_GROUP_H

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include "Condition.h"
#include "Character.h"
#include "Weapon.h"
#include "Artifact.h"

using namespace std;

extern ofstream outfile_result;
extern ofstream outfile_debug;

//parameters
extern bool out_header;
extern bool cal_enable_recharge_check;
extern double cal_min_critrate_valid;
extern double cal_max_critrate_valid;
extern int max_up_num_per_base;
extern int max_attribute_num_per_pos;
extern int max_entry_num;
extern int artifact_2_2_max_entry_bonus;

struct Team_config
{
    Character *teammate_1;
    Character *teammate_2;
    Character *teammate_3;
    string teammate_all;//FIND
    string team_weapon_artifact;//FIND
    string ele_attach_type;//FIND
    string ele_allow_spread;//FIND

    Team_config(Character *teammate_1_,
                Character *teammate_2_,
                Character *teammate_3_,
                string team_weapon_artifact_,
                string ele_attach_type_,
                string ele_allow_spread_)
    {
        teammate_1 = teammate_1_;
        teammate_2 = teammate_2_;
        teammate_3 = teammate_3_;
        teammate_all = teammate_1_->name + "_" + teammate_2_->name + "_" + teammate_3->name;
        team_weapon_artifact = team_weapon_artifact_;
        ele_attach_type = ele_attach_type_;
        ele_allow_spread = ele_allow_spread_;
    }
};

struct Attack_config
{
    //config
    Condition *condition;
    bool background;
    bool lockface;
    string react_type;//扩散（风+水火雷冰），结晶（岩+水火雷冰），绽放（草水+火雷），激化（草雷），燃烧（草火），蒸发（水火），融化（火冰），冻结（水冰），感电（雷水），超载（雷火），超导（雷冰）FIND
    map<string, bool> useful_attributes;
    int attack_time;

    Attack_config(Condition *condition_,
                  bool background_,
                  bool lockface_,
                  string react_type_,
                  bool if_life_useful,
                  bool if_atk_useful,
                  bool if_def_useful,
                  bool if_mastery_useful,
                  bool if_recharge_useful,
                  bool if_critrate_useful,
                  bool if_critdam_useful,
                  bool if_damplus_useful,
                  bool if_heal_useful,
                  bool if_shield_useful,
                  int attack_time_)
    {
        condition = condition_;
        background = background_;
        lockface = lockface_;
        react_type = react_type_;
        useful_attributes["生命值"] = if_life_useful;
        useful_attributes["攻击力"] = if_atk_useful;
        useful_attributes["防御力"] = if_def_useful;
        useful_attributes["额外倍率"] = true;
        useful_attributes["元素精通"] = if_mastery_useful;
        useful_attributes["元素充能效率"] = if_recharge_useful;
        useful_attributes["暴击率"] = if_critrate_useful;
        useful_attributes["暴击伤害"] = if_critdam_useful;
        useful_attributes["伤害加成"] = if_damplus_useful;
        useful_attributes["抗性削弱"] = true;
        useful_attributes["防御削弱"] = true;
        useful_attributes["防御无视"] = true;
        useful_attributes["治疗加成"] = if_heal_useful;
        useful_attributes["护盾强效"] = if_shield_useful;
        attack_time = attack_time_;
    }
};

struct attribute
{
    double percentage;
    double converted_percentage;
    int entry_num;
    double value_per_entry;
    bool useful;

    attribute(double percentage_,
              double converted_percentage_,
              int entry_num_,
              double value_per_entry_,
              bool useful_)
    {
        percentage = percentage_;
        converted_percentage = converted_percentage_;
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
    Team_config *team_config;
    Attack_config *attack_config;

    int base_life;
    int base_atk;
    int base_def;
    double base_skillrate;
    map<string, attribute *> data_list;

    Deployment(Character *c_point_,
               Weapon *w_point_,
               Artifact *suit1_,
               Artifact *suit2_,
               string a_main3_,
               string a_main4_,
               string a_main5_,
               Team_config *team_config_,
               Attack_config *attack_config_);

    ~Deployment();

    //配置数值初始化
    void init_check_data(bool &suit1_valid, bool &suit2_valid, bool &main3_valid, bool &main4_valid, bool &main5_valid);

    bool add_percentage(string type_, double value_, string source);

    bool add_converted_percentage(string type_, double value_, string source);

    bool change_useful_attribute(string type_, bool value_, string source);

    void check_useful_attribute();//在main中实现

    void get_team_data();//在main中实现

    void satisfy_recharge_requirement();//在main中实现

    //单人最佳伤害(副词条)计算
    double cal_damage(int life_num, int atk_num, int def_num, int mastery_num, int recharge_num, int critrate_num, int critdam_num);

    void get_convert_value(double &life, double &atk, double &def, double &mastery, double &recharge, double &critrate, double &critdam, double &damplus);//在main中实现

    void get_extra_rate_value(double life, double atk, double def, double mastery, double recharge, double critrate, double critdam, double damplus, double &extrarate);//在main中实现

    double get_react_value(double mastery, double &extrarate, double &growrate);//在main中实现
};

class Group
{
public:
    Character *c_point;
    Weapon *w_point;
    Artifact *suit1;
    Artifact *suit2;
    string a_main3;
    string a_main4;
    string a_main5;
    Team_config *team_config;
    vector<Deployment *> combination;

    vector<double> damage;
    map<string, attribute *> entry;
    double total_damage;

    Group(Character *c_point_,
          Weapon *w_point_,
          Artifact *suit1_,
          Artifact *suit2_,
          string a_main3_,
          string a_main4_,
          string a_main5_,
          Team_config *team_config_,
          vector<Attack_config *> attack_config_list);

    int init_check_data();

    void cal_damage_entry_num();

    void out();
};

#endif //GENSHINCALCULATOR_GROUP_H
