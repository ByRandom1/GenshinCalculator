#include <iostream>
#include <queue>
#include <ctime>
#include "Condition.h"
#include "Character.h"
#include "Weapon.h"
#include "Artifact.h"
#include "Deployment.h"

using namespace std;
//"生命值"-0 "攻击力"-1 "防御力"-2 "额外倍率"-3 "元素精通"-4 "元素充能效率"-5 "暴击率"-6 "暴击伤害"-7 "伤害加成"-8 "抗性削弱"-9 "防御削弱"-10 "防御无视"-11 "治疗加成"-12 "护盾强效"-13
vector<Character *> character_list;

Character *find_character_by_name(string c_name)
{
    for (auto &c: character_list)
        if (c->name == c_name)
            return c;
    return nullptr;
}

vector<Weapon *> weapon_list;

Weapon *find_weapon_by_name(string w_name)
{
    for (auto &w: weapon_list)
        if (w->name == w_name)
            return w;
    return nullptr;
}

vector<Artifact *> artifact_list;

Artifact *find_artifact_by_name(string a_name)
{
    for (auto &a: artifact_list)
        if (a->name == a_name)
            return a;
    return nullptr;
}

string a_main3[5] = {"生命值", "攻击力", "防御力", "元素精通", "元素充能效率"};
string a_main4[5] = {"生命值", "攻击力", "防御力", "元素精通", "伤害加成"};
string a_main5[7] = {"生命值", "攻击力", "防御力", "元素精通", "暴击率", "暴击伤害", "治疗加成"};

class Character_special_arguments
{
public:
    bool background;//华馆。team，四风原典，匣里日月，嘟嘟可故事集，暗巷猎手，幽夜华尔兹，王下近侍，息灾
    bool lockface;//持续性技能，team，息灾

    bool accumulate_attacks;//斫峰之刃，试作斩岩，尘世之锁，钢轮弓，无工之剑，白影剑，和璞鸢，贯虹之槊
    int E_release_times;//默认一定能释放，魔女，神乐之真意，试作星镰，圣显之钥，赤沙之杖
    int E_release_interval;//魔女，神乐之真意，试作星镰，圣显之钥，赤沙之杖
    double E_hit_interval;//苍白，千岩，草套
    double Q_hit_interval;//草套

    bool shield_sustain;//逆飞，双岩，斫峰之刃，尘世之锁，无工之剑，钟剑
    bool heal_sustain;//海染

    bool Q_damage_self;//辰砂
    int sword_wuqie_level;//雾切，普通攻击造成元素伤害时，持续5秒；施放元素爆发时，持续10秒；角色元素能量低于100%时
    int bow_feilei_level;//飞雷，普通攻击造成伤害时，持续5秒；施放元素战技时，持续10秒；此外，角色元素能量低于100%时
    int bow_dongji_level;//冬极，普通攻击、重击、元素战技或元素爆发命中敌人
    bool bow_pomo_fullenergy;//破魔
    bool polearm_humo_halflife;//护摩

    Character_special_arguments(bool background_,
                                bool lockface_,
                                bool accumulate_attacks_,
                                int E_release_times_,
                                int E_release_interval_,
                                double E_hit_interval_,
                                double Q_hit_interval_,
                                bool shield_sustain_,
                                bool heal_sustain_,
                                bool Q_damage_self_,
                                int sword_wuqie_level_,
                                int bow_feilei_level_,
                                int bow_dongji_level_,
                                bool bow_pomo_fullenergy_,
                                bool polearm_humo_halflife_)
    {
        background = background_;
        lockface = lockface_;

        accumulate_attacks = accumulate_attacks_;
        E_release_times = E_release_times_;
        E_release_interval = E_release_interval_;
        E_hit_interval = E_hit_interval_;
        Q_hit_interval = Q_hit_interval_;

        shield_sustain = shield_sustain_;
        heal_sustain = heal_sustain_;

        Q_damage_self = Q_damage_self_;
        sword_wuqie_level = sword_wuqie_level_;
        bow_feilei_level = bow_feilei_level_;
        bow_dongji_level = bow_dongji_level_;
        bow_pomo_fullenergy = bow_pomo_fullenergy_;
        polearm_humo_halflife = polearm_humo_halflife_;
    }
};

//build new character(needed) 保证AEQ单一攻击次数
void init_character_data()
{
    vector<Set *> temp;

    temp.push_back(nullptr);//E结束 队友12%暴击8s (get_team)
    temp.push_back(new Set(new Condition("火", "ALL", "ALL"), "伤害加成", 0.33));//血量低于50%
    temp.push_back(nullptr);//E下重击不消耗体力
    temp.push_back(nullptr);//Q附着血梅香，伤害提升10%最大生命 (extra_rate)
    temp.push_back(nullptr);//血梅香敌人被击败 队友12%暴击15s (get_team)
    temp.push_back(nullptr);
    //temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "暴击率", 1));//25%血以下，CD60s，不考虑
    character_list.push_back(new Character("胡桃", "火", "长柄武器", 15552, 106, 876, "暴击伤害", 0.384,
                                           10, "火", (0.836 + 0.861 + 1.089 + 1.171 + 0.594 + 0.628 + 1.534) / 7,
                                           (0.789 + 0.812 + 1.027 + 1.104 + 0.56 + 0.592 + 1.446) / 7, "火", 2.426,
                                           2.287, "火", 2.92, 2.75,
                                           10, 5, 1.36, 1.28, 1.15, 1.09,
                                           10, 60, 7.06, 6.76, 6.17, 5.88,
                                           1, temp,
                                           new Character_special_arguments(false, false, true, 1, 16, 4, 15,
                                                                           false, false, false, -1, -1, -1, false, true)));
    //Q默认低血量，E为血梅香伤害
    temp.clear();
    //E 生命转化攻击6.26%*max_life<400%*base_atk (convert)

    temp.push_back(new Set(new Condition("ALL", "ALL", "平A|重A"), "伤害加成", 0.3));//E后
    temp.push_back(new Set(new Condition("冰", "ALL", "ALL"), "伤害加成", 0.18));//冲刺命中敌人
    temp.push_back(nullptr);//平A重A50%减少ECD0.3s
    temp.push_back(nullptr);//生成2个20%Q伤害的旋风，不考虑
    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "防御削弱", 0.3));//Q命中 (get_team)
    temp.push_back(nullptr);
    //temp.push_back(new Set(new Condition("ALL", "ALL", "重A"), "伤害加成", 2.98));//CD10s，不考虑
    character_list.push_back(new Character("神里绫华", "冰", "单手剑", 12858, 342, 784, "暴击伤害", 0.384,
                                           10, "冰", (0.904 + 0.962 + 1.238 + 0.448 * 3 + 1.545) / 7,
                                           (0.84 + 0.894 + 1.15 + 0.416 * 3 + 1.436) / 7, "冰", 1.09,
                                           1.013, "冰", 3.156, 2.934,
                                           10, 4.5, 5.083, 4.784, 4.305, 4.066,
                                           10, 80, (19 * 2.386 + 3.58) / 20, (19 * 2.246 + 3.369) / 20,
                                           (19 * 2.021 + 3.032) / 20, (19 * 1.909 + 2.864) / 20,
                                           0, temp,
                                           new Character_special_arguments(false, true, true, 1, 10, 10, 0.25,
                                                                           false, false, false, 3, -1, -1, false, false)));//Q 19切割+1绽放
    temp.clear();

    temp.push_back(nullptr);//队伍获得元素微粒增加2层愿力
    temp.push_back(nullptr);//充能转化雷伤(recharge-1)*0.4 (convert) 回能提高(recharge-1)*0.6 (recharge)
    temp.push_back(nullptr);//雷元素角色释放元素爆发后，积攒的愿力提升80%；其他元素类型角色施放元素爆发后，积攒的愿力提升20%。
    temp.push_back(new Set(new Condition("ALL", "ALL", "E|Q"), "防御无视", 0.6));//Q后
    temp.push_back(nullptr);//Q结束 队友30%攻击10s (get_team)
    temp.push_back(nullptr);//Q伤害命中，1次减队友CD1s/5次
    character_list.push_back(new Character("雷电将军", "雷", "长柄武器", 12907, 337, 789, "元素充能效率", 0.32,
                                           9, "物理", (0.784 + 0.785 + 0.986 + 0.573 + 0.573 + 1.294) / 6,
                                           (0.728 + 0.73 + 0.916 + 0.533 + 0.533 + 1.202) / 6, "物理", 1.969, 1.83,
                                           "物理", 3.16, 2.93,
                                           9, 10, (2.491 + 25 * 0.893) / 26, (2.344 + 25 * 0.84) / 26,
                                           (2.11 + 25 * 0.756) / 26, (1.992 + 25 * 0.714) / 26,
                                           10, 90, 8.52 + 0.0826 * 60, 8.02 + 0.0778 * 60, 7.21 + 0.07 * 60,
                                           6.81 + 0.0661 * 60,
                                           0, temp,
                                           new Character_special_arguments(false, false, true, 1, 10, 1, 0.5,
                                                                           false, false, false, -1, -1, -1, false, false)));//E 1+25，Q 60层愿力，不考虑后续平A
    temp.clear();
    //E 全体大招增伤0.3*Q_energy (special get_team) Q全体回能12.5 (recharge)

    temp.push_back(new Set(new Condition("ALL", "ALL", "重A"), "暴击率", 0.2));//重A后
    temp.push_back(new Set(new Condition("冰", "ALL", "平A|重A|下落A|E"), "伤害加成", 0.2));//Q内前台角色 (get_team)
    temp.push_back(new Set(new Condition("冰", "ALL", "ALL"), "抗性削弱", 0.15));//(get_team) 每次重A命中恢复2点能量 (recharge)
    temp.push_back(nullptr);//E变为2次 (recharge)
    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "伤害加成", 0.15));//Q内 (get_team)
    temp.push_back(nullptr);//E后瞬发重A
    character_list.push_back(new Character("甘雨", "冰", "弓", 9797, 335, 630, "暴击伤害", 0.384,
                                           9, "物理", (0.627 + 0.704 + 0.899 + 0.899 + 0.954 + 1.139) / 6,
                                           (0.583 + 0.654 + 0.836 + 0.836 + 0.886 + 1.059) / 6, "冰", (2.3 + 3.92) / 2,
                                           (2.18 + 3.7) / 2, "物理", 2.81, 2.61,
                                           9, 4, 2.81, 2.64, 2.38, 2.24,
                                           9, 60, 1.49, 1.41, 1.26, 1.19,
                                           0, temp,
                                           new Character_special_arguments(false, true, true, 1, 10, 6, 0.5,
                                                                           false, false, false, -1, 3, 4, true, false)));
    temp.clear();

    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "生命值", 0.18));//3色队
    temp.push_back(nullptr);//前台角色伤害提升1+3.5x% (get_team)
    temp.push_back(nullptr);//E变为2次 (recharge)
    temp.push_back(nullptr);//每1.8s Q倍率增加14%*LIFE (extra_rate)
    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "生命值", 0.4));//E后 25s (get_team)
    temp.push_back(nullptr);//Q后平A变为重A，伤害*1.56*5
    character_list.push_back(new Character("夜兰", "水", "弓", 14450, 244, 548, "暴击率", 0.192,
                                           9, "物理", (0.804 + 0.772 + 1.02 + 0.643 * 2) / 5,
                                           (0.747 + 0.717 + 0.948 + 0.597 * 2) / 5, "水", 0, 0,
                                           "物理", 2.81, 2.61,
                                           9, 4, 0, 0, 0, 0,
                                           9, 70, 0, 0,
                                           0, 0,
                                           0, temp,
                                           new Character_special_arguments(true, false, false, 1, 10, 10, 0.1,
                                                                           false, false, false, -1, 3, 4, true, false)));
    temp.clear();
    //重A 生命倍率19.68% E 生命倍率38.4% Q 生命倍率(12.42+8.28*45)/46 (extra_rate)

    temp.push_back(nullptr);//每柄雨帘剑恢复当前角色6%行秋最大生命
    temp.push_back(new Set(new Condition("水", "ALL", "ALL"), "伤害加成", 0.2));
    temp.push_back(nullptr);//雨帘剑变为4柄
    temp.push_back(new Set(new Condition("水", "ALL", "ALL"), "抗性削弱", 0.15));//Q变为18s Q命中 (get_team)
    temp.push_back(nullptr);//Q期间，E倍率*1.5 (special)
    temp.push_back(nullptr);//每2次攻击，下次攻击变为5柄剑，回复3点能量 (recharge)
    character_list.push_back(new Character("行秋", "水", "单手剑", 10222, 202, 758, "攻击力", 0.24,
                                           9, "物理", (0.921 + 0.942 + 0.564 + 0.564 + 1.107 + 0.709 + 0.709) / 7,
                                           (0.856 + 0.875 + 0.525 + 0.525 + 1.029 + 0.659 + 0.659) / 7, "物理",
                                           (0.935 + 1.11) / 2, (0.869 + 1.032) / 2, "物理", 3.16, 2.93,
                                           9 + 3, 5, (3.57 + 4.06) / 2, (3.36 + 3.82) / 2, (3.02 + 3.44) / 2,
                                           (2.86 + 3.25) / 2,
                                           9 + 3, 80, 1.15, 1.09, 0.977, 0.923,
                                           6, temp,
                                           new Character_special_arguments(true, false, false, 1, 21, 0.5, 0.1,
                                                                           false, false, false, 2, -1, -1, false, false)));
    temp.clear();

    temp.push_back(nullptr);//E喷火距离提高20%
    temp.push_back(nullptr);//E结束拾取辣椒提高10%攻击 (get_team)，不考虑
    temp.push_back(new Set(new Condition("火", "ALL", "ALL"), "抗性削弱", 0.15));//E伤害 (get_team)
    temp.push_back(nullptr);//平A最后一击2s后造成75%火元素伤害
    temp.push_back(nullptr);//Q变为14s
    temp.push_back(new Set(new Condition("火", "ALL", "平A|重A|下落A|E"), "伤害加成", 0.15));//Q持续期间 (get_team)
    character_list.push_back(new Character("香菱", "火", "长柄武器", 10875, 225, 669, "元素精通", 96,
                                           9, "物理", (0.831 + 0.833 + 0.515 + 0.515 + 0.279 * 4 + 1.4) / 9,
                                           (0.773 + 0.774 + 0.479 + 0.479 + 0.259 * 4 + 1.31) / 9, "物理", 2.41, 2.24,
                                           "物理", 3.16, 2.93,
                                           9 + 3, 4, 2.36, 2.23, 2.0, 1.89,
                                           9 + 3, 80, (1.53 + 1.87 + 2.33 + 12 * 2.38) / 15,
                                           (1.44 + 1.76 + 2.19 + 12 * 2.24) / 15, (1.3 + 1.58 + 1.97 + 12 * 2.02) / 15,
                                           (1.22 + 1.5 + 1.86 + 12 * 1.9) / 15,
                                           6, temp,
                                           new Character_special_arguments(true, true, false, 1, 12, 1.5, 1,
                                                                           false, false, false, -1, -1, -1, false, false)));//E 4次，Q 12次
    temp.clear();

    temp.push_back(nullptr);//Q重置E
    temp.push_back(nullptr);//精通转化E伤害mastery*0.15% (convert)
    temp.push_back(nullptr);//每个E为Q后恢复8点能量，24点 (recharge)
    temp.push_back(nullptr);//E倍率提升25% (get_special)
    temp.push_back(new Set(new Condition("雷", "ALL", "ALL"), "伤害加成", 0.2));//E命中 (get_team)
    temp.push_back(new Set(new Condition("ALL", "ALL", "E"), "防御无视", 0.6));
    character_list.push_back(new Character("八重神子", "雷", "法器", 10372, 340, 569, "暴击率", 0.192,
                                           9, "雷", (0.714 + 0.693 + 1.024) / 3, (0.674 + 0.655 + 0.967) / 3, "雷",
                                           2.572, 2.429, "雷", 2.81, 2.61,
                                           9, 5, 2.015, 1.896, 1.706, 1.612,
                                           9, 90, (5.53 + 7.09 * 3) / 4, (5.2 + 6.68 * 3) / 4, (4.68 + 6.01 * 3) / 4,
                                           (4.42 + 5.68 * 3) / 4,
                                           1, temp,
                                           new Character_special_arguments(true, false, false, 3, 1, 1, 1,
                                                                           false, false, false, -1, -1, -1, false, false)));//E 15次
    temp.clear();

    temp.push_back(nullptr);//E产生上升气流20s
    temp.push_back(nullptr);//Q恢复自己与转化元素15点能量 (recharge)
    temp.push_back(nullptr);//重A发射分裂箭
    temp.push_back(new Set(new Condition("风|物理", "ALL", "ALL"), "抗性削弱", 0.24));//E后起飞0.24，落地0.12 (get_team)
    temp.push_back(new Set(new Condition("风", "ALL", "ALL"), "伤害加成", 0.25));//获得元素微粒
    temp.push_back(new Set(new Condition("火|水|雷|冰|风", "ALL", "ALL"), "抗性削弱", 0.2));//Q (get_team)
    character_list.push_back(new Character("温迪", "风", "弓", 10531, 263, 669, "元素充能效率", 0.32,
                                           9, "物理",
                                           (0.403 + 0.403 + 0.877 + 1.035 + 0.515 + 0.515 + 1.001 + 1.403) / 8,
                                           (0.375 + 0.375 + 0.815 + 0.962 + 0.479 + 0.479 + 0.931 + 1.304) / 8, "风",
                                           2.23, 2.11, "物理", 2.81, 2.61,
                                           9, 3, 5.87, 5.52, 4.97, 4.69,
                                           9, 60, 0.799, 0.752, 0.677, 0.639,
                                           0, temp,
                                           new Character_special_arguments(true, true, false, 1, 6, 6, 0.1,
                                                                           false, false, false, -1, 3, 3, true, false)));//E短按 Q 20风伤+20转化+12扩散
    temp.clear();

    temp.push_back(nullptr);//冲刺自动产生E
    temp.push_back(nullptr);//充能转化水伤20%*recharge (get_convert)
    temp.push_back(nullptr);//Q后感电、蒸发、扩散伤害提升15%，冻结时间提升15% (react)
    temp.push_back(nullptr);//平A后20%重A一次，CD5s
    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "暴击率", 0.15));//Q后 (get_team)
    temp.push_back(new Set(new Condition("ALL", "ALL", "重A"), "伤害加成", 1.8));//冲刺 60%/s
    character_list.push_back(new Character("莫娜", "水", "法器", 10409, 287, 653, "元素充能效率", 0.32,
                                           9, "水", (0.677 + 0.648 + 0.806 + 1.01) / 4,
                                           (0.639 + 0.612 + 0.762 + 0.955) / 4, "水", 2.69, 2.55,
                                           "水", 2.81, 2.61,
                                           9, 3, (0.68 * 4 + 2.82) / 5, (0.64 * 4 + 2.66) / 5, (0.576 * 4 + 2.39) / 5,
                                           (0.544 * 4 + 2.26) / 5,
                                           9, 60, 9.4, 8.85, 7.96, 7.52,
                                           1, temp,
                                           new Character_special_arguments(true, false, false, 1, 12, 1, 8,
                                                                           false, false, false, -1, -1, -1, false, false)));
    temp.clear();
    //Q 58%增伤(special get_team)

    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "护盾强效", 0.25));//护盾受伤害，5%强效/层，5层 (get_team)
    temp.push_back(nullptr);//普通攻击、重击与下落攻击造成的伤害提高值：生命值上限的1.39%。地心的岩脊、共鸣伤害与长按伤害提高值：生命值上限的1.9%。天星伤害提高值：生命值上限33% (extra_rate)
    temp.push_back(nullptr);//岩脊至多可以同时存在2个
    temp.push_back(nullptr);//Q加盾
    temp.push_back(nullptr);//Q影响范围扩大20%，石化效果持续时间延长2秒。
    temp.push_back(nullptr);//护盾生效期间，受到伤害的40%转为等额生命值恢复。单次恢复量不超过角色最大生命值的8%。
    character_list.push_back(new Character("钟离", "岩", "长柄武器", 14695, 251, 738, "伤害加成", 0.288,
                                           9, "物理", (0.608 + 0.616 + 0.763 + 0.849 + 0.213 * 4 + 1.077) / 9,
                                           (0.565 + 0.572 + 0.709 + 0.789 + 0.198 * 4 + 1.001) / 9, "物理", 2.2, 2.04,
                                           "物理", 3.16, 2.93,
                                           9, 0.5, 1.7, 1.6, 1.44, 1.36,
                                           9, 40, 10.84, 10.3, 9, 8.35,
                                           0, temp,
                                           new Character_special_arguments(true, false, false, 1, 12, 1, 12,
                                                                           true, false, false, -1, -1, -1, false, false)));//E长按不考虑后续伤害
    temp.clear();
    //E护盾减抗20%(special get_team)


    //NO NEED TO CALCULATE
    //BUILD BASIC INFORMATION AND ARGS
    temp.push_back(nullptr);
    temp.push_back(nullptr);
    temp.push_back(nullptr);
    temp.push_back(nullptr);
    temp.push_back(nullptr);
    temp.push_back(nullptr);
    character_list.push_back(new Character("枫原万叶", "风", "单手剑", 13348, 297, 807, "元素精通", 115.0,
                                           9, "物理", 0,
                                           0, "物理", 0, 0,
                                           "物理", 0, 0,
                                           9, 4, 0, 0, 0, 0,
                                           9, 60, 0, 0, 0, 0,
                                           0, temp,
                                           new Character_special_arguments(true, true, false, 1, 6, 6, 2,
                                                                           false, false, false, 2, -1, -1, false, false)));
    temp.clear();

    temp.push_back(nullptr);
    temp.push_back(nullptr);
    temp.push_back(nullptr);
    temp.push_back(nullptr);
    temp.push_back(nullptr);
    temp.push_back(nullptr);
    character_list.push_back(new Character("班尼特", "火", "单手剑", 12397, 191, 771, "元素充能效率", 0.267,
                                           9, "物理", 0,
                                           0, "物理", 0, 0,
                                           "物理", 0, 0,
                                           9, 2, 0, 0, 0, 0,
                                           9, 60, 0, 0, 0, 0,
                                           0, temp,
                                           new Character_special_arguments(true, false, false, 1, 4, 4, 15,
                                                                           false, true, false, 2, -1, -1, false, false)));//E长按不考虑后续伤害
    temp.clear();
}

