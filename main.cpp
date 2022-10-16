#include <iostream>
#include <queue>
#include <ctime>
#include "Condition.h"
#include "Character.h"
#include "Weapon.h"
#include "Artifact.h"
#include "Group.h"

using namespace std;
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
    character_list.push_back(new Character("胡桃", "火", "长柄武器", 15552, 107, 876, "暴击伤害", 0.384,
                                           10, "火", (0.836 + 0.861 + 1.089 + 1.171 + 0.594 + 0.628 + 1.534) / 7, (0.789 + 0.812 + 1.027 + 1.104 + 0.56 + 0.592 + 1.446) / 7,
                                           "火", 2.426, 2.287, "火", 2.92, 2.75,
                                           10, 5, false, 1.36, 1.28, 1.15, 1.09,
                                           10, 60, false, 7.06, 6.76, 6.17, 5.88,
                                           1, temp,
                                           new weapon_artifact_related_arguments(false, false, 0, false, 1, false, false, -1,
                                                                                 -1, -1, -1, -1, false, true, 3, 1)));
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
                                           10, "冰", (0.904 + 0.962 + 1.238 + 0.448 * 3 + 1.545) / 7, (0.84 + 0.894 + 1.15 + 0.416 * 3 + 1.436) / 7,
                                           "冰", 1.09, 1.013, "冰", 3.156, 2.934,
                                           10, 4.5, false, 5.083, 4.784, 4.305, 4.066,
                                           10, 80, true, (19 * 2.386 + 3.58) / 20, (19 * 2.246 + 3.369) / 20, (19 * 2.021 + 3.032) / 20, (19 * 1.909 + 2.864) / 20,
                                           0, temp,
                                           new weapon_artifact_related_arguments(false, false, 1, false, 1, false, true, 3,
                                                                                 2, -1, -1, -1, false, false, -1, -1)));//Q 19切割+1绽放
    temp.clear();

    temp.push_back(nullptr);//队伍获得元素微粒增加2层愿力
    temp.push_back(nullptr);//充能转化雷伤(recharge-1)*0.4 (convert) 回能提高(recharge-1)*0.6 (recharge)
    temp.push_back(nullptr);//雷元素角色释放元素爆发后，积攒的愿力提升80%；其他元素类型角色施放元素爆发后，积攒的愿力提升20%。
    temp.push_back(new Set(new Condition("ALL", "ALL", "E|Q"), "防御无视", 0.6));//Q后
    temp.push_back(nullptr);//Q结束 队友30%攻击10s (get_team)
    temp.push_back(nullptr);//Q伤害命中，1次减队友CD1s/5次
    character_list.push_back(new Character("雷电将军", "雷", "长柄武器", 12907, 337, 789, "元素充能效率", 0.32,
                                           9, "物理", (0.784 + 0.785 + 0.986 + 0.573 + 0.573 + 1.294) / 6, (0.728 + 0.73 + 0.916 + 0.533 + 0.533 + 1.202) / 6,
                                           "物理", 1.969, 1.83, "物理", 3.16, 2.93,
                                           9, 10, false, (2.491 + 18 * 0.893) / 19, (2.344 + 18 * 0.84) / 19, (2.11 + 18 * 0.756) / 19, (1.992 + 18 * 0.714) / 19,
                                           10, 90, false, 8.52 + 0.0826 * 60, 8.02 + 0.0778 * 60, 7.21 + 0.07 * 60, 6.81 + 0.0661 * 60,
                                           0, temp,
                                           new weapon_artifact_related_arguments(false, false, 2, true, 1, false, true, -1,
                                                                                 -1, -1, -1, -1, false, false, 3, 2)));//E 1+25，Q 60层愿力，不考虑后续平A
    temp.clear();
    //E 全体大招增伤0.3*Q_energy (special get_team) Q全体回能12.5 (recharge)

    temp.push_back(new Set(new Condition("ALL", "ALL", "重A"), "暴击率", 0.2));//重A后
    temp.push_back(nullptr);//Q内前台角色 (special get_team)
    temp.push_back(new Set(new Condition("冰", "ALL", "ALL"), "抗性削弱", 0.15));//(get_team) 每次重A命中恢复2点能量 (recharge)
    temp.push_back(nullptr);//E变为2次 (recharge)
    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "伤害加成", 0.15));//Q内 (get_team)
    temp.push_back(nullptr);//E后瞬发重A
    character_list.push_back(new Character("甘雨", "冰", "弓", 9797, 335, 630, "暴击伤害", 0.384,
                                           9, "物理", (0.627 + 0.704 + 0.899 + 0.899 + 0.954 + 1.139) / 6, (0.583 + 0.654 + 0.836 + 0.836 + 0.886 + 1.059) / 6,
                                           "冰", (2.3 + 3.92) / 2, (2.18 + 3.7) / 2, "物理", 2.81, 2.61,
                                           9, 4, true, 2.81, 2.64, 2.38, 2.24,
                                           9, 60, true, 1.49, 1.41, 1.26, 1.19,
                                           0, temp,
                                           new weapon_artifact_related_arguments(false, false, 2, false, 1, false, true, -1,
                                                                                 -1, -1, 3, 4, true, false, -1, -1)));
    temp.clear();

    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "生命值", 0.18));//3色队
    temp.push_back(nullptr);//前台角色伤害提升1+3.5x% (special get_team)
    temp.push_back(nullptr);//E变为2次 (recharge)
    temp.push_back(nullptr);//每1.8s Q倍率增加14%*LIFE (extra_rate)
    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "生命值", 0.4));//E后 25s (get_team)
    temp.push_back(nullptr);//Q后平A变为重A，伤害*1.56*5
    character_list.push_back(new Character("夜兰", "水", "弓", 14450, 244, 548, "暴击率", 0.192,
                                           9, "物理", (0.804 + 0.772 + 1.02 + 0.643 * 2) / 5, (0.747 + 0.717 + 0.948 + 0.597 * 2) / 5,
                                           "水", 0, 0, "物理", 2.81, 2.61,
                                           9, 4, false, 0, 0, 0, 0,
                                           9, 70, false, 0, 0, 0, 0,
                                           0, temp,
                                           new weapon_artifact_related_arguments(false, false, 1, false, 1, false, true, -1,
                                                                                 -1, -1, 3, 4, true, false, -1, -1)));
    temp.clear();
    //重A 生命倍率19.68% E 生命倍率38.4% Q 生命倍率(12.42+8.28*45)/46 (extra_rate)

    temp.push_back(nullptr);//每柄雨帘剑恢复当前角色6%行秋最大生命
    temp.push_back(new Set(new Condition("水", "ALL", "ALL"), "伤害加成", 0.2));
    temp.push_back(nullptr);//雨帘剑变为4柄
    temp.push_back(new Set(new Condition("水", "ALL", "ALL"), "抗性削弱", 0.15));//Q变为18s Q命中 (get_team)
    temp.push_back(nullptr);//Q期间，E倍率*1.5 (special)
    temp.push_back(nullptr);//每2次攻击，下次攻击变为5柄剑，回复3点能量 (recharge)
    character_list.push_back(new Character("行秋", "水", "单手剑", 10222, 202, 758, "攻击力", 0.24,
                                           9, "物理", (0.921 + 0.942 + 0.564 + 0.564 + 1.107 + 0.709 + 0.709) / 7, (0.856 + 0.875 + 0.525 + 0.525 + 1.029 + 0.659 + 0.659) / 7,
                                           "物理", (0.935 + 1.11) / 2, (0.869 + 1.032) / 2, "物理", 3.16, 2.93,
                                           9 + 3, 5, false, (3.57 + 4.06) / 2, (3.36 + 3.82) / 2, (3.02 + 3.44) / 2, (2.86 + 3.25) / 2,
                                           9 + 3, 80, false, 1.15, 1.09, 0.977, 0.923,
                                           6, temp,
                                           new weapon_artifact_related_arguments(false, false, 2, false, 1, false, true, 2,
                                                                                 2, -1, -1, -1, false, false, -1, -1)));
    temp.clear();

    temp.push_back(nullptr);//E喷火距离提高20%
    temp.push_back(nullptr);//E结束拾取辣椒提高10%攻击 (get_team)，不考虑
    temp.push_back(new Set(new Condition("火", "ALL", "ALL"), "抗性削弱", 0.15));//E伤害 (get_team)
    temp.push_back(nullptr);//平A最后一击2s后造成75%火元素伤害
    temp.push_back(nullptr);//Q变为14s
    temp.push_back(new Set(new Condition("火", "ALL", "平A|重A|下落A|E"), "伤害加成", 0.15));//Q持续期间 (get_team)
    character_list.push_back(new Character("香菱", "火", "长柄武器", 10875, 225, 669, "元素精通", 96,
                                           9, "物理", (0.831 + 0.833 + 0.515 + 0.515 + 0.279 * 4 + 1.4) / 9, (0.773 + 0.774 + 0.479 + 0.479 + 0.259 * 4 + 1.31) / 9,
                                           "物理", 2.41, 2.24, "物理", 3.16, 2.93,
                                           9 + 3, 4, true, 2.36, 2.23, 2.0, 1.89,
                                           9 + 3, 80, true, (1.53 + 1.87 + 2.33 + 12 * 2.38) / 15, (1.44 + 1.76 + 2.19 + 12 * 2.24) / 15, (1.3 + 1.58 + 1.97 + 12 * 2.02) / 15, (1.22 + 1.5 + 1.86 + 12 * 1.9) / 15,
                                           6, temp,
                                           new weapon_artifact_related_arguments(false, false, 2, false, 1, false, true, -1,
                                                                                 -1, -1, -1, -1, false, false, 3, 1)));//E 4次，Q 12次
    temp.clear();

    temp.push_back(nullptr);//Q重置E
    temp.push_back(nullptr);//精通转化E伤害mastery*0.15% (convert)
    temp.push_back(nullptr);//每个E为Q后恢复8点能量，24点 (recharge)
    temp.push_back(nullptr);//E倍率提升25% (get_special)
    temp.push_back(new Set(new Condition("雷", "ALL", "ALL"), "伤害加成", 0.2));//E命中 (get_team)
    temp.push_back(new Set(new Condition("ALL", "ALL", "E"), "防御无视", 0.6));
    character_list.push_back(new Character("八重神子", "雷", "法器", 10372, 340, 569, "暴击率", 0.192,
                                           9, "雷", (0.714 + 0.693 + 1.024) / 3, (0.674 + 0.655 + 0.967) / 3,
                                           "雷", 2.572, 2.429, "雷", 2.81, 2.61,
                                           9, 5, false, 2.015, 1.896, 1.706, 1.612,
                                           9, 90, false, (5.53 + 7.09 * 3) / 4, (5.2 + 6.68 * 3) / 4, (4.68 + 6.01 * 3) / 4, (4.42 + 5.68 * 3) / 4,
                                           1, temp,
                                           new weapon_artifact_related_arguments(false, false, 2, true, 3, false, true, -1,
                                                                                 -1, 3, -1, -1, false, false, -1, -1)));//E 15次
    temp.clear();

    temp.push_back(nullptr);//E产生上升气流20s
    temp.push_back(nullptr);//Q恢复自己与转化元素15点能量 (recharge)
    temp.push_back(nullptr);//重A发射分裂箭
    temp.push_back(new Set(new Condition("风|物理", "ALL", "ALL"), "抗性削弱", 0.24));//E后起飞0.24，落地0.12 (get_team)
    temp.push_back(new Set(new Condition("风", "ALL", "ALL"), "伤害加成", 0.25));//获得元素微粒
    temp.push_back(new Set(new Condition("火|水|雷|冰|风", "ALL", "ALL"), "抗性削弱", 0.2));//Q (get_team)
    character_list.push_back(new Character("温迪", "风", "弓", 10531, 263, 669, "元素充能效率", 0.32,
                                           9, "物理", (0.403 + 0.403 + 0.877 + 1.035 + 0.515 + 0.515 + 1.001 + 1.403) / 8, (0.375 + 0.375 + 0.815 + 0.962 + 0.479 + 0.479 + 0.931 + 1.304) / 8,
                                           "风", 2.23, 2.11, "物理", 2.81, 2.61,
                                           9, 3, false, 5.87, 5.52, 4.97, 4.69,
                                           9, 60, true, 0.799, 0.752, 0.677, 0.639,
                                           0, temp,
                                           new weapon_artifact_related_arguments(false, false, 2, false, 2, false, true, -1,
                                                                                 -1, -1, 3, 3, true, false, -1, -1)));//E短按 Q 20风伤+20转化+12扩散
    temp.clear();

    temp.push_back(nullptr);//冲刺自动产生E
    temp.push_back(nullptr);//充能转化水伤20%*recharge (get_convert)
    temp.push_back(nullptr);//Q后感电、蒸发、扩散伤害提升15%，冻结时间提升15% (react)
    temp.push_back(nullptr);//平A后20%重A一次，CD5s
    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "暴击率", 0.15));//Q后 (get_team)
    temp.push_back(new Set(new Condition("ALL", "ALL", "重A"), "伤害加成", 1.8));//冲刺 60%/s
    character_list.push_back(new Character("莫娜", "水", "法器", 10409, 287, 653, "元素充能效率", 0.32,
                                           9, "水", (0.677 + 0.648 + 0.806 + 1.01) / 4, (0.639 + 0.612 + 0.762 + 0.955) / 4,
                                           "水", 2.69, 2.55, "水", 2.81, 2.61,
                                           9, 3, true, (0.68 * 4 + 2.82) / 5, (0.64 * 4 + 2.66) / 5, (0.576 * 4 + 2.39) / 5, (0.544 * 4 + 2.26) / 5,
                                           9, 60, false, 9.4, 8.85, 7.96, 7.52,
                                           1, temp,
                                           new weapon_artifact_related_arguments(false, false, 2, false, 1, false, true, -1,
                                                                                 -1, 2, -1, -1, false, false, -1, -1)));
    temp.clear();
    //Q 58%增伤(special get_team)

    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "护盾强效", 0.25));//护盾受伤害，5%强效/层，5层 (get_team)
    temp.push_back(nullptr);//普通攻击、重击与下落攻击造成的伤害提高值：生命值上限的1.39%。地心的岩脊、共鸣伤害与长按伤害提高值：生命值上限的1.9%。天星伤害提高值：生命值上限33% (extra_rate)
    temp.push_back(nullptr);//岩脊至多可以同时存在2个
    temp.push_back(nullptr);//Q加盾
    temp.push_back(nullptr);//Q影响范围扩大20%，石化效果持续时间延长2秒。
    temp.push_back(nullptr);//护盾生效期间，受到伤害的40%转为等额生命值恢复。单次恢复量不超过角色最大生命值的8%。
    character_list.push_back(new Character("钟离", "岩", "长柄武器", 14695, 251, 738, "伤害加成", 0.288,
                                           9, "物理", (0.608 + 0.616 + 0.763 + 0.849 + 0.213 * 4 + 1.077) / 9, (0.565 + 0.572 + 0.709 + 0.789 + 0.198 * 4 + 1.001) / 9,
                                           "物理", 2.2, 2.04, "物理", 3.16, 2.93,
                                           9, 0.5, true, 1.7, 1.6, 1.44, 1.36,
                                           9, 40, false, 10.84, 10.3, 9, 8.35,
                                           0, temp,
                                           new weapon_artifact_related_arguments(true, false, 2, true, 3, false, true, -1,
                                                                                 -1, -1, -1, -1, false, false, 3, 2)));//E长按不考虑后续伤害
    temp.clear();
    //E护盾减抗20%(special get_team)

    //TODO:NEW
    temp.push_back(nullptr);
    temp.push_back(nullptr);
    temp.push_back(nullptr);
    temp.push_back(nullptr);
    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "元素精通", 130));//默认平均
    temp.push_back(nullptr);//与E一致，倍率为200%ATK+400%EM
    character_list.push_back(new Character("纳西妲", "草", "法器", 10360, 299, 630, "元素精通", 115.0,
                                           9, "草", (0.726 + 0.666 + 0.826 + 1.051) / 4, (0.685 + 0.629 + 0.78 + 0.993) / 4,
                                           "草", 2.376, 2.244, "草", 2.81, 2.61,
                                           9, 3, false, 2.193, 2.064, 1.858, 1.754,
                                           9, 50, false, 0, 0, 0, 0,
                                           2, temp,
                                           new weapon_artifact_related_arguments(false, false, 2, true, 2, false, true, -1,
                                                                                 -1, 3, -1, -1, false, false, -1, -1)));
    temp.clear();
    //E 只考虑后续伤害 (extra_rate) Q、1命、2命(special) 2命、天赋1(get_team) 天赋2(get_convert)

    //NO NEED TO CALCULATE
    //BUILD BASIC INFORMATION AND ARGS
    temp.push_back(nullptr);
    temp.push_back(nullptr);
    temp.push_back(nullptr);
    temp.push_back(nullptr);
    temp.push_back(nullptr);
    temp.push_back(nullptr);
    character_list.push_back(new Character("枫原万叶", "风", "单手剑", 13348, 297, 807, "元素精通", 115.0,
                                           9, "物理", 0, 0, "物理", 0, 0, "物理", 0, 0,
                                           9, 4, false, 0, 0, 0, 0,
                                           9, 60, true, 0, 0, 0, 0,
                                           0, temp,
                                           new weapon_artifact_related_arguments(false, false, 2, false, 2, false, true, 2,
                                                                                 3, -1, -1, -1, false, false, -1, -1)));
    temp.clear();

    temp.push_back(nullptr);
    temp.push_back(nullptr);
    temp.push_back(nullptr);
    temp.push_back(nullptr);
    temp.push_back(nullptr);
    temp.push_back(nullptr);
    character_list.push_back(new Character("班尼特", "火", "单手剑", 12397, 191, 771, "元素充能效率", 0.267,
                                           9, "物理", 0, 0, "物理", 0, 0, "物理", 0, 0,
                                           9 + 3, 2, false, 0, 0, 0, 0,
                                           9 + 3, 60, false, 0, 0, 0, 0,
                                           5, temp,
                                           new weapon_artifact_related_arguments(false, true, 2, false, 3, false, false, 3,
                                                                                 3, -1, -1, -1, false, false, -1, -1)));
    temp.clear();

    //MODEL Character
    temp.push_back(nullptr);
    temp.push_back(nullptr);
    temp.push_back(nullptr);
    temp.push_back(nullptr);
    temp.push_back(nullptr);
    temp.push_back(nullptr);
    character_list.push_back(new Character("火_test", "火", "单手剑", 10000, 300, 700, "暴击率", 0.192,
                                           9, "火", 0, 0, "火", 0, 0, "火", 0, 0,
                                           9, 3, false, 0, 0, 0, 0,
                                           9, 60, false, 0, 0, 0, 0,
                                           0, temp,
                                           new weapon_artifact_related_arguments(false, false, -1, false, -1, false, false, -1,
                                                                                 -1, -1, -1, -1, false, false, -1, -1)));
    temp.clear();

    temp.push_back(nullptr);
    temp.push_back(nullptr);
    temp.push_back(nullptr);
    temp.push_back(nullptr);
    temp.push_back(nullptr);
    temp.push_back(nullptr);
    character_list.push_back(new Character("水_test", "水", "单手剑", 10000, 300, 700, "暴击率", 0.192,
                                           9, "水", 0, 0, "水", 0, 0, "水", 0, 0,
                                           9, 3, false, 0, 0, 0, 0,
                                           9, 60, false, 0, 0, 0, 0,
                                           0, temp,
                                           new weapon_artifact_related_arguments(false, false, -1, false, -1, false, false, -1,
                                                                                 -1, -1, -1, -1, false, false, -1, -1)));
    temp.clear();

    temp.push_back(nullptr);
    temp.push_back(nullptr);
    temp.push_back(nullptr);
    temp.push_back(nullptr);
    temp.push_back(nullptr);
    temp.push_back(nullptr);
    character_list.push_back(new Character("雷_test", "雷", "单手剑", 10000, 300, 700, "暴击率", 0.192,
                                           9, "雷", 0, 0, "雷", 0, 0, "雷", 0, 0,
                                           9, 3, false, 0, 0, 0, 0,
                                           9, 60, false, 0, 0, 0, 0,
                                           0, temp,
                                           new weapon_artifact_related_arguments(false, false, -1, false, -1, false, false, -1,
                                                                                 -1, -1, -1, -1, false, false, -1, -1)));
    temp.clear();

    temp.push_back(nullptr);
    temp.push_back(nullptr);
    temp.push_back(nullptr);
    temp.push_back(nullptr);
    temp.push_back(nullptr);
    temp.push_back(nullptr);
    character_list.push_back(new Character("冰_test", "冰", "单手剑", 10000, 300, 700, "暴击率", 0.192,
                                           9, "冰", 0, 0, "冰", 0, 0, "冰", 0, 0,
                                           9, 3, false, 0, 0, 0, 0,
                                           9, 60, false, 0, 0, 0, 0,
                                           0, temp,
                                           new weapon_artifact_related_arguments(false, false, -1, false, -1, false, false, -1,
                                                                                 -1, -1, -1, -1, false, false, -1, -1)));
    temp.clear();

    temp.push_back(nullptr);
    temp.push_back(nullptr);
    temp.push_back(nullptr);
    temp.push_back(nullptr);
    temp.push_back(nullptr);
    temp.push_back(nullptr);
    character_list.push_back(new Character("风_test", "风", "单手剑", 10000, 300, 700, "暴击率", 0.192,
                                           9, "风", 0, 0, "风", 0, 0, "风", 0, 0,
                                           9, 3, false, 0, 0, 0, 0,
                                           9, 60, false, 0, 0, 0, 0,
                                           0, temp,
                                           new weapon_artifact_related_arguments(false, false, -1, false, -1, false, false, -1,
                                                                                 -1, -1, -1, -1, false, false, -1, -1)));
    temp.clear();

    temp.push_back(nullptr);
    temp.push_back(nullptr);
    temp.push_back(nullptr);
    temp.push_back(nullptr);
    temp.push_back(nullptr);
    temp.push_back(nullptr);
    character_list.push_back(new Character("岩_test", "岩", "单手剑", 10000, 300, 700, "暴击率", 0.192,
                                           9, "岩", 0, 0, "岩", 0, 0, "岩", 0, 0,
                                           9, 3, false, 0, 0, 0, 0,
                                           9, 60, false, 0, 0, 0, 0,
                                           0, temp,
                                           new weapon_artifact_related_arguments(false, false, -1, false, -1, false, false, -1,
                                                                                 -1, -1, -1, -1, false, false, -1, -1)));
    temp.clear();

    temp.push_back(nullptr);
    temp.push_back(nullptr);
    temp.push_back(nullptr);
    temp.push_back(nullptr);
    temp.push_back(nullptr);
    temp.push_back(nullptr);
    character_list.push_back(new Character("草_test", "草", "单手剑", 10000, 300, 700, "暴击率", 0.192,
                                           9, "草", 0, 0, "草", 0, 0, "草", 0, 0,
                                           9, 3, false, 0, 0, 0, 0,
                                           9, 60, false, 0, 0, 0, 0,
                                           0, temp,
                                           new weapon_artifact_related_arguments(false, false, -1, false, -1, false, false, -1,
                                                                                 -1, -1, -1, -1, false, false, -1, -1)));
    temp.clear();
}

