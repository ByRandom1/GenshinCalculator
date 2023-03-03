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

class Character
{
public:
    string name;
    string english_name;
    string ele_type;
    string weapon_type;
    int life;
    int atk;
    int def;
    string break_type;
    double break_value;
    int A_level;
    vector<bool> A_useful_attributes;
    string normal_A_ele_type;
    vector<vector<double>> normal_A;
    string heavy_A_ele_type;
    vector<vector<double>> heavy_A;
    string down_A_ele_type;
    vector<vector<double>> down_A;
    int E_level;
    double E_energy;
//    double E_release_times;
//    double E_release_interval;
//    double E_hit_times;
//    double E_hit_interval;
    bool E_lockface;
    vector<bool> E_useful_attributes;
    vector<vector<double>> E;
    int Q_level;
    double Q_energy;
//    double Q_release_times;
//    double Q_release_interval;
//    double Q_hit_times;
//    double Q_hit_interval;
    bool Q_lockface;
    vector<bool> Q_useful_attributes;
    vector<vector<double>> Q;
    int constellation;
    vector<Set *> extra_value;

    bool shield_sustain;
    bool heal_sustain;
    bool E_sustain;

    Character(string name_,
              string english_name_,
              string ele_type_,
              string weapon_type_,
              int life_,
              int atk_,
              int def_,
              string break_type_,
              double break_value_,
              int A_level_,
              vector<bool> A_useful_attributes_,
              string normal_A_ele_type_,
              vector<double> normal_A_10, vector<double> normal_A_9,
              string heavy_A_ele_type_,
              vector<double> heavy_A_10, vector<double> heavy_A_9,
              string down_A_ele_type_,
              vector<double> down_A_10, vector<double> down_A_9,
              int E_level_,
              double E_energy_,
//              double E_release_times_,
//              double E_release_interval_,
//              double E_hit_times_,
//              double E_hit_interval_,
              bool E_lockface_,
              vector<bool> E_useful_attributes_,
              vector<double> E_13, vector<double> E_12, vector<double> E_10, vector<double> E_9,
              int Q_level_,
              int Q_energy_,
//              double Q_release_times_,
//              double Q_release_interval_,
//              double Q_hit_times_,
//              double Q_hit_interval_,
              bool Q_lockface_,
              vector<bool> Q_useful_attributes_,
              vector<double> Q_13, vector<double> Q_12, vector<double> Q_10, vector<double> Q_9,
              int constellation_,
              vector<Set *> extra_value_,
              bool shield_sustain_,
              bool heal_sustain_,
              bool E_sustain_);

    int get_life();

    int get_atk();

    int get_def();

    bool get_break(Deployment *data);

    double get_normal_A(int rate_pos);

    double get_heavy_A(int rate_pos);

    double get_down_A(int rate_pos);

    double get_E(int rate_pos);

    double get_Q(int rate_pos);

    bool get_extra(Deployment *data);

    bool get_extra_special(Deployment *data) const;//在main中实现
};

#endif //GENSHINCALCULATOR_CHARACTER_H