//build new character(needed)
bool Character::get_extra_special(Deployment *data) const
{
    if (data->c_point->name == "雷电将军")
    {
        if (data->config->condition->attack_way == "Q")
            data->add_percentage("伤害加成", 0.27, (name + "_extra_special"));
    }
    else if (data->c_point->name == "行秋")
    {
        if (data->config->condition->attack_way == "E" && data->c_point->constellation >= 4)
            data->base_skillrate *= 1.5;
    }
    else if (data->c_point->name == "八重神子")
    {
        if (data->config->condition->attack_way == "E" && data->c_point->constellation >= 2)
            data->base_skillrate *= 1.25;
    }
    else if (data->c_point->name == "莫娜")
    {
        data->add_percentage("伤害加成", 0.58, (name + "_extra_special"));
    }
    else if (data->c_point->name == "钟离")
    {
        data->add_percentage("抗性削弱", 0.2, (name + "_extra_special"));
    }
    return true;
}

//build new weapon(all)
//高估：
//  原木刀/森林王器/贯月矢：自己吃不到种子（默认触发）
//  铁峰刺：元素伤害，2层6s（默认满层）
//      万国诸海图谱：元素反应，2层10s（默认满层）
//  匣里日月：EQ命中，6s（默认触发）
//  流浪乐章：随机（默认每个效果取1/4）
//  盈满之实：元素反应，5层6s（默认满层）
//      阿莫斯之弓：0.1s一层，5层（默认平A2层，重A4层）
//  幽夜华尔兹：E命中，5s（默认触发）
//  千岩古剑/千岩长枪：璃月队友数量（默认4个）
//  决斗之枪：（默认单怪攻击，多怪防御）
void init_weapon_data()
{
    vector<Set *> temp;

    //(special)
    temp.push_back(new Set(new Condition("火|水|雷|冰|风|岩|草", "ALL", "ALL"), "伤害加成", 0.12));
    weapon_list.push_back(new Weapon("雾切之回光", "单手剑", 674, "暴击伤害", 0.441, 1, temp));
    temp.clear();

    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "攻击力", 0.2));
    weapon_list.push_back(new Weapon("风鹰剑", "单手剑", 674, "伤害加成", 0.413, 1, temp));
    temp.clear();

    temp.push_back(new Set(new Condition("火|水|雷|冰|风|岩|草", "ALL", "ALL"), "伤害加成", 0.12));
    temp.push_back(new Set(new Condition("ALL", "ALL", "平A"), "伤害加成", 0.4));//队友E，2层
    weapon_list.push_back(new Weapon("波乱月白经津", "单手剑", 608, "暴击率", 0.331, 1, temp));
    temp.clear();

    //(special)
    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "护盾强效", 0.2));
    weapon_list.push_back(new Weapon("斫峰之刃", "单手剑", 608, "攻击力", 0.496, 1, temp));
    temp.clear();

    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "暴击率", 0.04));
    temp.push_back(new Set(new Condition("ALL", "ALL", "平A|重A"), "额外倍率", 0.2));//Q后
    weapon_list.push_back(new Weapon("天空之刃", "单手剑", 608, "元素充能效率", 0.551, 1, temp));
    temp.clear();

    //(special)
    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "伤害加成", 0.1));
    weapon_list.push_back(new Weapon("苍古自由之誓", "单手剑", 608, "元素精通", 198.0, 1, temp));
    temp.clear();

    //(get_convert)
    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "生命值", 0.2));
    weapon_list.push_back(new Weapon("磐岩结绿", "单手剑", 542, "暴击率", 0.441, 1, temp));
    temp.clear();

    //TODO:NEW
    //(convert)
    //(get_team)
    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "生命值", 0.2));
    weapon_list.push_back(new Weapon("圣显之钥", "单手剑", 542, "生命值", 0.661, 1, temp));
    temp.clear();

    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "伤害加成", 0.12));
    weapon_list.push_back(new Weapon("暗巷闪光", "单手剑", 620, "元素精通", 55.0, 1, temp));
    temp.clear();

    //(special)
    weapon_list.push_back(new Weapon("试作斩岩", "单手剑", 565, "伤害加成", 0.345, 5, temp));
    temp.clear();

    //(special) (get_team)
    weapon_list.push_back(new Weapon("原木刀", "单手剑", 565, "元素充能效率", 0.306, 5, temp));
    temp.clear();

    temp.push_back(new Set(new Condition("ALL", "ALL", "平A|重A"), "伤害加成", 0.2));
    weapon_list.push_back(new Weapon("黑剑", "单手剑", 510, "暴击率", 0.276, 1, temp));
    temp.clear();

    //(special)
    weapon_list.push_back(new Weapon("匣里龙吟", "单手剑", 510, "攻击力", 0.413, 5, temp));
    temp.clear();

    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "攻击力", 0.15));//平A重A下落A
    weapon_list.push_back(new Weapon("笼钓瓶一心", "单手剑", 510, "攻击力", 0.413, 1, temp));
    temp.clear();

    weapon_list.push_back(new Weapon("笛剑", "单手剑", 510, "攻击力", 0.413, 3, temp));
    temp.clear();

    temp.push_back(new Set(new Condition("ALL", "ALL", "E"), "伤害加成", 0.16));
    temp.push_back(new Set(new Condition("ALL", "ALL", "E"), "暴击率", 0.06));
    weapon_list.push_back(new Weapon("腐殖之剑", "单手剑", 510, "元素充能效率", 0.459, 5, temp));
    temp.clear();

    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "伤害加成", 0.12));//元素伤害，2层
    weapon_list.push_back(new Weapon("铁蜂刺", "单手剑", 510, "元素精通", 165.0, 5, temp));
    temp.clear();

    //TODO:NEW
    //(convert)
    //(get_team)
    weapon_list.push_back(new Weapon("西福斯的月光", "单手剑", 510, "元素精通", 165.0, 1, temp));
    temp.clear();

    //(get_extra_rate)
    weapon_list.push_back(new Weapon("辰砂之纺锤", "单手剑", 454, "防御力", 0.69, 5, temp));
    temp.clear();

    //(recharge)
    weapon_list.push_back(new Weapon("天目影打刀", "单手剑", 454, "攻击力", 0.551, 5, temp));
    temp.clear();

    //(recharge)
    weapon_list.push_back(new Weapon("西风剑", "单手剑", 454, "元素充能效率", 0.613, 4, temp));
    temp.clear();

    //(recharge)
    weapon_list.push_back(new Weapon("祭礼剑", "单手剑", 454, "元素充能效率", 0.613, 5, temp));
    temp.clear();

    temp.push_back(new Set(new Condition("火|水|雷|冰|风|岩|草", "ALL", "ALL"), "伤害加成", 0.12));
    weapon_list.push_back(new Weapon("天空之卷", "法器", 674, "攻击力", 0.331, 1, temp));
    temp.clear();

    //(special)
    weapon_list.push_back(new Weapon("四风原典", "法器", 608, "暴击率", 0.331, 1, temp));
    temp.clear();

    //(special)
    weapon_list.push_back(new Weapon("神乐之真意", "法器", 608, "暴击伤害", 0.662, 1, temp));
    temp.clear();

    //(special)
    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "护盾强效", 0.2));
    weapon_list.push_back(new Weapon("尘世之锁", "法器", 608, "攻击力", 0.496, 1, temp));
    temp.clear();

    //(get_extra_rate)
    //(recharge)
    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "治疗加成", 0.1));
    weapon_list.push_back(new Weapon("不灭月华", "法器", 608, "生命值", 0.496, 1, temp));
    temp.clear();

    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "元素充能效率", 0.24));//E后
    weapon_list.push_back(new Weapon("证誓之明瞳", "法器", 565, "攻击力", 0.276, 5, temp));
    temp.clear();

    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "攻击力", 0.2));//冲刺
    weapon_list.push_back(new Weapon("暗巷的酒与诗", "法器", 565, "元素充能效率", 0.306, 1, temp));
    temp.clear();

    //(special) (get_team)
    weapon_list.push_back(new Weapon("白辰之环", "法器", 565, "元素充能效率", 0.306, 5, temp));
    temp.clear();

    //(special)
    weapon_list.push_back(new Weapon("万国诸海图谱", "法器", 565, "元素精通", 110.0, 5, temp));
    temp.clear();

    //(special)
    weapon_list.push_back(new Weapon("匣里日月", "法器", 510, "暴击率", 0.276, 1, temp));
    temp.clear();

    //(special)
    weapon_list.push_back(new Weapon("流浪乐章", "法器", 510, "暴击伤害", 0.551, 5, temp));
    temp.clear();

    weapon_list.push_back(new Weapon("忍冬之果", "法器", 510, "攻击力", 0.413, 5, temp));
    temp.clear();

    //(recharge)
    weapon_list.push_back(new Weapon("试作金珀", "法器", 510, "生命值", 0.413, 5, temp));
    temp.clear();

    //(recharge)
    weapon_list.push_back(new Weapon("西风秘典", "法器", 510, "元素充能效率", 0.459, 4, temp));
    temp.clear();

    //TODO:NEW
    //(convert)
    //(get_team)
    weapon_list.push_back(new Weapon("流浪的晚星", "法器", 510, "元素精通", 165.0, 1, temp));
    temp.clear();

    //(special)
    weapon_list.push_back(new Weapon("盈满之实", "法器", 510, "元素充能效率", 0.459, 5, temp));
    temp.clear();

    weapon_list.push_back(new Weapon("昭心", "法器", 454, "攻击力", 0.551, 3, temp));
    temp.clear();

    //(special)
    weapon_list.push_back(new Weapon("嘟嘟可故事集", "法器", 454, "攻击力", 0.551, 5, temp));
    temp.clear();

    //(recharge)
    weapon_list.push_back(new Weapon("祭礼残章", "法器", 454, "元素精通", 221.0, 5, temp));
    temp.clear();

    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "暴击伤害", 0.2));
    weapon_list.push_back(new Weapon("天空之翼", "弓", 674, "暴击率", 0.221, 1, temp));
    temp.clear();

    //(special)
    temp.push_back(new Set(new Condition("ALL", "ALL", "E|Q"), "伤害加成", 0.12));
    weapon_list.push_back(new Weapon("冬极白星", "弓", 608, "暴击率", 0.331, 1, temp));
    temp.clear();

    //(special)
    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "攻击力", 0.2));
    weapon_list.push_back(new Weapon("飞雷之弦振", "弓", 608, "暴击伤害", 0.662, 1, temp));
    temp.clear();

    //高估
    temp.push_back(new Set(new Condition("ALL", "ALL", "平A|重A"), "伤害加成", 0.12));
    temp.push_back(new Set(new Condition("ALL", "ALL", "平A"), "伤害加成", 0.16));//2层
    temp.push_back(new Set(new Condition("ALL", "ALL", "重A"), "伤害加成", 0.32));//4层
    weapon_list.push_back(new Weapon("阿莫斯之弓", "弓", 608, "攻击力", 0.496, 1, temp));
    temp.clear();

    //(get_team)
    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "元素精通", 60.0));
    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "元素精通", 100.0));
    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "攻击力", 0.2));
    weapon_list.push_back(new Weapon("终末嗟叹之诗", "弓", 608, "元素充能效率", 0.551, 1, temp));
    temp.clear();

    //(get_extra_rate)
    temp.push_back(new Set(new Condition("火|水|雷|冰|风|岩|草", "ALL", "ALL"), "伤害加成", 0.12));
    weapon_list.push_back(new Weapon("猎人之径", "弓", 542, "暴击率", 0.441, 1, temp));
    temp.clear();

    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "生命值", 0.16));
    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "伤害加成", 0.2));
    weapon_list.push_back(new Weapon("若水", "弓", 542, "暴击伤害", 0.882, 1, temp));
    temp.clear();

    //(special)
    weapon_list.push_back(new Weapon("暗巷猎手", "弓", 565, "攻击力", 0.276, 1, temp));
    temp.clear();

    //(special)
    weapon_list.push_back(new Weapon("曚云之月", "弓", 565, "攻击力", 0.276, 2, temp));
    temp.clear();

    //(recharge)
    weapon_list.push_back(new Weapon("祭礼弓", "弓", 565, "元素充能效率", 0.306, 3, temp));
    temp.clear();

    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "伤害加成", 0.1));
    weapon_list.push_back(new Weapon("落霞", "弓", 565, "元素充能效率", 0.306, 5, temp));
    temp.clear();

    weapon_list.push_back(new Weapon("苍翠猎弓", "弓", 510, "暴击率", 0.276, 1, temp));
    temp.clear();

    //(special)
    temp.push_back(new Set(new Condition("ALL", "ALL", "平A"), "伤害加成", 0.4));
    weapon_list.push_back(new Weapon("弓藏", "弓", 510, "攻击力", 0.413, 2, temp));
    temp.clear();

    weapon_list.push_back(new Weapon("试作澹月", "弓", 510, "攻击力", 0.413, 5, temp));
    temp.clear();

    //(special)
    weapon_list.push_back(new Weapon("幽夜华尔兹", "弓", 510, "伤害加成", 0.517, 1, temp));
    temp.clear();

    weapon_list.push_back(new Weapon("竭泽", "弓", 510, "元素充能效率", 0.459, 5, temp));
    temp.clear();

    temp.push_back(new Set(new Condition("ALL", "ALL", "E|Q"), "伤害加成", 0.24));
    weapon_list.push_back(new Weapon("绝弦", "弓", 510, "元素精通", 165.0, 4, temp));
    temp.clear();

    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "攻击力", 0.16));//E后
    weapon_list.push_back(new Weapon("风花之颂", "弓", 510, "元素精通", 165.0, 5, temp));
    temp.clear();

    //(special)
    weapon_list.push_back(new Weapon("破魔之弓", "弓", 454, "攻击力", 0.551, 5, temp));
    temp.clear();

    //(special)
    weapon_list.push_back(new Weapon("王下近侍", "弓", 454, "攻击力", 0.551, 5, temp));
    temp.clear();

    //(special)
    weapon_list.push_back(new Weapon("钢轮弓", "弓", 454, "伤害加成", 0.69, 5, temp));
    temp.clear();

    //(recharge)
    weapon_list.push_back(new Weapon("西风猎弓", "弓", 454, "元素充能效率", 0.613, 5, temp));
    temp.clear();

    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "攻击力", 0.16));
    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "攻击力", 0.2));//平A重A4次 (get_team)
    weapon_list.push_back(new Weapon("松籁响起之时", "双手剑", 741, "伤害加成", 0.207, 1, temp));
    temp.clear();

    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "伤害加成", 0.08));
    weapon_list.push_back(new Weapon("天空之傲", "双手剑", 674, "元素充能效率", 0.368, 1, temp));
    temp.clear();

    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "攻击力", 0.2));//默认不触发特效
    weapon_list.push_back(new Weapon("狼的末路", "双手剑", 608, "攻击力", 0.496, 1, temp));
    temp.clear();

    //(special)
    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "护盾强效", 0.2));
    weapon_list.push_back(new Weapon("无工之剑", "双手剑", 608, "攻击力", 0.496, 1, temp));
    temp.clear();

    //(get_extra_rate)
    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "防御力", 0.28));
    weapon_list.push_back(new Weapon("赤角石溃杵", "双手剑", 542, "暴击伤害", 0.882, 1, temp));
    temp.clear();

    weapon_list.push_back(new Weapon("试作古华", "双手剑", 565, "攻击力", 0.276, 5, temp));
    temp.clear();

    weapon_list.push_back(new Weapon("雪葬的星银", "双手剑", 565, "伤害加成", 0.345, 5, temp));
    temp.clear();

    //(recharge)
    weapon_list.push_back(new Weapon("祭礼大剑", "双手剑", 565, "元素充能效率", 0.306, 4, temp));
    temp.clear();

    //(special) (get_team)
    weapon_list.push_back(new Weapon("森林王器", "双手剑", 565, "元素充能效率", 0.306, 5, temp));
    temp.clear();

    //(special)
    weapon_list.push_back(new Weapon("螭骨剑", "双手剑", 510, "暴击率", 0.276, 5, temp));
    temp.clear();

    //(special)
    weapon_list.push_back(new Weapon("白影剑", "双手剑", 510, "防御力", 0.517, 5, temp));
    temp.clear();

    //(special)
    weapon_list.push_back(new Weapon("千岩古剑", "双手剑", 510, "攻击力", 0.413, 1, temp));
    temp.clear();

    //(special)
    weapon_list.push_back(new Weapon("恶王丸", "双手剑", 510, "攻击力", 0.413, 1, temp));
    temp.clear();

    //(special)
    weapon_list.push_back(new Weapon("钟剑", "双手剑", 510, "生命值", 0.413, 4, temp));
    temp.clear();

    //(recharge)
    temp.push_back(new Set(new Condition("ALL", "ALL", "E"), "伤害加成", 0.06));
    weapon_list.push_back(new Weapon("桂木斩长正", "双手剑", 510, "元素充能效率", 0.459, 5, temp));
    temp.clear();

    //(special)
    weapon_list.push_back(new Weapon("雨裁", "双手剑", 510, "元素精通", 165.0, 5, temp));
    temp.clear();

    //TODO:NEW
    //(convert)
    //(get_team)
    weapon_list.push_back(new Weapon("玛海菈的水色", "双手剑", 510, "元素精通", 165.0, 1, temp));
    temp.clear();

    //(recharge)
    weapon_list.push_back(new Weapon("西风大剑", "双手剑", 454, "元素充能效率", 0.613, 5, temp));
    temp.clear();

    temp.push_back(new Set(new Condition("ALL", "ALL", "Q"), "伤害加成", 0.12));
    weapon_list.push_back(new Weapon("衔珠海皇", "双手剑", 454, "攻击力", 0.551, 5, temp));
    temp.clear();

    //(special)
    temp.push_back(new Set(new Condition("火|水|雷|冰|风|岩|草", "ALL", "ALL"), "伤害加成", 0.12));
    weapon_list.push_back(new Weapon("息灾", "长柄武器", 741, "攻击力", 0.165, 1, temp));
    temp.clear();

    //(special)
    weapon_list.push_back(new Weapon("和璞鸢", "长柄武器", 674, "暴击率", 0.221, 1, temp));
    temp.clear();

    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "暴击率", 0.08));
    weapon_list.push_back(new Weapon("天空之脊", "长柄武器", 674, "元素充能效率", 0.368, 1, temp));
    temp.clear();

    //(get_convert)
    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "生命值", 0.2));
    weapon_list.push_back(new Weapon("护摩之杖", "长柄武器", 608, "暴击伤害", 0.662, 1, temp));
    temp.clear();

    //(special)
    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "护盾强效", 0.2));
    weapon_list.push_back(new Weapon("贯虹之槊", "长柄武器", 608, "攻击力", 0.496, 1, temp));
    temp.clear();

    //(special) (get_convert)
    weapon_list.push_back(new Weapon("薙草之稻光", "长柄武器", 608, "元素充能效率", 0.551, 1, temp));
    temp.clear();

    //TODO:NEW
    //(convert)
    //(get_team)
    weapon_list.push_back(new Weapon("赤沙之杖", "长柄武器", 542, "暴击率", 0.441, 1, temp));
    temp.clear();

    //(special)
    weapon_list.push_back(new Weapon("断浪长鳍", "长柄武器", 620, "攻击力", 0.138, 4, temp));
    temp.clear();

    //(special)
    weapon_list.push_back(new Weapon("千岩长枪", "长柄武器", 565, "攻击力", 0.276, 1, temp));
    temp.clear();

    temp.push_back(new Set(new Condition("ALL", "ALL", "平A|重A"), "额外倍率", 0.2));
    weapon_list.push_back(new Weapon("流月针", "长柄武器", 565, "伤害加成", 0.345, 5, temp));
    temp.clear();

    //(recharge)
    weapon_list.push_back(new Weapon("西风长枪", "长柄武器", 565, "元素充能效率", 0.306, 5, temp));
    temp.clear();

    //(recharge)
    temp.push_back(new Set(new Condition("ALL", "ALL", "E"), "伤害加成", 0.06));
    weapon_list.push_back(new Weapon("喜多院十文字", "长柄武器", 565, "元素精通", 110.0, 5, temp));
    temp.clear();

    //(special) (get_team)
    weapon_list.push_back(new Weapon("贯月矢", "长柄武器", 565, "元素精通", 110.0, 5, temp));
    temp.clear();

    //(special)
    weapon_list.push_back(new Weapon("试作星镰", "长柄武器", 510, "元素充能效率", 0.459, 5, temp));
    temp.clear();

    //TODO:NEW
    //(special)
    weapon_list.push_back(new Weapon("风信之锋", "长柄武器", 510, "攻击力", 0.413, 5, temp));
    temp.clear();

    temp.push_back(new Set(new Condition("ALL", "ALL", "Q"), "伤害加成", 0.16));
    temp.push_back(new Set(new Condition("ALL", "ALL", "Q"), "暴击率", 0.06));
    weapon_list.push_back(new Weapon("渔获", "长柄武器", 510, "元素充能效率", 0.459, 5, temp));
    temp.clear();

    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "攻击力", 0.24));//单怪条件
    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "防御力", 0.16));//多怪条件
    weapon_list.push_back(new Weapon("决斗之枪", "长柄武器", 454, "暴击率", 0.368, 3, temp));
    temp.clear();

    weapon_list.push_back(new Weapon("龙脊长枪", "长柄武器", 454, "伤害加成", 0.69, 5, temp));
    temp.clear();

    //(special)
    weapon_list.push_back(new Weapon("匣里灭辰", "长柄武器", 454, "元素精通", 221.0, 2, temp));
    temp.clear();
}