//build new character(needed)
bool Character::get_extra_special(Deployment *data) const
{
    if (data->c_point->name == "雷电将军")
    {
        if (data->attack_config->condition->attack_way == "Q")
            data->add_percentage("伤害加成", 0.27, (name + "_extra_special"));
    }
    else if (data->c_point->name == "甘雨")
    {
        if (!(data->attack_config->background && !data->attack_config->lockface) && data->attack_config->condition->attack_way != "Q")
            data->add_percentage("伤害加成", 0.2, (name + "_extra_special"));
    }
    else if (data->c_point->name == "夜兰")
    {
        if (!data->attack_config->background)
            data->add_percentage("伤害加成", 0.25, (name + "_extra_special"));
    }
    else if (data->c_point->name == "行秋")
    {
        if (data->attack_config->condition->attack_way == "E" && data->c_point->constellation >= 4)
            data->add_percentage("额外倍率", data->base_skillrate * 0.5, (name + "_extra_special"));
//            data->base_skillrate *= 1.5;
    }
    else if (data->c_point->name == "八重神子")
    {
        if (data->attack_config->condition->attack_way == "E" && data->c_point->constellation >= 2)
            data->add_percentage("额外倍率", data->base_skillrate * 0.25, (name + "_extra_special"));
//            data->base_skillrate *= 1.25;
    }
    else if (data->c_point->name == "莫娜")
    {
        data->add_percentage("伤害加成", 0.58, (name + "_extra_special"));
    }
    else if (data->c_point->name == "钟离")
    {
        data->add_percentage("抗性削弱", 0.2, (name + "_extra_special"));
    }
        //TODO:NEW
    else if (data->c_point->name == "纳西妲")
    {
        int pyro_num = 0;
        if (data->team_config->teammate_1->ele_type == "火") pyro_num++;
        if (data->team_config->teammate_2->ele_type == "火") pyro_num++;
        if (data->team_config->teammate_3->ele_type == "火") pyro_num++;
        if (data->c_point->constellation >= 1) pyro_num++;

        if (data->attack_config->condition->attack_way == "E")
        {
            if (pyro_num == 1) data->add_percentage("伤害加成", 0.253, (name + "_extra_special"));//lv9
            else if (pyro_num >= 2) data->add_percentage("伤害加成", 0.379, (name + "_extra_special"));//lv9
        }

        if (!(data->attack_config->background && !data->attack_config->lockface))
            data->add_converted_percentage("元素精通", 250, (name + "_extra_special"));

        if (data->c_point->constellation >= 2 && data->attack_config->react_type.find("激化") != string::npos)
            data->add_percentage("防御削弱", 0.3, (name + "_extra_special"));
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

    //(convert)
    //(get_team)
    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "生命值", 0.2));
    weapon_list.push_back(new Weapon("圣显之钥", "单手剑", 542, "生命值", 0.662, 1, temp));
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

    weapon_list.push_back(new Weapon("笛剑", "单手剑", 510, "攻击力", 0.413, 5, temp));
    temp.clear();

    temp.push_back(new Set(new Condition("ALL", "ALL", "E"), "伤害加成", 0.16));
    temp.push_back(new Set(new Condition("ALL", "ALL", "E"), "暴击率", 0.06));
    weapon_list.push_back(new Weapon("腐殖之剑", "单手剑", 510, "元素充能效率", 0.459, 5, temp));
    temp.clear();

    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "伤害加成", 0.12));//元素伤害，2层
    weapon_list.push_back(new Weapon("铁蜂刺", "单手剑", 510, "元素精通", 165.0, 5, temp));
    temp.clear();

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
    weapon_list.push_back(new Weapon("西风剑", "单手剑", 454, "元素充能效率", 0.613, 5, temp));
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

    //TODO:NEW
    //(special)
    //(get_team)
    weapon_list.push_back(new Weapon("千夜浮梦", "法器", 542, "元素精通", 265.0, 1, temp));
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

    //TODO:拆分成4种分别计算
    //(special)
    weapon_list.push_back(new Weapon("流浪乐章", "法器", 510, "暴击伤害", 0.551, 5, temp));
    temp.clear();

    weapon_list.push_back(new Weapon("忍冬之果", "法器", 510, "攻击力", 0.413, 5, temp));
    temp.clear();

    //(recharge)
    weapon_list.push_back(new Weapon("试作金珀", "法器", 510, "生命值", 0.413, 5, temp));
    temp.clear();

    //(recharge)
    weapon_list.push_back(new Weapon("西风秘典", "法器", 510, "元素充能效率", 0.459, 5, temp));
    temp.clear();

    //(convert)
    //(get_team)
    weapon_list.push_back(new Weapon("流浪的晚星", "法器", 510, "元素精通", 165.0, 1, temp));
    temp.clear();

    //(special)
    weapon_list.push_back(new Weapon("盈满之实", "法器", 510, "元素充能效率", 0.459, 5, temp));
    temp.clear();

    weapon_list.push_back(new Weapon("昭心", "法器", 454, "攻击力", 0.551, 5, temp));
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
    weapon_list.push_back(new Weapon("祭礼弓", "弓", 565, "元素充能效率", 0.306, 5, temp));
    temp.clear();

    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "伤害加成", 0.1));
    weapon_list.push_back(new Weapon("落霞", "弓", 565, "元素充能效率", 0.306, 5, temp));
    temp.clear();

    weapon_list.push_back(new Weapon("苍翠猎弓", "弓", 510, "暴击率", 0.276, 1, temp));
    temp.clear();

    //(special)
    temp.push_back(new Set(new Condition("ALL", "ALL", "平A"), "伤害加成", 0.4));
    weapon_list.push_back(new Weapon("弓藏", "弓", 510, "攻击力", 0.413, 5, temp));
    temp.clear();

    weapon_list.push_back(new Weapon("试作澹月", "弓", 510, "攻击力", 0.413, 5, temp));
    temp.clear();

    //(special)
    weapon_list.push_back(new Weapon("幽夜华尔兹", "弓", 510, "伤害加成", 0.517, 1, temp));
    temp.clear();

    weapon_list.push_back(new Weapon("竭泽", "弓", 510, "元素充能效率", 0.459, 5, temp));
    temp.clear();

    temp.push_back(new Set(new Condition("ALL", "ALL", "E|Q"), "伤害加成", 0.24));
    weapon_list.push_back(new Weapon("绝弦", "弓", 510, "元素精通", 165.0, 5, temp));
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
    weapon_list.push_back(new Weapon("祭礼大剑", "双手剑", 565, "元素充能效率", 0.306, 5, temp));
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
    weapon_list.push_back(new Weapon("钟剑", "双手剑", 510, "生命值", 0.413, 5, temp));
    temp.clear();

    //(recharge)
    temp.push_back(new Set(new Condition("ALL", "ALL", "E"), "伤害加成", 0.06));
    weapon_list.push_back(new Weapon("桂木斩长正", "双手剑", 510, "元素充能效率", 0.459, 5, temp));
    temp.clear();

    //(special)
    weapon_list.push_back(new Weapon("雨裁", "双手剑", 510, "元素精通", 165.0, 5, temp));
    temp.clear();

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
    weapon_list.push_back(new Weapon("匣里灭辰", "长柄武器", 454, "元素精通", 221.0, 5, temp));
    temp.clear();
}

