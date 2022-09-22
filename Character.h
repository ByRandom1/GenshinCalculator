//
// Created by 陈劭彬 on 2022/8/10.
//

#ifndef GENSHINCALCULATOR_CHARACTER_H
#define GENSHINCALCULATOR_CHARACTER_H

#include <iostream>
#include <vector>
#include "Condition.h"

using namespace std;

class Deployment;
class Character_special_arguments;

class Character
{
public:
    string name;
    string ele_type;
    string weapon_type;
    int life;
    int atk;
    int def;
    string break_type;
    double break_value;
    int A_level;
    string normal_A_ele_type;
    vector<double> normal_A;
    string heavy_A_ele_type;
    vector<double> heavy_A;
    string down_A_ele_type;
    vector<double> down_A;
    int E_level;
    double E_energy_num;
    vector<double> E;
    int Q_level;
    int Q_energy;
    vector<double> Q;
    int constellation;
    vector<Set *> extra_value;

    Character_special_arguments* args;

    Character(string name_,
              string ele_type_,
              string weapon_type_,
              int life_,
              int atk_,
              int def_,
              string break_type_,
              double break_value_,
              int A_level_,
              string normal_A_ele_type_,
              double normal_A_10, double normal_A_9,
              string heavy_A_ele_type_,
              double heavy_A_10, double heavy_A_9,
              string down_A_ele_type_,
              double down_A_10, double down_A_9,
              int E_level_,
              double E_energy_num_,
              double E_13, double E_12, double E_10, double E_9,
              int Q_level_,
              int Q_energy_,
              double Q_13, double Q_12, double Q_10, double Q_9,
              int constellation_,
              vector<Set *> extra_value_,
              Character_special_arguments* args);

    int get_life();

    int get_atk();

    int get_def();

    bool get_break(Deployment *data);

    double get_normal_A(int A_level_ = 0);

    double get_heavy_A(int A_level_ = 0);

    double get_down_A(int A_level_ = 0);

    double get_E(int E_level_ = 0);

    double get_Q(int Q_level_ = 0);

    bool get_extra(Deployment *data);

    bool get_extra_special(Deployment *data) const;//在main中实现
};

#endif //GENSHINCALCULATOR_CHARACTER_H