//build new weapon(all)
bool Weapon::get_extra_special(Deployment *data) const
{
    if (name == "雾切之回光")
    {
        if (data->config->condition->ele_type != "物理")
        {
            if (data->c_point->args->sword_wuqie_level == 1) data->add_percentage("伤害加成", (0.08 * (0.75 + level * 0.25)), (name + "_extra_special"));
            else if (data->c_point->args->sword_wuqie_level == 2) data->add_percentage("伤害加成", (0.16 * (0.75 + level * 0.25)), (name + "_extra_special"));
            else if (data->c_point->args->sword_wuqie_level == 3) data->add_percentage("伤害加成", (0.28 * (0.75 + level * 0.25)), (name + "_extra_special"));
        }
    }
    else if (name == "斫峰之刃")
    {
        if (data->c_point->args->accumulate_attacks)
        {
            data->add_percentage("攻击力", (0.04 * 5 * (0.75 + level * 0.25)), (name + "_extra_special"));
            if (data->c_point->args->shield_sustain || data->config->teammate_1->args->shield_sustain || data->config->teammate_2->args->shield_sustain ||
                data->config->teammate_3->args->shield_sustain)
                data->add_percentage("攻击力", (0.04 * 5 * (0.75 + level * 0.25)), (name + "_extra_special"));
        }
    }
    else if (name == "苍古自由之誓")
    {
        if (data->config->react_type != "NONE")
        {
            if (data->config->condition->attack_way == "平A" || data->config->condition->attack_way == "重A" || data->config->condition->attack_way == "下落A")
                data->add_percentage("伤害加成", (0.16 * (0.75 + level * 0.25)), (name + "_extra_special"));
            data->add_percentage("攻击力", (0.2 * (0.75 + level * 0.25)), (name + "_extra_special"));
        }
    }
    else if (name == "试作斩岩")
    {
        if (data->c_point->args->accumulate_attacks)
        {
            data->add_percentage("攻击力", (0.04 * 4 * (0.75 + level * 0.25)), (name + "_extra_special"));
            data->add_percentage("防御力", (0.04 * 4 * (0.75 + level * 0.25)), (name + "_extra_special"));
        }
    }
    else if (name == "原木刀")
    {
        if (data->config->react_type.find("燃烧") != string::npos || data->config->react_type.find("激化") != string::npos || data->config->react_type.find("绽放") != string::npos)
            data->add_percentage("元素精通", (60.0 * (0.75 + level * 0.25)), (name + "_extra_special"));
    }
    else if (name == "匣里龙吟")
    {
        if (data->config->ele_attach_type.find("火") != string::npos || data->config->ele_attach_type.find("雷") != string::npos)
            data->add_percentage("伤害加成", (0.16 + level * 0.04), (name + "_extra_special"));
    }

    else if (name == "四风原典")
    {
        if (!data->c_point->args->background)
            if (data->config->condition->ele_type != "物理")
                data->add_percentage("伤害加成", (0.16 * (0.75 + level * 0.25)), (name + "_extra_special"));
    }
    else if (name == "神乐之真意")
    {
        if (data->c_point->args->E_release_times >= 3 || data->c_point->args->E_release_interval < 8)
        {
            if (data->config->condition->attack_way == "E")
                data->add_percentage("伤害加成", (0.12 * 3 * (0.75 + level * 0.25)), (name + "_extra_special"));
            if (data->config->condition->ele_type != "物理")
                data->add_percentage("伤害加成", (0.12 * (0.75 + level * 0.25)), (name + "_extra_special"));
        }
        else if (data->c_point->args->E_release_times == 2 || data->c_point->args->E_release_interval < 16)
        {
            if (data->config->condition->attack_way == "E")
                data->add_percentage("伤害加成", (0.12 * 2 * (0.75 + level * 0.25)), (name + "_extra_special"));
        }
        else if (data->c_point->args->E_release_times == 1)
        {
            if (data->config->condition->attack_way == "E")
                data->add_percentage("伤害加成", (0.12 * (0.75 + level * 0.25)), (name + "_extra_special"));
        }
    }
    else if (name == "尘世之锁")
    {
        if (data->c_point->args->accumulate_attacks)
        {
            data->add_percentage("攻击力", (0.04 * 5 * (0.75 + level * 0.25)), (name + "_extra_special"));
            if (data->c_point->args->shield_sustain || data->config->teammate_1->args->shield_sustain || data->config->teammate_2->args->shield_sustain ||
                data->config->teammate_3->args->shield_sustain)
                data->add_percentage("攻击力", (0.04 * 5 * (0.75 + level * 0.25)), (name + "_extra_special"));
        }
    }
    else if (name == "白辰之环")
    {
        if ((data->config->react_type.find("超载") && (data->config->condition->ele_type == "雷" || data->config->condition->ele_type == "火")) ||
            (data->config->react_type.find("感电") && (data->config->condition->ele_type == "雷" || data->config->condition->ele_type == "水")) ||
            (data->config->react_type.find("激化") && (data->config->condition->ele_type == "雷" || data->config->condition->ele_type == "草")) ||
            (data->config->react_type.find("超导") && (data->config->condition->ele_type == "雷" || data->config->condition->ele_type == "冰")))
            data->add_percentage("伤害加成", (0.1 * (0.75 + level * 0.25)), (name + "_extra_special"));
    }
    else if (name == "万国诸海图谱")
    {
        if (data->config->react_type != "NONE")
            if (data->config->condition->ele_type != "物理")
                data->add_percentage("伤害加成", (0.16 * (0.75 + level * 0.25)), (name + "_extra_special"));
    }
    else if (name == "匣里日月")
    {
        if (!data->c_point->args->background)
        {
            if (data->config->condition->attack_way == "平A" || data->config->condition->attack_way == "E" || data->config->condition->attack_way == "Q")
                data->add_percentage("伤害加成", (0.2 * (0.75 + level * 0.25)), (name + "_extra_special"));
        }
    }
    else if (name == "流浪乐章")
    {
        //每个各取1/4
        data->add_percentage("攻击力", (0.15 * (0.75 + level * 0.25)), (name + "_extra_special"));
        data->add_percentage("伤害加成", (0.12 * (0.75 + level * 0.25)), (name + "_extra_special"));
        data->add_percentage("元素精通", (60.0 * (0.75 + level * 0.25)), (name + "_extra_special"));
    }
    else if (name == "盈满之实")
    {
        if (data->config->react_type != "NONE")
        {
            data->add_percentage("元素精通", (5 * (21.0 + level * 3.0)), (name + "_extra_special"));//元素反应，5层
            data->add_percentage("攻击力", -0.25, (name + "_extra_special"));//元素反应，5层
        }
    }
    else if (name == "嘟嘟可故事集")
    {
        if (!data->c_point->args->background)
        {
            if (data->config->condition->attack_way == "重A")
                data->add_percentage("伤害加成", (0.16 * (0.75 + level * 0.25)), (name + "_extra_special"));
            data->add_percentage("攻击力", (0.08 * (0.75 + level * 0.25)), (name + "_extra_special"));
        }
    }

    else if (name == "冬极白星")
    {
        if (data->c_point->args->bow_dongji_level == 1) data->add_percentage("攻击力", (0.1 * (0.75 + level * 0.25)), (name + "_extra_special"));
        else if (data->c_point->args->bow_dongji_level == 2) data->add_percentage("攻击力", (0.2 * (0.75 + level * 0.25)), (name + "_extra_special"));
        else if (data->c_point->args->bow_dongji_level == 3) data->add_percentage("攻击力", (0.3 * (0.75 + level * 0.25)), (name + "_extra_special"));
        else if (data->c_point->args->bow_dongji_level == 4) data->add_percentage("攻击力", (0.48 * (0.75 + level * 0.25)), (name + "_extra_special"));
    }
    else if (name == "飞雷之弦振")
    {
        if (data->config->condition->attack_way == "平A")
        {
            if (data->c_point->args->bow_feilei_level == 1) data->add_percentage("伤害加成", (0.12 * (0.75 + level * 0.25)), (name + "_extra_special"));
            else if (data->c_point->args->bow_feilei_level == 2) data->add_percentage("伤害加成", (0.24 * (0.75 + level * 0.25)), (name + "_extra_special"));
            else if (data->c_point->args->bow_feilei_level == 3) data->add_percentage("伤害加成", (0.40 * (0.75 + level * 0.25)), (name + "_extra_special"));
        }
    }
    else if (name == "暗巷猎手")
    {
        if (data->c_point->args->background)
            data->add_percentage("伤害加成", 0.2 * (0.75 + level * 0.25), (name + "_extra_special"));
    }
    else if (name == "曚云之月")
    {
        if (data->config->condition->attack_way == "Q")
            data->add_percentage("伤害加成",
                                 min(334, data->c_point->Q_energy + data->config->teammate_1->Q_energy + data->config->teammate_2->Q_energy + data->config->teammate_3->Q_energy)
                                 * 0.0012 * (0.75 + level * 0.25), (name + "_extra_special"));
    }
    else if (name == "弓藏")
    {
        if (data->config->condition->attack_way == "重A")
            data->add_percentage("伤害加成", -0.1, (name + "_extra_special"));
    }
    else if (name == "幽夜华尔兹")
    {
        if (!data->c_point->args->background)
        {
            if (data->config->condition->attack_way == "平A" || data->config->condition->attack_way == "E")
                data->add_percentage("伤害加成", (0.2 * (0.75 + level * 0.25)), (name + "_extra_special"));
        }
    }
    else if (name == "破魔之弓")
    {
        if (data->config->condition->attack_way == "平A")
            data->add_percentage("伤害加成", ((data->c_point->args->bow_pomo_fullenergy ? 2 : 1) * 0.16 * (0.75 + level * 0.25)), (name + "_extra_special"));
        if (data->config->condition->attack_way == "重A")
            data->add_percentage("伤害加成", ((data->c_point->args->bow_pomo_fullenergy ? 2 : 1) * 0.12 * (0.75 + level * 0.25)), (name + "_extra_special"));
    }
    else if (name == "王下近侍")
    {
        if (!data->c_point->args->background)
            data->add_percentage("元素精通", (40.0 + level * 20.0), (name + "_extra_special"));//EQ后
    }
    else if (name == "钢轮弓")
    {
        if (data->c_point->args->accumulate_attacks)
            data->add_percentage("攻击力", (0.04 * 4 * (0.75 + level * 0.25)), (name + "_extra_special"));
    }

    else if (name == "无工之剑")
    {
        if (data->c_point->args->accumulate_attacks)
        {
            data->add_percentage("攻击力", (0.04 * 5 * (0.75 + level * 0.25)), (name + "_extra_special"));
            if (data->c_point->args->shield_sustain || data->config->teammate_1->args->shield_sustain || data->config->teammate_2->args->shield_sustain ||
                data->config->teammate_3->args->shield_sustain)
                data->add_percentage("攻击力", (0.04 * 5 * (0.75 + level * 0.25)), (name + "_extra_special"));
        }
    }
    else if (name == "森林王器")
    {
        if (data->config->react_type.find("燃烧") != string::npos || data->config->react_type.find("激化") != string::npos || data->config->react_type.find("绽放") != string::npos)
            data->add_percentage("元素精通", (60.0 * (0.75 + level * 0.25)), (name + "_extra_special"));
    }
    else if (name == "螭骨剑")
    {
        data->add_percentage("伤害加成", (0.25 + level * 0.05), (name + "_extra_special"));
    }
    else if (name == "白影剑")
    {
        if (data->c_point->args->accumulate_attacks)
        {
            data->add_percentage("攻击力", (0.06 * 4 * (0.75 + level * 0.25)), (name + "_extra_special"));
            data->add_percentage("防御力", (0.06 * 4 * (0.75 + level * 0.25)), (name + "_extra_special"));
        }
    }
    else if (name == "千岩古剑")
    {
        data->add_percentage("攻击力", (0.24 + level * 0.04), (name + "_extra_special"));//默认3层
        data->add_percentage("暴击率", (0.08 + level * 0.04), (name + "_extra_special"));//默认3层
    }
    else if (name == "恶王丸")
    {
        if (data->config->condition->attack_way == "Q")
            data->add_percentage("伤害加成",
                                 min(334, data->c_point->Q_energy + data->config->teammate_1->Q_energy + data->config->teammate_2->Q_energy + data->config->teammate_3->Q_energy)
                                 * 0.0012 * (0.75 + level * 0.25), (name + "_extra_special"));
    }
    else if (name == "钟剑")
    {
        if (data->c_point->args->shield_sustain || data->config->teammate_1->args->shield_sustain || data->config->teammate_2->args->shield_sustain ||
            data->config->teammate_3->args->shield_sustain)
            data->add_percentage("伤害加成", (0.12 * (0.75 + level * 0.25)), (name + "_extra_special"));

    }
    else if (name == "雨裁")
    {
        if (data->config->ele_attach_type.find("水") != string::npos || data->config->ele_attach_type.find("雷") != string::npos)
            data->add_percentage("伤害加成", (0.16 + level * 0.04), (name + "_extra_special"));
    }

    else if (name == "息灾")
    {
        data->add_percentage("攻击力", (0.192 * (0.75 + level * 0.25)), (name + "_extra_special"));
        if (data->c_point->args->background && !data->c_point->args->lockface)
            data->add_percentage("攻击力", (0.192 * (0.75 + level * 0.25)), (name + "_extra_special"));
    }
    else if (name == "和璞鸢")
    {
        if (data->c_point->args->accumulate_attacks)
        {
            data->add_percentage("攻击力", (0.025 + level * 0.007) * 7, (name + "_extra_special"));//默认满层
            data->add_percentage("伤害加成", (0.75 + level * 0.25) * 0.12, (name + "_extra_special"));//默认满层
        }
    }
    else if (name == "贯虹之槊")
    {
        if (data->c_point->args->accumulate_attacks)
        {
            data->add_percentage("攻击力", (0.04 * 5 * (0.75 + level * 0.25)), (name + "_extra_special"));
            if (data->c_point->args->shield_sustain || data->config->teammate_1->args->shield_sustain || data->config->teammate_2->args->shield_sustain ||
                data->config->teammate_3->args->shield_sustain)
                data->add_percentage("攻击力", (0.04 * 5 * (0.75 + level * 0.25)), (name + "_extra_special"));
        }
    }
    else if (name == "薙草之稻光")
    {
        data->add_percentage("元素充能效率", (0.25 + level * 0.05), (name + "_extra_special"));//Q后
    }
    else if (name == "断浪长鳍")
    {
        if (data->config->condition->attack_way == "Q")
            data->add_percentage("伤害加成",
                                 min(334, data->c_point->Q_energy + data->config->teammate_1->Q_energy + data->config->teammate_2->Q_energy + data->config->teammate_3->Q_energy)
                                 * 0.0012 * (0.75 + level * 0.25), (name + "_extra_special"));
    }
    else if (name == "千岩长枪")
    {
        data->add_percentage("攻击力", (0.24 + level * 0.04), (name + "_extra_special"));//默认3层
        data->add_percentage("暴击率", (0.08 + level * 0.04), (name + "_extra_special"));//默认3层
    }
    else if (name == "贯月矢")
    {
        if (data->config->react_type.find("燃烧") != string::npos || data->config->react_type.find("激化") != string::npos || data->config->react_type.find("绽放") != string::npos)
            data->add_percentage("攻击力", (0.16 * (0.75 + level * 0.25)), (name + "_extra_special"));
    }
    else if (name == "试作星镰")
    {
        if (data->c_point->args->E_release_times >= 2 || data->c_point->args->E_release_interval < 12)
        {
            if (data->config->condition->attack_way == "平A" || data->config->condition->attack_way == "重A")
                data->add_percentage("伤害加成", (0.08 * 2 * (0.75 + level * 0.25)), (name + "_extra_special"));
        }
        else if (data->c_point->args->E_release_times == 1)
        {
            if (data->config->condition->attack_way == "平A" || data->config->condition->attack_way == "重A")
                data->add_percentage("伤害加成", (0.08 * (0.75 + level * 0.25)), (name + "_extra_special"));
        }
    }
        //TODO:NEW
    else if (name == "风信之锋")
    {
        if (data->config->react_type != "NONE")
        {
            data->add_percentage("攻击力", (0.12 * (0.75 + level * 0.25)), (name + "_extra_special"));
            data->add_percentage("元素精通", (48.0 * (0.75 + level * 0.25)), (name + "_extra_special"));
        }
    }
    else if (name == "匣里灭辰")
    {
        if (data->config->ele_attach_type.find("水") != string::npos || data->config->ele_attach_type.find("火") != string::npos)
            data->add_percentage("伤害加成", (0.16 + level * 0.04), (name + "_extra_special"));
    }
    return true;
}