//build new weapon(all)
bool Weapon::get_extra_special(Deployment *data) const
{
    if (name == "雾切之回光")
    {
        if (data->attack_config->condition->ele_type != "物理")
        {
            if (data->c_point->args->sword_wuqie_level == 1) data->add_percentage("伤害加成", (0.08 * (0.75 + level * 0.25)), (name + "_extra_special"));
            else if (data->c_point->args->sword_wuqie_level == 2) data->add_percentage("伤害加成", (0.16 * (0.75 + level * 0.25)), (name + "_extra_special"));
            else if (data->c_point->args->sword_wuqie_level == 3) data->add_percentage("伤害加成", (0.28 * (0.75 + level * 0.25)), (name + "_extra_special"));
        }
    }
    else if (name == "斫峰之刃")
    {
//        if (data->c_point->args->accumulate_attacks)
        if (!data->attack_config->background)
        {
            data->add_percentage("攻击力", (0.04 * 5 * (0.75 + level * 0.25)), (name + "_extra_special"));
            if (data->c_point->args->shield_sustain || data->team_config->teammate_1->args->shield_sustain || data->team_config->teammate_2->args->shield_sustain || data->team_config->teammate_3->args->shield_sustain)
                data->add_percentage("攻击力", (0.04 * 5 * (0.75 + level * 0.25)), (name + "_extra_special"));
        }
    }
    else if (name == "苍古自由之誓")
    {
        if (data->attack_config->react_type != "NONE")
        {
            if (data->attack_config->condition->attack_way == "平A" || data->attack_config->condition->attack_way == "重A" || data->attack_config->condition->attack_way == "下落A")
                data->add_percentage("伤害加成", (0.16 * (0.75 + level * 0.25)), (name + "_extra_special"));
            data->add_percentage("攻击力", (0.2 * (0.75 + level * 0.25)), (name + "_extra_special"));
        }
    }
    else if (name == "试作斩岩")
    {
//        if (data->c_point->args->accumulate_attacks)
        if (!data->attack_config->background)
        {
            data->add_percentage("攻击力", (0.04 * 4 * (0.75 + level * 0.25)), (name + "_extra_special"));
            data->add_percentage("防御力", (0.04 * 4 * (0.75 + level * 0.25)), (name + "_extra_special"));
        }
    }
    else if (name == "原木刀")
    {
        if (data->attack_config->react_type.find("燃烧") != string::npos || data->attack_config->react_type.find("激化") != string::npos || data->attack_config->react_type.find("绽放") != string::npos)
            data->add_percentage("元素精通", (60.0 * (0.75 + level * 0.25)), (name + "_extra_special"));
    }
    else if (name == "匣里龙吟")
    {
        if (data->team_config->ele_attach_type.find("火") != string::npos || data->team_config->ele_attach_type.find("雷") != string::npos)
            data->add_percentage("伤害加成", (0.16 + level * 0.04), (name + "_extra_special"));
    }

    else if (name == "四风原典")
    {
        if (!data->attack_config->background)
            if (data->attack_config->condition->ele_type != "物理")
                data->add_percentage("伤害加成", (0.16 * (0.75 + level * 0.25)), (name + "_extra_special"));
    }
    else if (name == "神乐之真意")
    {
        if (data->c_point->args->catalyst_shenle_level == 3)
        {
            if (data->attack_config->condition->attack_way == "E")
                data->add_percentage("伤害加成", (0.12 * 3 * (0.75 + level * 0.25)), (name + "_extra_special"));
            if (data->attack_config->condition->ele_type != "物理")
                data->add_percentage("伤害加成", (0.12 * (0.75 + level * 0.25)), (name + "_extra_special"));
        }
        else if (data->c_point->args->catalyst_shenle_level == 2)
        {
            if (data->attack_config->condition->attack_way == "E")
                data->add_percentage("伤害加成", (0.12 * 2 * (0.75 + level * 0.25)), (name + "_extra_special"));
        }
        else if (data->c_point->args->catalyst_shenle_level == 1)
        {
            if (data->attack_config->condition->attack_way == "E")
                data->add_percentage("伤害加成", (0.12 * (0.75 + level * 0.25)), (name + "_extra_special"));
        }
    }
    else if (name == "尘世之锁")
    {
//        if (data->c_point->args->accumulate_attacks)
        if (!data->attack_config->background)
        {
            data->add_percentage("攻击力", (0.04 * 5 * (0.75 + level * 0.25)), (name + "_extra_special"));
            if (data->c_point->args->shield_sustain || data->team_config->teammate_1->args->shield_sustain || data->team_config->teammate_2->args->shield_sustain || data->team_config->teammate_3->args->shield_sustain)
                data->add_percentage("攻击力", (0.04 * 5 * (0.75 + level * 0.25)), (name + "_extra_special"));
        }
    }
        //TODO:NEW
    else if (name == "千夜浮梦")
    {
        int same = 0;
        int diff = 0;
        if (data->team_config->teammate_1->ele_type == data->c_point->ele_type) same += 1;
        else diff += 1;
        if (data->team_config->teammate_2->ele_type == data->c_point->ele_type) same += 1;
        else diff += 1;
        if (data->team_config->teammate_3->ele_type == data->c_point->ele_type) same += 1;
        else diff += 1;
        data->add_percentage("元素精通", 32.0 * (0.75 + level * 0.25) * same, (name + "_extra_special"));
        data->add_percentage("伤害加成", (0.06 + level * 0.04) * diff, (name + "_extra_special"));
    }
    else if (name == "白辰之环")
    {
        if ((data->attack_config->react_type.find("超载") != string::npos && (data->attack_config->condition->ele_type == "雷" || data->attack_config->condition->ele_type == "火")) ||
            (data->attack_config->react_type.find("感电") != string::npos && (data->attack_config->condition->ele_type == "雷" || data->attack_config->condition->ele_type == "水")) ||
            (data->attack_config->react_type.find("激化") != string::npos && (data->attack_config->condition->ele_type == "雷" || data->attack_config->condition->ele_type == "草")) ||
            (data->attack_config->react_type.find("超导") != string::npos && (data->attack_config->condition->ele_type == "雷" || data->attack_config->condition->ele_type == "冰")))
            data->add_percentage("伤害加成", (0.1 * (0.75 + level * 0.25)), (name + "_extra_special"));
    }
    else if (name == "万国诸海图谱")
    {
        if (data->attack_config->react_type != "NONE")
            if (data->attack_config->condition->ele_type != "物理")
                data->add_percentage("伤害加成", (0.16 * (0.75 + level * 0.25)), (name + "_extra_special"));
    }
    else if (name == "匣里日月")
    {
        if (!data->attack_config->background)
        {
            if (data->attack_config->condition->attack_way == "平A" || data->attack_config->condition->attack_way == "E" || data->attack_config->condition->attack_way == "Q")
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
        if (data->attack_config->react_type != "NONE")
        {
            data->add_percentage("元素精通", (5 * (21.0 + level * 3.0)), (name + "_extra_special"));//元素反应，5层
            data->add_percentage("攻击力", -0.25, (name + "_extra_special"));//元素反应，5层
        }
    }
    else if (name == "嘟嘟可故事集")
    {
        if (!data->attack_config->background)
        {
            if (data->attack_config->condition->attack_way == "重A")
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
        if (data->attack_config->condition->attack_way == "平A")
        {
            if (data->c_point->args->bow_feilei_level == 1) data->add_percentage("伤害加成", (0.12 * (0.75 + level * 0.25)), (name + "_extra_special"));
            else if (data->c_point->args->bow_feilei_level == 2) data->add_percentage("伤害加成", (0.24 * (0.75 + level * 0.25)), (name + "_extra_special"));
            else if (data->c_point->args->bow_feilei_level == 3) data->add_percentage("伤害加成", (0.40 * (0.75 + level * 0.25)), (name + "_extra_special"));
        }
    }
    else if (name == "暗巷猎手")
    {
        if (data->attack_config->background)
            data->add_percentage("伤害加成", 0.2 * (0.75 + level * 0.25), (name + "_extra_special"));
    }
    else if (name == "曚云之月")
    {
        if (data->attack_config->condition->attack_way == "Q")
            data->add_percentage("伤害加成",
                                 min(334.0, data->c_point->Q_energy + data->team_config->teammate_1->Q_energy + data->team_config->teammate_2->Q_energy + data->team_config->teammate_3->Q_energy)
                                 * 0.0012 * (0.75 + level * 0.25), (name + "_extra_special"));
    }
    else if (name == "弓藏")
    {
        if (data->attack_config->condition->attack_way == "重A")
            data->add_percentage("伤害加成", -0.1, (name + "_extra_special"));
    }
    else if (name == "幽夜华尔兹")
    {
        if (!data->attack_config->background)
        {
            if (data->attack_config->condition->attack_way == "平A" || data->attack_config->condition->attack_way == "E")
                data->add_percentage("伤害加成", (0.2 * (0.75 + level * 0.25)), (name + "_extra_special"));
        }
    }
    else if (name == "破魔之弓")
    {
        if (data->attack_config->condition->attack_way == "平A")
            data->add_percentage("伤害加成", ((data->c_point->args->bow_pomo_fullenergy ? 2 : 1) * 0.16 * (0.75 + level * 0.25)), (name + "_extra_special"));
        if (data->attack_config->condition->attack_way == "重A")
            data->add_percentage("伤害加成", ((data->c_point->args->bow_pomo_fullenergy ? 2 : 1) * 0.12 * (0.75 + level * 0.25)), (name + "_extra_special"));
    }
    else if (name == "王下近侍")
    {
        if (!data->attack_config->background)
            data->add_percentage("元素精通", (40.0 + level * 20.0), (name + "_extra_special"));//EQ后
    }
    else if (name == "钢轮弓")
    {
//        if (data->c_point->args->accumulate_attacks)
        if (!data->attack_config->background)
            data->add_percentage("攻击力", (0.04 * 4 * (0.75 + level * 0.25)), (name + "_extra_special"));
    }

    else if (name == "无工之剑")
    {
//        if (data->c_point->args->accumulate_attacks)
        if (!data->attack_config->background)
        {
            data->add_percentage("攻击力", (0.04 * 5 * (0.75 + level * 0.25)), (name + "_extra_special"));
            if (data->c_point->args->shield_sustain || data->team_config->teammate_1->args->shield_sustain || data->team_config->teammate_2->args->shield_sustain || data->team_config->teammate_3->args->shield_sustain)
                data->add_percentage("攻击力", (0.04 * 5 * (0.75 + level * 0.25)), (name + "_extra_special"));
        }
    }
    else if (name == "森林王器")
    {
        if (data->attack_config->react_type.find("燃烧") != string::npos || data->attack_config->react_type.find("激化") != string::npos || data->attack_config->react_type.find("绽放") != string::npos)
            data->add_percentage("元素精通", (60.0 * (0.75 + level * 0.25)), (name + "_extra_special"));
    }
    else if (name == "螭骨剑")
    {
        data->add_percentage("伤害加成", (0.25 + level * 0.05), (name + "_extra_special"));
    }
    else if (name == "白影剑")
    {
//        if (data->c_point->args->accumulate_attacks)
        if (!data->attack_config->background)
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
        if (data->attack_config->condition->attack_way == "Q")
            data->add_percentage("伤害加成",
                                 min(334.0, data->c_point->Q_energy + data->team_config->teammate_1->Q_energy + data->team_config->teammate_2->Q_energy + data->team_config->teammate_3->Q_energy)
                                 * 0.0012 * (0.75 + level * 0.25), (name + "_extra_special"));
    }
    else if (name == "钟剑")
    {
        if (data->c_point->args->shield_sustain || data->team_config->teammate_1->args->shield_sustain || data->team_config->teammate_2->args->shield_sustain || data->team_config->teammate_3->args->shield_sustain)
            data->add_percentage("伤害加成", (0.12 * (0.75 + level * 0.25)), (name + "_extra_special"));

    }
    else if (name == "雨裁")
    {
        if (data->team_config->ele_attach_type.find("水") != string::npos || data->team_config->ele_attach_type.find("雷") != string::npos)
            data->add_percentage("伤害加成", (0.16 + level * 0.04), (name + "_extra_special"));
    }

    else if (name == "息灾")
    {
        data->add_percentage("攻击力", (0.192 * (0.75 + level * 0.25)), (name + "_extra_special"));
        if (data->attack_config->background && !data->attack_config->lockface)
            data->add_percentage("攻击力", (0.192 * (0.75 + level * 0.25)), (name + "_extra_special"));
    }
    else if (name == "和璞鸢")
    {
//        if (data->c_point->args->accumulate_attacks)
        if (!data->attack_config->background)
        {
            data->add_percentage("攻击力", (0.025 + level * 0.007) * 7, (name + "_extra_special"));//默认满层
            data->add_percentage("伤害加成", (0.75 + level * 0.25) * 0.12, (name + "_extra_special"));//默认满层
        }
    }
    else if (name == "贯虹之槊")
    {
//        if (data->c_point->args->accumulate_attacks)
        if (!data->attack_config->background)
        {
            data->add_percentage("攻击力", (0.04 * 5 * (0.75 + level * 0.25)), (name + "_extra_special"));
            if (data->c_point->args->shield_sustain || data->team_config->teammate_1->args->shield_sustain || data->team_config->teammate_2->args->shield_sustain || data->team_config->teammate_3->args->shield_sustain)
                data->add_percentage("攻击力", (0.04 * 5 * (0.75 + level * 0.25)), (name + "_extra_special"));
        }
    }
    else if (name == "薙草之稻光")
    {
        data->add_percentage("元素充能效率", (0.25 + level * 0.05), (name + "_extra_special"));//Q后
    }
    else if (name == "断浪长鳍")
    {
        if (data->attack_config->condition->attack_way == "Q")
            data->add_percentage("伤害加成",
                                 min(334.0, data->c_point->Q_energy + data->team_config->teammate_1->Q_energy + data->team_config->teammate_2->Q_energy + data->team_config->teammate_3->Q_energy)
                                 * 0.0012 * (0.75 + level * 0.25), (name + "_extra_special"));
    }
    else if (name == "千岩长枪")
    {
        data->add_percentage("攻击力", (0.24 + level * 0.04), (name + "_extra_special"));//默认3层
        data->add_percentage("暴击率", (0.08 + level * 0.04), (name + "_extra_special"));//默认3层
    }
    else if (name == "贯月矢")
    {
        if (data->attack_config->react_type.find("燃烧") != string::npos || data->attack_config->react_type.find("激化") != string::npos || data->attack_config->react_type.find("绽放") != string::npos)
            data->add_percentage("攻击力", (0.16 * (0.75 + level * 0.25)), (name + "_extra_special"));
    }
    else if (name == "试作星镰")
    {
        if (data->c_point->args->polearm_shizuo_level == 2)
        {
            if (data->attack_config->condition->attack_way == "平A" || data->attack_config->condition->attack_way == "重A")
                data->add_percentage("伤害加成", (0.08 * 2 * (0.75 + level * 0.25)), (name + "_extra_special"));
        }
        else if (data->c_point->args->polearm_shizuo_level == 1)
        {
            if (data->attack_config->condition->attack_way == "平A" || data->attack_config->condition->attack_way == "重A")
                data->add_percentage("伤害加成", (0.08 * (0.75 + level * 0.25)), (name + "_extra_special"));
        }
    }
    else if (name == "风信之锋")
    {
        if (data->attack_config->react_type != "NONE")
        {
            data->add_percentage("攻击力", (0.12 * (0.75 + level * 0.25)), (name + "_extra_special"));
            data->add_percentage("元素精通", (48.0 * (0.75 + level * 0.25)), (name + "_extra_special"));
        }
    }
    else if (name == "匣里灭辰")
    {
        if (data->team_config->ele_attach_type.find("水") != string::npos || data->team_config->ele_attach_type.find("火") != string::npos)
            data->add_percentage("伤害加成", (0.16 + level * 0.04), (name + "_extra_special"));
    }
    return true;
}

//build new weapon(all) 被转化的属性有效
//考虑Q充能
void Weapon::modify_useful_attribute(Deployment *data)
{
    if (data->w_point->name == "磐岩结绿" && data->data_list[1]->useful) data->data_list[0]->useful = true;
    else if (data->w_point->name == "圣显之钥" && data->c_point->args->sword_shengxian_level > 0 && data->data_list[4]->useful) data->data_list[0]->useful = true;
    else if (data->w_point->name == "辰砂之纺锤" && data->attack_config->condition->attack_way == "E") data->data_list[2]->useful = true;
    else if (data->w_point->name == "西福斯的月光" && (data->data_list[5]->useful || (cal_enable_recharge_num && data->attack_config->condition->attack_way == "Q"))) data->data_list[4]->useful = true;
    else if (data->w_point->name == "不灭月华" && data->attack_config->condition->attack_way == "平A") data->data_list[0]->useful = true;
    else if (data->w_point->name == "流浪的晚星" && data->data_list[1]->useful) data->data_list[4]->useful = true;
    else if (data->w_point->name == "猎人之径" && data->attack_config->condition->attack_way == "重A") data->data_list[4]->useful = true;
    else if (data->w_point->name == "赤角石溃杵" && (data->attack_config->condition->attack_way == "平A" || data->attack_config->condition->attack_way == "重A"))
        data->data_list[2]->useful = true;
    else if (data->w_point->name == "玛海菈的水色" && data->data_list[1]->useful) data->data_list[4]->useful = true;
    else if (data->w_point->name == "护摩之杖" && data->data_list[1]->useful) data->data_list[0]->useful = true;
    else if (data->w_point->name == "薙草之稻光" && data->data_list[1]->useful) data->data_list[5]->useful = true;
    else if (data->w_point->name == "赤沙之杖" && data->data_list[1]->useful) data->data_list[4]->useful = true;
}

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
        if (data->team_config->ele_attach_type.find("冰") != string::npos)//冰附着
            data->add_percentage("暴击率", 0.2, (name + "_extra_special"));
        if (data->attack_config->react_type.find("冻结") != string::npos)
            data->add_percentage("暴击率", 0.2, (name + "_extra_special"));//冻结
    }
    else if (if_4_piece && name == "逆飞的流星")
    {
        if (data->c_point->args->shield_sustain || data->team_config->teammate_1->args->shield_sustain || data->team_config->teammate_2->args->shield_sustain || data->team_config->teammate_3->args->shield_sustain)
            if (data->attack_config->condition->attack_way == "平A" || data->attack_config->condition->attack_way == "重A")
                data->add_percentage("伤害加成", 0.4, (name + "_extra_special"));
    }
    else if (if_4_piece && name == "苍白之火")
    {
        if (data->c_point->args->cangbai_level == 2)
        {
            data->add_percentage("攻击力", 0.18, name + "_extra_special");
            if (data->attack_config->condition->ele_type == "物理") data->add_percentage("伤害加成", 0.25, (name + "_extra_special"));
        }
        else if (data->c_point->args->cangbai_level == 1)
        {
            data->add_percentage("攻击力", 0.09, name + "_extra_special");
        }
    }
    else if (if_4_piece && name == "千岩牢固")
    {
        if (data->c_point->args->sustain_E_hit)
        {
            data->add_percentage("攻击力", 0.2, (name + "_extra_special"));
            data->add_percentage("护盾强效", 0.3, (name + "_extra_special"));
        }
    }
    else if (if_4_piece && name == "炽烈的炎之魔女")
    {
        if (data->attack_config->condition->ele_type == "火")
        {
            if (data->c_point->args->monv_level == 3)
            {
                if (data->attack_config->condition->attack_way != "E") data->add_percentage("伤害加成", 0.225, (name + "_extra_special"));
                else data->add_percentage("伤害加成", 0.075, (name + "_extra_special"));
            }
            else if (data->c_point->args->monv_level == 2)
            {
                if (data->attack_config->condition->attack_way != "E") data->add_percentage("伤害加成", 0.15, (name + "_extra_special"));
                else data->add_percentage("伤害加成", 0.075 / 2, (name + "_extra_special"));
            }
            else if (data->c_point->args->monv_level == 1)
            {
                if (data->attack_config->condition->attack_way != "E") data->add_percentage("伤害加成", 0.075, (name + "_extra_special"));
            }
        }
    }
    else if (if_4_piece && name == "渡过烈火的贤人")
    {
        if (data->team_config->ele_attach_type.find("火") != string::npos)//火附着
            data->add_percentage("伤害加成", 0.35, (name + "_extra_special"));
    }
    else if (if_4_piece && name == "平息鸣雷的尊者")
    {
        if (data->team_config->ele_attach_type.find("雷") != string::npos)//雷附着
            data->add_percentage("伤害加成", 0.35, (name + "_extra_special"));
    }
    else if (if_4_piece && name == "华馆梦醒形骸记")
    {
        if (data->attack_config->background || (!data->attack_config->background && data->c_point->ele_type == "岩"))
        {
            data->add_percentage("防御力", 0.24, (name + "_extra_special"));
            if (data->attack_config->condition->ele_type == "岩") data->add_percentage("伤害加成", 0.24, (name + "_extra_special"));
        }
    }
    else if (if_4_piece && name == "辰砂往生录")
    {
        if (data->c_point->args->Q_damage_self)
            data->add_percentage("攻击力", 0.48, (name + "_extra_special"));
    }
    else if (if_4_piece && name == "深林的记忆")
    {
        if (data->c_point->args->shenlin_enable)
            if (data->attack_config->condition->ele_type == "草")
                data->add_percentage("抗性削弱", 0.3, (name + "_extra_special"));
    }
    else if (if_4_piece && name == "饰金之梦")
    {
        if (data->attack_config->react_type != "NONE")
        {
            int same = 0;
            int diff = 0;
            if (data->team_config->teammate_1->ele_type == data->c_point->ele_type) same += 1;
            else diff += 1;
            if (data->team_config->teammate_2->ele_type == data->c_point->ele_type) same += 1;
            else diff += 1;
            if (data->team_config->teammate_3->ele_type == data->c_point->ele_type) same += 1;
            else diff += 1;
            data->add_percentage("攻击力", 0.14 * same, (name + "_extra_special"));
            data->add_percentage("元素精通", 50.0 * diff, (name + "_extra_special"));
        }
    }
    return true;
}

