//
// Created by 陈劭彬 on 2022/8/10.
//

#include "Character.h"
#include "Deployment.h"

Character::Character(string name_,
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
                     double E_energy_,
                     double E_release_times_,
                     double E_release_interval_,
                     double E_hit_interval_,
                     bool E_lockface_,
                     double E_13, double E_12, double E_10, double E_9,
                     int Q_level_,
                     int Q_energy_,
                     double Q_release_times_,
                     double Q_release_interval_,
                     double Q_hit_interval_,
                     bool Q_lockface_,
                     double Q_13, double Q_12, double Q_10, double Q_9,
                     int constellation_,
                     vector<Set *> extra_value_,
                     weapon_artifact_related_arguments *args_)
{
    name = name_;
    ele_type = ele_type_;
    weapon_type = weapon_type_;
    life = life_;
    atk = atk_;
    def = def_;
    break_type = break_type_;
    break_value = break_value_;
    A_level = A_level_;
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
    E_release_times = E_release_times_;
    E_release_interval = E_release_interval_;
    E_hit_interval = E_hit_interval_;
    E_lockface = E_lockface_;
    E.push_back(E_13);
    E.push_back(E_12);
    E.push_back(E_10);
    E.push_back(E_9);
    Q_level = Q_level_;
    Q_energy = Q_energy_;
    Q_release_times = Q_release_times_;
    Q_release_interval = Q_release_interval_;
    Q_hit_interval = Q_hit_interval_;
    Q_lockface = Q_lockface_;
    Q.push_back(Q_13);
    Q.push_back(Q_12);
    Q.push_back(Q_10);
    Q.push_back(Q_9);
    constellation = constellation_;
    extra_value = extra_value_;
    args = args_;
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

double Character::get_normal_A(int A_level_)
{
    int target = (A_level_ == 0) ? A_level : A_level_;
    if (target == 10) return normal_A[0];
    else if (target == 9) return normal_A[1];
    else return 1.0;
}

double Character::get_heavy_A(int A_level_)
{
    int target = (A_level_ == 0) ? A_level : A_level_;
    if (target == 10) return heavy_A[0];
    else if (target == 9) return heavy_A[1];
    else return 1.0;
}

double Character::get_down_A(int A_level_)
{
    int target = (A_level_ == 0) ? A_level : A_level_;
    if (target == 10) return down_A[0];
    else if (target == 9) return down_A[1];
    else return 1.0;
}

double Character::get_E(int E_level_)
{
    int target = (E_level_ == 0) ? E_level : E_level_;
    if (target == 13) return E[0];
    else if (target == 12) return E[1];
    else if (target == 10) return E[2];
    else if (target == 9) return E[3];
    else return 1.0;
}

double Character::get_Q(int Q_level_)
{
    int target = (Q_level_ == 0) ? Q_level : Q_level_;
    if (target == 13) return Q[0];
    else if (target == 12) return Q[1];
    else if (target == 10) return Q[2];
    else if (target == 9) return Q[3];
    else return 1.0;
}

bool Character::get_extra(Deployment *data)
{
    if (extra_value[0] != nullptr && *data->config->condition <= *extra_value[0]->condition)
        data->add_percentage(extra_value[0]->type, extra_value[0]->value, (name + "_talent1"));
    if (extra_value[1] != nullptr && *data->config->condition <= *extra_value[1]->condition)
        data->add_percentage(extra_value[1]->type, extra_value[1]->value, (name + "_talent2"));
    if (extra_value[2] != nullptr && *data->config->condition <= *extra_value[2]->condition && constellation >= 1)
        data->add_percentage(extra_value[2]->type, extra_value[2]->value, (name + "_constellation1"));
    if (extra_value[3] != nullptr && *data->config->condition <= *extra_value[3]->condition && constellation >= 2)
        data->add_percentage(extra_value[3]->type, extra_value[3]->value, (name + "_constellation2"));
    if (extra_value[4] != nullptr && *data->config->condition <= *extra_value[4]->condition && constellation >= 4)
        data->add_percentage(extra_value[4]->type, extra_value[4]->value, (name + "_constellation4"));
    if (extra_value[5] != nullptr && *data->config->condition <= *extra_value[5]->condition && constellation >= 6)
        data->add_percentage(extra_value[5]->type, extra_value[5]->value, (name + "_constellation6"));

    get_extra_special(data);

    return true;
}