//build new weapon(all)
void Weapon::modify_useful_attribute(Deployment *data)
{
    if (data->w_point->name == "磐岩结绿") data->data_list[0]->useful = true;
        //TODO:NEW
    else if (data->w_point->name == "圣显之钥") data->data_list[0]->useful = true;
    else if (data->w_point->name == "辰砂之纺锤" && data->config->condition->attack_way == "E") data->data_list[2]->useful = true;
        //TODO:NEW
    else if (data->w_point->name == "西福斯的月光") data->data_list[4]->useful = true;
    else if (data->w_point->name == "不灭月华" && data->config->condition->attack_way == "平A") data->data_list[0]->useful = true;
        //TODO:NEW
    else if (data->w_point->name == "流浪的晚星") data->data_list[4]->useful = true;
    else if (data->w_point->name == "猎人之径" && data->config->condition->attack_way == "重A") data->data_list[4]->useful = true;
    else if (data->w_point->name == "赤角石溃杵" && (data->config->condition->attack_way == "平A" || data->config->condition->attack_way == "重A"))
        data->data_list[2]->useful = true;
        //TODO:NEW
    else if (data->w_point->name == "玛海菈的水色") data->data_list[4]->useful = true;
    else if (data->w_point->name == "护摩之杖") data->data_list[0]->useful = true;
    else if (data->w_point->name == "薙草之稻光") data->data_list[5]->useful = true;
        //TODO:NEW
    else if (data->w_point->name == "赤沙之杖") data->data_list[4]->useful = true;
}//被转化的属性有效

//build new weapon(all)
void Deployment::check_weapon_special(bool &valid)
{
    if (w_point->name == "磐岩结绿")
    {
        if (data_list[1]->useful)
            valid = true;
    }
        //TODO:NEW
    else if (w_point->name == "圣显之钥")
    {
        if (data_list[4]->useful)
            valid = true;
    }
    else if (w_point->name == "辰砂之纺锤")
    {
        if (config->condition->attack_way == "E")
            valid = true;
    }
        //TODO:NEW
    else if (w_point->name == "西福斯的月光")
    {
        if (data_list[5]->useful)
            valid = true;
    }
    else if (w_point->name == "不灭月华")
    {
        if (config->condition->attack_way == "平A")
            valid = true;
    }
        //TODO:NEW
    else if (w_point->name == "流浪的晚星")
    {
        if (data_list[1]->useful)
            valid = true;
    }
    else if (w_point->name == "猎人之径")
    {
        if (config->condition->attack_way == "重A")
            valid = true;
    }
    else if (w_point->name == "赤角石溃杵")
    {
        if (config->condition->attack_way == "平A" || config->condition->attack_way == "重A")
            valid = true;
    }
        //TODO:NEW
    else if (w_point->name == "玛海菈的水色")
    {
        if (data_list[1]->useful)
            valid = true;
    }
    else if (w_point->name == "护摩之杖")
    {
        if (data_list[1]->useful)
            valid = true;
    }
    else if (w_point->name == "薙草之稻光")
    {
        if (data_list[1]->useful)
            valid = true;
    }
        //TODO:NEW
    else if (w_point->name == "赤沙之杖")
    {
        if (data_list[1]->useful)
            valid = true;
    }
}//转化成的属性有效则武器有效

//build new artifact(all)
void init_artifact_data()
{
    artifact_list.push_back(new Artifact("流浪大地的乐团", new Set(new Condition("ALL", "ALL", "ALL"), "元素精通", 80.0),
                                         new Set(new Condition("ALL", "法器|弓", "重A"), "伤害加成", 0.35)));
    artifact_list.push_back(new Artifact("角斗士的终幕礼", new Set(new Condition("ALL", "ALL", "ALL"), "攻击力", 0.18),
                                         new Set(new Condition("ALL", "单手剑|双手剑|长柄武器", "平A"), "伤害加成", 0.35)));
    artifact_list.push_back(new Artifact("冰风迷途的勇士", new Set(new Condition("冰", "ALL", "ALL"), "伤害加成", 0.15),
                                         nullptr));//(special)
    artifact_list.push_back(new Artifact("沉沦之心", new Set(new Condition("水", "ALL", "ALL"), "伤害加成", 0.15),
                                         new Set(new Condition("ALL", "ALL", "平A|重A"), "伤害加成", 0.3)));
    artifact_list.push_back(new Artifact("悠古的磐岩", new Set(new Condition("岩", "ALL", "ALL"), "伤害加成", 0.15),
                                         nullptr));//(get_team)
    artifact_list.push_back(new Artifact("逆飞的流星", new Set(new Condition("ALL", "ALL", "ALL"), "护盾强效", 0.35),
                                         nullptr));//(special)
    artifact_list.push_back(new Artifact("染血的骑士道", new Set(new Condition("物理", "ALL", "ALL"), "伤害加成", 0.25),
                                         nullptr));//new Set(new Condition("ALL", "ALL", "重A"), "伤害加成", 0.5)));//击败敌人
    artifact_list.push_back(new Artifact("昔日宗室之仪", new Set(new Condition("ALL", "ALL", "Q"), "伤害加成", 0.2),
                                         new Set(new Condition("ALL", "ALL", "ALL"), "攻击力", 0.2)));//(get_team) 高估
    artifact_list.push_back(new Artifact("翠绿之影", new Set(new Condition("风", "ALL", "ALL"), "伤害加成", 0.15),
                                         nullptr));//(react) (get_team)
    artifact_list.push_back(new Artifact("被怜爱的少女", new Set(new Condition("ALL", "ALL", "ALL"), "治疗加成", 0.15),
                                         nullptr));//受治疗加成
    artifact_list.push_back(new Artifact("苍白之火", new Set(new Condition("物理", "ALL", "ALL"), "伤害加成", 0.25),
                                         nullptr));//(special)
    artifact_list.push_back(new Artifact("千岩牢固", new Set(new Condition("ALL", "ALL", "ALL"), "生命值", 0.2),
                                         nullptr));//(special) (get_team)
    artifact_list.push_back(new Artifact("炽烈的炎之魔女", new Set(new Condition("火", "ALL", "ALL"), "伤害加成", 0.15),
                                         nullptr));//(special) (react)
    artifact_list.push_back(new Artifact("渡过烈火的贤人", nullptr, nullptr));//(special)
    artifact_list.push_back(new Artifact("如雷的盛怒", new Set(new Condition("雷", "ALL", "ALL"), "伤害加成", 0.15),
                                         nullptr));//(react) 减CD
    artifact_list.push_back(new Artifact("平息鸣雷的尊者", nullptr, nullptr));//(special)
    artifact_list.push_back(new Artifact("追忆之注连", new Set(new Condition("ALL", "ALL", "ALL"), "攻击力", 0.18),
                                         nullptr));//new Set(new Condition("ALL", "ALL", "平A|重A|下落A"), "伤害加成", 0.5)));//15能量
    artifact_list.push_back(new Artifact("绝缘之旗印", new Set(new Condition("ALL", "ALL", "ALL"), "元素充能效率", 0.2),
                                         nullptr));//(get_convert)
    artifact_list.push_back(new Artifact("华馆梦醒形骸记", new Set(new Condition("ALL", "ALL", "ALL"), "防御力", 0.3),
                                         nullptr));//(special)
    artifact_list.push_back(new Artifact("海染砗磲", new Set(new Condition("ALL", "ALL", "ALL"), "治疗加成", 0.15),
                                         nullptr));//额外物理伤害
    artifact_list.push_back(new Artifact("辰砂往生录", new Set(new Condition("ALL", "ALL", "ALL"), "攻击力", 0.18),
                                         nullptr));//(special)
    artifact_list.push_back(new Artifact("来歆余响", new Set(new Condition("ALL", "ALL", "ALL"), "攻击力", 0.18),
                                         new Set(new Condition("ALL", "ALL", "平A"), "额外倍率", 0.35)));
    artifact_list.push_back(new Artifact("深林的记忆", new Set(new Condition("草", "ALL", "ALL"), "伤害加成", 0.15),
                                         nullptr));//(special) (get_team)
    artifact_list.push_back(new Artifact("饰金之梦", new Set(new Condition("ALL", "ALL", "ALL"), "元素精通", 80.0),
                                         nullptr));//(special)
}