//build new artifact(all) 被转化的属性有效
//考虑Q充能
void Artifact::modify_useful_attribute(Deployment *data)
{
    if (data->suit1->name == "绝缘之旗印" && data->suit2->name == "绝缘之旗印" && data->attack_config->condition->attack_way == "Q")
        data->data_list[5]->useful = true;
}

//build new artifact(all) 提供充能、队友加成的有效
//考虑Q充能
void Artifact::check_artifact_special(Deployment *data, bool &suit1_valid, bool &suit2_valid, bool if_4_piece)
{
    //特殊判断圣遗物套装，原来肯定-现在肯定；原来否定-现在肯定；原来肯定-现在否定；原来否定-现在否定
    if (if_4_piece)
    {
        if (data->suit1->name == "悠古的磐岩")
        {
            if (data->c_point->ele_type == "岩")
                suit1_valid = suit2_valid = true;//原来肯定-现在肯定；原来否定-现在肯定；
        }
        else if (data->suit1->name == "翠绿之影")
        {
            if (data->c_point->ele_type == "风")
                suit1_valid = suit2_valid = true;//原来肯定-现在肯定；原来否定-现在肯定；
        }
        else if (data->suit1->name == "如雷的盛怒")
        {
            if (data->c_point->ele_type == "雷")
                suit1_valid = suit2_valid = true;//原来肯定-现在肯定；原来否定-现在肯定；
        }
        else if (data->suit1->name == "绝缘之旗印")
        {
            if (data->attack_config->condition->attack_way == "Q")
                suit1_valid = suit2_valid = true;//原来肯定-现在肯定；原来否定-现在肯定；
        }
        else if (data->suit1->name == "海染砗磲" || data->suit1->name == "被怜爱的少女")
        {
            if (data->c_point->args->heal_sustain)
                suit1_valid = suit2_valid = true;//原来肯定-现在肯定；原来否定-现在肯定；
        }

            //team
        else if (data->suit1->name == "昔日宗室之仪")
        {
            suit1_valid = suit2_valid = true;//原来肯定-现在肯定；原来否定-现在肯定；
        }
        else if (data->suit1->name == "千岩牢固")
        {
            if (data->c_point->args->sustain_E_hit)
                suit1_valid = suit2_valid = true;//原来肯定-现在肯定；原来否定-现在肯定；
        }
        else if (data->suit1->name == "深林的记忆")
        {
            if (data->c_point->args->shenlin_enable)
                suit1_valid = suit2_valid = true;//原来肯定-现在肯定；原来否定-现在肯定；
        }

        //recharge
    }
    else
    {
        //2+2
        //只允许 角斗+其他/追忆 染血+其他/苍白 少女+其他/海染 乐团+其他/饰金
        //suit1
        if ((data->suit1->name == "追忆之注连" && data->suit2->name != "角斗士的终幕礼") || data->suit1->name == "辰砂往生录" || data->suit1->name == "来歆余响") suit1_valid = false;//原来肯定-现在否定；原来否定-现在否定
        if (data->suit1->name == "苍白之火" && data->suit2->name != "染血的骑士道") suit1_valid = false;//原来肯定-现在否定；原来否定-现在否定
        if (data->suit1->name == "海染砗磲" && data->suit2->name != "被怜爱的少女") suit1_valid = false;//原来肯定-现在否定；原来否定-现在否定
        if (data->suit1->name == "饰金之梦" && data->suit2->name != "流浪大地的乐团") suit1_valid = false;//原来肯定-现在否定；原来否定-现在否定
        //suit2
        if ((data->suit2->name == "追忆之注连" && data->suit1->name != "角斗士的终幕礼") || data->suit2->name == "辰砂往生录" || data->suit2->name == "来歆余响") suit2_valid = false;//原来肯定-现在否定；原来否定-现在否定
        if (data->suit2->name == "苍白之火" && data->suit1->name != "染血的骑士道") suit2_valid = false;//原来肯定-现在否定；原来否定-现在否定
        if (data->suit2->name == "海染砗磲" && data->suit1->name != "被怜爱的少女") suit2_valid = false;//原来肯定-现在否定；原来否定-现在否定
        if (data->suit2->name == "饰金之梦" && data->suit1->name != "流浪大地的乐团") suit2_valid = false;//原来肯定-现在否定；原来否定-现在否定
    }
}

//build new character(needed)||build new weapon(all)||build new artifact(all) 所有转化类属性的有效百分比决定开关
//考虑Q充能
void Deployment::check_useful_attribute()
{
    //实际上应该采用（武器-圣遗物）对的形式考虑还原useful
    //如果useful相对于config没有改变，那不影响；如果useful相对与config改变了，一定是转化类的属性新增了有效词条，那要检查是否值得新增
    //convert:如果转化后的词条值还不如直接堆转化后的属性，那么还原
    //extrarate:1、base_skillrate=0 默认有效
    //          2、base_skillrate!=0 如果攻击词条（一定有效）的收益更大，那么还原
    if (w_point->name == "磐岩结绿")
    {
        if ((data_list[0]->value_per_entry * 0.012 * (0.75 + w_point->level * 0.25) * base_life / base_atk) < data_list[1]->value_per_entry)
            data_list[0]->useful = attack_config->useful_attributes[0];
    }
    else if (w_point->name == "圣显之钥")
    {
        if ((data_list[0]->value_per_entry * 0.0056 * (0.75 + w_point->level * 0.25) * base_life) < data_list[4]->value_per_entry)
            data_list[0]->useful = attack_config->useful_attributes[0];
    }
    else if (w_point->name == "辰砂之纺锤")
    {
        if (base_skillrate != 0)
            if ((data_list[2]->value_per_entry * 0.4 * (0.75 + w_point->level * 0.25) * base_def / (base_skillrate * base_atk)) < data_list[1]->value_per_entry)
                data_list[2]->useful = attack_config->useful_attributes[2];
    }
    else if (w_point->name == "西福斯的月光")
    {
        if ((data_list[4]->value_per_entry * 0.00036 * (0.75 + w_point->level * 0.25)) < data_list[5]->value_per_entry)
            data_list[4]->useful = attack_config->useful_attributes[4];
    }
    else if (w_point->name == "不灭月华")
    {
        if (base_skillrate != 0)
            if ((data_list[0]->value_per_entry * (0.005 + w_point->level * 0.005) * base_life / (base_skillrate * base_atk)) < data_list[1]->value_per_entry)
                data_list[0]->useful = attack_config->useful_attributes[0];
    }
    else if (w_point->name == "流浪的晚星")
    {
        if ((data_list[4]->value_per_entry * 0.24 * (0.75 + w_point->level * 0.25) / base_atk) < data_list[1]->value_per_entry)
            data_list[4]->useful = attack_config->useful_attributes[4];
    }
    else if (w_point->name == "猎人之径")
    {
        if (base_skillrate != 0)
            if ((data_list[4]->value_per_entry * 1.6 * (0.75 + w_point->level * 0.25) / (base_skillrate * base_atk)) < data_list[1]->value_per_entry)
                data_list[4]->useful = attack_config->useful_attributes[4];
    }
    else if (w_point->name == "赤角石溃杵")
    {
        if (base_skillrate != 0)
            if ((data_list[2]->value_per_entry * 0.4 * (0.75 + w_point->level * 0.25) * base_def / (base_skillrate * base_atk)) < data_list[1]->value_per_entry)
                data_list[2]->useful = attack_config->useful_attributes[2];
    }
    else if (w_point->name == "玛海菈的水色")
    {
        if ((data_list[4]->value_per_entry * 0.24 * (0.75 + w_point->level * 0.25) / base_atk) < data_list[1]->value_per_entry)
            data_list[4]->useful = attack_config->useful_attributes[4];
    }
    else if (w_point->name == "护摩之杖")
    {
        if ((data_list[0]->value_per_entry * (0.008 * (0.75 + w_point->level * 0.25) + (0.008 + w_point->level * 0.002)) * base_life / base_atk) < data_list[1]->value_per_entry)
            data_list[0]->useful = attack_config->useful_attributes[0];
    }
    else if (w_point->name == "薙草之稻光")
    {
        if ((data_list[5]->value_per_entry * 0.28 * (0.75 + w_point->level * 0.25)) < data_list[1]->value_per_entry)
            data_list[5]->useful = attack_config->useful_attributes[5];
    }
    else if (w_point->name == "赤沙之杖")
    {
        if ((data_list[4]->value_per_entry * 1.36 * (0.75 + w_point->level * 0.25) / base_atk) < data_list[1]->value_per_entry)
            data_list[4]->useful = attack_config->useful_attributes[4];
    }

    //artifact
    //绝缘肯定有效（增伤效益认为无穷）
    if (suit1->name == "绝缘之旗印" && suit2->name == "绝缘之旗印" && attack_config->condition->attack_way == "Q")
        data_list[5]->useful = true;

    //character
    if (c_point->name == "胡桃") data_list[1]->useful = false;//生命>攻击，除非有攻击转什么
    else if (c_point->name == "钟离") data_list[1]->useful = false;//要考虑盾
}

