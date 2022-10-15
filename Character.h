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

class weapon_artifact_related_arguments
{
public:
//    bool background;//华馆。team，四风原典，匣里日月，嘟嘟可故事集，暗巷猎手，幽夜华尔兹，王下近侍，息灾
//    bool accumulate_attacks;//=!background 斫峰之刃，试作斩岩，尘世之锁，钢轮弓，无工之剑，白影剑，和璞鸢，贯虹之槊
//    bool lockface;//持续性技能，team，息灾

    bool shield_sustain;//逆飞，双岩，斫峰之刃，尘世之锁，无工之剑，钟剑
    bool heal_sustain;//海染

    int cangbai_level;//苍白，E命中，7s，2层
    bool sustain_E_hit;//千岩，E命中，3s
    int monv_level;//魔女，释放E，10s，3层
    bool Q_damage_self;//辰砂
    bool shenlin_enable;//草套，EQ命中，8s

    int sword_wuqie_level;//雾切，普通攻击造成元素伤害时，持续5秒；施放元素爆发时，持续10秒；角色元素能量低于100%时
    int sword_shengxian_level;//圣显之钥，E命中。20s，3层
    int catalyst_shenle_level;//神乐铃，释放E，16s，3层
    int bow_feilei_level;//飞雷，普通攻击造成伤害时，持续5秒；施放元素战技时，持续10秒；此外，角色元素能量低于100%时
    int bow_dongji_level;//冬极，普通攻击、重击、元素战技或元素爆发命中敌人
    bool bow_pomo_fullenergy;//破魔
    bool polearm_humo_halflife;//护摩
    int polearm_chisha_level;//赤砂之杖，E命中，10s，3层
    int polearm_shizuo_level;//试作星镰，释放E，12s，2层

    weapon_artifact_related_arguments(bool shield_sustain_,
                                      bool heal_sustain_,

                                      int cangbai_level_,
                                      bool sustain_E_hit_,
                                      int monv_level_,
                                      bool Q_damage_self_,
                                      bool shenlin_enable_,

                                      int sword_wuqie_level_,
                                      int sword_shengxian_level_,
                                      int catalyst_shenle_level_,
                                      int bow_feilei_level_,
                                      int bow_dongji_level_,
                                      bool bow_pomo_fullenergy_,
                                      bool polearm_humo_halflife_,
                                      int polearm_chisha_level_,
                                      int polearm_shizuo_level_)
    {
        shield_sustain = shield_sustain_;
        heal_sustain = heal_sustain_;

        cangbai_level = cangbai_level_;
        sustain_E_hit = sustain_E_hit_;
        monv_level = monv_level_;
        Q_damage_self = Q_damage_self_;
        shenlin_enable = shenlin_enable_;

        sword_wuqie_level = sword_wuqie_level_;
        sword_shengxian_level = sword_shengxian_level_;
        catalyst_shenle_level = catalyst_shenle_level_;
        bow_feilei_level = bow_feilei_level_;
        bow_dongji_level = bow_dongji_level_;
        bow_pomo_fullenergy = bow_pomo_fullenergy_;
        polearm_humo_halflife = polearm_humo_halflife_;
        polearm_chisha_level = polearm_chisha_level_;
        polearm_shizuo_level = polearm_shizuo_level_;
    }
};

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
    double E_energy;
//    double E_release_times;
//    double E_release_interval;
//    double E_hit_times;
//    double E_hit_interval;
    bool E_lockface;
    vector<double> E;
    int Q_level;
    double Q_energy;
//    double Q_release_times;
//    double Q_release_interval;
//    double Q_hit_times;
//    double Q_hit_interval;
    bool Q_lockface;
    vector<double> Q;
    int constellation;
    vector<Set *> extra_value;

    weapon_artifact_related_arguments *args;

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
              double E_energy_,
//              double E_release_times_,
//              double E_release_interval_,
//              double E_hit_times_,
//              double E_hit_interval_,
              bool E_lockface_,
              double E_13, double E_12, double E_10, double E_9,
              int Q_level_,
              int Q_energy_,
//              double Q_release_times_,
//              double Q_release_interval_,
//              double Q_hit_times_,
//              double Q_hit_interval_,
              bool Q_lockface_,
              double Q_13, double Q_12, double Q_10, double Q_9,
              int constellation_,
              vector<Set *> extra_value_,
              weapon_artifact_related_arguments *args_);

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