//build new artifact(all) 保证二件套效果和四件套效果分开
bool Artifact::get_extra_special(Deployment *data, bool if_4_piece) const
{
    if (if_4_piece && name == "冰风迷途的勇士")
    {
        if (data->config->ele_attach_type.find("冰") != string::npos)//冰附着
            data->add_percentage("暴击率", 0.2, (name + "_extra_special"));
        if (data->config->react_type.find("冻结") != string::npos)
            data->add_percentage("暴击率", 0.2, (name + "_extra_special"));//冻结
    }
    else if (if_4_piece && name == "逆飞的流星")
    {
        if (data->c_point->args->shield_sustain || data->config->teammate_1->args->shield_sustain || data->config->teammate_2->args->shield_sustain ||
            data->config->teammate_3->args->shield_sustain)
            if (data->config->condition->attack_way == "平A" || data->config->condition->attack_way == "重A")
                data->add_percentage("伤害加成", 0.4, (name + "_extra_special"));
    }
    else if (if_4_piece && name == "苍白之火")
    {
        if (data->c_point->args->E_hit_interval < 7 && data->c_point->args->E_hit_interval > 0)
        {
            data->add_percentage("攻击力", 0.18, name + "_extra_special");
            if (data->config->condition->ele_type == "物理") data->add_percentage("伤害加成", 0.25, (name + "_extra_special"));
        }
        else if (data->c_point->args->E_hit_interval >= 7)
        {
            data->add_percentage("攻击力", 0.09, name + "_extra_special");
        }
    }
    else if (if_4_piece && name == "千岩牢固")
    {
        if (data->c_point->args->E_hit_interval < 3 && data->c_point->args->E_hit_interval > 0)
        {
            data->add_percentage("攻击力", 0.2, (name + "_extra_special"));
            data->add_percentage("护盾强效", 0.3, (name + "_extra_special"));
        }
    }
    else if (if_4_piece && name == "炽烈的炎之魔女")
    {
        if (data->config->condition->ele_type == "火")
        {
            if (data->c_point->args->E_release_times >= 3 || data->c_point->args->E_release_interval < 5)
            {
                if (data->config->condition->attack_way != "E") data->add_percentage("伤害加成", 0.225, (name + "_extra_special"));
                else data->add_percentage("伤害加成", 0.075, (name + "_extra_special"));
            }
            else if (data->c_point->args->E_release_times == 2 || data->c_point->args->E_release_interval < 10)
            {
                if (data->config->condition->attack_way != "E") data->add_percentage("伤害加成", 0.15, (name + "_extra_special"));
                else data->add_percentage("伤害加成", 0.075 / 2, (name + "_extra_special"));
            }
            else if (data->c_point->args->E_release_times == 1)
            {
                if (data->config->condition->attack_way != "E") data->add_percentage("伤害加成", 0.075, (name + "_extra_special"));
            }
        }
    }
    else if (if_4_piece && name == "渡过烈火的贤人")
    {
        if (data->config->ele_attach_type.find("火") != string::npos)//火附着
            data->add_percentage("伤害加成", 0.35, (name + "_extra_special"));
    }
    else if (if_4_piece && name == "平息鸣雷的尊者")
    {
        if (data->config->ele_attach_type.find("雷") != string::npos)//雷附着
            data->add_percentage("伤害加成", 0.35, (name + "_extra_special"));
    }
    else if (if_4_piece && name == "华馆梦醒形骸记")
    {
        if (data->c_point->args->background || (!data->c_point->args->background && data->c_point->ele_type == "岩"))
        {
            data->add_percentage("防御力", 0.24, (name + "_extra_special"));
            if (data->config->condition->ele_type == "岩") data->add_percentage("伤害加成", 0.24, (name + "_extra_special"));
        }
    }
    else if (if_4_piece && name == "辰砂往生录")
    {
        if (data->c_point->args->Q_damage_self)
            data->add_percentage("攻击力", 0.48, (name + "_extra_special"));
    }
    else if (if_4_piece && name == "深林的记忆")
    {
        if (data->c_point->args->E_hit_interval > 0 || data->c_point->args->Q_hit_interval > 0)
            if (data->config->condition->ele_type == "草")
                data->add_percentage("抗性削弱", 0.3, (name + "_extra_special"));
    }
    else if (if_4_piece && name == "饰金之梦")
    {
        if (data->config->react_type != "NONE")
        {
            int same = 0;
            int diff = 0;
            if (data->config->teammate_1->ele_type == data->c_point->ele_type) same += 1;
            else diff += 1;
            if (data->config->teammate_2->ele_type == data->c_point->ele_type) same += 1;
            else diff += 1;
            if (data->config->teammate_3->ele_type == data->c_point->ele_type) same += 1;
            else diff += 1;
            data->add_percentage("攻击力", 0.14 * same, (name + "_extra_special"));
            data->add_percentage("元素精通", 50.0 * diff, (name + "_extra_special"));
        }
    }
    return true;
}

//build new artifact(all)
void Artifact::modify_useful_attribute(Deployment *data)
{
    if (data->suit1->name == "绝缘之旗印" && data->suit2->name == "绝缘之旗印" && data->config->condition->attack_way == "Q") data->data_list[5]->useful = true;
}//被转化的属性有效

//build new artifact(all)
void Deployment::check_artifact_special(bool &valid)
{
    //特殊判断圣遗物套装
    if (suit1 == suit2)
    {
        if (suit1->name == "悠古的磐岩")
        {
            if (c_point->ele_type == "岩") valid = true;
            else valid = false;
        }
        else if (suit1->name == "翠绿之影")
        {
            if (c_point->ele_type == "风") valid = true;
            else valid = false;
        }
        else if (suit1->name == "如雷的盛怒")
        {
            if (c_point->ele_type == "雷") valid = true;
            else valid = false;
        }
        else if (suit1->name == "绝缘之旗印")
        {
            if (config->condition->attack_way == "Q") valid = true;
            else valid = false;
        }
        else if (suit1->name == "海染砗磲" || suit1->name == "被怜爱的少女")
        {
            if (c_point->args->heal_sustain) valid = true;
            else valid = false;
        }
    }
    else
    {
        //2+2
        //只允许 角斗+其他/追忆 染血+其他/苍白 少女+其他/海染 乐团+其他/饰金
        if (suit1->name == "追忆之注连" || suit2->name == "追忆之注连") valid = false;
        if (suit1->name == "辰砂往生录" || suit2->name == "辰砂往生录") valid = false;
        if (suit1->name == "来歆余响" || suit2->name == "来歆余响") valid = false;
        if ((suit1->name == "追忆之注连" && suit2->name == "角斗士的终幕礼") || (suit2->name == "追忆之注连" && suit1->name == "角斗士的终幕礼")) valid = true;

        if (suit1->name == "苍白之火" || suit2->name == "苍白之火") valid = false;
        if ((suit1->name == "染血的骑士道" && suit2->name == "苍白之火") || (suit2->name == "染血的骑士道" && suit1->name == "苍白之火")) valid = true;

        if (suit1->name == "海染砗磲" || suit2->name == "海染砗磲") valid = false;
        if ((suit1->name == "被怜爱的少女" && suit2->name == "海染砗磲") || (suit2->name == "被怜爱的少女" && suit1->name == "海染砗磲")) valid = true;

        if (suit1->name == "饰金之梦" || suit2->name == "饰金之梦") valid = false;
        if ((suit1->name == "流浪大地的乐团" && suit2->name == "饰金之梦") || (suit2->name == "流浪大地的乐团" && suit1->name == "饰金之梦")) valid = true;
    }
}

//build new character(needed)||build new weapon(all)||build new artifact(all)
void Deployment::get_team_data()
{
    //默认队友5星0命，技能均为9级，4星6命，技能均为12级

    //weapon
    if (config->team_weapon_artifact.find("终末") != string::npos)
    {
        add_percentage("攻击力", 0.2, "team_终末");
        add_percentage("元素精通", 100.0, "team_终末");
    }
    else if (config->team_weapon_artifact.find("苍古") != string::npos)
    {
        add_percentage("攻击力", 0.2, "team_苍古");
        if (config->condition->attack_way == "平A" || config->condition->attack_way == "重A" || config->condition->attack_way == "下落A")
            add_percentage("伤害加成", 0.16, "team_苍古");
    }
    else if (config->team_weapon_artifact.find("松籁") != string::npos)
    {
        add_percentage("攻击力", 0.2, "team_松籁");
    }
    if (config->team_weapon_artifact.find("白辰之环") != string::npos)
    {
        add_percentage("伤害加成", 0.2, "team_白辰之环");
    }
    if (config->team_weapon_artifact.find("讨龙") != string::npos)
    {
        add_percentage("攻击力", 0.48, "team_讨龙");
    }
    if (config->team_weapon_artifact.find("原木刀") != string::npos)
    {
        add_percentage("元素精通", 120, "team_原木刀");
    }
    else if (config->team_weapon_artifact.find("森林王器") != string::npos)
    {
        add_percentage("元素精通", 120, "team_森林王器");
    }
    if (config->team_weapon_artifact.find("贯月矢") != string::npos)
    {
        add_percentage("攻击力", 0.32, "team_贯月矢");
    }
    //TODO:NEW
//    if (config->team_weapon_artifact.find("圣显之钥") != string::npos)
//    {
//        add_percentage("元素精通", 100, "team_圣显之钥");
//    }
//    if (config->team_weapon_artifact.find("西福斯的月光") != string::npos)
//    {
//        add_percentage("元素充能效率", 0.12, "team_西福斯的月光");
//    }
//    if (config->team_weapon_artifact.find("流浪的晚星") != string::npos)
//    {
//        add_percentage("攻击力", 40 / base_atk, "team_流浪的晚星");
//    }
//    if (config->team_weapon_artifact.find("玛海菈的水色") != string::npos)
//    {
//        add_percentage("攻击力", 40 / base_atk, "team_玛海菈的水色");
//    }

    //artifact
    if (config->team_weapon_artifact.find("岩套") != string::npos)
    {
        add_percentage("伤害加成", 0.35, "team_岩套");
    }
    if (config->team_weapon_artifact.find("宗室") != string::npos)
    {
        add_percentage("攻击力", 0.2, "team_宗室");
    }
    if (config->team_weapon_artifact.find("风套") != string::npos)
    {
        if (config->condition->ele_type == "水" || config->condition->ele_type == "火" || config->condition->ele_type == "雷" || config->condition->ele_type == "冰")
            //if (config->ele_attach_type.find(config->condition->ele_type) != string::npos)//TODO:扩散与元素附着
            add_percentage("抗性削弱", 0.4, "team_风套");
    }
    if (config->team_weapon_artifact.find("千岩") != string::npos)
    {
        add_percentage("攻击力", 0.2, "team_千岩");
        add_percentage("护盾强效", 0.3, "team_千岩");
    }
    if (config->team_weapon_artifact.find("草套") != string::npos)
    {
        if (config->condition->ele_type == "草")
            add_percentage("抗性削弱", 0.3, "team_草套");
    }

    int Cryo_num = 0;//15%CR
    int Pyro_num = 0;//25%ATK
    int Hydro_num = 0;//25%LIFE
    int Anemo_num = 0;//5%CD
    int Electro_num = 0;//partical
    int Geo_num = 0;//15% shield 15%DAM 20%Geo RIS
    int Dendro_num = 0;//50EM 绽放激化燃烧30EM 二层绽放激化20EM

    if (config->teammate_all.find("胡桃") != string::npos)
    {
        //talent E后
//        add_percentage("暴击率", 0.12, "team_胡桃");//时间太短
        //constellation>=4 击败
//        add_percentage("暴击率", 0.12, "team_胡桃");
        Pyro_num++;
    }
    if (config->teammate_all.find("神里绫华") != string::npos)
    {
        //constellation>=4 Q
//        add_percentage("防御削弱", 0.3, "team_神里绫华");
        Cryo_num++;
    }
    if (config->teammate_all.find("雷电将军") != string::npos)
    {
        //E
        if (config->condition->attack_way == "Q")
            add_percentage("伤害加成", 0.003 * c_point->Q_energy, "team_雷电将军");
        //constellation>=4 Q后
//        add_percentage("攻击力", 0.3, "team_雷电将军");//时间太短
        Electro_num++;
    }
    if (config->teammate_all.find("甘雨") != string::npos)
    {
        //talent Q内
        if (config->condition->ele_type == "冰" && (!(c_point->args->background && !c_point->args->lockface)))
            add_percentage("伤害加成", 0.2, "team_甘雨");
        //constellation>=1 重A后
//        if (config->condition->ele_type == "冰")
//            add_percentage("抗性削弱", 0.15, "team_甘雨");
        //constellation>=4 Q内
//        add_percentage("伤害加成", 0.15, "team_甘雨");
        Cryo_num++;
    }
    if (config->teammate_all.find("夜兰") != string::npos)
    {
        //talent Q后
        if (!(c_point->args->background && c_point->args->lockface))
            add_percentage("伤害加成", 0.3, "team_夜兰");
        //constellation>=4 E后
//        add_percentage("生命值", 0.4, "team_夜兰");
        Hydro_num++;
    }
    if (config->teammate_all.find("行秋") != string::npos)
    {
        //constellation>=2
        if (config->condition->ele_type == "水")
            add_percentage("抗性削弱", 0.15, "team_行秋");
        Hydro_num++;
    }
    if (config->teammate_all.find("香菱") != string::npos)
    {
        //constellation>=1 E命中
        if (config->condition->ele_type == "火")
            add_percentage("抗性削弱", 0.15, "team_香菱");
        //constellation>=6 Q内
        if (config->condition->ele_type == "火")
            add_percentage("伤害加成", 0.15, "team_香菱");
        Pyro_num++;
    }
    if (config->teammate_all.find("八重神子") != string::npos)
    {
        //constellation>=4 E命中
//        if (config->condition->ele_type == "雷")
//            add_percentage("伤害加成", 0.2, "team_八重神子");
        Electro_num++;
    }
    if (config->teammate_all.find("枫原万叶") != string::npos)
    {
        //talent 扩散
        //扩散增伤由于是转化的，放在get_convert里避免二阶转化
        //constellation>=2 Q后
//        if (back_character.find(c_point->name) == string::npos)
//            add_percentage("元素精通", 200.0, "team_枫原万叶");
        Anemo_num++;
    }
    if (config->teammate_all.find("班尼特") != string::npos)
    {
        //Q
        if (!(c_point->args->background && !c_point->args->lockface))
            add_percentage("攻击力", (191 + 608) * (1.12 + 0.2) / base_atk, "team_班尼特");
        //constellation>=6 Q内
//        if (config->condition->ele_type == "火" && if (!(c_point->args->background && !c_point->args->lockface)))
//            add_percentage("伤害加成", 0.15, "team_班尼特");
        Pyro_num++;
    }
    if (config->teammate_all.find("温迪") != string::npos)
    {
        //constellation>=2 E命中
//        if (config->condition->ele_type == "风" || config->condition->ele_type == "物理")
//            add_percentage("抗性削弱", 0.24, "team_温迪");
        //constellation>=6 Q
//        if (config->condition->ele_type == "风") add_percentage("抗性削弱", 0.2, "team_温迪");
//        if (config->condition->ele_type == "水" || config->condition->ele_type == "火" || config->condition->ele_type == "雷" || config->condition->ele_type == "冰")
//          //if (config->ele_attach_type.find(config->condition->ele_type) != string::npos)//TODO:扩散与元素附着
//            add_percentage("抗性削弱", 0.2, "team_温迪");
        Anemo_num++;
    }
    if (config->teammate_all.find("莫娜") != string::npos)
    {
        //Q
        add_percentage("伤害加成", 0.58, "team_莫娜");
        //constellation>=4 Q
//        add_percentage("暴击率", 0.15, "team_莫娜");
        Hydro_num++;
    }
    if (config->teammate_all.find("钟离") != string::npos)
    {
        //E
        add_percentage("抗性削弱", 0.2, "team_钟离");
        //talent E
        add_percentage("护盾强效", 0.25, "team_钟离");
        Geo_num++;
    }

    if (this->c_point->ele_type == "冰") Cryo_num++;
    else if (this->c_point->ele_type == "火") Pyro_num++;
    else if (this->c_point->ele_type == "水") Hydro_num++;
    else if (this->c_point->ele_type == "风") Anemo_num++;
    else if (this->c_point->ele_type == "雷") Electro_num++;
    else if (this->c_point->ele_type == "岩") Geo_num++;
    else if (this->c_point->ele_type == "草") Dendro_num++;
    if (Cryo_num >= 2)
    {
        if (config->ele_attach_type.find("冰") != string::npos)//冰附着
            add_percentage("暴击率", 0.15, "team_Cryo");
    }
    if (Pyro_num >= 2) add_percentage("攻击力", 0.25, "team_Pyro");
    if (Hydro_num >= 2) add_percentage("生命值", 0.25, "team_Hydro");
    if (Geo_num >= 2)
    {
        add_percentage("护盾强效", 0.15, "team_Geo");
        if (c_point->args->shield_sustain || config->teammate_1->args->shield_sustain || config->teammate_2->args->shield_sustain || config->teammate_3->args->shield_sustain)
        {
            add_percentage("伤害加成", 0.15, "team_Geo");
            if (this->c_point->ele_type == "岩") add_percentage("抗性削弱", 0.2, "team_Geo");
        }
    }
    if (Dendro_num >= 2)
    {
        add_percentage("元素精通", 50.0, "team_Dendro");
        if (config->react_type.find("燃烧") != string::npos || config->react_type.find("绽放") != string::npos || config->react_type.find("激化") != string::npos)
            add_percentage("元素精通", 30.0, "team_Dendro");
        if (config->react_type.find("超绽放") != string::npos || config->react_type.find("烈绽放") != string::npos || config->react_type.find("蔓激化") != string::npos ||
            config->react_type.find("超激化") != string::npos)
            add_percentage("元素精通", 20.0, "team_Dendro");
    }
}