//build new character(needed)||build new weapon(all)||build new artifact(all)
void Deployment::get_team_data()
{
    //默认队友5星0命，技能均为9级，4星6命，技能均为12级

    //weapon
    if ((team_config->team_weapon_artifact.find("终末嗟叹之诗") != string::npos || team_config->team_weapon_artifact.find("苍古自由之誓") != string::npos || team_config->team_weapon_artifact.find("松籁响起之时") != string::npos)
        && (w_point->name != "终末嗟叹之诗" && w_point->name != "苍古自由之誓" && w_point->name != "松籁响起之时"))
    {
        add_percentage("攻击力", 0.2, "team_大乐章");
    }
    if (team_config->team_weapon_artifact.find("终末嗟叹之诗") != string::npos && w_point->name != "终末嗟叹之诗")
    {
        add_percentage("元素精通", 100.0, "team_终末嗟叹之诗");
    }
    if (team_config->team_weapon_artifact.find("苍古自由之誓") != string::npos && w_point->name != "苍古自由之誓")
    {
        if (attack_config->condition->attack_way == "平A" || attack_config->condition->attack_way == "重A" || attack_config->condition->attack_way == "下落A")
            add_percentage("伤害加成", 0.16, "team_苍古自由之誓");
    }
    if (team_config->team_weapon_artifact.find("白辰之环") != string::npos && w_point->name != "白辰之环")
    {
        if ((attack_config->react_type.find("超载") != string::npos && (attack_config->condition->ele_type == "雷" || attack_config->condition->ele_type == "火")) ||
            (attack_config->react_type.find("感电") != string::npos && (attack_config->condition->ele_type == "雷" || attack_config->condition->ele_type == "水")) ||
            (attack_config->react_type.find("激化") != string::npos && (attack_config->condition->ele_type == "雷" || attack_config->condition->ele_type == "草")) ||
            (attack_config->react_type.find("超导") != string::npos && (attack_config->condition->ele_type == "雷" || attack_config->condition->ele_type == "冰")))
            add_percentage("伤害加成", 0.2, "team_白辰之环");
    }
    if (team_config->team_weapon_artifact.find("讨龙") != string::npos)
    {
        add_percentage("攻击力", 0.48, "team_讨龙");
    }
    if ((team_config->team_weapon_artifact.find("原木刀") != string::npos || team_config->team_weapon_artifact.find("森林王器") != string::npos) && (w_point->name != "原木刀" && w_point->name != "森林王器"))
    {
        add_percentage("元素精通", 120, "team_原木刀/森林王器");
    }
    if (team_config->team_weapon_artifact.find("贯月矢") != string::npos && w_point->name != "贯月矢")
    {
        add_percentage("攻击力", 0.32, "team_贯月矢");
    }
    //TODO:NEW
    if (team_config->team_weapon_artifact.find("千夜浮梦") != string::npos)
    {
        add_percentage("元素精通", 40.0, "team_千夜浮梦");
    }
    //转化类
    if (team_config->team_weapon_artifact.find("圣显之钥") != string::npos && w_point->name != "圣显之钥")
    {
        add_converted_percentage("元素精通", 100.0, "team_圣显之钥");
    }
    if (team_config->team_weapon_artifact.find("西福斯的月光") != string::npos)
    {
        add_converted_percentage("元素充能效率", 0.054, "team_西福斯的月光");
    }
    if (team_config->team_weapon_artifact.find("流浪的晚星") != string::npos)
    {
        add_converted_percentage("攻击力", 36.0 / base_atk, "team_流浪的晚星");
    }
    if (team_config->team_weapon_artifact.find("玛海菈的水色") != string::npos)
    {
        add_converted_percentage("攻击力", 36.0 / base_atk, "team_玛海菈的水色");
    }

    //artifact
    if (team_config->team_weapon_artifact.find("悠古的磐岩") != string::npos && (suit1->name != "悠古的磐岩" || suit2->name != "悠古的磐岩"))
    {
        if (team_config->ele_allow_spread.find(attack_config->condition->ele_type) != string::npos)//扩散与元素附着
            add_percentage("伤害加成", 0.35, "team_悠古的磐岩");
    }
    if (team_config->team_weapon_artifact.find("昔日宗室之仪") != string::npos && (suit1->name != "昔日宗室之仪" || suit2->name != "昔日宗室之仪"))
    {
        add_percentage("攻击力", 0.2, "team_昔日宗室之仪");
    }
    if (team_config->team_weapon_artifact.find("翠绿之影") != string::npos && (suit1->name != "翠绿之影" || suit2->name != "翠绿之影"))
    {
        if (attack_config->condition->ele_type == "水" || attack_config->condition->ele_type == "火" || attack_config->condition->ele_type == "雷" || attack_config->condition->ele_type == "冰")
            if (team_config->ele_allow_spread.find(attack_config->condition->ele_type) != string::npos)//扩散与元素附着
                add_percentage("抗性削弱", 0.4, "team_翠绿之影");
    }
    if (team_config->team_weapon_artifact.find("千岩牢固") != string::npos && (suit1->name != "千岩牢固" || suit2->name != "千岩牢固"))
    {
        add_percentage("攻击力", 0.2, "team_千岩牢固");
        add_percentage("护盾强效", 0.3, "team_千岩牢固");
    }
    if (team_config->team_weapon_artifact.find("深林的记忆") != string::npos && (suit1->name != "深林的记忆" || suit2->name != "深林的记忆"))
    {
        if (attack_config->condition->ele_type == "草")
            add_percentage("抗性削弱", 0.3, "team_深林的记忆");
    }

    int Cryo_num = 0;//15%CR
    int Pyro_num = 0;//25%ATK
    int Hydro_num = 0;//25%LIFE
    int Anemo_num = 0;//5%CD
    int Electro_num = 0;//partical
    int Geo_num = 0;//15% shield 15%DAM 20%Geo RIS
    int Dendro_num = 0;//50EM 绽放激化燃烧30EM 二层绽放激化20EM

    if (team_config->teammate_all.find("胡桃") != string::npos)
    {
        //talent E后
//        add_percentage("暴击率", 0.12, "team_胡桃");
        //constellation>=4 击败
//        add_percentage("暴击率", 0.12, "team_胡桃");
        Pyro_num++;
    }
    if (team_config->teammate_all.find("神里绫华") != string::npos)
    {
        //constellation>=4 Q
//        add_percentage("防御削弱", 0.3, "team_神里绫华");
        Cryo_num++;
    }
    if (team_config->teammate_all.find("雷电将军") != string::npos)
    {
        //E
        if (attack_config->condition->attack_way == "Q")
            add_percentage("伤害加成", 0.003 * c_point->Q_energy, "team_雷电将军");
        //constellation>=4 Q后
//        add_percentage("攻击力", 0.3, "team_雷电将军");
        Electro_num++;
    }
    if (team_config->teammate_all.find("甘雨") != string::npos)
    {
        //talent Q内
        if (attack_config->condition->ele_type == "冰" && (!(attack_config->background && !attack_config->lockface)))
            add_percentage("伤害加成", 0.2, "team_甘雨");
        //constellation>=1 重A后
//        if (config->condition->ele_type == "冰")
//            add_percentage("抗性削弱", 0.15, "team_甘雨");
        //constellation>=4 Q内
//        add_percentage("伤害加成", 0.15, "team_甘雨");
        Cryo_num++;
    }
    if (team_config->teammate_all.find("夜兰") != string::npos)
    {
        //talent Q后
        if (!attack_config->background)
            add_percentage("伤害加成", 0.3, "team_夜兰");
        //constellation>=4 E后
//        add_percentage("生命值", 0.4, "team_夜兰");
        Hydro_num++;
    }
    if (team_config->teammate_all.find("行秋") != string::npos)
    {
        //constellation>=2
        if (attack_config->condition->ele_type == "水")
            add_percentage("抗性削弱", 0.15, "team_行秋");
        Hydro_num++;
    }
    if (team_config->teammate_all.find("香菱") != string::npos)
    {
        //constellation>=1 E命中
        if (attack_config->condition->ele_type == "火")
            add_percentage("抗性削弱", 0.15, "team_香菱");
        //constellation>=6 Q内
        if (attack_config->condition->ele_type == "火")
            add_percentage("伤害加成", 0.15, "team_香菱");
        Pyro_num++;
    }
    if (team_config->teammate_all.find("八重神子") != string::npos)
    {
        //constellation>=4 E命中
//        if (config->condition->ele_type == "雷")
//            add_percentage("伤害加成", 0.2, "team_八重神子");
        Electro_num++;
    }
    if (team_config->teammate_all.find("枫原万叶") != string::npos)
    {
        //talent 扩散
        //转化类
        if (attack_config->condition->ele_type == "水" || attack_config->condition->ele_type == "火" || attack_config->condition->ele_type == "雷" || attack_config->condition->ele_type == "冰")
            if (team_config->ele_allow_spread.find(attack_config->condition->ele_type) != string::npos)//扩散与元素附着
                add_converted_percentage("伤害加成", 0.36, "team_枫原万叶");
        //constellation>=2 Q后
//        if (!(config->background && !config->lockface))
//            add_percentage("元素精通", 200.0, "team_枫原万叶");
        Anemo_num++;
    }
    if (team_config->teammate_all.find("班尼特") != string::npos)
    {
        //Q
        if (!(attack_config->background && !attack_config->lockface))
            add_percentage("攻击力", (191 + 608) * (1.12 + 0.2) / base_atk, "team_班尼特");
        //constellation>=6 Q内
//        if (config->condition->ele_type == "火" && if (!(c_point->args->background && !c_point->args->lockface)))
//            add_percentage("伤害加成", 0.15, "team_班尼特");
        Pyro_num++;
    }
    if (team_config->teammate_all.find("温迪") != string::npos)
    {
        //constellation>=2 E命中
//        if (config->condition->ele_type == "风" || config->condition->ele_type == "物理")
//            add_percentage("抗性削弱", 0.24, "team_温迪");
        //constellation>=6 Q
//        if (config->condition->ele_type == "风") add_percentage("抗性削弱", 0.2, "team_温迪");
//        if (config->condition->ele_type == "水" || config->condition->ele_type == "火" || config->condition->ele_type == "雷" || config->condition->ele_type == "冰")
//            if (team_config->ele_allow_spread.find(attack_config->condition->ele_type) != string::npos)//扩散与元素附着
//                add_percentage("抗性削弱", 0.2, "team_温迪");
        Anemo_num++;
    }
    if (team_config->teammate_all.find("莫娜") != string::npos)
    {
        //Q
        add_percentage("伤害加成", 0.58, "team_莫娜");
        //constellation>=4 Q
//        add_percentage("暴击率", 0.15, "team_莫娜");
        Hydro_num++;
    }
    if (team_config->teammate_all.find("钟离") != string::npos)
    {
        //E
        add_percentage("抗性削弱", 0.2, "team_钟离");
        //talent E
        add_percentage("护盾强效", 0.25, "team_钟离");
        Geo_num++;
    }
    //TODO:NEW
    if (team_config->teammate_all.find("纳西妲") != string::npos)
    {
        //talent Q后
        //转化类
        if (!(attack_config->background && !attack_config->lockface))
            add_converted_percentage("元素精通", 250, "team_纳西妲");
        //constellation>=2 激化后
        if (attack_config->react_type.find("激化") != string::npos)
            add_percentage("防御削弱", 0.3, "team_纳西妲");
        Dendro_num++;
    }

    if (team_config->teammate_1->name == "冰_test") Cryo_num++;
    else if (team_config->teammate_1->name == "火_test") Pyro_num++;
    else if (team_config->teammate_1->name == "水_test") Hydro_num++;
    else if (team_config->teammate_1->name == "风_test") Anemo_num++;
    else if (team_config->teammate_1->name == "雷_test") Electro_num++;
    else if (team_config->teammate_1->name == "岩_test") Geo_num++;
    else if (team_config->teammate_1->name == "草_test") Dendro_num++;
    if (team_config->teammate_2->name == "冰_test") Cryo_num++;
    else if (team_config->teammate_2->name == "火_test") Pyro_num++;
    else if (team_config->teammate_2->name == "水_test") Hydro_num++;
    else if (team_config->teammate_2->name == "风_test") Anemo_num++;
    else if (team_config->teammate_2->name == "雷_test") Electro_num++;
    else if (team_config->teammate_2->name == "岩_test") Geo_num++;
    else if (team_config->teammate_2->name == "草_test") Dendro_num++;
    if (team_config->teammate_3->name == "冰_test") Cryo_num++;
    else if (team_config->teammate_3->name == "火_test") Pyro_num++;
    else if (team_config->teammate_3->name == "水_test") Hydro_num++;
    else if (team_config->teammate_3->name == "风_test") Anemo_num++;
    else if (team_config->teammate_3->name == "雷_test") Electro_num++;
    else if (team_config->teammate_3->name == "岩_test") Geo_num++;
    else if (team_config->teammate_3->name == "草_test") Dendro_num++;

    if (this->c_point->ele_type == "冰") Cryo_num++;
    else if (this->c_point->ele_type == "火") Pyro_num++;
    else if (this->c_point->ele_type == "水") Hydro_num++;
    else if (this->c_point->ele_type == "风") Anemo_num++;
    else if (this->c_point->ele_type == "雷") Electro_num++;
    else if (this->c_point->ele_type == "岩") Geo_num++;
    else if (this->c_point->ele_type == "草") Dendro_num++;

    if (Cryo_num >= 2)
    {
        if (team_config->ele_attach_type.find("冰") != string::npos)//冰附着
            add_percentage("暴击率", 0.15, "team_Cryo");
    }
    if (Pyro_num >= 2) add_percentage("攻击力", 0.25, "team_Pyro");
    if (Hydro_num >= 2) add_percentage("生命值", 0.25, "team_Hydro");
    if (Geo_num >= 2)
    {
        add_percentage("护盾强效", 0.15, "team_Geo");
        if (c_point->args->shield_sustain || team_config->teammate_1->args->shield_sustain || team_config->teammate_2->args->shield_sustain || team_config->teammate_3->args->shield_sustain)
        {
            add_percentage("伤害加成", 0.15, "team_Geo");
            if (this->c_point->ele_type == "岩") add_percentage("抗性削弱", 0.2, "team_Geo");
        }
    }
    if (Dendro_num >= 2)
    {
        add_percentage("元素精通", 50.0, "team_Dendro");
        if (attack_config->react_type.find("燃烧") != string::npos || attack_config->react_type.find("绽放") != string::npos || attack_config->react_type.find("激化") != string::npos)
            add_percentage("元素精通", 30.0, "team_Dendro");
        if (attack_config->react_type.find("超绽放") != string::npos || attack_config->react_type.find("烈绽放") != string::npos || attack_config->react_type.find("蔓激化") != string::npos ||
            attack_config->react_type.find("超激化") != string::npos)
            add_percentage("元素精通", 20.0, "team_Dendro");
    }
}

//build new character(needed)||build new weapon(all)||build new artifact(all) 有关充能的转化类属性要考虑
void Deployment::satisfy_recharge_requirement()
{
    //调整充能数值
    if (cal_enable_recharge_num && attack_config->condition->attack_way == "Q")
    {
        //能量回复值 = 微粒数 * 系数 * 实时的元素充能效率
        double front = 1;
        double back = 0.6;
        double same = 3;
        double white = 2;
        double diff = 1;
        double monster_drop = 10;

        double Q_energy_modify = c_point->Q_energy;
        if (team_config->teammate_all.find("雷电将军") != string::npos) Q_energy_modify -= 24;
        //扩散与元素附着
        if (team_config->teammate_all.find("温迪") != string::npos && team_config->ele_allow_spread.find(attack_config->condition->ele_type) != string::npos) Q_energy_modify -= 15;

        double energy = monster_drop;
        energy += team_config->teammate_1->E_energy * back * ((team_config->teammate_1->ele_type == c_point->ele_type) ? same : diff) * ((double_E_per_round.find(team_config->teammate_1->name) != string::npos) ? 2 : 1);
        energy += team_config->teammate_2->E_energy * back * ((team_config->teammate_2->ele_type == c_point->ele_type) ? same : diff) * ((double_E_per_round.find(team_config->teammate_2->name) != string::npos) ? 2 : 1);
        energy += team_config->teammate_3->E_energy * back * ((team_config->teammate_3->ele_type == c_point->ele_type) ? same : diff) * ((double_E_per_round.find(team_config->teammate_3->name) != string::npos) ? 2 : 1);

        double converted_recharge = 0;

        //"天目影打刀" 12/E 不吃充能
        //"西风剑" 3*2/6s
        //"祭礼剑" 1E/16s
        //"西福斯的月光" 充能效率 mastery * 0.00036 * (0.75 + w_point->level * 0.25)

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
            energy += ((double_E_per_round.find(c_point->name) != string::npos) ? 2 : 1) * c_point->E_energy * front * same;

            if (w_point->name == "西风剑") energy += 3 * front * white;
            else if (w_point->name == "祭礼剑") energy += c_point->E_energy * front * same;
            else if (w_point->name == "天目影打刀") Q_energy_modify -= 12;
            else if (w_point->name == "西福斯的月光") converted_recharge += data_list[4]->percentage * 0.00036 * (0.75 + w_point->level * 0.25);
        }
        else if (c_point->name == "雷电将军")
        {
            //Q 90-24=66 E 10b 1E/Q
            Q_energy_modify -= 24;
            energy += ((double_E_per_round.find(c_point->name) != string::npos) ? 2 : 1) * c_point->E_energy * back * same;

            if (w_point->name == "西风长枪") energy += 3 * front * white;
            else if (w_point->name == "喜多院十文字") Q_energy_modify -= 12;

            energy = min(energy, Q_energy_modify / 2.4);
        }
        else if (c_point->name == "甘雨")
        {
            //Q 60 E 2f+2b 2E/Q
            //constellation>=1:每次重A命中恢复2点能量 constellation>=2:E变为2次
            energy += ((double_E_per_round.find(c_point->name) != string::npos) ? 2 : 1) * c_point->E_energy * (front / 2 + back / 2) * same;

            if (w_point->name == "西风猎弓") energy += 3 * front * white;
            else if (w_point->name == "祭礼弓") energy += c_point->E_energy * front * same;
        }
        else if (c_point->name == "夜兰")
        {
            //Q 70 E 4f 1E/Q
            energy += ((double_E_per_round.find(c_point->name) != string::npos) ? 2 : 1) * c_point->E_energy * front * same;
            //constellation>=1
//            energy += 2 * c_point->E_energy * front * same;

            if (w_point->name == "西风猎弓") energy += 3 * front * white;
            else if (w_point->name == "祭礼弓") energy += c_point->E_energy * front * same;
        }
        else if (c_point->name == "行秋")
        {
            //Q 80-3*5=65 E 5f 1E/Q
            Q_energy_modify -= 12;
            energy += ((double_E_per_round.find(c_point->name) != string::npos) ? 2 : 1) * c_point->E_energy * front * same;

            if (w_point->name == "西风剑") energy += 3 * front * white;
            else if (w_point->name == "祭礼剑") energy += c_point->E_energy * front * same;
            else if (w_point->name == "天目影打刀") Q_energy_modify -= 12;
            else if (w_point->name == "西福斯的月光") converted_recharge += data_list[4]->percentage * 0.00036 * (0.75 + w_point->level * 0.25);
        }
        else if (c_point->name == "香菱")
        {
            //Q 80 E 4b 1E/Q
            energy += ((double_E_per_round.find(c_point->name) != string::npos) ? 2 : 1) * c_point->E_energy * back * same;

            if (w_point->name == "西风长枪") energy += 3 * front * white;
            else if (w_point->name == "喜多院十文字") Q_energy_modify -= 12;
        }
        else if (c_point->name == "八重神子")
        {
            //Q 90-24=66 E 5b 1E/Q
            Q_energy_modify -= 24;
            energy += ((double_E_per_round.find(c_point->name) != string::npos) ? 2 : 1) * c_point->E_energy * back * same;

            if (w_point->name == "西风秘典") energy += 3 * front * white;
            else if (w_point->name == "祭礼残章") energy += 0;
            else if (w_point->name == "试作金珀") Q_energy_modify -= 18;
            //"不灭月华" 0.6/A Q后12s内 不吃充能
        }
        else if (c_point->name == "温迪")
        {
            //Q 60-15=45 E 3f 2E/Q
            Q_energy_modify -= 15;
            energy += ((double_E_per_round.find(c_point->name) != string::npos) ? 2 : 1) * c_point->E_energy * front * same;

            if (w_point->name == "西风猎弓") energy += 3 * front * white;
            else if (w_point->name == "祭礼弓") energy += c_point->E_energy * front * same;
        }
        else if (c_point->name == "莫娜")
        {
            //Q 60 E 3b 1E/Q
            energy += ((double_E_per_round.find(c_point->name) != string::npos) ? 2 : 1) * c_point->E_energy * back * same;

            if (w_point->name == "西风秘典") energy += 3 * front * white;
            else if (w_point->name == "祭礼残章") energy += c_point->E_energy * front * same;
            else if (w_point->name == "试作金珀") Q_energy_modify -= 18;
            //"不灭月华" 0.6/A Q后12s内 不吃充能
        }
        else if (c_point->name == "钟离")
        {
            //Q 40 E 0.5f 1E/Q
//            energy = ((double_E_per_round.find(c_point->name) != string::npos) ? 2 : 1) * c_point->E_energy * front * same + back_back_total;
//
//            if (w_point->name == "西风长枪") energy += 3 * front * white;
//            if (w_point->name == "喜多院十文字") Q_energy_modify -= 12;
            //通过歪充能词条和队友保证循环
            energy = Q_energy_modify;
        }
            //TODO:NEW
        else if (c_point->name == "纳西妲")
        {
            //Q 50 E 3f 2E/Q
            energy += ((double_E_per_round.find(c_point->name) != string::npos) ? 2 : 1) * c_point->E_energy * front * same;

            if (w_point->name == "西风秘典") energy += 3 * front * white;
            else if (w_point->name == "祭礼残章") energy += c_point->E_energy * front * same;
            else if (w_point->name == "试作金珀") Q_energy_modify -= 18;
            //"不灭月华" 0.6/A Q后12s内 不吃充能
        }
        else energy = Q_energy_modify;

        min_recharge_num = max(0, (int) ((Q_energy_modify / energy - data_list[5]->percentage - data_list[5]->converted_percentage - converted_recharge) / data_list[5]->value_per_entry));
    }
}

