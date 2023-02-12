//
// Created by 陈劭彬 on 2022/10/11.
//

#ifndef GENSHINCALCULATOR_GROUP_H
#define GENSHINCALCULATOR_GROUP_H

#include <iostream>
#include <fstream>
#include <vector>
#include "Condition.h"
#include "Character.h"
#include "Weapon.h"
#include "Artifact.h"
#include "Reinforced_Artifact.h"

using namespace std;

extern ofstream outfile_result;
extern ofstream outfile_debug;

//parameters
extern bool out_header;
extern string double_E_per_round;
extern int max_up_num_per_base;
extern double max_attribute_num_per_pos;
extern int max_entry_num;
extern int artifact_2_2_max_entry_bonus;

extern bool out_debug;

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
        teammate_all = teammate_1->name + "_" + teammate_2->name + "_" + teammate_3->name;
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
    bool useful_attributes[14];
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
        useful_attributes[0] = if_life_useful;
        useful_attributes[1] = if_atk_useful;
        useful_attributes[2] = if_def_useful;
        useful_attributes[3] = true;
        useful_attributes[4] = if_mastery_useful;
        useful_attributes[5] = if_recharge_useful;
        useful_attributes[6] = if_critrate_useful;
        useful_attributes[7] = if_critdam_useful;
        useful_attributes[8] = if_damplus_useful;
        useful_attributes[9] = true;
        useful_attributes[10] = true;
        useful_attributes[11] = true;
        useful_attributes[12] = if_heal_useful;
        useful_attributes[13] = if_shield_useful;
        attack_time = attack_time_;
    }
};

struct attribute
{
    double percentage;
    double converted_percentage;
    double value_per_entry;
    bool useful;

    attribute(double percentage_,
              double converted_percentage_,
              double value_per_entry_,
              bool useful_)
    {
        percentage = percentage_;
        converted_percentage = converted_percentage_;
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
    bool need_to_satisfy_recharge;

    int base_life;
    int base_atk;
    int base_def;
    double base_skillrate;
    string get_data_info;
    int min_recharge_num;
    attribute *data_list[14];

    Deployment(Character *c_point_,
               Weapon *w_point_,
               Artifact *suit1_,
               Artifact *suit2_,
               string a_main3_,
               string a_main4_,
               string a_main5_,
               Team_config *team_config_,
               Attack_config *attack_config_,
               bool need_to_satisfy_recharge_);

    ~Deployment();

    //检查数据
    void check_data(bool &suit1_valid, bool &suit2_valid, bool &main3_valid, bool &main4_valid, bool &main5_valid);

    //获取数据
    void init_data();

    void add_percentage(string type_, double value_, string source);

    void add_converted_percentage(string type_, double value_, string source);

    void check_useful_attribute();//在main中实现

    void get_team_data();//在main中实现

    void satisfy_recharge_requirement();//在main中实现

    //单人最佳伤害(副词条)计算
    double cal_damage(double life_value, double atk_value, double def_value, double mastery_value, double recharge_value, double critrate_value, double critdam_value);

    void get_convert_value(double &life, double &atk, double &def, double &mastery, double &recharge, double &critrate, double &critdam, double &damplus);//在main中实现

    void get_extra_rate_value(double life, double atk, double def, double mastery, double recharge, double critrate, double critdam, double damplus, double &extrarate);//在main中实现

    void get_react_value(double mastery, double &extrarate, double &growrate, double &extra_damage);//在main中实现
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
    bool need_to_satisfy_recharge;

    bool useful[7];

    int entry[7];

    Reinforced_Artifact *data[5];

    double *temp_damage;
    double *damage;
    double total_damage;

    Group(Character *c_point_,
          Weapon *w_point_,
          Artifact *suit1_,
          Artifact *suit2_,
          string a_main3_,
          string a_main4_,
          string a_main5_,
          Team_config *team_config_,
          vector<Attack_config *> attack_config_list,
          bool need_to_satisfy_recharge_);

    ~Group();

    int init_check_data(int recharge_restriction_num);

    void cal_damage_entry_num();

    void out();

    void cal_assigned_artifact_damage();

    void out_assigned_artifact();
};

#endif //GENSHINCALCULATOR_GROUP_H