//build new character(needed)||build new weapon(all)||build new artifact(all)
bool Deployment::check_recharge_requirement()
{
    int max_recharge_num = 12;
    //调整充能数值
    if (config->condition->attack_way == "Q")
    {
        //能量回复值 = 微粒数 * 系数 * 实时的元素充能效率
        double front = 1;
        double back = 0.6;
        double same = 3;
        double white = 2;
        double diff = 1;
        double monster_drop = 10;

        double Q_energy_modify = c_point->Q_energy;
        if (config->teammate_all.find("雷电将军") != string::npos) Q_energy_modify -= 24;
        //TODO:没扩散的不回能
        if (config->teammate_all.find("温迪") != string::npos) Q_energy_modify -= 15;

        double energy = monster_drop;
        energy += config->teammate_1->E_energy_num * back * ((config->teammate_1->ele_type == c_point->ele_type) ? same : diff);
        energy += config->teammate_2->E_energy_num * back * ((config->teammate_2->ele_type == c_point->ele_type) ? same : diff);
        energy += config->teammate_3->E_energy_num * back * ((config->teammate_3->ele_type == c_point->ele_type) ? same : diff);

        //"天目影打刀" 12/E 不吃充能
        //"西风剑" 3*2/6s
        //"祭礼剑" 1E/16s

        //"不灭月华" 0.6/A Q后12s内 不吃充能
        //"试作金珀" 3*6/Q 不吃充能
        //"西风秘典" 3*2/6s
        //"祭礼残章" 1E/16s

        //"祭礼弓" 1E/16s
        //"西风猎弓" 3*2/6s

        //"祭礼大剑" 1E/16s
        //"桂木斩长正" 3*5-3/E 不吃充能
        //"西风大剑" 3*2/6s

        //"西风长枪" 3*2/6s
        //"喜多院十文字" 3*5-3/E 不吃充能

        if (c_point->name == "胡桃")
        {
            //Q 60 E 5f 2E/Q
            //通过歪充能词条和队友保证循环
            energy = Q_energy_modify;
        }
        else if (c_point->name == "神里绫华")
        {
            //Q 80 E 4.5f 2E/Q
            energy += 2 * c_point->E_energy_num * front * same;

            if (w_point->name == "西风剑") energy += 3 * front * white;
            else if (w_point->name == "祭礼剑") energy += c_point->E_energy_num * front * same;
            else if (w_point->name == "天目影打刀") Q_energy_modify -= 12;
        }
        else if (c_point->name == "雷电将军")
        {
            //Q 90-24=66 E 10b 1E/Q
            Q_energy_modify -= 24;
            energy += c_point->E_energy_num * back * same;

            if (w_point->name == "西风长枪") energy += 3 * front * white;
            else if (w_point->name == "喜多院十文字") Q_energy_modify -= 12;

            energy = min(energy, Q_energy_modify / 2.4);
        }
        else if (c_point->name == "甘雨")
        {
            //Q 60 E 2f+2b 2E/Q
            //constellation>=1:每次重A命中恢复2点能量 constellation>=2:E变为2次
            energy += 2 * c_point->E_energy_num * (front / 2 + back / 2) * same;

            if (w_point->name == "西风猎弓") energy += 3 * front * white;
            else if (w_point->name == "祭礼弓") energy += c_point->E_energy_num * front * same;
        }
        else if (c_point->name == "夜兰")
        {
            //Q 70 E 4f 1E/Q
            energy += c_point->E_energy_num * front * same;
            //constellation>=1
//            energy += 2 * c_point->E_energy_num * front * same;

            if (w_point->name == "西风猎弓") energy += 3 * front * white;
            else if (w_point->name == "祭礼弓") energy += c_point->E_energy_num * front * same;
        }
        else if (c_point->name == "行秋")
        {
            //Q 80-3*5=65 E 5f 1E/Q
            Q_energy_modify -= 12;
            energy += c_point->E_energy_num * front * same;

            if (w_point->name == "西风剑") energy += 3 * front * white;
            else if (w_point->name == "祭礼剑") energy += c_point->E_energy_num * front * same;
            else if (w_point->name == "天目影打刀") Q_energy_modify -= 12;
        }
        else if (c_point->name == "香菱")
        {
            //Q 80 E 4b 1E/Q
            energy += c_point->E_energy_num * back * same;

            if (w_point->name == "西风长枪") energy += 3 * front * white;
            else if (w_point->name == "喜多院十文字") Q_energy_modify -= 12;
        }
        else if (c_point->name == "八重神子")
        {
            //Q 90-24=66 E 5b 1E/Q
            Q_energy_modify -= 24;
            energy += c_point->E_energy_num * back * same;

            if (w_point->name == "西风秘典") energy += 3 * front * white;
            else if (w_point->name == "祭礼残章") energy += 0;
            else if (w_point->name == "试作金珀") Q_energy_modify -= 18;
            //"不灭月华" 0.6/A Q后12s内 不吃充能
        }
        else if (c_point->name == "温迪")
        {
            //Q 60-15=45 E 3f 2E/Q
            Q_energy_modify -= 15;
            energy += 2 * c_point->E_energy_num * front * same;

            if (w_point->name == "西风猎弓") energy += 3 * front * white;
            else if (w_point->name == "祭礼弓") energy += c_point->E_energy_num * front * same;
        }
        else if (c_point->name == "莫娜")
        {
            //Q 60 E 3b 1E/Q
            energy += c_point->E_energy_num * back * same;

            if (w_point->name == "西风秘典") energy += 3 * front * white;
            else if (w_point->name == "祭礼残章") energy += c_point->E_energy_num * front * same;
            else if (w_point->name == "试作金珀") Q_energy_modify -= 18;
            //"不灭月华" 0.6/A Q后12s内 不吃充能
        }
        else if (c_point->name == "钟离")
        {
            //Q 40 E 0.5f 1E/Q
//            energy = c_point->E_energy_num * front * same + back_back_total;
//
//            if (w_point->name == "西风长枪") energy += 3 * front * white;
//            if (w_point->name == "喜多院十文字") Q_energy_modify -= 12;
            //通过歪充能词条和队友保证循环
            energy = Q_energy_modify;
        }
        else energy = Q_energy_modify;

        int recharge_num = round((Q_energy_modify / energy - data_list[5]->percentage) / data_list[5]->value_per_entry);
        if (recharge_num > max_recharge_num) return false;
        else
        {
            data_list[5]->entry_num = max(0, recharge_num);
            return true;
        }
    }
    return true;
}

//build new character(needed)||build new weapon(all)||build new artifact(all)
void Deployment::get_convert_value(double &life, double &atk, double &def, double &mastery, double &recharge, double &critrate, double &critdam, double &damplus)
{
    //按传入面板计算转化，最后再执行加成
    double life_add = 0, atk_add = 0, def_add = 0, mastery_add = 0, recharge_add = 0, critrate_add = 0, critdam_add = 0, damplus_add = 0;
    //character
    if (c_point->name == "胡桃")//生命->攻击
        atk_add += min(life * 0.0626 * base_life / base_atk, 4.0);
    else if (c_point->name == "雷电将军" && config->condition->ele_type == "雷")//充能->增伤
        damplus_add += (recharge - 1) * 0.4;
    else if (c_point->name == "八重神子" && config->condition->attack_way == "E")//精通->E增伤
        damplus_add += mastery * 0.0015;
    else if (c_point->name == "莫娜" && config->condition->ele_type == "水")//充能->增伤
        damplus_add += recharge * 0.2;

    //weapon
    if (w_point->name == "磐岩结绿")//生命->攻击
        atk_add += life * 0.012 * (0.75 + w_point->level * 0.25) * base_life / base_atk;//生命->攻击
    else if (w_point->name == "圣显之钥")//生命->精通
    {
        if (c_point->args->E_hit_interval < 10 && c_point->args->E_hit_interval > 0)
        {
            mastery_add += life * 0.0012 * 3 * (0.75 + w_point->level * 0.25) * base_life;//生命->精通
            mastery_add += life * 0.002 * (0.75 + w_point->level * 0.25) * base_life;//生命->精通
        }
        else if (c_point->args->E_hit_interval < 20 && c_point->args->E_hit_interval > 0)
        {
            mastery_add += life * 0.0012 * 2 * (0.75 + w_point->level * 0.25) * base_life;//生命->精通
        }
        else if (c_point->args->E_hit_interval > 0)
        {
            mastery_add += life * 0.0012 * (0.75 + w_point->level * 0.25) * base_life;//生命->精通
        }
    }
        // TODO:NEW
    else if (w_point->name == "西福斯的月光")//精通->充能
        recharge_add += mastery * 0.00036 * (0.75 + w_point->level * 0.25);//精通->充能
        // TODO:NEW
    else if (w_point->name == "流浪的晚星")//精通->攻击
        atk_add += mastery * 0.24 * (0.75 + w_point->level * 0.25) / base_atk;//精通->攻击
        // TODO:NEW
    else if (w_point->name == "玛海菈的水色")//精通->攻击
        atk_add += mastery * 0.24 * (0.75 + w_point->level * 0.25) / base_atk;//精通->攻击
    else if (w_point->name == "护摩之杖")//生命->攻击
    {
        atk_add += life * 0.008 * (0.75 + w_point->level * 0.25) * base_life / base_atk;//生命->攻击
        if (c_point->args->polearm_humo_halflife)
            atk_add += life * (0.008 + w_point->level * 0.002) * base_life / base_atk;//生命->攻击
    }
    else if (w_point->name == "薙草之稻光")//充能->攻击
        atk_add += min((recharge - 1) * (0.75 + w_point->level * 0.25) * 0.28, (0.7 + w_point->level * 0.1));
        //TODO:NEW
    else if (w_point->name == "赤沙之杖")//精通->攻击
    {
        atk_add += mastery * 0.52 * (0.75 + w_point->level * 0.25) / base_atk;//精通->攻击
        if (c_point->args->E_hit_interval < 5 && c_point->args->E_hit_interval > 0)
        {
            atk_add += mastery * 0.28 * 3 * (0.75 + w_point->level * 0.25) / base_atk;//精通->攻击
        }
        else if (c_point->args->E_hit_interval < 10 && c_point->args->E_hit_interval > 0)
        {
            atk_add += mastery * 0.28 * 2 * (0.75 + w_point->level * 0.25) / base_atk;//精通->攻击
        }
        else if (c_point->args->E_hit_interval > 0)
        {
            atk_add += mastery * 0.28 * (0.75 + w_point->level * 0.25) / base_atk;//精通->攻击
        }
    }

    //artifact
    if (suit1->name == "绝缘之旗印" && suit2->name == "绝缘之旗印" && config->condition->attack_way == "Q")//充能->增伤
        damplus_add += min(recharge * 0.25, 0.75);

    //team
    if (config->condition->ele_type == "水" || config->condition->ele_type == "火" || config->condition->ele_type == "雷" || config->condition->ele_type == "冰")
        //if (config->ele_attach_type.find(config->condition->ele_type) != string::npos)//TODO:扩散与元素附着
        add_percentage("伤害加成", 0.36, "team_枫原万叶");

    life += life_add;
    atk += atk_add;
    def += def_add;
    mastery += mastery_add;
    recharge += recharge_add;
    critrate += critrate_add;
    critdam += critdam_add;
    damplus += damplus_add;
}

//build new character(needed)||build new weapon(all)||build new artifact(all)
void Deployment::get_extra_rate_value(double life, double atk, double def, double mastery, double recharge, double critrate, double critdam, double damplus, double &extrarate)
{
    //artifact
    //weapon
    if (w_point->name == "辰砂之纺锤" && config->condition->attack_way == "E")
        extrarate += 0.4 * (0.75 + w_point->level * 0.25) * def * base_def;
    else if (w_point->name == "不灭月华" && config->condition->attack_way == "平A")
        extrarate += (0.005 + w_point->level * 0.005) * life * base_life;
    else if (w_point->name == "赤角石溃杵" && (config->condition->attack_way == "平A" || config->condition->attack_way == "重A"))
        extrarate += 0.4 * (0.75 + w_point->level * 0.25) * def * base_def;
    else if (w_point->name == "猎人之径" && config->condition->attack_way == "重A")
        extrarate += 1.6 * (0.75 + w_point->level * 0.25) * mastery;
    //character
    if (c_point->name == "胡桃")
    {
        if (c_point->constellation >= 2 && config->condition->attack_way == "E")
            extrarate += 0.1 * life * base_life;
    }
    else if (c_point->name == "夜兰")
    {
        if (config->condition->attack_way == "重A")//LV9
            extrarate += 0.1968 * life * base_life;
        else if (config->condition->attack_way == "E")//LV9
            extrarate += 0.384 * life * base_life;
        else if (config->condition->attack_way == "Q")//LV9
        {
            if (c_point->constellation >= 2)
                extrarate += ((0.1242 + 0.0828 * 3 * 15 + 0.14 * 7) / 53) * life * base_life;
            else
                extrarate += ((0.1242 + 0.0828 * 3 * 15) / 46) * life * base_life;
        }
    }
    else if (c_point->name == "钟离")
    {
        if (config->condition->attack_way == "平A" || config->condition->attack_way == "重A" || config->condition->attack_way == "下落A")
            extrarate += 0.0139 * life * base_life;
        else if (config->condition->attack_way == "E")
            extrarate += 0.019 * life * base_life;
        else if (config->condition->attack_way == "Q")
            extrarate += 0.33 * life * base_life;
    }
}