//build new character(needed)||build new weapon(all)||build new artifact(all)
void Deployment::get_convert_value(double &life, double &atk, double &def, double &mastery, double &recharge, double &critrate, double &critdam, double &damplus)
{
    //先计算转化，最后再执行加成，不加到面板上的转化可以吃converted_percentage
    double life_add = 0, atk_add = 0, def_add = 0, mastery_add = 0, recharge_add = 0, critrate_add = 0, critdam_add = 0, damplus_add = 0;
    //character
    if (c_point->name == "胡桃")//生命->攻击
        atk_add += min(life * 0.0626 * base_life / base_atk, 4.0);
    else if (c_point->name == "雷电将军" && attack_config->condition->ele_type == "雷")//充能->增伤
        damplus_add += (recharge - 1) * 0.4;
    else if (c_point->name == "八重神子" && attack_config->condition->attack_way == "E")//精通->E增伤
        damplus_add += (mastery + data_list[4]->converted_percentage) * 0.0015;
    else if (c_point->name == "莫娜" && attack_config->condition->ele_type == "水")//充能->增伤
        damplus_add += recharge * 0.2;
        //TODO:NEW
    else if (c_point->name == "纳西妲" && attack_config->condition->attack_way == "E")//精通->E暴击增伤
    {
        damplus_add += min((mastery + data_list[4]->converted_percentage - 200.0), 800.0) * 0.001;
        critrate_add += min((mastery + data_list[4]->converted_percentage - 200.0), 800.0) * 0.0003;
    }

    //weapon
    if (w_point->name == "磐岩结绿")//生命->攻击
        atk_add += life * 0.012 * (0.75 + w_point->level * 0.25) * base_life / base_atk;//生命->攻击
    else if (w_point->name == "圣显之钥")//生命->精通
    {
        if (c_point->args->sword_shengxian_level == 3)
        {
            mastery_add += life * 0.0012 * 3 * (0.75 + w_point->level * 0.25) * base_life;//生命->精通
            mastery_add += life * 0.002 * (0.75 + w_point->level * 0.25) * base_life;//生命->精通
        }
        else if (c_point->args->sword_shengxian_level == 2)
        {
            mastery_add += life * 0.0012 * 2 * (0.75 + w_point->level * 0.25) * base_life;//生命->精通
        }
        else if (c_point->args->sword_shengxian_level == 1)
        {
            mastery_add += life * 0.0012 * (0.75 + w_point->level * 0.25) * base_life;//生命->精通
        }
    }
    else if (w_point->name == "西福斯的月光")//精通->充能
        recharge_add += mastery * 0.00036 * (0.75 + w_point->level * 0.25);//精通->充能
    else if (w_point->name == "流浪的晚星")//精通->攻击
        atk_add += mastery * 0.24 * (0.75 + w_point->level * 0.25) / base_atk;//精通->攻击
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
    else if (w_point->name == "赤沙之杖")//精通->攻击
    {
        atk_add += mastery * 0.52 * (0.75 + w_point->level * 0.25) / base_atk;//精通->攻击
        if (c_point->args->polearm_chisha_level == 3)
        {
            atk_add += mastery * 0.28 * 3 * (0.75 + w_point->level * 0.25) / base_atk;//精通->攻击
        }
        else if (c_point->args->polearm_chisha_level == 2)
        {
            atk_add += mastery * 0.28 * 2 * (0.75 + w_point->level * 0.25) / base_atk;//精通->攻击
        }
        else if (c_point->args->polearm_chisha_level == 1)
        {
            atk_add += mastery * 0.28 * (0.75 + w_point->level * 0.25) / base_atk;//精通->攻击
        }
    }

    //artifact
    if (suit1->name == "绝缘之旗印" && suit2->name == "绝缘之旗印" && attack_config->condition->attack_way == "Q")//充能->增伤
        damplus_add += min(recharge * 0.25, 0.75);

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
    if (w_point->name == "辰砂之纺锤" && attack_config->condition->attack_way == "E")
        extrarate += 0.4 * (0.75 + w_point->level * 0.25) * def * base_def;
    else if (w_point->name == "不灭月华" && attack_config->condition->attack_way == "平A")
        extrarate += (0.005 + w_point->level * 0.005) * life * base_life;
    else if (w_point->name == "赤角石溃杵" && (attack_config->condition->attack_way == "平A" || attack_config->condition->attack_way == "重A"))
        extrarate += 0.4 * (0.75 + w_point->level * 0.25) * def * base_def;
    else if (w_point->name == "猎人之径" && attack_config->condition->attack_way == "重A")
        extrarate += 1.6 * (0.75 + w_point->level * 0.25) * mastery;
    //character
    if (c_point->name == "胡桃")
    {
        if (c_point->constellation >= 2 && attack_config->condition->attack_way == "E")
            extrarate += 0.1 * life * base_life;
    }
    else if (c_point->name == "夜兰")
    {
        if (attack_config->condition->attack_way == "重A")//LV9
            extrarate += 0.1968 * life * base_life;
        else if (attack_config->condition->attack_way == "E")//LV9
            extrarate += 0.384 * life * base_life;
        else if (attack_config->condition->attack_way == "Q")//LV9
        {
            if (c_point->constellation >= 2)
                extrarate += ((0.1242 + 0.0828 * 3 * 15 + 0.14 * 7) / 53) * life * base_life;
            else
                extrarate += ((0.1242 + 0.0828 * 3 * 15) / 46) * life * base_life;
        }
    }
    else if (c_point->name == "钟离")
    {
        if (attack_config->condition->attack_way == "平A" || attack_config->condition->attack_way == "重A" || attack_config->condition->attack_way == "下落A")
            extrarate += 0.0139 * life * base_life;
        else if (attack_config->condition->attack_way == "E")
            extrarate += 0.019 * life * base_life;
        else if (attack_config->condition->attack_way == "Q")
            extrarate += 0.33 * life * base_life;
    }
        //TODO:NEW
    else if (c_point->name == "纳西妲")
    {
        if (attack_config->condition->attack_way == "E")
            extrarate += 3.509 * mastery;
    }
}

//build new character(needed)||build new weapon(all)||build new artifact(all)
void Deployment::get_react_value(double mastery, double &extrarate, double &growrate, double &extra_damage)
{
    //扩散（风+水火雷冰），结晶（岩+水火雷冰），绽放（草水+火雷），激化（草雷），燃烧（草火），蒸发（水火），融化（火冰），冻结（水冰），感电（雷水），超载（雷火），超导（雷冰）
    //默认抗性固定为0.1
    if (attack_config->react_type.find("扩散") != string::npos)
    {
        double extra_damplus = 0;
        if ((suit1->name == suit2->name) && (suit1->name == "翠绿之影")) extra_damplus += 0.6;
        if (team_config->teammate_all.find("莫娜") != string::npos && team_config->ele_allow_spread.find("水") != string::npos) extra_damplus += 0.15;
        extra_damage += 1.2 * 723.4 * (1.0 + (16.0 * mastery) / (mastery + 2000.0) + extra_damplus);
        //1.2 * 723.4 * (1.0 + (16.0 * mastery) / (mastery + 2000.0) + 如雷/魔女等) * resistance;
        //元素伤，吃各元素抗
    }
    if (attack_config->react_type.find("结晶") != string::npos)
    {}
    if (attack_config->react_type.find("绽放") != string::npos)
    {
        //TODO:绽放
//        if (attack_config->react_type.find("烈绽放") != string::npos)
//        {
//            double extra_damplus = 0;
//            if ((suit1->name == suit2->name) && (suit1->name == "炽烈的炎之魔女")) extra_damplus += 0.4;
//            //extra_damage += 6.0 * 723.4 * (1.0 + (16.0 * mastery) / (mastery + 2000.0) + extra_damplus);
//            extra_damage += 6.0 * 723.4 * (1.0 + (16.0 * mastery) / (mastery + 2000.0) + extra_damplus) * ((team_config->teammate_all.find("纳西妲") != string::npos) ? 1.2 : 1);
//            //6.0 * 723.4 * (1.0 + (16.0 * mastery) / (mastery + 2000.0) + 如雷/魔女等) * resistance;
//            //草伤，吃草抗
//        }
//        else if (attack_config->react_type.find("超绽放") != string::npos)
//        {
//            double extra_damplus = 0;
//            if ((suit1->name == suit2->name) && (suit1->name == "如雷的盛怒")) extra_damplus += 0.4;
//            //extra_damage += 6.0 * 723.4 * (1.0 + (16.0 * mastery) / (mastery + 2000.0) + extra_damplus);
//            extra_damage += 6.0 * 723.4 * (1.0 + (16.0 * mastery) / (mastery + 2000.0) + extra_damplus) * ((team_config->teammate_all.find("纳西妲") != string::npos) ? 1.2 : 1);
//            //6.0 * 723.4 * (1.0 + (16.0 * mastery) / (mastery + 2000.0) + 如雷/魔女等) * resistance;
//            //草伤，吃草抗
//        }
//        else
//        {
//            double extra_damplus = 0;
//            //extra_damage += 4.0 * 723.4 * (1.0 + (16.0 * mastery) / (mastery + 2000.0) + extra_damplus);
//            extra_damage += 4.0 * 723.4 * (1.0 + (16.0 * mastery) / (mastery + 2000.0) + extra_damplus) * ((team_config->teammate_all.find("纳西妲") != string::npos) ? 1.2 : 1);
//            //4.0 * 723.4 * (1.0 + (16.0 * mastery) / (mastery + 2000.0) + 如雷/魔女等) * resistance;
//            //草伤，吃草抗
//        }
    }
    if (attack_config->react_type.find("激化") != string::npos)
    {
        if (this->attack_config->condition->ele_type == "草") extrarate += 1447.0 * 1.25 * (1.0 + (5.0 * mastery) / (mastery + 1200.0));
        else if (this->attack_config->condition->ele_type == "雷") extrarate += 1447.0 * 1.15 * (1.0 + (5.0 * mastery) / (mastery + 1200.0));
    }
    if (attack_config->react_type.find("燃烧") != string::npos)
    {
        double extra_damplus = 0;
        if ((suit1->name == suit2->name) && (suit1->name == "炽烈的炎之魔女")) extra_damplus += 0.4;
        //extra_damage += 4.0 * 723.4 * (1.0 + (16.0 * mastery) / (mastery + 2000.0) + extra_damplus);
        extra_damage += 4.0 * 723.4 * (1.0 + (16.0 * mastery) / (mastery + 2000.0) + extra_damplus) * ((team_config->teammate_all.find("纳西妲") != string::npos) ? 1.2 : 1);
        //0.5 * 723.4 * (1.0 + (16.0 * mastery) / (mastery + 2000.0) + 如雷/魔女等) * resistance;
        //火伤，吃火抗，8段伤害
    }
    if (attack_config->react_type.find("蒸发") != string::npos)
    {
        growrate = 1.0 + (25.0 * mastery) / (9.0 * (mastery + 1401.0));
        if ((suit1->name == suit2->name) && (suit1->name == "炽烈的炎之魔女")) growrate += 0.15;
        if (team_config->teammate_all.find("莫娜") != string::npos) growrate += 0.15;
        if (attack_config->condition->ele_type == "火") growrate *= 1.5;
        else if (attack_config->condition->ele_type == "水") growrate *= 2.0;
    }
    if (attack_config->react_type.find("融化") != string::npos)
    {
        growrate = 1.0 + (25.0 * mastery) / (9.0 * (mastery + 1401.0));
        if ((suit1->name == suit2->name) && (suit1->name == "炽烈的炎之魔女")) growrate += 0.15;
        if (attack_config->condition->ele_type == "火") growrate *= 2.0;
        else if (attack_config->condition->ele_type == "冰") growrate *= 1.5;
    }
    if (attack_config->react_type.find("冻结") != string::npos)
    {}
    //碎冰 //3.0 * 723.4 * (1.0 + (16.0 * mastery) / (mastery + 2000.0) + 如雷/魔女等) * resistance;
    //物理伤，吃物理抗
    if (attack_config->react_type.find("感电") != string::npos)
    {
        double extra_damplus = 0;
        if ((suit1->name == suit2->name) && (suit1->name == "如雷的盛怒")) extra_damplus += 0.4;
        if (team_config->teammate_all.find("莫娜") != string::npos) extra_damplus += 0.15;
        extra_damage += 2.4 * 723.4 * (1.0 + (16.0 * mastery) / (mastery + 2000.0) + extra_damplus);
        //2.4 * 723.4 * (1.0 + (16.0 * mastery) / (mastery + 2000.0) + 如雷/魔女等) * resistance;
        //雷伤，吃雷抗
    }
    if (attack_config->react_type.find("超载") != string::npos)
    {
        double extra_damplus = 0;
        if ((suit1->name == suit2->name) && (suit1->name == "炽烈的炎之魔女")) extra_damplus += 0.4;
        if ((suit1->name == suit2->name) && (suit1->name == "如雷的盛怒")) extra_damplus += 0.4;
        extra_damage += 4.0 * 723.4 * (1.0 + (16.0 * mastery) / (mastery + 2000.0) + extra_damplus);
        //4.0 * 723.4 * (1.0 + (16.0 * mastery) / (mastery + 2000.0) + 如雷/魔女等) * resistance;
        //火伤，吃火抗
    }
    if (attack_config->react_type.find("超导") != string::npos)
    {
        double extra_damplus = 0;
        if ((suit1->name == suit2->name) && (suit1->name == "如雷的盛怒")) extra_damplus += 0.4;
        extra_damage += 1.0 * 723.4 * (1.0 + (16.0 * mastery) / (mastery + 2000.0) + extra_damplus);
        //1.0 * 723.4 * (1.0 + (16.0 * mastery) / (mastery + 2000.0) + 如雷/魔女等) * resistance;
        //冰伤，吃冰抗
    }
}

struct Combination
{
    Weapon *w_point;
    Artifact *suit1;
    Artifact *suit2;
    string a_main3;
    string a_main4;
    string a_main5;
    Team_config *team_config;
    vector<Attack_config *> attack_config_list;

    Combination(Weapon *w_point_,
                Artifact *suit1_,
                Artifact *suit2_,
                string a_main3_,
                string a_main4_,
                string a_main5_,
                Team_config *team_config_,
                vector<Attack_config *> attack_config_list_)
    {
        w_point = w_point_;
        suit1 = suit1_;
        suit2 = suit2_;
        a_main3 = a_main3_;
        a_main4 = a_main4_;
        a_main5 = a_main5_;
        team_config = team_config_;
        attack_config_list = attack_config_list_;
    }
};

bool out_header = true;
//TODO:配置编写
int top_k = 3;
bool cal_enable_recharge_num = true;
string double_E_per_round = "神里绫华甘雨温迪纳西妲";
double cal_min_critrate_valid = 0.625;//15*0.033+0.05+0.08
double cal_max_critrate_valid = 1.0;
int max_up_num_per_base = 4;
int max_attribute_num_per_pos = 3;
int max_entry_num = 30;
int artifact_2_2_max_entry_bonus = 2;

