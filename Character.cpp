//
// Created by 陈劭彬 on 2022/8/10.
//

#include "Character.h"
#include "Group.h"

Character::Character(string name_,
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
//                     double E_release_times_,
//                     double E_release_interval_,
//                     double E_hit_times_,
//                     double E_hit_interval_,
                     bool E_lockface_,
                     vector<bool> E_useful_attributes_,
                     vector<double> E_13, vector<double> E_12, vector<double> E_10, vector<double> E_9,
                     int Q_level_,
                     int Q_energy_,
//                     double Q_release_times_,
//                     double Q_release_interval_,
//                     double Q_hit_times_,
//                     double Q_hit_interval_,
                     bool Q_lockface_,
                     vector<bool> Q_useful_attributes_,
                     vector<double> Q_13, vector<double> Q_12, vector<double> Q_10, vector<double> Q_9,
                     int constellation_,
                     vector<Set *> extra_value_,
                     bool shield_sustain_,
                     bool heal_sustain_,
                     bool E_sustain_)
{
    name = name_;
    english_name = english_name_;
    ele_type = ele_type_;
    weapon_type = weapon_type_;
    life = life_;
    atk = atk_;
    def = def_;
    break_type = break_type_;
    break_value = break_value_;
    A_level = A_level_;
    A_useful_attributes = A_useful_attributes_;
    normal_A_ele_type = normal_A_ele_type_;
    normal_A.push_back(normal_A_10);
    normal_A.push_back(normal_A_9);
    heavy_A_ele_type = heavy_A_ele_type_;
    heavy_A.push_back(heavy_A_10);
    heavy_A.push_back(heavy_A_9);
    down_A_ele_type = down_A_ele_type_;
    down_A.push_back(down_A_10);
    down_A.push_back(down_A_9);
    E_level = E_level_;
    E_energy = E_energy_;
//    E_release_times = E_release_times_;
//    E_release_interval = E_release_interval_;
//    E_hit_times = E_hit_times_;
//    E_hit_interval = E_hit_interval_;
    E_lockface = E_lockface_;
    E_useful_attributes = E_useful_attributes_;
    E.push_back(E_13);
    E.push_back(E_12);
    E.push_back(E_10);
    E.push_back(E_9);
    Q_level = Q_level_;
    Q_energy = Q_energy_;
//    Q_release_times = Q_release_times_;
//    Q_release_interval = Q_release_interval_;
//    Q_hit_times = Q_hit_times_;
//    Q_hit_interval = Q_hit_interval_;
    Q_lockface = Q_lockface_;
    Q_useful_attributes = Q_useful_attributes_;
    Q.push_back(Q_13);
    Q.push_back(Q_12);
    Q.push_back(Q_10);
    Q.push_back(Q_9);
    constellation = constellation_;
    extra_value = extra_value_;
    shield_sustain = shield_sustain_;
    heal_sustain = heal_sustain_;
    E_sustain = E_sustain_;
}

int Character::get_life()
{ return life; }

int Character::get_atk()
{ return atk; }

int Character::get_def()
{ return def; }

bool Character::get_break(Deployment *data)
{
    data->add_percentage(break_type, break_value, (name + "_break"));
    return true;
}

double Character::get_normal_A(int rate_pos)
{
    if (A_level == 10) return normal_A[0][rate_pos];
    else return normal_A[1][rate_pos];
}

double Character::get_heavy_A(int rate_pos)
{
    if (A_level == 10) return heavy_A[0][rate_pos];
    else return heavy_A[1][rate_pos];
}

double Character::get_down_A(int rate_pos)
{
    if (A_level == 10) return down_A[0][rate_pos];
    else return down_A[1][rate_pos];
}

double Character::get_E(int rate_pos)
{
    if (E_level == 13) return E[0][rate_pos];
    else if (E_level == 12) return E[1][rate_pos];
    else if (E_level == 10) return E[2][rate_pos];
    else return E[3][rate_pos];
}

double Character::get_Q(int rate_pos)
{
    if (Q_level == 13) return Q[0][rate_pos];
    else if (Q_level == 12) return Q[1][rate_pos];
    else if (Q_level == 10) return Q[2][rate_pos];
    else return Q[3][rate_pos];
}

bool Character::get_extra(Deployment *data)
{
    if (extra_value[0] != nullptr && *data->attack_config->condition <= *extra_value[0]->condition)
        data->add_percentage(extra_value[0]->type, extra_value[0]->value, (name + "_talent1"));
    if (extra_value[1] != nullptr && *data->attack_config->condition <= *extra_value[1]->condition)
        data->add_percentage(extra_value[1]->type, extra_value[1]->value, (name + "_talent2"));
    if (extra_value[2] != nullptr && *data->attack_config->condition <= *extra_value[2]->condition && constellation >= 1)
        data->add_percentage(extra_value[2]->type, extra_value[2]->value, (name + "_constellation1"));
    if (extra_value[3] != nullptr && *data->attack_config->condition <= *extra_value[3]->condition && constellation >= 2)
        data->add_percentage(extra_value[3]->type, extra_value[3]->value, (name + "_constellation2"));
    if (extra_value[4] != nullptr && *data->attack_config->condition <= *extra_value[4]->condition && constellation >= 4)
        data->add_percentage(extra_value[4]->type, extra_value[4]->value, (name + "_constellation4"));
    if (extra_value[5] != nullptr && *data->attack_config->condition <= *extra_value[5]->condition && constellation >= 6)
        data->add_percentage(extra_value[5]->type, extra_value[5]->value, (name + "_constellation6"));

    get_extra_special(data);

    return true;
}