//build new character(needed)||build new weapon(all)||build new artifact(all)
void Deployment::get_react_value(double mastery, double &extrarate, double &growrate)
{
    //扩散（风+水火雷冰），结晶（岩+水火雷冰），绽放（草水+火雷），激化（草雷），燃烧（草火），蒸发（水火），融化（火冰），冻结（水冰），感电（雷水），超载（雷火），超导（雷冰）
    if (config->react_type.find("扩散") != string::npos)
    {
        //1.2 * 723.0 * (1 + (16 * mastery) / (mastery + 2000) + 如雷/魔女等) * defence * resistance;
        //元素伤，吃各元素抗
    }
    if (config->react_type.find("结晶") != string::npos)
    {}
    if (config->react_type.find("绽放") != string::npos)
    {
        //4.0 * 723.0 * (1 + (16 * mastery) / (mastery + 2000) + 如雷/魔女等) * defence * resistance;
        //草伤，吃草抗
        //6.0 * 723.0 * (1 + (16 * mastery) / (mastery + 2000) + 如雷/魔女等) * defence * resistance;
        //烈绽放、超绽放
    }
    if (config->react_type.find("激化") != string::npos)
    {
        if (this->config->condition->ele_type == "草") extrarate += 1447.0 * 1.25 * (1.0 + (5.0 * mastery) / (mastery + 1200.0));
        else if (this->config->condition->ele_type == "雷") extrarate += 1447.0 * 1.15 * (1.0 + (5.0 * mastery) / (mastery + 1200.0));
    }
    if (config->react_type.find("燃烧") != string::npos)
    {
        //0.5 * 723.0 * (1 + (16 * mastery) / (mastery + 2000) + 如雷/魔女等) * defence * resistance;
        //火伤，吃火抗，8段伤害
    }
    if (config->react_type.find("蒸发") != string::npos)
    {
        growrate = 1.0 + (25.0 * mastery) / (9.0 * (mastery + 1401.0));
        if ((suit1->name == suit2->name) && (suit1->name == "炽烈的炎之魔女")) growrate += 0.15;
        if (config->teammate_all.find("莫娜") != string::npos) growrate += 0.15;
        if (config->condition->ele_type == "火") growrate *= 1.5;
        else if (config->condition->ele_type == "水") growrate *= 2.0;
    }
    if (config->react_type.find("融化") != string::npos)
    {
        growrate = 1.0 + (25.0 * mastery) / (9.0 * (mastery + 1401.0));
        if ((suit1->name == suit2->name) && (suit1->name == "炽烈的炎之魔女")) growrate += 0.15;
        if (config->condition->ele_type == "火") growrate *= 2.0;
        else if (config->condition->ele_type == "冰") growrate *= 1.5;
    }
    if (config->react_type.find("冻结") != string::npos)
    {}
    //碎冰 //3.0 * 723.0 * (1 + (16 * mastery) / (mastery + 2000) + 如雷/魔女等) * defence * resistance;
    //物理伤，吃物理抗
    if (config->react_type.find("感电") != string::npos)
    {
        //2.4 * 723.0 * (1 + (16 * mastery) / (mastery + 2000) + 如雷/魔女等) * defence * resistance;
        //雷伤，吃雷抗
    }
    if (config->react_type.find("超载") != string::npos)
    {
        //4.0 * 723.0 * (1 + (16 * mastery) / (mastery + 2000) + 如雷/魔女等) * defence * resistance;
        //火伤，吃火抗
    }
    if (config->react_type.find("超导") != string::npos)
    {
        //1.0 * 723.0 * (1 + (16 * mastery) / (mastery + 2000) + 如雷/魔女等) * defence * resistance;
        //冰伤，吃冰抗
    }
}

//parameters
bool out_header;
string filter_type;
ofstream outfile_result;
ofstream outfile_debug;

//in and out
vector<vector<string>> pre_cal_data;//c_name,config,w_name,suit1_name,suit2_name,a_main3,a_main4,a_main5,damage

void Deployment::out()
{
    if (out_header)
    {
        outfile_result << "人物名称" << "," << "配置信息" << "," << "武器名称" << "," << "圣遗物1" << "," << "圣遗物2" << "," << "3号位" << "," << "4号位" << "," << "5号位" << "," << "期望伤害" << ","
                       << "LIFE" << "," << "life" << "," << "ATK" << "," << "atk" << "," << "DEF" << "," << "def" << "," << "mastery" << "," << "recharge" << "," << "critrate" << "," << "critdam" << "," << "damplus" << "," << "resistdec" << ","
                       << "defdec" << "," << "defign" << "," << "heal" << "," << "shield" << ","
                       << "lifenum" << "," << "atknum" << "," << "defnum" << "," << "masterynum" << "," << "rechargenum" << "," << "critratenum" << "," << "critdamnum" << "\n";
        out_header = false;
    }
    outfile_result << c_point->name << "," << (config->condition->attack_way + "_" + config->react_type + "_" + config->ele_attach_type + "_" + to_string(config->max_entry_all) + "_" + config->teammate_all + "_" + config->team_weapon_artifact) << ","
                   << w_point->name << "," << suit1->name << "," << suit2->name << "," << a_main3 << "," << a_main4 << "," << a_main5 << "," << damage << ",";
    outfile_result << base_life << "," << base_life * (data_list[0]->entry_num * data_list[0]->value_per_entry + data_list[0]->percentage - 1) << ",";
    outfile_result << base_atk << "," << base_atk * (data_list[1]->entry_num * data_list[1]->value_per_entry + data_list[1]->percentage - 1) << ",";
    outfile_result << base_def << "," << base_def * (data_list[2]->entry_num * data_list[2]->value_per_entry + data_list[2]->percentage - 1) << ",";
    outfile_result << data_list[4]->entry_num * data_list[4]->value_per_entry + data_list[4]->percentage << ",";
    outfile_result << data_list[5]->entry_num * data_list[5]->value_per_entry + data_list[5]->percentage << ",";
    outfile_result << data_list[6]->entry_num * data_list[6]->value_per_entry + data_list[6]->percentage << ",";
    outfile_result << data_list[7]->entry_num * data_list[7]->value_per_entry + data_list[7]->percentage << ",";
    outfile_result << data_list[8]->percentage << ",";
    outfile_result << data_list[9]->percentage << ",";
    outfile_result << data_list[10]->percentage << ",";
    outfile_result << data_list[11]->percentage << ",";
    outfile_result << data_list[12]->percentage << ",";
    outfile_result << data_list[13]->percentage << ",";
    outfile_result << data_list[0]->entry_num << ",";
    outfile_result << data_list[1]->entry_num << ",";
    outfile_result << data_list[2]->entry_num << ",";
    outfile_result << data_list[4]->entry_num << ",";
    outfile_result << data_list[5]->entry_num << ",";
    outfile_result << data_list[6]->entry_num << ",";
    outfile_result << data_list[7]->entry_num << "\n";
}

void read_pre_cal_data(string filename)
{
    ifstream infile;
    infile.open(filename);
    string line;
    getline(infile, line);
    while (!infile.eof())
    {
        getline(infile, line);
        vector<string> temp;
        for (int i = 0; i < 9; ++i)
        {
            int pos = (int) line.find(',');
            if (pos != string::npos) temp.push_back(line.substr(0, pos));
            line = line.substr(pos + 1);
        }
        pre_cal_data.push_back(temp);
    }
//    for (auto &i: pre_cal_data)
//    {
//        for (auto &j: i) cout << j << " ";
//        cout << endl;
//    }
}

//TODO:圣遗物增量更新（每次总是更新最后两个）
bool if_pre_cal_data_exists(string c_name, Config *config, string w_name)
{
    for (auto &i: pre_cal_data)
    {
        if (c_name == i[0] && (config->condition->attack_way + "_" + config->react_type + "_" + config->ele_attach_type + "_" + to_string(config->max_entry_all) + "_" + config->teammate_all + "_" + config->team_weapon_artifact) == i[1] && w_name == i[2])
            return true;
    }
    return false;
}

#define topk 3

//加快计算速度
void Deployment::modify_useful_attribute()
{
    if (c_point->name == "胡桃") data_list[1]->useful = false;
    if (c_point->name == "钟离") data_list[1]->useful = false;
    if (c_point->name != "雷电将军" && c_point->name != "莫娜") data_list[5]->useful = false;
    Weapon::modify_useful_attribute(this);
    Artifact::modify_useful_attribute(this);
}

//限定特殊武器/圣遗物
string Deployment::check_disabled_combinations()
{
    if (c_point->name == "温迪")
    {
        if (suit1->name == "翠绿之影" && suit2->name == "翠绿之影") return "artifact_skip";
        else return "false";
    }
    else if (c_point->name == "莫娜")
    {
        if (suit1->name == "昔日宗室之仪" && suit2->name == "昔日宗室之仪") return "artifact_skip";
//        if (suit1->name == "千岩牢固" && suit2->name == "千岩牢固") return "artifact_skip";
//        if (suit1->name == "深林的记忆" && suit2->name == "深林的记忆") return "artifact_skip";
        else return "false";
    }
    else if (c_point->name == "钟离")
    {
        if (suit1->name == "昔日宗室之仪" && suit2->name == "昔日宗室之仪") return "artifact_skip";
//        if (suit1->name == "深林的记忆" && suit2->name == "深林的记忆") return "artifact_skip";
//        if (suit1->name == "悠古的磐岩" && suit2->name == "悠古的磐岩") return "artifact_skip";
//        if (suit1->name == "千岩牢固" && suit2->name == "千岩牢固") return "artifact_skip";
        else return "false";
    }
    else return "none";
}