void get_all_config(string c_name, vector<Combination *> &combination_list)
{
    //"白辰之环" "苍古自由之誓" "松籁响起之时" "圣显之钥" "西福斯的月光" "流浪的晚星" "玛海菈的水色"

    //"终末嗟叹之诗" "讨龙" "原木刀" "森林王器" "贯月矢" "千夜浮梦"
    //"悠古的磐岩" "昔日宗室之仪" "翠绿之影" "千岩牢固" "深林的记忆"

    //"胡桃" "行秋" "钟离" "夜兰" "终末嗟叹之诗_昔日宗室之仪"/"昔日宗室之仪" "水" "水"
    //"胡桃" "行秋" "钟离" "莫娜" "讨龙_昔日宗室之仪" "水" "水"
    //"胡桃" "夜兰" "钟离" "莫娜" "终末嗟叹之诗_讨龙_昔日宗室之仪"/"讨龙_昔日宗室之仪" "水" "水"

    //"神里绫华" "甘雨" "温迪" "莫娜" "终末嗟叹之诗_昔日宗室之仪_翠绿之影"/"昔日宗室之仪_翠绿之影" "冰水" "冰"
    //"神里绫华" "甘雨" "枫原万叶" "莫娜" "昔日宗室之仪_翠绿之影" "冰水" "冰"

    //"雷电将军" "行秋" "香菱" "班尼特" "昔日宗室之仪" "水雷" "水雷火"
    //"雷电将军" "夜兰" "香菱" "班尼特" "终末嗟叹之诗_昔日宗室之仪"/"昔日宗室之仪" "水雷" "水雷火"
    //"雷电将军" "八重神子" "香菱" "班尼特" "昔日宗室之仪" "雷" "雷火"
    //"雷电将军" "枫原万叶" "香菱" "班尼特" "昔日宗室之仪_翠绿之影" "火" "雷火"

    //"纳西妲" "草_test" "八重神子" "钟离" "昔日宗室之仪_千夜浮梦_深林的记忆" "雷草" "雷"
    //"纳西妲" "草_test" "八重神子" "班尼特" "原木刀_昔日宗室之仪_千夜浮梦_深林的记忆" "雷草" "雷火"
    //"纳西妲" "草_test" "八重神子" "雷电将军" "千夜浮梦_深林的记忆" "雷草" "雷"
    //"纳西妲" "草_test" "钟离" "雷电将军" "昔日宗室之仪_千夜浮梦_深林的记忆" "雷草" "雷"
    //"纳西妲" "草_test" "班尼特" "雷电将军" "原木刀_昔日宗室之仪_千夜浮梦_深林的记忆" "雷草" "雷火"

    if (c_name == "胡桃")
    {
        auto *tc1 = new Team_config(find_character_by_name("钟离"), find_character_by_name("行秋"), find_character_by_name("莫娜"),
                                    "讨龙_昔日宗室之仪", "水", "水");
        auto *tc2 = new Team_config(find_character_by_name("钟离"), find_character_by_name("行秋"), find_character_by_name("夜兰"),
                                    "终末嗟叹之诗_昔日宗室之仪", "水", "水");
        auto *tc3 = new Team_config(find_character_by_name("钟离"), find_character_by_name("行秋"), find_character_by_name("夜兰"),
                                    "昔日宗室之仪", "水", "水");
        auto *tc4 = new Team_config(find_character_by_name("钟离"), find_character_by_name("夜兰"), find_character_by_name("莫娜"),
                                    "终末嗟叹之诗_讨龙_昔日宗室之仪", "水", "水");
        auto *tc5 = new Team_config(find_character_by_name("钟离"), find_character_by_name("夜兰"), find_character_by_name("莫娜"),
                                    "讨龙_昔日宗室之仪", "水", "水");

        vector<Attack_config *> ac1;//9AZ
        ac1.push_back(new Attack_config(new Condition("火", "长柄武器", "平A"), false, false, "NONE",
                                        true, true, false, false, false,
                                        true, true, true, false, false, 6));
        ac1.push_back(new Attack_config(new Condition("火", "长柄武器", "平A"), false, false, "蒸发",
                                        true, true, false, true, false,
                                        true, true, true, false, false, 3));
        ac1.push_back(new Attack_config(new Condition("火", "长柄武器", "重A"), false, false, "蒸发",
                                        true, true, false, true, false,
                                        true, true, true, false, false, 9));

        combination_list.push_back(new Combination(find_weapon_by_name(""), find_artifact_by_name(""), find_artifact_by_name(""),
                                                   "", "", "", tc1, ac1));
        combination_list.push_back(new Combination(find_weapon_by_name(""), find_artifact_by_name(""), find_artifact_by_name(""),
                                                   "", "", "", tc2, ac1));
        combination_list.push_back(new Combination(find_weapon_by_name(""), find_artifact_by_name(""), find_artifact_by_name(""),
                                                   "", "", "", tc3, ac1));
        combination_list.push_back(new Combination(find_weapon_by_name(""), find_artifact_by_name(""), find_artifact_by_name(""),
                                                   "", "", "", tc4, ac1));
        combination_list.push_back(new Combination(find_weapon_by_name(""), find_artifact_by_name(""), find_artifact_by_name(""),
                                                   "", "", "", tc5, ac1));
    }
    if (c_name == "神里绫华")
    {
        auto *tc1 = new Team_config(find_character_by_name("甘雨"), find_character_by_name("莫娜"), find_character_by_name("温迪"),
                                    "昔日宗室之仪_翠绿之影", "冰水", "冰");
        auto *tc2 = new Team_config(find_character_by_name("甘雨"), find_character_by_name("莫娜"), find_character_by_name("温迪"),
                                    "终末嗟叹之诗_昔日宗室之仪_翠绿之影", "冰水", "冰");
        auto *tc3 = new Team_config(find_character_by_name("甘雨"), find_character_by_name("莫娜"), find_character_by_name("枫原万叶"),
                                    "昔日宗室之仪_翠绿之影", "冰水", "冰");

        vector<Attack_config *> ac1;//EAQAZAZAZ EAZAZAZ
        ac1.push_back(new Attack_config(new Condition("冰", "单手剑", "平A"), false, false, "冻结",
                                        false, true, false, false, false,
                                        true, true, true, false, false, 1 + 6));
        ac1.push_back(new Attack_config(new Condition("冰", "单手剑", "重A"), false, false, "冻结",
                                        false, true, false, false, false,
                                        true, true, true, false, false, 3 * 6));
        ac1.push_back(new Attack_config(new Condition("冰", "单手剑", "E"), false, false, "冻结",
                                        false, true, false, false, false,
                                        true, true, true, false, false, 2));
        ac1.push_back(new Attack_config(new Condition("冰", "单手剑", "Q"), false, false, "冻结",
                                        false, true, false, false, false,
                                        true, true, true, false, false, 20));

        combination_list.push_back(new Combination(find_weapon_by_name(""), find_artifact_by_name(""), find_artifact_by_name(""),
                                                   "", "", "", tc1, ac1));
        combination_list.push_back(new Combination(find_weapon_by_name(""), find_artifact_by_name(""), find_artifact_by_name(""),
                                                   "", "", "", tc2, ac1));
        combination_list.push_back(new Combination(find_weapon_by_name(""), find_artifact_by_name(""), find_artifact_by_name(""),
                                                   "", "", "", tc3, ac1));
    }
    if (c_name == "雷电将军")
    {
        auto *tc1 = new Team_config(find_character_by_name("香菱"), find_character_by_name("班尼特"), find_character_by_name("行秋"),
                                    "昔日宗室之仪", "水雷", "水雷火");
        auto *tc2 = new Team_config(find_character_by_name("香菱"), find_character_by_name("班尼特"), find_character_by_name("夜兰"),
                                    "终末嗟叹之诗_昔日宗室之仪", "水雷", "水雷火");
        auto *tc3 = new Team_config(find_character_by_name("香菱"), find_character_by_name("班尼特"), find_character_by_name("夜兰"),
                                    "昔日宗室之仪", "水雷", "水雷火");
        auto *tc4 = new Team_config(find_character_by_name("香菱"), find_character_by_name("班尼特"), find_character_by_name("八重神子"),
                                    "昔日宗室之仪", "雷", "雷火");
        auto *tc5 = new Team_config(find_character_by_name("香菱"), find_character_by_name("班尼特"), find_character_by_name("枫原万叶"),
                                    "昔日宗室之仪_翠绿之影", "火", "雷火");
        //TODO:NEW
        auto *tc6 = new Team_config(find_character_by_name("纳西妲"), find_character_by_name("草_test"), find_character_by_name("八重神子"),
                                    "千夜浮梦_深林的记忆", "雷草", "雷");
        auto *tc7 = new Team_config(find_character_by_name("纳西妲"), find_character_by_name("草_test"), find_character_by_name("钟离"),
                                    "昔日宗室之仪_千夜浮梦_深林的记忆", "雷草", "雷");
        auto *tc8 = new Team_config(find_character_by_name("纳西妲"), find_character_by_name("草_test"), find_character_by_name("班尼特"),
                                    "原木刀_昔日宗室之仪_千夜浮梦_深林的记忆", "雷草", "雷火");

        //无法准确建立反应次数和倍率之间的关系（Q的后续攻击），所以在非增幅反应队中不考虑反应，在增幅反应队中次次反应
        vector<Attack_config *> ac1;
        ac1.push_back(new Attack_config(new Condition("雷", "长柄武器", "Q"), false, false, "NONE",
                                        false, true, false, false, true,
                                        true, true, true, false, false, 1));

        vector<Attack_config *> ac2;
        ac2.push_back(new Attack_config(new Condition("雷", "长柄武器", "Q"), false, false, "超激化",
                                        false, true, false, true, true,
                                        true, true, true, false, false, 1));

        combination_list.push_back(new Combination(find_weapon_by_name(""), find_artifact_by_name(""), find_artifact_by_name(""),
                                                   "", "", "", tc1, ac1));
        combination_list.push_back(new Combination(find_weapon_by_name(""), find_artifact_by_name(""), find_artifact_by_name(""),
                                                   "", "", "", tc2, ac1));
        combination_list.push_back(new Combination(find_weapon_by_name(""), find_artifact_by_name(""), find_artifact_by_name(""),
                                                   "", "", "", tc3, ac1));
        combination_list.push_back(new Combination(find_weapon_by_name(""), find_artifact_by_name(""), find_artifact_by_name(""),
                                                   "", "", "", tc4, ac1));
        combination_list.push_back(new Combination(find_weapon_by_name(""), find_artifact_by_name(""), find_artifact_by_name(""),
                                                   "", "", "", tc5, ac1));

        //TODO:配置未计算，6有效词条
//        combination_list.push_back(new Combination(find_weapon_by_name(""), find_artifact_by_name(""), find_artifact_by_name(""),
//                                                   "", "", "", tc6, ac2));
//        combination_list.push_back(new Combination(find_weapon_by_name(""), find_artifact_by_name(""), find_artifact_by_name(""),
//                                                   "", "", "", tc7, ac2));
//        combination_list.push_back(new Combination(find_weapon_by_name(""), find_artifact_by_name(""), find_artifact_by_name(""),
//                                                   "", "", "", tc8, ac2));
    }
    if (c_name == "甘雨")
    {
        auto *tc1 = new Team_config(find_character_by_name("神里绫华"), find_character_by_name("莫娜"), find_character_by_name("温迪"),
                                    "昔日宗室之仪_翠绿之影", "冰水", "冰");
        auto *tc2 = new Team_config(find_character_by_name("神里绫华"), find_character_by_name("莫娜"), find_character_by_name("温迪"),
                                    "终末嗟叹之诗_昔日宗室之仪_翠绿之影", "冰水", "冰");
        auto *tc3 = new Team_config(find_character_by_name("神里绫华"), find_character_by_name("莫娜"), find_character_by_name("枫原万叶"),
                                    "昔日宗室之仪_翠绿之影", "冰水", "冰");

        vector<Attack_config *> ac1;//Q EZ EZ
        ac1.push_back(new Attack_config(new Condition("冰", "弓", "重A"), false, false, "冻结",
                                        false, true, false, false, false,
                                        true, true, true, false, false, 2 * 2));
        ac1.push_back(new Attack_config(new Condition("冰", "弓", "E"), false, true, "冻结",
                                        false, true, false, false, false,
                                        true, true, true, false, false, 2 * 2));
        ac1.push_back(new Attack_config(new Condition("冰", "弓", "Q"), true, true, "冻结",
                                        false, true, false, false, false,
                                        true, true, true, false, false, 25));

        combination_list.push_back(new Combination(find_weapon_by_name(""), find_artifact_by_name(""), find_artifact_by_name(""),
                                                   "", "", "", tc1, ac1));
        combination_list.push_back(new Combination(find_weapon_by_name(""), find_artifact_by_name(""), find_artifact_by_name(""),
                                                   "", "", "", tc2, ac1));
        combination_list.push_back(new Combination(find_weapon_by_name(""), find_artifact_by_name(""), find_artifact_by_name(""),
                                                   "", "", "", tc3, ac1));
    }
    if (c_name == "夜兰")
    {
        auto *tc1 = new Team_config(find_character_by_name("胡桃"), find_character_by_name("钟离"), find_character_by_name("行秋"),
                                    "昔日宗室之仪", "水", "水");
        auto *tc2 = new Team_config(find_character_by_name("胡桃"), find_character_by_name("钟离"), find_character_by_name("莫娜"),
                                    "昔日宗室之仪", "水", "水");
        auto *tc3 = new Team_config(find_character_by_name("雷电将军"), find_character_by_name("香菱"), find_character_by_name("班尼特"),
                                    "昔日宗室之仪", "水雷", "水雷火");

        vector<Attack_config *> ac1;//EQ
        ac1.push_back(new Attack_config(new Condition("水", "弓", "E"), false, false, "NONE",
                                        true, false, false, false, false,
                                        true, true, true, false, false, 1));
        ac1.push_back(new Attack_config(new Condition("水", "弓", "Q"), true, false, "NONE",
                                        true, false, false, false, false,
                                        true, true, true, false, false, 46));

        combination_list.push_back(new Combination(find_weapon_by_name(""), find_artifact_by_name(""), find_artifact_by_name(""),
                                                   "", "", "", tc1, ac1));
        combination_list.push_back(new Combination(find_weapon_by_name(""), find_artifact_by_name(""), find_artifact_by_name(""),
                                                   "", "", "", tc2, ac1));
        combination_list.push_back(new Combination(find_weapon_by_name(""), find_artifact_by_name(""), find_artifact_by_name(""),
                                                   "", "", "", tc3, ac1));
    }
    if (c_name == "行秋")
    {
        auto *tc1 = new Team_config(find_character_by_name("胡桃"), find_character_by_name("钟离"), find_character_by_name("莫娜"),
                                    "昔日宗室之仪", "水", "水");
        auto *tc2 = new Team_config(find_character_by_name("胡桃"), find_character_by_name("钟离"), find_character_by_name("夜兰"),
                                    "终末嗟叹之诗_昔日宗室之仪", "水", "水");
        auto *tc3 = new Team_config(find_character_by_name("胡桃"), find_character_by_name("钟离"), find_character_by_name("夜兰"),
                                    "昔日宗室之仪", "水", "水");

        auto *tc4 = new Team_config(find_character_by_name("雷电将军"), find_character_by_name("香菱"), find_character_by_name("班尼特"),
                                    "昔日宗室之仪", "水雷", "水雷火");

        vector<Attack_config *> ac1;//EQ
        ac1.push_back(new Attack_config(new Condition("水", "单手剑", "E"), false, false, "NONE",
                                        false, true, false, false, false,
                                        true, true, true, false, false, 2 * 1));
        ac1.push_back(new Attack_config(new Condition("水", "单手剑", "Q"), true, false, "NONE",
                                        false, true, false, false, false,
                                        true, true, true, false, false, 45));

        vector<Attack_config *> ac2;//EQ
        ac2.push_back(new Attack_config(new Condition("水", "单手剑", "E"), false, false, "蒸发",
                                        false, true, false, true, false,
                                        true, true, true, false, false, 2 * 1));
        ac2.push_back(new Attack_config(new Condition("水", "单手剑", "Q"), true, false, "NONE",
                                        false, true, false, false, false,
                                        true, true, true, false, false, 45));

        combination_list.push_back(new Combination(find_weapon_by_name(""), find_artifact_by_name(""), find_artifact_by_name(""),
                                                   "", "", "", tc1, ac1));
        combination_list.push_back(new Combination(find_weapon_by_name(""), find_artifact_by_name(""), find_artifact_by_name(""),
                                                   "", "", "", tc2, ac1));
        combination_list.push_back(new Combination(find_weapon_by_name(""), find_artifact_by_name(""), find_artifact_by_name(""),
                                                   "", "", "", tc3, ac1));
        combination_list.push_back(new Combination(find_weapon_by_name(""), find_artifact_by_name(""), find_artifact_by_name(""),
                                                   "", "", "", tc4, ac2));
    }
    if (c_name == "香菱")
    {
        auto *tc1 = new Team_config(find_character_by_name("雷电将军"), find_character_by_name("班尼特"), find_character_by_name("行秋"),
                                    "昔日宗室之仪", "水雷", "水雷火");
        auto *tc2 = new Team_config(find_character_by_name("雷电将军"), find_character_by_name("班尼特"), find_character_by_name("夜兰"),
                                    "终末嗟叹之诗_昔日宗室之仪", "水雷", "水雷火");
        auto *tc3 = new Team_config(find_character_by_name("雷电将军"), find_character_by_name("班尼特"), find_character_by_name("夜兰"),
                                    "昔日宗室之仪", "水雷", "水雷火");

        auto *tc4 = new Team_config(find_character_by_name("雷电将军"), find_character_by_name("班尼特"), find_character_by_name("八重神子"),
                                    "昔日宗室之仪", "雷", "雷火");

        auto *tc5 = new Team_config(find_character_by_name("雷电将军"), find_character_by_name("班尼特"), find_character_by_name("枫原万叶"),
                                    "昔日宗室之仪_翠绿之影", "火", "雷火");

        vector<Attack_config *> ac1;
        ac1.push_back(new Attack_config(new Condition("火", "长柄武器", "E"), true, true, "蒸发",
                                        false, true, false, true, false,
                                        true, true, true, false, false, 4));
        ac1.push_back(new Attack_config(new Condition("火", "长柄武器", "Q"), true, true, "NONE",
                                        false, true, false, false, false,
                                        true, true, true, false, false, 2));
        ac1.push_back(new Attack_config(new Condition("火", "长柄武器", "Q"), true, true, "蒸发",
                                        false, true, false, true, false,
                                        true, true, true, false, false, 7));
        ac1.push_back(new Attack_config(new Condition("火", "长柄武器", "Q"), true, true, "蒸发_超载",
                                        false, true, false, true, false,
                                        true, true, true, false, false, 6));

        vector<Attack_config *> ac2;
        ac2.push_back(new Attack_config(new Condition("火", "长柄武器", "E"), true, true, "超载",
                                        false, true, false, true, false,
                                        true, true, true, false, false, 4));
        ac2.push_back(new Attack_config(new Condition("火", "长柄武器", "Q"), true, true, "NONE",
                                        false, true, false, false, false,
                                        true, true, true, false, false, 2));
        ac2.push_back(new Attack_config(new Condition("火", "长柄武器", "Q"), true, true, "超载",
                                        false, true, false, true, false,
                                        true, true, true, false, false, 13));

        vector<Attack_config *> ac3;
        ac3.push_back(new Attack_config(new Condition("火", "长柄武器", "E"), true, true, "NONE",
                                        false, true, false, false, false,
                                        true, true, true, false, false, 4));
        ac3.push_back(new Attack_config(new Condition("火", "长柄武器", "Q"), true, true, "NONE",
                                        false, true, false, false, false,
                                        true, true, true, false, false, 15));

        combination_list.push_back(new Combination(find_weapon_by_name(""), find_artifact_by_name(""), find_artifact_by_name(""),
                                                   "", "", "", tc1, ac1));
        combination_list.push_back(new Combination(find_weapon_by_name(""), find_artifact_by_name(""), find_artifact_by_name(""),
                                                   "", "", "", tc2, ac1));
        combination_list.push_back(new Combination(find_weapon_by_name(""), find_artifact_by_name(""), find_artifact_by_name(""),
                                                   "", "", "", tc3, ac1));
        combination_list.push_back(new Combination(find_weapon_by_name(""), find_artifact_by_name(""), find_artifact_by_name(""),
                                                   "", "", "", tc4, ac2));
        combination_list.push_back(new Combination(find_weapon_by_name(""), find_artifact_by_name(""), find_artifact_by_name(""),
                                                   "", "", "", tc5, ac3));
    }
    if (c_name == "八重神子")
    {
        auto *tc1 = new Team_config(find_character_by_name("雷电将军"), find_character_by_name("香菱"), find_character_by_name("班尼特"),
                                    "昔日宗室之仪", "雷", "雷火");
        //TODO:NEW
        auto *tc2 = new Team_config(find_character_by_name("纳西妲"), find_character_by_name("草_test"), find_character_by_name("钟离"),
                                    "昔日宗室之仪_千夜浮梦_深林的记忆", "雷草", "雷");
        auto *tc3 = new Team_config(find_character_by_name("纳西妲"), find_character_by_name("草_test"), find_character_by_name("雷电将军"),
                                    "千夜浮梦_深林的记忆", "雷草", "雷");
        auto *tc4 = new Team_config(find_character_by_name("纳西妲"), find_character_by_name("草_test"), find_character_by_name("班尼特"),
                                    "昔日宗室之仪_千夜浮梦_深林的记忆", "雷草", "雷火");

        vector<Attack_config *> ac1;
        ac1.push_back(new Attack_config(new Condition("雷", "法器", "E"), true, false, "NONE",
                                        false, true, false, true, false,
                                        true, true, true, false, false, 21));
        ac1.push_back(new Attack_config(new Condition("雷", "法器", "Q"), false, true, "NONE",
                                        false, true, false, false, false,
                                        true, true, true, false, false, 4));

        vector<Attack_config *> ac2;
        ac2.push_back(new Attack_config(new Condition("雷", "法器", "E"), true, false, "超激化",
                                        false, true, false, true, false,
                                        true, true, true, false, false, 7));
        ac2.push_back(new Attack_config(new Condition("雷", "法器", "E"), true, false, "NONE",
                                        false, true, false, true, false,
                                        true, true, true, false, false, 14));
        ac2.push_back(new Attack_config(new Condition("雷", "法器", "Q"), false, true, "超激化",
                                        false, true, false, true, false,
                                        true, true, true, false, false, 4));

        vector<Attack_config *> ac3;
        ac3.push_back(new Attack_config(new Condition("雷", "法器", "E"), true, false, "超激化",
                                        false, true, false, true, false,
                                        true, true, true, false, false, 14));
        ac3.push_back(new Attack_config(new Condition("雷", "法器", "E"), true, false, "NONE",
                                        false, true, false, true, false,
                                        true, true, true, false, false, 28));
        ac3.push_back(new Attack_config(new Condition("雷", "法器", "Q"), false, true, "超激化",
                                        false, true, false, true, false,
                                        true, true, true, false, false, 4));

        combination_list.push_back(new Combination(find_weapon_by_name(""), find_artifact_by_name(""), find_artifact_by_name(""),
                                                   "", "", "", tc1, ac1));
        //TODO:配置未计算，set cal_enable_recharge_num = false
//        combination_list.push_back(new Combination(find_weapon_by_name(""), find_artifact_by_name(""), find_artifact_by_name(""),
//                                                   "", "", "", tc2, ac3));
        combination_list.push_back(new Combination(find_weapon_by_name(""), find_artifact_by_name(""), find_artifact_by_name(""),
                                                   "", "", "", tc3, ac2));
        //TODO:配置未计算，set cal_enable_recharge_num = false
//        combination_list.push_back(new Combination(find_weapon_by_name(""), find_artifact_by_name(""), find_artifact_by_name(""),
//                                                   "", "", "", tc4, ac3));
    }
    if (c_name == "温迪")
    {
        auto *tc1 = new Team_config(find_character_by_name("风_test"), find_character_by_name("风_test"), find_character_by_name("风_test"),
                                    "", "冰水", "冰水");

        vector<Attack_config *> ac1;
        ac1.push_back(new Attack_config(new Condition("风", "弓", "E"), false, false, "扩散",
                                        false, true, false, true, false,
                                        true, true, true, false, false, 1));
        ac1.push_back(new Attack_config(new Condition("风", "弓", "Q"), true, true, "扩散",
                                        false, true, false, true, false,
                                        true, true, true, false, false, 20));

        combination_list.push_back(new Combination(find_weapon_by_name(""), find_artifact_by_name("翠绿之影"), find_artifact_by_name("翠绿之影"),
                                                   "", "", "", tc1, ac1));
    }
    if (c_name == "莫娜")
    {
        auto *tc1 = new Team_config(find_character_by_name("胡桃"), find_character_by_name("钟离"), find_character_by_name("行秋"),
                                    "昔日宗室之仪", "水", "水");
        auto *tc2 = new Team_config(find_character_by_name("胡桃"), find_character_by_name("钟离"), find_character_by_name("夜兰"),
                                    "终末嗟叹之诗_昔日宗室之仪", "水", "水");
        auto *tc3 = new Team_config(find_character_by_name("胡桃"), find_character_by_name("钟离"), find_character_by_name("夜兰"),
                                    "昔日宗室之仪", "水", "水");

        auto *tc4 = new Team_config(find_character_by_name("神里绫华"), find_character_by_name("甘雨"), find_character_by_name("温迪"),
                                    "翠绿之影", "冰水", "冰");
        auto *tc5 = new Team_config(find_character_by_name("神里绫华"), find_character_by_name("甘雨"), find_character_by_name("温迪"),
                                    "终末嗟叹之诗_翠绿之影", "冰水", "冰");
        auto *tc6 = new Team_config(find_character_by_name("神里绫华"), find_character_by_name("甘雨"), find_character_by_name("枫原万叶"),
                                    "翠绿之影", "冰水", "冰");

        vector<Attack_config *> ac1;
        ac1.push_back(new Attack_config(new Condition("水", "法器", "E"), true, false, "NONE",
                                        false, true, false, false, true,
                                        true, true, true, false, false, 5));
        ac1.push_back(new Attack_config(new Condition("水", "法器", "Q"), true, false, "NONE",
                                        false, true, false, false, true,
                                        true, true, true, false, false, 1));

        vector<Attack_config *> ac2;
        ac2.push_back(new Attack_config(new Condition("水", "法器", "E"), true, false, "冻结",
                                        false, true, false, false, true,
                                        true, true, true, false, false, 5));
        ac2.push_back(new Attack_config(new Condition("水", "法器", "Q"), true, false, "冻结",
                                        false, true, false, false, true,
                                        true, true, true, false, false, 1));

        combination_list.push_back(new Combination(find_weapon_by_name(""), find_artifact_by_name("昔日宗室之仪"), find_artifact_by_name("昔日宗室之仪"),
                                                   "元素充能效率", "", "", tc1, ac1));
        combination_list.push_back(new Combination(find_weapon_by_name(""), find_artifact_by_name("昔日宗室之仪"), find_artifact_by_name("昔日宗室之仪"),
                                                   "元素充能效率", "", "", tc2, ac1));
        combination_list.push_back(new Combination(find_weapon_by_name(""), find_artifact_by_name("昔日宗室之仪"), find_artifact_by_name("昔日宗室之仪"),
                                                   "元素充能效率", "", "", tc3, ac1));
        combination_list.push_back(new Combination(find_weapon_by_name(""), find_artifact_by_name("昔日宗室之仪"), find_artifact_by_name("昔日宗室之仪"),
                                                   "元素充能效率", "", "", tc4, ac2));
        combination_list.push_back(new Combination(find_weapon_by_name(""), find_artifact_by_name("昔日宗室之仪"), find_artifact_by_name("昔日宗室之仪"),
                                                   "元素充能效率", "", "", tc5, ac2));
        combination_list.push_back(new Combination(find_weapon_by_name(""), find_artifact_by_name("昔日宗室之仪"), find_artifact_by_name("昔日宗室之仪"),
                                                   "元素充能效率", "", "", tc6, ac2));
    }
    if (c_name == "钟离")
    {
        auto *tc1 = new Team_config(find_character_by_name("风_test"), find_character_by_name("风_test"), find_character_by_name("风_test"),
                                    "", "", "");

        vector<Attack_config *> ac1;
        ac1.push_back(new Attack_config(new Condition("岩", "长柄武器", "Q"), false, false, "结晶",
                                        true, true, false, false, false,
                                        true, true, true, false, false, 1));

        combination_list.push_back(new Combination(find_weapon_by_name(""), find_artifact_by_name("昔日宗室之仪"), find_artifact_by_name("昔日宗室之仪"),
                                                   "生命值", "", "", tc1, ac1));
        combination_list.push_back(new Combination(find_weapon_by_name(""), find_artifact_by_name("深林的记忆"), find_artifact_by_name("深林的记忆"),
                                                   "生命值", "", "", tc1, ac1));
        combination_list.push_back(new Combination(find_weapon_by_name(""), find_artifact_by_name(""), find_artifact_by_name(""),
                                                   "生命值", "", "", tc1, ac1));
    }
    //TODO:NEW
    if (c_name == "纳西妲")
    {
        auto *tc1 = new Team_config(find_character_by_name("草_test"), find_character_by_name("八重神子"), find_character_by_name("钟离"),
                                    "昔日宗室之仪_深林的记忆", "雷草", "雷");
        auto *tc2 = new Team_config(find_character_by_name("草_test"), find_character_by_name("八重神子"), find_character_by_name("班尼特"),
                                    "昔日宗室之仪_深林的记忆", "雷草", "雷火");
        auto *tc3 = new Team_config(find_character_by_name("草_test"), find_character_by_name("八重神子"), find_character_by_name("雷电将军"),
                                    "深林的记忆", "雷草", "雷");
        auto *tc4 = new Team_config(find_character_by_name("草_test"), find_character_by_name("雷电将军"), find_character_by_name("钟离"),
                                    "昔日宗室之仪_深林的记忆", "雷草", "雷");
        auto *tc5 = new Team_config(find_character_by_name("草_test"), find_character_by_name("雷电将军"), find_character_by_name("班尼特"),
                                    "昔日宗室之仪_深林的记忆", "雷草", "雷火");

        vector<Attack_config *> ac1;
        ac1.push_back(new Attack_config(new Condition("草", "法器", "E"), true, false, "蔓激化",
                                        false, true, false, true, false,
                                        true, true, true, false, false, 1));

        combination_list.push_back(new Combination(find_weapon_by_name(""), find_artifact_by_name("深林的记忆"), find_artifact_by_name("深林的记忆"),
                                                   "", "", "", tc1, ac1));
        combination_list.push_back(new Combination(find_weapon_by_name(""), find_artifact_by_name("深林的记忆"), find_artifact_by_name("深林的记忆"),
                                                   "", "", "", tc2, ac1));
        combination_list.push_back(new Combination(find_weapon_by_name(""), find_artifact_by_name("深林的记忆"), find_artifact_by_name("深林的记忆"),
                                                   "", "", "", tc3, ac1));
        combination_list.push_back(new Combination(find_weapon_by_name(""), find_artifact_by_name("深林的记忆"), find_artifact_by_name("深林的记忆"),
                                                   "", "", "", tc4, ac1));
        combination_list.push_back(new Combination(find_weapon_by_name(""), find_artifact_by_name("深林的记忆"), find_artifact_by_name("深林的记忆"),
                                                   "", "", "", tc5, ac1));
    }
}

struct cmp
{
    bool operator()(Group *a, Group *b)
    {
        if (a->total_damage > b->total_damage) return true;
        else return false;
    }
};

void cal_deployment()
{
    for (auto &c_index: character_list)
    {
        vector<Combination *> combination_list;
        get_all_config(c_index->name, combination_list);
        for (auto &comb_index: combination_list)
        {
            for (auto &w_index: weapon_list)
            {
                if (c_index->weapon_type != w_index->weapon_type) continue;

                if (comb_index->w_point != nullptr && comb_index->w_point != w_index) continue;

                priority_queue<Group *, vector<Group *>, cmp> c_w_pair;

                clock_t start = clock();

                for (int a_index1 = 0; a_index1 < artifact_list.size(); a_index1++)
                {
                    if (comb_index->suit1 != nullptr && comb_index->suit1 != artifact_list[a_index1]) continue;

                    for (int a_index2 = a_index1; a_index2 < artifact_list.size(); a_index2++)
                    {
                        if (comb_index->suit2 != nullptr && comb_index->suit2 != artifact_list[a_index2]) continue;

                        for (int main3_index = 0; main3_index < 5; main3_index++)
                        {
                            if (!comb_index->a_main3.empty() && comb_index->a_main3 != a_main3[main3_index]) continue;

                            for (int main4_index = (main3_index == 4) ? 0 : main3_index; main4_index < 5; main4_index++)
                            {
                                if (!comb_index->a_main4.empty() && comb_index->a_main4 != a_main4[main4_index]) continue;

                                for (int main5_index = (main4_index == 4) ? ((main3_index == 4) ? 0 : main3_index) : main4_index; main5_index < 7; main5_index++)
                                {
                                    if (!comb_index->a_main5.empty() && comb_index->a_main5 != a_main5[main5_index]) continue;

                                    auto *temp = new Group(c_index, w_index, artifact_list[a_index1], artifact_list[a_index2], a_main3[main3_index],
                                                           a_main4[main4_index], a_main5[main5_index], comb_index->team_config, comb_index->attack_config_list);
                                    int check_num = temp->init_check_data();
                                    if (check_num == 0)//pass
                                    {
                                        temp->cal_damage_entry_num();
                                        if (c_w_pair.size() < top_k) c_w_pair.push(temp);
                                        else if (temp->total_damage > c_w_pair.top()->total_damage)
                                        {
                                            Group *smallest = c_w_pair.top();
                                            c_w_pair.pop();
                                            delete smallest;
                                            c_w_pair.push(temp);
                                        }
                                        else delete temp;
                                    }
                                    else if (check_num == 1)//error:suit1
                                    {
                                        delete temp;
                                        goto NEXTARTIFACT1;
                                    }
                                    else if (check_num == 2)//error:suit2
                                    {
                                        delete temp;
                                        goto NEXTARTIFACT2;
                                    }
                                    else if (check_num == 3)//error:main3
                                    {
                                        delete temp;
                                        goto NEXTMAIN3;
                                    }
                                    else if (check_num == 4)//error:main4
                                    {
                                        delete temp;
                                        goto NEXTMAIN4;
                                    }
                                    else if (check_num == 5)//error:main5
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
                        NEXTARTIFACT2:;
                    }
                    NEXTARTIFACT1:;
                }

                clock_t end = clock();
                double time = (double) (end - start) / CLOCKS_PER_SEC;

                while (!c_w_pair.empty())
                {
                    Group *c_w = c_w_pair.top();
                    c_w_pair.pop();
                    c_w->out();
                    delete c_w;
                }
                cout << c_index->name << " " << w_index->name << " " << " time=" << time << "s" << ((time > 30) ? "!!!" : "") << " ";
                for (auto &i: comb_index->attack_config_list)
                    cout << to_string(i->attack_time) + "*" + i->condition->attack_way + "_" + (i->background ? "后台" : "前台") + "_" + i->react_type + "/";
                cout << " " << comb_index->team_config->teammate_all + "_" + comb_index->team_config->team_weapon_artifact << "\n";
            }
        }
        combination_list.clear();
    }
}

ofstream outfile_result;
ofstream outfile_debug;

int main()
{
    init_character_data();
    init_weapon_data();
    init_artifact_data();

    outfile_result.open("./data.csv");
    outfile_debug.open("./log_data.csv");
    cal_deployment();
    outfile_result.close();
    outfile_debug.close();
    return 0;
}