//配置编写
void get_all_config(string c_name, vector<Config *> &config_list)
{
    //"终末_讨龙"
    //"岩套_宗室_风套_千岩_草套"
    //有岩套、风套的地方注意一下元素附着的问题
    //胡桃 钟离_岩套 (行秋 莫娜_讨龙宗室 夜兰_终末)
    //神里绫华 甘雨 温迪_风套/枫原万叶_风套 莫娜_宗室
    //雷电将军 香菱 班尼特_宗室 行秋/夜兰_终末/八重神子/枫原万叶_风套
    if (c_name == "胡桃")
    {
        config_list.push_back(new Config(new Condition("火", "长柄武器", "重A"), "蒸发", "水", 30, true,
                                         true, false, true, false, true, true, true,
                                         false, false, find_character_by_name("行秋"), find_character_by_name("钟离"), find_character_by_name("莫娜"), "讨龙_宗室"));
        config_list.push_back(new Config(new Condition("火", "长柄武器", "重A"), "蒸发", "水", 30, true,
                                         true, false, true, false, true, true, true,
                                         false, false, find_character_by_name("行秋"), find_character_by_name("钟离"), find_character_by_name("夜兰"), "终末"));
        config_list.push_back(new Config(new Condition("火", "长柄武器", "重A"), "蒸发", "水", 30, true,
                                         true, false, true, false, true, true, true,
                                         false, false, find_character_by_name("莫娜"), find_character_by_name("钟离"), find_character_by_name("夜兰"), "终末_讨龙_宗室"));
        config_list.push_back(new Config(new Condition("火", "长柄武器", "重A"), "蒸发", "水", 30, true,
                                         true, false, true, false, true, true, true,
                                         false, false, find_character_by_name("行秋"), find_character_by_name("钟离"), find_character_by_name("夜兰"), ""));
        config_list.push_back(new Config(new Condition("火", "长柄武器", "重A"), "蒸发", "水", 30, true,
                                         true, false, true, false, true, true, true,
                                         false, false, find_character_by_name("莫娜"), find_character_by_name("钟离"), find_character_by_name("夜兰"), "讨龙_宗室"));
    }
    if (c_name == "神里绫华")
    {
        config_list.push_back(new Config(new Condition("冰", "单手剑", "Q"), "冻结", "冰水", 30, false,
                                         true, false, false, true, true, true, true,
                                         false, false, find_character_by_name("甘雨"), find_character_by_name("温迪"), find_character_by_name("莫娜"), "宗室_风套"));
        config_list.push_back(new Config(new Condition("冰", "单手剑", "Q"), "冻结", "冰水", 30, false,
                                         true, false, false, true, true, true, true,
                                         false, false, find_character_by_name("甘雨"), find_character_by_name("温迪"), find_character_by_name("莫娜"), "终末_宗室_风套"));
        config_list.push_back(new Config(new Condition("冰", "单手剑", "Q"), "冻结", "冰水", 30, false,
                                         true, false, false, true, true, true, true,
                                         false, false, find_character_by_name("甘雨"), find_character_by_name("枫原万叶"), find_character_by_name("莫娜"), "宗室_风套"));
    }
    if (c_name == "雷电将军")
    {
        config_list.push_back(new Config(new Condition("雷", "长柄武器", "Q"), "感电", "水雷", 30, false,
                                         true, false, false, true, true, true, true,
                                         false, false, find_character_by_name("行秋"), find_character_by_name("香菱"), find_character_by_name("班尼特"), "宗室"));
        config_list.push_back(new Config(new Condition("雷", "长柄武器", "Q"), "感电", "水雷", 30, false,
                                         true, false, false, true, true, true, true,
                                         false, false, find_character_by_name("夜兰"), find_character_by_name("香菱"), find_character_by_name("班尼特"), "终末_宗室"));
        config_list.push_back(new Config(new Condition("雷", "长柄武器", "Q"), "感电", "水雷", 30, false,
                                         true, false, false, true, true, true, true,
                                         false, false, find_character_by_name("夜兰"), find_character_by_name("香菱"), find_character_by_name("班尼特"), "宗室"));
        config_list.push_back(new Config(new Condition("雷", "长柄武器", "Q"), "超载", "雷火", 30, false,
                                         true, false, false, true, true, true, true,
                                         false, false, find_character_by_name("八重神子"), find_character_by_name("香菱"), find_character_by_name("班尼特"), "宗室"));
        config_list.push_back(new Config(new Condition("雷", "长柄武器", "Q"), "超载", "雷火", 30, false,
                                         true, false, false, true, true, true, true,
                                         false, false, find_character_by_name("枫原万叶"), find_character_by_name("香菱"), find_character_by_name("班尼特"), "宗室_风套"));
    }
    if (c_name == "甘雨")
    {
        config_list.push_back(new Config(new Condition("冰", "弓", "重A"), "融化", "火", 30, false,
                                         true, false, true, false, true, true, true,
                                         false, false, find_character_by_name("钟离"), find_character_by_name("香菱"), find_character_by_name("班尼特"), "宗室"));

        config_list.push_back(new Config(new Condition("冰", "弓", "重A"), "冻结", "冰水", 30, false,
                                         true, false, false, false, true, true, true,
                                         false, false, find_character_by_name("神里绫华"), find_character_by_name("温迪"), find_character_by_name("莫娜"), "宗室_风套"));
        config_list.push_back(new Config(new Condition("冰", "弓", "重A"), "冻结", "冰水", 30, false,
                                         true, false, false, false, true, true, true,
                                         false, false, find_character_by_name("神里绫华"), find_character_by_name("温迪"), find_character_by_name("莫娜"), "终末_宗室_风套"));
        config_list.push_back(new Config(new Condition("冰", "弓", "重A"), "冻结", "冰水", 30, false,
                                         true, false, false, false, true, true, true,
                                         false, false, find_character_by_name("神里绫华"), find_character_by_name("枫原万叶"), find_character_by_name("莫娜"), "宗室_风套"));

        config_list.push_back(new Config(new Condition("冰", "弓", "Q"), "冻结", "冰水", 30, false,
                                         true, false, false, true, true, true, true,
                                         false, false, find_character_by_name("神里绫华"), find_character_by_name("温迪"), find_character_by_name("莫娜"), "宗室_风套"));
        config_list.push_back(new Config(new Condition("冰", "弓", "Q"), "冻结", "冰水", 30, false,
                                         true, false, false, true, true, true, true,
                                         false, false, find_character_by_name("神里绫华"), find_character_by_name("温迪"), find_character_by_name("莫娜"), "终末_宗室_风套"));
        config_list.push_back(new Config(new Condition("冰", "弓", "Q"), "冻结", "冰水", 30, false,
                                         true, false, false, true, true, true, true,
                                         false, false, find_character_by_name("神里绫华"), find_character_by_name("枫原万叶"), find_character_by_name("莫娜"), "宗室_风套"));
    }
    if (c_name == "夜兰")
    {
        config_list.push_back(new Config(new Condition("水", "弓", "Q"), "NONE", "水", 30, true,
                                         false, false, false, true, true, true, true,
                                         false, false, find_character_by_name("胡桃"), find_character_by_name("行秋"), find_character_by_name("钟离"), "宗室"));
        config_list.push_back(new Config(new Condition("水", "弓", "Q"), "NONE", "水", 30, true,
                                         false, false, false, true, true, true, true,
                                         false, false, find_character_by_name("胡桃"), find_character_by_name("莫娜"), find_character_by_name("钟离"), "宗室"));
        config_list.push_back(new Config(new Condition("水", "弓", "Q"), "感电", "水雷", 30, true,
                                         false, false, false, true, true, true, true,
                                         false, false, find_character_by_name("雷电将军"), find_character_by_name("香菱"), find_character_by_name("班尼特"), "宗室"));
    }
    if (c_name == "行秋")
    {
        config_list.push_back(new Config(new Condition("水", "单手剑", "Q"), "感电", "水雷", 30, false,
                                         true, false, false, true, true, true, true,
                                         false, false, find_character_by_name("雷电将军"), find_character_by_name("香菱"), find_character_by_name("班尼特"), "宗室"));
        config_list.push_back(new Config(new Condition("水", "单手剑", "Q"), "NONE", "水", 30, false,
                                         true, false, false, true, true, true, true,
                                         false, false, find_character_by_name("胡桃"), find_character_by_name("钟离"), find_character_by_name("莫娜"), "宗室"));
        config_list.push_back(new Config(new Condition("水", "单手剑", "Q"), "NONE", "水", 30, false,
                                         true, false, false, true, true, true, true,
                                         false, false, find_character_by_name("胡桃"), find_character_by_name("钟离"), find_character_by_name("夜兰"), "终末_宗室"));
        config_list.push_back(new Config(new Condition("水", "单手剑", "Q"), "NONE", "水", 30, false,
                                         true, false, false, true, true, true, true,
                                         false, false, find_character_by_name("胡桃"), find_character_by_name("钟离"), find_character_by_name("夜兰"), "宗室"));
    }
    if (c_name == "香菱")
    {
        config_list.push_back(new Config(new Condition("火", "长柄武器", "Q"), "蒸发_超载", "水雷", 30, false,
                                         true, false, true, true, true, true, true,
                                         false, false, find_character_by_name("雷电将军"), find_character_by_name("行秋"), find_character_by_name("班尼特"), "宗室"));
        config_list.push_back(new Config(new Condition("火", "长柄武器", "Q"), "蒸发_超载", "水雷", 30, false,
                                         true, false, true, true, true, true, true,
                                         false, false, find_character_by_name("雷电将军"), find_character_by_name("夜兰"), find_character_by_name("班尼特"), "终末_宗室"));
        config_list.push_back(new Config(new Condition("火", "长柄武器", "Q"), "蒸发_超载", "水雷", 30, false,
                                         true, false, true, true, true, true, true,
                                         false, false, find_character_by_name("雷电将军"), find_character_by_name("夜兰"), find_character_by_name("班尼特"), "宗室"));

        config_list.push_back(new Config(new Condition("火", "长柄武器", "Q"), "超载", "雷火", 30, false,
                                         true, false, false, true, true, true, true,
                                         false, false, find_character_by_name("雷电将军"), find_character_by_name("八重神子"), find_character_by_name("班尼特"), "宗室"));
        config_list.push_back(new Config(new Condition("火", "长柄武器", "Q"), "超载", "雷火", 30, false,
                                         true, false, false, true, true, true, true,
                                         false, false, find_character_by_name("雷电将军"), find_character_by_name("枫原万叶"), find_character_by_name("班尼特"), "宗室_风套"));
    }
    if (c_name == "八重神子")
    {
        config_list.push_back(new Config(new Condition("雷", "法器", "E"), "超载", "雷火", 30, false,
                                         true, false, true, false, true, true, true,
                                         false, false, find_character_by_name("雷电将军"), find_character_by_name("香菱"), find_character_by_name("班尼特"), "宗室"));
        config_list.push_back(new Config(new Condition("雷", "法器", "Q"), "超载", "雷火", 30, false,
                                         true, false, false, true, true, true, true,
                                         false, false, find_character_by_name("雷电将军"), find_character_by_name("香菱"), find_character_by_name("班尼特"), "宗室"));
    }
    if (c_name == "温迪")
    {
        config_list.push_back(new Config(new Condition("风", "弓", "Q"), "扩散", "冰水", 30, false,
                                         true, false, false, true, true, true, true,
                                         false, false, find_character_by_name("神里绫华"), find_character_by_name("甘雨"), find_character_by_name("莫娜"), "宗室"));
    }
    if (c_name == "莫娜")
    {
        config_list.push_back(new Config(new Condition("水", "法器", "Q"), "NONE", "水", 30, false,
                                         true, false, false, true, true, true, true,
                                         false, false, find_character_by_name("胡桃"), find_character_by_name("行秋"), find_character_by_name("钟离"), "宗室"));
        config_list.push_back(new Config(new Condition("水", "法器", "Q"), "NONE", "水", 30, false,
                                         true, false, false, true, true, true, true,
                                         false, false, find_character_by_name("胡桃"), find_character_by_name("夜兰"), find_character_by_name("钟离"), "终末_宗室"));
        config_list.push_back(new Config(new Condition("水", "法器", "Q"), "NONE", "水", 30, false,
                                         true, false, false, true, true, true, true,
                                         false, false, find_character_by_name("胡桃"), find_character_by_name("夜兰"), find_character_by_name("钟离"), "宗室"));
        config_list.push_back(new Config(new Condition("水", "法器", "Q"), "冻结", "冰水", 30, false,
                                         true, false, false, true, true, true, true,
                                         false, false, find_character_by_name("神里绫华"), find_character_by_name("甘雨"), find_character_by_name("温迪"), "风套"));
        config_list.push_back(new Config(new Condition("水", "法器", "Q"), "冻结", "冰水", 30, false,
                                         true, false, false, true, true, true, true,
                                         false, false, find_character_by_name("神里绫华"), find_character_by_name("甘雨"), find_character_by_name("温迪"), "终末_风套"));
        config_list.push_back(new Config(new Condition("水", "法器", "Q"), "冻结", "冰水", 30, false,
                                         true, false, false, true, true, true, true,
                                         false, false, find_character_by_name("神里绫华"), find_character_by_name("甘雨"), find_character_by_name("枫原万叶"), "风套"));
    }
    if (c_name == "钟离")
    {
        config_list.push_back(new Config(new Condition("岩", "长柄武器", "Q"), "结晶", "水", 30, true,
                                         true, false, false, true, true, true, true,
                                         false, false, find_character_by_name("胡桃"), find_character_by_name("行秋"), find_character_by_name("莫娜"), "宗室"));
        config_list.push_back(new Config(new Condition("岩", "长柄武器", "Q"), "结晶", "水", 30, true,
                                         true, false, false, true, true, true, true,
                                         false, false, find_character_by_name("胡桃"), find_character_by_name("行秋"), find_character_by_name("夜兰"), "终末"));
        config_list.push_back(new Config(new Condition("岩", "长柄武器", "Q"), "结晶", "水", 30, true,
                                         true, false, false, true, true, true, true,
                                         false, false, find_character_by_name("胡桃"), find_character_by_name("莫娜"), find_character_by_name("夜兰"), "终末_宗室"));
        config_list.push_back(new Config(new Condition("岩", "长柄武器", "Q"), "结晶", "水", 30, true,
                                         true, false, false, true, true, true, true,
                                         false, false, find_character_by_name("胡桃"), find_character_by_name("行秋"), find_character_by_name("夜兰"), ""));
        config_list.push_back(new Config(new Condition("岩", "长柄武器", "Q"), "结晶", "水", 30, true,
                                         true, false, false, true, true, true, true,
                                         false, false, find_character_by_name("胡桃"), find_character_by_name("莫娜"), find_character_by_name("夜兰"), "宗室"));
    }
}

void get_certain_combination(string c_name, vector<vector<string>> &combination_list)
{
    if (c_name == "行秋")
    {
        vector<string> temp1;
        temp1.emplace_back("祭礼剑");
        temp1.emplace_back("沉沦之心");
        temp1.emplace_back("昔日宗室之仪");
        temp1.emplace_back("攻击力");
        temp1.emplace_back("伤害加成");
        temp1.emplace_back("暴击率");
        combination_list.push_back(temp1);

        vector<string> temp2;
        temp2.emplace_back("祭礼剑");
        temp2.emplace_back("绝缘之旗印");
        temp2.emplace_back("绝缘之旗印");
        temp2.emplace_back("攻击力");
        temp2.emplace_back("伤害加成");
        temp2.emplace_back("暴击率");
        combination_list.push_back(temp2);
    }
    if (c_name == "香菱")
    {
        vector<string> temp1;
        temp1.emplace_back("断浪长鳍");
        temp1.emplace_back("流浪大地的乐团");
        temp1.emplace_back("昔日宗室之仪");
        temp1.emplace_back("元素充能效率");
        temp1.emplace_back("伤害加成");
        temp1.emplace_back("暴击率");
        combination_list.push_back(temp1);

        vector<string> temp2;
        temp2.emplace_back("断浪长鳍");
        temp2.emplace_back("炽烈的炎之魔女");
        temp2.emplace_back("炽烈的炎之魔女");
        temp2.emplace_back("元素充能效率");
        temp2.emplace_back("伤害加成");
        temp2.emplace_back("暴击率");
        combination_list.push_back(temp2);

        vector<string> temp3;
        temp3.emplace_back("断浪长鳍");
        temp3.emplace_back("绝缘之旗印");
        temp3.emplace_back("绝缘之旗印");
        temp3.emplace_back("元素充能效率");
        temp3.emplace_back("伤害加成");
        temp3.emplace_back("暴击率");
        combination_list.push_back(temp3);
    }
    if (c_name == "八重神子")
    {
        vector<string> temp1;
        temp1.emplace_back("流浪乐章");
        temp1.emplace_back("追忆之注连");
        temp1.emplace_back("角斗士的终幕礼");
        temp1.emplace_back("攻击力");
        temp1.emplace_back("伤害加成");
        temp1.emplace_back("暴击率");
        combination_list.push_back(temp1);

        vector<string> temp2;
        temp2.emplace_back("流浪乐章");
        temp2.emplace_back("如雷的盛怒");
        temp2.emplace_back("角斗士的终幕礼");
        temp2.emplace_back("攻击力");
        temp2.emplace_back("伤害加成");
        temp2.emplace_back("暴击率");
        combination_list.push_back(temp2);
    }
}

struct cmp
{
    bool operator()(Deployment *a, Deployment *b)
    {
        if (a->damage > b->damage) return true;
        else return false;
    }
};

void cal_deployment(string mode)
{
    for (auto &c_index: character_list)
    {
        vector<Config *> config_list;
        get_all_config(c_index->name, config_list);
        for (auto &conf_index: config_list)
        {
            if (mode == "cal_all" || mode == "cal_update")
            {
                for (auto &w_index: weapon_list)
                {
                    if (c_index->weapon_type != w_index->weapon_type) continue;

                    if (mode == "cal_update" && if_pre_cal_data_exists(c_index->name, conf_index, w_index->name)) continue;

                    priority_queue<Deployment *, vector<Deployment *>, cmp> c_w_pair;

                    clock_t start = clock();

                    for (int a_index1 = 0; a_index1 < artifact_list.size(); a_index1++)
                        for (int a_index2 = a_index1; a_index2 < artifact_list.size(); a_index2++)
                        {
                            for (int main3_index = 0; main3_index < 5; main3_index++)
                            {
                                for (int main4_index = (main3_index == 4) ? 0 : main3_index; main4_index < 5; main4_index++)
                                {
                                    for (int main5_index = (main4_index == 4) ? ((main3_index == 4) ? 0 : main3_index) : main4_index; main5_index < 7; main5_index++)
                                    {
                                        auto *temp = new Deployment(c_index, w_index, artifact_list[a_index1], artifact_list[a_index2],
                                                                    a_main3[main3_index], a_main4[main4_index], a_main5[main5_index], conf_index);
                                        int check_num = temp->init_check_data();
                                        if (check_num == 0)//pass
                                        {
                                            temp->cal_damage_entry_num();
                                            if (c_w_pair.size() < topk) c_w_pair.push(temp);
                                            else if (temp->damage > c_w_pair.top()->damage)
                                            {
                                                Deployment *smallest = c_w_pair.top();
                                                c_w_pair.pop();
                                                delete smallest;
                                                c_w_pair.push(temp);
                                            }
                                            else delete temp;
                                        }
                                        else if (check_num == 1)//error:special_restriction(character_restriction,recharge_failure)
                                        {
                                            delete temp;
                                        }
                                        else if (check_num == 2)//error:weapon
                                        {
                                            delete temp;
                                            goto NEXTWEAPON;
                                        }
                                        else if (check_num == 3)//error:suit1 or suit2
                                        {
                                            delete temp;
                                            goto NEXTARTIFACT;
                                        }
                                        else if (check_num == 4)//error:main3
                                        {
                                            delete temp;
                                            goto NEXTMAIN3;
                                        }
                                        else if (check_num == 5)//error:main4
                                        {
                                            delete temp;
                                            goto NEXTMAIN4;
                                        }
                                        else if (check_num == 6)//error:main5
                                        {
                                            delete temp;
                                            goto NEXTMAIN5;
                                        }
                                        NEXTMAIN5:;
                                    }
                                    NEXTMAIN4:;
                                }
                                NEXTMAIN3:;
                            }
                            NEXTARTIFACT:;
                        }
                    NEXTWEAPON:;

                    clock_t end = clock();

                    while (!c_w_pair.empty())
                    {
                        Deployment *c_w = c_w_pair.top();
                        c_w_pair.pop();
                        c_w->out();
                        delete c_w;
                    }
                    cout << c_index->name << " " << conf_index->condition->attack_way << " " << conf_index->react_type << " " << conf_index->teammate_all << " " << conf_index->team_weapon_artifact << " " << w_index->name
                         << " time=" << (double) (end - start) / CLOCKS_PER_SEC << "s" << "\n";
                }
            }
            else if (mode == "cal_certain")
            {
                vector<vector<string>> combination_list;
                get_certain_combination(c_index->name, combination_list);
                for (auto &i: combination_list)
                {
                    Weapon *w_point = find_weapon_by_name(i[0]);
                    Artifact *suit1 = find_artifact_by_name(i[1]);
                    Artifact *suit2 = find_artifact_by_name(i[2]);
                    if (w_point != nullptr && suit1 != nullptr && suit2 != nullptr)
                    {
                        auto *temp = new Deployment(c_index, w_point, suit1, suit2, i[3], i[4], i[5], conf_index);
                        int check_num = temp->init_check_data();
                        if (check_num == 0)
                        {
                            temp->cal_damage_entry_num();
                            temp->out();
                        }
                        else cout << check_num << endl;
                        delete temp;
                    }
                }
                combination_list.clear();
            }
        }
        config_list.clear();
    }
}

int main()
{
    read_pre_cal_data("./data.csv");
    init_character_data();
    init_weapon_data();
    init_artifact_data();

    int mode;
    int filter;
    cout << "输入计算方式(1:全计算 2:增量计算 3:特定计算):";
    cin >> mode;
    cout << "输入filter_type(1:all 2:artifact 3:main 4:none):";
    cin >> filter;

    if (mode == 1)
    {
        out_header = true;
        if (filter == 1) filter_type = "all";
        else if (filter == 2) filter_type = "artifact";
        else if (filter == 3) filter_type = "main";
        else if (filter == 4) filter_type = "none";
        outfile_result.open("./data.csv");
        outfile_debug.open("./log_data.csv");
        cal_deployment("cal_all");
    }
    else if (mode == 2)
    {
        out_header = false;
        if (filter == 1) filter_type = "all";
        else if (filter == 2) filter_type = "artifact";
        else if (filter == 3) filter_type = "main";
        else if (filter == 4) filter_type = "none";
        outfile_result.open("./data.csv", ios::app);
        outfile_debug.open("./log_data.csv", ios::app);
        cal_deployment("cal_update");
    }
    else if (mode == 3)
    {
        out_header = true;
        filter_type = "none";
        outfile_result.open("./certain_data.csv");
        outfile_debug.open("./certain_log_data.csv");
        cal_deployment("cal_certain");
    }
    outfile_result.close();
    outfile_debug.close();
    return 0;
}