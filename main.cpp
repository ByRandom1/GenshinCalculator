#include <iostream>
#include <thread>
#include <chrono>
#include <queue>
#include <algorithm>
#include "Condition.h"
#include "Character.h"
#include "Weapon.h"
#include "Artifact.h"
#include "Reinforced_Artifact.h"
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

//TODO:less important 将所有添加的属性分为face_percentage,converted_face_percentage,monster_percentage，使用传入的string加到不同的变量上，转化时，先face->convert，然后加上convert在进行convert->extra，最后加上monster
//TODO:less important 确认所有属性加成的分类，目前来看只有增伤、暴击、爆伤可能不加在面板上，目前不影响结果（没有增伤/暴击/爆伤转别的）
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
    character_list.push_back(new Character("胡桃", "hutao", "火", "长柄武器", 15552, 107, 876, "暴击伤害", 0.384,
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
    character_list.push_back(new Character("神里绫华", "ayaka", "冰", "单手剑", 12858, 342, 784, "暴击伤害", 0.384,
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
    character_list.push_back(new Character("雷电将军", "raiden", "雷", "长柄武器", 12907, 337, 789, "元素充能效率", 0.32,
                                           6, "物理", (0.784 + 0.785 + 0.986 + 0.573 + 0.573 + 1.294) / 6, (0.728 + 0.73 + 0.916 + 0.533 + 0.533 + 1.202) / 6,
                                           "物理", 1.969, 1.83, "物理", 3.16, 2.93,
                                           10, 10, false, (2.491 + 18 * 0.893) / 19, (2.344 + 18 * 0.84) / 19, (2.11 + 18 * 0.756) / 19, (1.992 + 18 * 0.714) / 19,
                                           10, 90, false, 8.52 + 0.0826 * 60, 8.02 + 0.0778 * 60, 7.21 + 0.07 * 60, 6.81 + 0.0661 * 60,
                                           0, temp,
                                           new weapon_artifact_related_arguments(false, false, 2, true, 1, false, true, -1,
                                                                                 -1, -1, -1, -1, false, false, 3, 2)));//E 1+25，Q 60层愿力，不考虑后续平A
    temp.clear();
    //E 全体大招增伤0.3*Q_energy (special get_team) Q全体回能12.5 (recharge)

    temp.push_back(nullptr);
    temp.push_back(nullptr);
    temp.push_back(nullptr);
    temp.push_back(nullptr);
    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "元素精通", 130));//默认平均
    temp.push_back(nullptr);//与E一致，倍率为200%ATK+400%EM
    character_list.push_back(new Character("纳西妲", "nahida", "草", "法器", 10360, 299, 630, "元素精通", 115.0,
                                           6, "草", (0.726 + 0.666 + 0.826 + 1.051) / 4, (0.685 + 0.629 + 0.78 + 0.993) / 4,
                                           "草", 2.376, 2.244, "草", 2.81, 2.61,
                                           10, 7, false, 2.193, 2.064, 1.858, 1.754,
                                           10, 50, false, 0, 0, 0, 0,
                                           2, temp,
                                           new weapon_artifact_related_arguments(false, false, 2, true, 2, false, true, -1,
                                                                                 -1, 3, -1, -1, false, false, -1, -1)));
    temp.clear();
    //E 只考虑后续伤害 (extra_rate) Q、1命、2命(special) 2命、天赋1(get_team) 天赋2(get_convert)

    temp.push_back(new Set(new Condition("ALL", "ALL", "重A"), "暴击率", 0.2));//重A后
    temp.push_back(nullptr);//Q内前台角色 (special get_team)
    temp.push_back(new Set(new Condition("冰", "ALL", "ALL"), "抗性削弱", 0.15));//(get_team) 每次重A命中恢复2点能量 (recharge)
    temp.push_back(nullptr);//E变为2次 (recharge)
    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "伤害加成", 0.15));//Q内 (get_team)
    temp.push_back(nullptr);//E后瞬发重A
    character_list.push_back(new Character("甘雨", "ganyu", "冰", "弓", 9797, 335, 630, "暴击伤害", 0.384,
                                           10, "物理", (0.627 + 0.704 + 0.899 + 0.899 + 0.954 + 1.139) / 6, (0.583 + 0.654 + 0.836 + 0.836 + 0.886 + 1.059) / 6,
                                           "冰", (2.3 + 3.92) / 2, (2.18 + 3.7) / 2, "物理", 2.81, 2.61,
                                           10, 4, true, 2.81, 2.64, 2.38, 2.24,
                                           10, 60, true, 1.49, 1.41, 1.26, 1.19,
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
    character_list.push_back(new Character("夜兰", "yelan", "水", "弓", 14450, 244, 548, "暴击率", 0.192,
                                           6, "物理", (0.804 + 0.772 + 1.02 + 0.643 * 2) / 5, (0.747 + 0.717 + 0.948 + 0.597 * 2) / 5,
                                           "水", 0, 0, "物理", 2.81, 2.61,
                                           10, 4, false, 0, 0, 0, 0,
                                           10, 70, false, 0, 0, 0, 0,
                                           2, temp,
                                           new weapon_artifact_related_arguments(false, false, 2, false, 2, false, true, -1,
                                                                                 -1, -1, 3, 4, true, false, -1, -1)));
    temp.clear();
    //重A 生命倍率19.68% E 生命倍率38.4% Q 生命倍率(12.42+8.28*45)/46 (extra_rate)

    temp.push_back(nullptr);//E喷火距离提高20%
    temp.push_back(nullptr);//E结束拾取辣椒提高10%攻击 (get_team)，不考虑
    temp.push_back(new Set(new Condition("火", "ALL", "ALL"), "抗性削弱", 0.15));//E伤害 (get_team)
    temp.push_back(nullptr);//平A最后一击2s后造成75%火元素伤害
    temp.push_back(nullptr);//Q变为14s
    temp.push_back(new Set(new Condition("火", "ALL", "平A|重A|下落A|E"), "伤害加成", 0.15));//Q持续期间 (get_team)
    character_list.push_back(new Character("香菱", "xiangling", "火", "长柄武器", 10875, 225, 669, "元素精通", 96,
                                           6, "物理", (0.831 + 0.833 + 0.515 + 0.515 + 0.279 * 4 + 1.4) / 9, (0.773 + 0.774 + 0.479 + 0.479 + 0.259 * 4 + 1.31) / 9,
                                           "物理", 2.41, 2.24, "物理", 3.16, 2.93,
                                           9 + 3, 4, true, 2.36, 2.23, 2.0, 1.89,
                                           9 + 3, 80, true, (1.53 + 1.87 + 2.33 + 12 * 2.38) / 15, (1.44 + 1.76 + 2.19 + 12 * 2.24) / 15, (1.3 + 1.58 + 1.97 + 12 * 2.02) / 15, (1.22 + 1.5 + 1.86 + 12 * 1.9) / 15,
                                           6, temp,
                                           new weapon_artifact_related_arguments(false, false, 2, false, 1, false, true, -1,
                                                                                 -1, -1, -1, -1, false, false, 3, 1)));//E 4次，Q 12次
    temp.clear();

    temp.push_back(nullptr);//每柄雨帘剑恢复当前角色6%行秋最大生命
    temp.push_back(new Set(new Condition("水", "ALL", "ALL"), "伤害加成", 0.2));
    temp.push_back(nullptr);//雨帘剑变为4柄
    temp.push_back(new Set(new Condition("水", "ALL", "ALL"), "抗性削弱", 0.15));//Q变为18s Q命中 (get_team)
    temp.push_back(nullptr);//Q期间，E倍率*1.5 (special)
    temp.push_back(nullptr);//每2次攻击，下次攻击变为5柄剑，回复3点能量 (recharge)
    character_list.push_back(new Character("行秋", "xingqiu", "水", "单手剑", 10222, 202, 758, "攻击力", 0.24,
                                           6, "物理", (0.921 + 0.942 + 0.564 + 0.564 + 1.107 + 0.709 + 0.709) / 7, (0.856 + 0.875 + 0.525 + 0.525 + 1.029 + 0.659 + 0.659) / 7,
                                           "物理", (0.935 + 1.11) / 2, (0.869 + 1.032) / 2, "物理", 3.16, 2.93,
                                           9 + 3, 5, false, (3.57 * 1.5 + 4.06 * 1.5) / 2, (3.36 * 1.5 + 3.82 * 1.5) / 2, (3.02 * 1.5 + 3.44 * 1.5) / 2, (2.86 * 1.5 + 3.25 * 1.5) / 2,
                                           9 + 3, 80, false, 1.15, 1.09, 0.977, 0.923,
                                           6, temp,
                                           new weapon_artifact_related_arguments(false, false, 2, false, 1, false, true, 2,
                                                                                 1, -1, -1, -1, false, false, -1, -1)));
    temp.clear();
    //默认Q后E，触发4命

    temp.push_back(nullptr);//Q重置E
    temp.push_back(nullptr);//精通转化E伤害mastery*0.15% (convert)
    temp.push_back(nullptr);//每个E为Q后恢复8点能量，24点 (recharge)
    temp.push_back(nullptr);//E倍率提升25% (get_special)
    temp.push_back(new Set(new Condition("雷", "ALL", "ALL"), "伤害加成", 0.2));//E命中 (get_team)
    temp.push_back(new Set(new Condition("ALL", "ALL", "E"), "防御无视", 0.6));
    character_list.push_back(new Character("八重神子", "yaemiko", "雷", "法器", 10372, 340, 569, "暴击率", 0.192,
                                           6, "雷", (0.714 + 0.693 + 1.024) / 3, (0.674 + 0.655 + 0.967) / 3,
                                           "雷", 2.572, 2.429, "雷", 2.81, 2.61,
                                           10, 5, false, 2.015 * 1.25, 1.896 * 1.25, 1.706 * 1.25, 1.612 * 1.25,
                                           10, 90, false, (5.53 + 7.09 * 3) / 4, (5.2 + 6.68 * 3) / 4, (4.68 + 6.01 * 3) / 4, (4.42 + 5.68 * 3) / 4,
                                           2, temp,
                                           new weapon_artifact_related_arguments(false, false, 2, true, 3, false, true, -1,
                                                                                 -1, 3, -1, -1, false, false, -1, -1)));//E 15次
    temp.clear();

    temp.push_back(nullptr);//E产生上升气流20s
    temp.push_back(nullptr);//Q恢复自己与转化元素15点能量 (recharge)
    temp.push_back(nullptr);//重A发射分裂箭
    temp.push_back(new Set(new Condition("风|物理", "ALL", "ALL"), "抗性削弱", 0.24));//E后起飞0.24，落地0.12 (get_team)
    temp.push_back(new Set(new Condition("风", "ALL", "ALL"), "伤害加成", 0.25));//获得元素微粒
    temp.push_back(new Set(new Condition("火|水|雷|冰|风", "ALL", "ALL"), "抗性削弱", 0.2));//Q (get_team)
    character_list.push_back(new Character("温迪", "venti", "风", "弓", 10531, 263, 669, "元素充能效率", 0.32,
                                           6, "物理", (0.403 + 0.403 + 0.877 + 1.035 + 0.515 + 0.515 + 1.001 + 1.403) / 8, (0.375 + 0.375 + 0.815 + 0.962 + 0.479 + 0.479 + 0.931 + 1.304) / 8,
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
    character_list.push_back(new Character("莫娜", "mona", "水", "法器", 10409, 287, 653, "元素充能效率", 0.32,
                                           6, "水", (0.677 + 0.648 + 0.806 + 1.01) / 4, (0.639 + 0.612 + 0.762 + 0.955) / 4,
                                           "水", 2.69, 2.55, "水", 2.81, 2.61,
                                           9, 3, true, (0.68 * 4 + 2.82) / 5, (0.64 * 4 + 2.66) / 5, (0.576 * 4 + 2.39) / 5, (0.544 * 4 + 2.26) / 5,
                                           9 + 3, 60, false, 9.4, 8.85, 7.96, 7.52,
                                           4, temp,
                                           new weapon_artifact_related_arguments(false, false, 2, false, 1, false, true, -1,
                                                                                 -1, 1, -1, -1, false, false, -1, -1)));
    temp.clear();
    //Q 58%增伤(special get_team)

    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "护盾强效", 0.25));//护盾受伤害，5%强效/层，5层 (get_team)
    temp.push_back(nullptr);//普通攻击、重击与下落攻击造成的伤害提高值：生命值上限的1.39%。地心的岩脊、共鸣伤害与长按伤害提高值：生命值上限的1.9%。天星伤害提高值：生命值上限33% (extra_rate)
    temp.push_back(nullptr);//岩脊至多可以同时存在2个
    temp.push_back(nullptr);//Q加盾
    temp.push_back(nullptr);//Q影响范围扩大20%，石化效果持续时间延长2秒。
    temp.push_back(nullptr);//护盾生效期间，受到伤害的40%转为等额生命值恢复。单次恢复量不超过角色最大生命值的8%。
    character_list.push_back(new Character("钟离", "zhongli", "岩", "长柄武器", 14695, 251, 738, "伤害加成", 0.288,
                                           6, "物理", (0.608 + 0.616 + 0.763 + 0.849 + 0.213 * 4 + 1.077) / 9, (0.565 + 0.572 + 0.709 + 0.789 + 0.198 * 4 + 1.001) / 9,
                                           "物理", 2.2, 2.04, "物理", 3.16, 2.93,
                                           9, 5, true, 1.7, 1.6, 1.44, 1.36,
                                           9, 40, false, 10.84, 10.3, 9, 8.35,
                                           0, temp,
                                           new weapon_artifact_related_arguments(true, false, 2, true, 3, false, true, -1,
                                                                                 -1, -1, -1, -1, false, false, 3, 2)));//E长按不考虑后续伤害
    temp.clear();
    //E护盾减抗20%(special get_team)

    temp.push_back(nullptr);//重击或下落攻击命中敌人时，将产生一枚琢光镜。该效果每12秒至多触发一次。
    temp.push_back(nullptr);//(convert)
    temp.push_back(nullptr);//光幕攻击命中敌人时，将使共相·理式摹写的冷却时间减少1.2秒。该效果每1秒至多触发一次。->e cd 12s
    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "元素精通", 150));//每1枚产生的琢光镜将使元素精通提升50点，4层，持续8秒，最多4层。默认3层，8s
    temp.push_back(new Set(new Condition("草", "ALL", "ALL"), "伤害加成", 0.3));//Q每消耗1枚琢光镜，使队伍中的其他角色元素精通提升30点，持续15秒，每产生1枚琢光镜，使艾尔海森获得10%草元素伤害加成，持续15秒。默认先Q，3层，15s
    temp.push_back(nullptr);//(special)产生琢光镜时，若琢光镜数量已达到上限，艾尔海森的暴击率提升10%，暴击伤害提升70%，持续6秒，默认触发
    character_list.push_back(new Character("艾尔海森", "alhaitham", "草", "单手剑", 13348, 313, 782, "伤害加成", 0.288,
                                           10, "草", (0.979 + 1.003 + 0.676 + 0.676 + 1.32 + 1.658) / 6, (0.91 + 0.932 + 0.628 + 0.628 + 1.227 + 1.541) / 6,
                                           "草", 1.092 + 1.092, 1.015 + 1.015, "草", 3.16, 2.93,
                                           10, 7, false, 1.482, 1.344, 1.21, 1.142,
                                           10, 70, true, 2.584, 2.432, 2.189, 2.067,
                                           0, temp,
                                           new weapon_artifact_related_arguments(false, false, 2, true, 1, false, true, 3,
                                                                                 3, -1, -1, -1, false, false, -1, -1)));
    temp.clear();
    //E默认1枚，EQ精通倍率(extra_rate)，天赋2转化

    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "元素精通", 50));
    temp.push_back(nullptr);//(get_convert)
    temp.push_back(new Set(new Condition("ALL", "ALL", "重A"), "暴击率", 0.15));
    temp.push_back(new Set(new Condition("草", "ALL", "ALL"), "伤害加成", 0.2));
    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "元素精通", 120));//(get_team)
    temp.push_back(nullptr);//花筥箭所需的蓄力时间减少0.9秒，并在命中后能产生1枚额外的藏蕴花矢，能造成提纳里攻击力150%的伤害。
    character_list.push_back(new Character("提纳里", "tighnari", "草", "弓", 10850, 268, 630, "伤害加成", 0.288,
                                           9, "物理", (0.882 + 0.83 + 0.523 + 0.523 + 1.357) / 5, (0.82 + 0.771 + 0.486 + 0.486 + 1.261) / 5,
                                           "草", (1.57 + 0.695 * 4) / 5, (1.482 + 0.656 * 4) / 5, "物理", 2.81, 2.61,
                                           9, 3.5, false, 3.179, 2.992, 2.693, 2.543,
                                           9, 40, false, (1.182 + 1.445) / 2, (1.112 + 1.36) / 2, (1.001 + 1.224) / 2, (0.946 + 1.156) / 2,
                                           0, temp,
                                           new weapon_artifact_related_arguments(false, false, 1, false, 1, false, false, -1,
                                                                                 -1, -1, 2, 3, true, false, -1, -1)));
    temp.clear();
    //天赋2转化

    //NO NEED TO CALCULATE
    //BUILD BASIC INFORMATION AND ARGS
    temp.push_back(nullptr);
    temp.push_back(nullptr);
    temp.push_back(nullptr);
    temp.push_back(nullptr);
    temp.push_back(nullptr);
    temp.push_back(nullptr);
    character_list.push_back(new Character("枫原万叶", "kazuha", "风", "单手剑", 13348, 297, 807, "元素精通", 115.0,
                                           6, "物理", 0, 0, "物理", 0, 0, "物理", 0, 0,
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
    character_list.push_back(new Character("班尼特", "bennett", "火", "单手剑", 12397, 191, 771, "元素充能效率", 0.267,
                                           6, "物理", 0, 0, "物理", 0, 0, "物理", 0, 0,
                                           9 + 3, 2, false, 0, 0, 0, 0,
                                           9 + 3, 60, false, 0, 0, 0, 0,
                                           6, temp,
                                           new weapon_artifact_related_arguments(false, true, 2, false, 3, false, false, 3,
                                                                                 3, -1, -1, -1, false, false, -1, -1)));
    temp.clear();

    temp.push_back(nullptr);
    temp.push_back(nullptr);
    temp.push_back(nullptr);
    temp.push_back(nullptr);
    temp.push_back(nullptr);
    temp.push_back(nullptr);
    character_list.push_back(new Character("久岐忍", "kuki", "雷", "单手剑", 12289, 212, 751, "生命值", 0.24,
                                           1, "物理", 0, 0, "物理", 0, 0, "物理", 0, 0,
                                           6, 5, false, 0, 0, 0, 0,
                                           6, 60, false, 0, 0, 0, 0,
                                           0, temp,
                                           new weapon_artifact_related_arguments(false, true, 2, true, 1, false, true, 2,
                                                                                 3, -1, -1, -1, false, false, -1, -1)));
    temp.clear();

    //MODEL Character
    temp.push_back(nullptr);
    temp.push_back(nullptr);
    temp.push_back(nullptr);
    temp.push_back(nullptr);
    temp.push_back(nullptr);
    temp.push_back(nullptr);
    character_list.push_back(new Character("火_test", "", "火", "单手剑", 10000, 300, 700, "暴击率", 0.192,
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
    character_list.push_back(new Character("水_test", "", "水", "单手剑", 10000, 300, 700, "暴击率", 0.192,
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
    character_list.push_back(new Character("雷_test", "", "雷", "单手剑", 10000, 300, 700, "暴击率", 0.192,
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
    character_list.push_back(new Character("冰_test", "", "冰", "单手剑", 10000, 300, 700, "暴击率", 0.192,
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
    character_list.push_back(new Character("风_test", "", "风", "单手剑", 10000, 300, 700, "暴击率", 0.192,
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
    character_list.push_back(new Character("岩_test", "", "岩", "单手剑", 10000, 300, 700, "暴击率", 0.192,
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
    character_list.push_back(new Character("草_test", "", "草", "单手剑", 10000, 300, 700, "暴击率", 0.192,
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
    else if (data->c_point->name == "莫娜")
    {
        data->add_percentage("伤害加成", 0.6, (name + "_extra_special"));
        data->add_percentage("暴击率", 0.15, (name + "_extra_special"));
    }
    else if (data->c_point->name == "钟离")
    {
        data->add_percentage("抗性削弱", 0.2, (name + "_extra_special"));
    }
    else if (data->c_point->name == "纳西妲")
    {
        int pyro_num = 0;
        if (data->team_config->teammate_1->ele_type == "火") pyro_num++;
        if (data->team_config->teammate_2->ele_type == "火") pyro_num++;
        if (data->team_config->teammate_3->ele_type == "火") pyro_num++;
        if (data->c_point->constellation >= 1) pyro_num++;

        if (data->attack_config->condition->attack_way == "E")
        {
            if (pyro_num == 1) data->add_percentage("伤害加成", 0.268, (name + "_extra_special"));//lv10
            else if (pyro_num >= 2) data->add_percentage("伤害加成", 0.402, (name + "_extra_special"));//lv10
        }

        if (!(data->attack_config->background && !data->attack_config->lockface))
            data->add_converted_percentage("元素精通", 200, (name + "_extra_special"));

        if (data->c_point->constellation >= 2 && data->attack_config->react_type.find("激化") != string::npos)
            data->add_percentage("防御削弱", 0.3, (name + "_extra_special"));
    }
    else if (data->c_point->name == "艾尔海森")
    {
        if (data->c_point->constellation >= 6)
        {
            data->add_converted_percentage("暴击率", 0.1, (name + "_extra_special"));
            data->add_converted_percentage("暴击伤害", 0.7, (name + "_extra_special"));
        }
    }
    return true;
}

//build new weapon(all)
//高估：
//  原木刀/森林王器/贯月矢：自己吃不到种子（默认触发）
//  铁峰刺：元素伤害，2层6s（默认满层）
//  图莱杜拉的回忆：（默认叠满）
//      万国诸海图谱：元素反应，2层10s（默认满层）
//  匣里日月：EQ命中，6s（默认触发）
//  ！！！低估：流浪乐章：随机（默认无加成）
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
    weapon_list.push_back(new Weapon("雾切之回光", "mistsplitterreforged", "单手剑", 674, "暴击伤害", 0.441, 1, temp));
    temp.clear();

    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "攻击力", 0.2));
    weapon_list.push_back(new Weapon("风鹰剑", "aquilafavonia", "单手剑", 674, "伤害加成", 0.413, 1, temp));
    temp.clear();

    temp.push_back(new Set(new Condition("火|水|雷|冰|风|岩|草", "ALL", "ALL"), "伤害加成", 0.12));
    temp.push_back(new Set(new Condition("ALL", "ALL", "平A"), "伤害加成", 0.4));//队友E，2层
    weapon_list.push_back(new Weapon("波乱月白经津", "harangeppakufutsu", "单手剑", 608, "暴击率", 0.331, 1, temp));
    temp.clear();

    //(special)
    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "护盾强效", 0.2));
    weapon_list.push_back(new Weapon("斫峰之刃", "summitshaper", "单手剑", 608, "攻击力", 0.496, 1, temp));
    temp.clear();

    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "暴击率", 0.04));
    temp.push_back(new Set(new Condition("ALL", "ALL", "平A|重A"), "额外倍率", 0.2));//Q后
    weapon_list.push_back(new Weapon("天空之刃", "skywardblade", "单手剑", 608, "元素充能效率", 0.551, 1, temp));
    temp.clear();

    //(special)
    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "伤害加成", 0.1));
    weapon_list.push_back(new Weapon("苍古自由之誓", "freedomsworn", "单手剑", 608, "元素精通", 198.0, 1, temp));
    temp.clear();

    //(get_convert)
    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "生命值", 0.2));
    weapon_list.push_back(new Weapon("磐岩结绿", "primordialjadecutter", "单手剑", 542, "暴击率", 0.441, 1, temp));
    temp.clear();

    //(convert)
    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "暴击率", 0.04));
    weapon_list.push_back(new Weapon("裁叶萃光", "lightoffoliarincision", "单手剑", 542, "暴击伤害", 0.882, 1, temp));
    temp.clear();

    //(convert)
    //(get_team)
    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "生命值", 0.2));
    weapon_list.push_back(new Weapon("圣显之钥", "keyofkhajnisut", "单手剑", 542, "生命值", 0.662, 1, temp));
    temp.clear();

    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "伤害加成", 0.12));
    weapon_list.push_back(new Weapon("暗巷闪光", "thealleyflash", "单手剑", 620, "元素精通", 55.0, 1, temp));
    temp.clear();

    //(special)
    weapon_list.push_back(new Weapon("试作斩岩", "prototyperancour", "单手剑", 565, "伤害加成", 0.345, 5, temp));
    temp.clear();

    //(special) (get_team)
    weapon_list.push_back(new Weapon("原木刀", "sapwoodblade", "单手剑", 565, "元素充能效率", 0.306, 5, temp));
    temp.clear();

    temp.push_back(new Set(new Condition("ALL", "ALL", "平A|重A"), "伤害加成", 0.2));
    weapon_list.push_back(new Weapon("黑剑", "theblacksword", "单手剑", 510, "暴击率", 0.276, 1, temp));
    temp.clear();

    //(special)
    weapon_list.push_back(new Weapon("匣里龙吟", "lionsroar", "单手剑", 510, "攻击力", 0.413, 5, temp));
    temp.clear();

    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "攻击力", 0.15));//平A重A下落A
    weapon_list.push_back(new Weapon("笼钓瓶一心", "kagotsurubeisshin", "单手剑", 510, "攻击力", 0.413, 1, temp));
    temp.clear();

    weapon_list.push_back(new Weapon("笛剑", "theflute", "单手剑", 510, "攻击力", 0.413, 5, temp));
    temp.clear();

    temp.push_back(new Set(new Condition("ALL", "ALL", "E"), "伤害加成", 0.16));
    temp.push_back(new Set(new Condition("ALL", "ALL", "E"), "暴击率", 0.06));
    weapon_list.push_back(new Weapon("腐殖之剑", "festeringdesire", "单手剑", 510, "元素充能效率", 0.459, 5, temp));
    temp.clear();

    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "伤害加成", 0.12));//元素伤害，2层
    weapon_list.push_back(new Weapon("铁蜂刺", "ironsting", "单手剑", 510, "元素精通", 165.0, 5, temp));
    temp.clear();

    //(convert)
    //(get_team)
    weapon_list.push_back(new Weapon("西福斯的月光", "xiphosmoonlight", "单手剑", 510, "元素精通", 165.0, 1, temp));
    temp.clear();

    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "伤害加成", 0.16));
    weapon_list.push_back(new Weapon("东花坊时雨", "toukaboushigure", "单手剑", 510, "元素精通", 165.0, 5, temp));
    temp.clear();

    //(get_extra_rate)
    weapon_list.push_back(new Weapon("辰砂之纺锤", "cinnabarspindle", "单手剑", 454, "防御力", 0.69, 5, temp));
    temp.clear();

    //(recharge)
    weapon_list.push_back(new Weapon("天目影打刀", "amenomakageuchi", "单手剑", 454, "攻击力", 0.551, 5, temp));
    temp.clear();

    //(recharge)
    weapon_list.push_back(new Weapon("西风剑", "favoniussword", "单手剑", 454, "元素充能效率", 0.613, 5, temp));
    temp.clear();

    //(recharge)
    weapon_list.push_back(new Weapon("祭礼剑", "sacrificialsword", "单手剑", 454, "元素充能效率", 0.613, 5, temp));
    temp.clear();

    weapon_list.push_back(new Weapon("图莱杜拉的回忆", "tulaytullahsremembrance", "法器", 674, "暴击伤害", 0.441, 1, temp));
    temp.clear();

    temp.push_back(new Set(new Condition("火|水|雷|冰|风|岩|草", "ALL", "ALL"), "伤害加成", 0.12));
    weapon_list.push_back(new Weapon("天空之卷", "skywardatlas", "法器", 674, "攻击力", 0.331, 1, temp));
    temp.clear();

    //(special)
    weapon_list.push_back(new Weapon("四风原典", "lostprayertothesacredwinds", "法器", 608, "暴击率", 0.331, 1, temp));
    temp.clear();

    //(special)
    weapon_list.push_back(new Weapon("神乐之真意", "kagurasverity", "法器", 608, "暴击伤害", 0.662, 1, temp));
    temp.clear();

    //(special)
    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "护盾强效", 0.2));
    weapon_list.push_back(new Weapon("尘世之锁", "memoryofdust", "法器", 608, "攻击力", 0.496, 1, temp));
    temp.clear();

    //(get_extra_rate)
    //(recharge)
    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "治疗加成", 0.1));
    weapon_list.push_back(new Weapon("不灭月华", "everlastingmoonglow", "法器", 608, "生命值", 0.496, 1, temp));
    temp.clear();

    //(special)
    //(get_team)
    weapon_list.push_back(new Weapon("千夜浮梦", "athousandfloatingdreams", "法器", 542, "元素精通", 265.0, 1, temp));
    temp.clear();

    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "元素充能效率", 0.24));//E后
    weapon_list.push_back(new Weapon("证誓之明瞳", "oathsworneye", "法器", 565, "攻击力", 0.276, 5, temp));
    temp.clear();

    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "攻击力", 0.2));//冲刺
    weapon_list.push_back(new Weapon("暗巷的酒与诗", "wineandsong", "法器", 565, "元素充能效率", 0.306, 1, temp));
    temp.clear();

    //(special) (get_team)
    weapon_list.push_back(new Weapon("白辰之环", "hakushinring", "法器", 565, "元素充能效率", 0.306, 5, temp));
    temp.clear();

    //(special)
    weapon_list.push_back(new Weapon("万国诸海图谱", "mappamare", "法器", 565, "元素精通", 110.0, 5, temp));
    temp.clear();

    //(special)
    weapon_list.push_back(new Weapon("匣里日月", "solarpearl", "法器", 510, "暴击率", 0.276, 1, temp));
    temp.clear();

    //(special)
    weapon_list.push_back(new Weapon("流浪乐章", "thewidsith", "法器", 510, "暴击伤害", 0.551, 5, temp));
    temp.clear();

    weapon_list.push_back(new Weapon("忍冬之果", "frostbearer", "法器", 510, "攻击力", 0.413, 5, temp));
    temp.clear();

    //(recharge)
    weapon_list.push_back(new Weapon("试作金珀", "prototypeamber", "法器", 510, "生命值", 0.413, 5, temp));
    temp.clear();

    //(recharge)
    weapon_list.push_back(new Weapon("西风秘典", "favoniuscodex", "法器", 510, "元素充能效率", 0.459, 5, temp));
    temp.clear();

    //(convert)
    //(get_team)
    weapon_list.push_back(new Weapon("流浪的晚星", "wanderingevenstar", "法器", 510, "元素精通", 165.0, 1, temp));
    temp.clear();

    //(special)
    weapon_list.push_back(new Weapon("盈满之实", "fruitoffulfillment", "法器", 510, "元素充能效率", 0.459, 5, temp));
    temp.clear();

    weapon_list.push_back(new Weapon("昭心", "eyeofperception", "法器", 454, "攻击力", 0.551, 5, temp));
    temp.clear();

    //(special)
    weapon_list.push_back(new Weapon("嘟嘟可故事集", "dodocotales", "法器", 454, "攻击力", 0.551, 5, temp));
    temp.clear();

    //(recharge)
    weapon_list.push_back(new Weapon("祭礼残章", "sacrificialfragments", "法器", 454, "元素精通", 221.0, 5, temp));
    temp.clear();

    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "暴击伤害", 0.2));
    weapon_list.push_back(new Weapon("天空之翼", "skywardharp", "弓", 674, "暴击率", 0.221, 1, temp));
    temp.clear();

    //(special)
    temp.push_back(new Set(new Condition("ALL", "ALL", "E|Q"), "伤害加成", 0.12));
    weapon_list.push_back(new Weapon("冬极白星", "polarstar", "弓", 608, "暴击率", 0.331, 1, temp));
    temp.clear();

    //(special)
    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "攻击力", 0.2));
    weapon_list.push_back(new Weapon("飞雷之弦振", "thunderingpulse", "弓", 608, "暴击伤害", 0.662, 1, temp));
    temp.clear();

    //高估
    temp.push_back(new Set(new Condition("ALL", "ALL", "平A|重A"), "伤害加成", 0.12));
    temp.push_back(new Set(new Condition("ALL", "ALL", "平A"), "伤害加成", 0.16));//2层
    temp.push_back(new Set(new Condition("ALL", "ALL", "重A"), "伤害加成", 0.32));//4层
    weapon_list.push_back(new Weapon("阿莫斯之弓", "amosbow", "弓", 608, "攻击力", 0.496, 1, temp));
    temp.clear();

    //(get_team)
    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "元素精通", 60.0));
    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "元素精通", 100.0));
    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "攻击力", 0.2));
    weapon_list.push_back(new Weapon("终末嗟叹之诗", "elegyfortheend", "弓", 608, "元素充能效率", 0.551, 1, temp));
    temp.clear();

    //(get_extra_rate)
    temp.push_back(new Set(new Condition("火|水|雷|冰|风|岩|草", "ALL", "ALL"), "伤害加成", 0.12));
    weapon_list.push_back(new Weapon("猎人之径", "hunterspath", "弓", 542, "暴击率", 0.441, 1, temp));
    temp.clear();

    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "生命值", 0.16));
    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "伤害加成", 0.2));
    weapon_list.push_back(new Weapon("若水", "aquasimulacra", "弓", 542, "暴击伤害", 0.882, 1, temp));
    temp.clear();

    //(special)
    weapon_list.push_back(new Weapon("暗巷猎手", "alleyhunter", "弓", 565, "攻击力", 0.276, 1, temp));
    temp.clear();

    //(special)
    weapon_list.push_back(new Weapon("曚云之月", "mouunsmoon", "弓", 565, "攻击力", 0.276, 2, temp));
    temp.clear();

    //(recharge)
    weapon_list.push_back(new Weapon("祭礼弓", "sacrificialbow", "弓", 565, "元素充能效率", 0.306, 5, temp));
    temp.clear();

    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "伤害加成", 0.1));
    weapon_list.push_back(new Weapon("落霞", "fadingtwilight", "弓", 565, "元素充能效率", 0.306, 5, temp));
    temp.clear();

    weapon_list.push_back(new Weapon("苍翠猎弓", "theviridescenthunt", "弓", 510, "暴击率", 0.276, 1, temp));
    temp.clear();

    //(special)
    temp.push_back(new Set(new Condition("ALL", "ALL", "平A"), "伤害加成", 0.4));
    weapon_list.push_back(new Weapon("弓藏", "rust", "弓", 510, "攻击力", 0.413, 5, temp));
    temp.clear();

    weapon_list.push_back(new Weapon("试作澹月", "prototypecrescent", "弓", 510, "攻击力", 0.413, 5, temp));
    temp.clear();

    //(special)
    weapon_list.push_back(new Weapon("幽夜华尔兹", "mitternachtswaltz", "弓", 510, "伤害加成", 0.517, 1, temp));
    temp.clear();

    weapon_list.push_back(new Weapon("竭泽", "endoftheline", "弓", 510, "元素充能效率", 0.459, 5, temp));
    temp.clear();

    temp.push_back(new Set(new Condition("ALL", "ALL", "E|Q"), "伤害加成", 0.24));
    weapon_list.push_back(new Weapon("绝弦", "thestringless", "弓", 510, "元素精通", 165.0, 5, temp));
    temp.clear();

    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "攻击力", 0.16));//E后
    weapon_list.push_back(new Weapon("风花之颂", "windblumeode", "弓", 510, "元素精通", 165.0, 5, temp));
    temp.clear();

    //(special)
    weapon_list.push_back(new Weapon("破魔之弓", "hamayumi", "弓", 454, "攻击力", 0.551, 5, temp));
    temp.clear();

    //(special)
    weapon_list.push_back(new Weapon("王下近侍", "kingssquire", "弓", 454, "攻击力", 0.551, 5, temp));
    temp.clear();

    //(special)
    weapon_list.push_back(new Weapon("钢轮弓", "compoundbow", "弓", 454, "伤害加成", 0.69, 5, temp));
    temp.clear();

    //(recharge)
    weapon_list.push_back(new Weapon("西风猎弓", "favoniuswarbow", "弓", 454, "元素充能效率", 0.613, 5, temp));
    temp.clear();

    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "攻击力", 0.16));
    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "攻击力", 0.2));//平A重A4次 (get_team)
    weapon_list.push_back(new Weapon("松籁响起之时", "songofbrokenpines", "双手剑", 741, "伤害加成", 0.207, 1, temp));
    temp.clear();

    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "伤害加成", 0.08));
    weapon_list.push_back(new Weapon("天空之傲", "skywardpride", "双手剑", 674, "元素充能效率", 0.368, 1, temp));
    temp.clear();

    //TODO:NEW
    //(special)
    weapon_list.push_back(new Weapon("苇海信标", "beaconofthereedsea", "双手剑", 608, "暴击率", 0.331, 1, temp));
    temp.clear();

    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "攻击力", 0.2));//默认不触发特效
    weapon_list.push_back(new Weapon("狼的末路", "wolfsgravestone", "双手剑", 608, "攻击力", 0.496, 1, temp));
    temp.clear();

    //(special)
    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "护盾强效", 0.2));
    weapon_list.push_back(new Weapon("无工之剑", "theunforged", "双手剑", 608, "攻击力", 0.496, 1, temp));
    temp.clear();

    //(get_extra_rate)
    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "防御力", 0.28));
    weapon_list.push_back(new Weapon("赤角石溃杵", "redhornstonethresher", "双手剑", 542, "暴击伤害", 0.882, 1, temp));
    temp.clear();

    weapon_list.push_back(new Weapon("试作古华", "prototypearchaic", "双手剑", 565, "攻击力", 0.276, 5, temp));
    temp.clear();

    weapon_list.push_back(new Weapon("雪葬的星银", "snowtombedstarsilver", "双手剑", 565, "伤害加成", 0.345, 5, temp));
    temp.clear();

    //(recharge)
    weapon_list.push_back(new Weapon("祭礼大剑", "sacrificialgreatsword", "双手剑", 565, "元素充能效率", 0.306, 5, temp));
    temp.clear();

    //(special) (get_team)
    weapon_list.push_back(new Weapon("森林王器", "forestregalia", "双手剑", 565, "元素充能效率", 0.306, 5, temp));
    temp.clear();

    //TODO:NEW
    //(special)
    weapon_list.push_back(new Weapon("饰铁之花", "mailedflower", "双手剑", 565, "元素精通", 110.0, 5, temp));
    temp.clear();

    //(special)
    weapon_list.push_back(new Weapon("螭骨剑", "serpentspine", "双手剑", 510, "暴击率", 0.276, 5, temp));
    temp.clear();

    //(special)
    weapon_list.push_back(new Weapon("白影剑", "whiteblind", "双手剑", 510, "防御力", 0.517, 5, temp));
    temp.clear();

    //(special)
    weapon_list.push_back(new Weapon("千岩古剑", "lithicblade", "双手剑", 510, "攻击力", 0.413, 1, temp));
    temp.clear();

    //(special)
    weapon_list.push_back(new Weapon("恶王丸", "akuoumaru", "双手剑", 510, "攻击力", 0.413, 1, temp));
    temp.clear();

    //(special)
    weapon_list.push_back(new Weapon("钟剑", "thebell", "双手剑", 510, "生命值", 0.413, 5, temp));
    temp.clear();

    //(recharge)
    temp.push_back(new Set(new Condition("ALL", "ALL", "E"), "伤害加成", 0.06));
    weapon_list.push_back(new Weapon("桂木斩长正", "katsuragikirinagamasa", "双手剑", 510, "元素充能效率", 0.459, 5, temp));
    temp.clear();

    //(special)
    weapon_list.push_back(new Weapon("雨裁", "rainslasher", "双手剑", 510, "元素精通", 165.0, 5, temp));
    temp.clear();

    //(convert)
    //(get_team)
    weapon_list.push_back(new Weapon("玛海菈的水色", "makhairaaquamarine", "双手剑", 510, "元素精通", 165.0, 1, temp));
    temp.clear();

    //(recharge)
    weapon_list.push_back(new Weapon("西风大剑", "favoniusgreatsword", "双手剑", 454, "元素充能效率", 0.613, 5, temp));
    temp.clear();

    temp.push_back(new Set(new Condition("ALL", "ALL", "Q"), "伤害加成", 0.12));
    weapon_list.push_back(new Weapon("衔珠海皇", "luxurioussealord", "双手剑", 454, "攻击力", 0.551, 5, temp));
    temp.clear();

    //(special)
    temp.push_back(new Set(new Condition("火|水|雷|冰|风|岩|草", "ALL", "ALL"), "伤害加成", 0.12));
    weapon_list.push_back(new Weapon("息灾", "calamityqueller", "长柄武器", 741, "攻击力", 0.165, 1, temp));
    temp.clear();

    //(special)
    weapon_list.push_back(new Weapon("和璞鸢", "primordialjadewingedspear", "长柄武器", 674, "暴击率", 0.221, 1, temp));
    temp.clear();

    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "暴击率", 0.08));
    weapon_list.push_back(new Weapon("天空之脊", "skywardspine", "长柄武器", 674, "元素充能效率", 0.368, 1, temp));
    temp.clear();

    //(get_convert)
    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "生命值", 0.2));
    weapon_list.push_back(new Weapon("护摩之杖", "staffofhoma", "长柄武器", 608, "暴击伤害", 0.662, 1, temp));
    temp.clear();

    //(special)
    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "护盾强效", 0.2));
    weapon_list.push_back(new Weapon("贯虹之槊", "vortexvanquisher", "长柄武器", 608, "攻击力", 0.496, 1, temp));
    temp.clear();

    //(special) (get_convert)
    weapon_list.push_back(new Weapon("薙草之稻光", "engulfinglightning", "长柄武器", 608, "元素充能效率", 0.551, 1, temp));
    temp.clear();

    //(convert)
    //(get_team)
    weapon_list.push_back(new Weapon("赤沙之杖", "staffofthescarletsands", "长柄武器", 542, "暴击率", 0.441, 1, temp));
    temp.clear();

    //(special)
    weapon_list.push_back(new Weapon("断浪长鳍", "wavebreakersfin", "长柄武器", 620, "攻击力", 0.138, 4, temp));
    temp.clear();

    //(special)
    weapon_list.push_back(new Weapon("千岩长枪", "lithicspear", "长柄武器", 565, "攻击力", 0.276, 1, temp));
    temp.clear();

    temp.push_back(new Set(new Condition("ALL", "ALL", "平A|重A"), "额外倍率", 0.2));
    weapon_list.push_back(new Weapon("流月针", "crescentpike", "长柄武器", 565, "伤害加成", 0.345, 5, temp));
    temp.clear();

    //(recharge)
    weapon_list.push_back(new Weapon("西风长枪", "favoniuslance", "长柄武器", 565, "元素充能效率", 0.306, 5, temp));
    temp.clear();

    //(recharge)
    temp.push_back(new Set(new Condition("ALL", "ALL", "E"), "伤害加成", 0.06));
    weapon_list.push_back(new Weapon("喜多院十文字", "kitaincrossspear", "长柄武器", 565, "元素精通", 110.0, 5, temp));
    temp.clear();

    //(special) (get_team)
    weapon_list.push_back(new Weapon("贯月矢", "moonpiercer", "长柄武器", 565, "元素精通", 110.0, 5, temp));
    temp.clear();

    //(special)
    weapon_list.push_back(new Weapon("试作星镰", "prototypestarglitter", "长柄武器", 510, "元素充能效率", 0.459, 5, temp));
    temp.clear();

    //(special)
    weapon_list.push_back(new Weapon("风信之锋", "missivewindspear", "长柄武器", 510, "攻击力", 0.413, 5, temp));
    temp.clear();

    temp.push_back(new Set(new Condition("ALL", "ALL", "Q"), "伤害加成", 0.16));
    temp.push_back(new Set(new Condition("ALL", "ALL", "Q"), "暴击率", 0.06));
    weapon_list.push_back(new Weapon("渔获", "thecatch", "长柄武器", 510, "元素充能效率", 0.459, 5, temp));
    temp.clear();

    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "攻击力", 0.24));//单怪条件
    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "防御力", 0.16));//多怪条件
    weapon_list.push_back(new Weapon("决斗之枪", "deathmatch", "长柄武器", 454, "暴击率", 0.368, 3, temp));
    temp.clear();

    weapon_list.push_back(new Weapon("龙脊长枪", "dragonspinespear", "长柄武器", 454, "伤害加成", 0.69, 5, temp));
    temp.clear();

    //(special)
    weapon_list.push_back(new Weapon("匣里灭辰", "dragonsbane", "长柄武器", 454, "元素精通", 221.0, 5, temp));
    temp.clear();

    //(special)
    weapon_list.push_back(new Weapon("讨龙英杰谭", "thrillingtalesofdragonslayers", "法器", 401, "生命值", 0.352, 5, temp));
    temp.clear();
}

//build new weapon(all)
//TODO:important 技能获取的叠层不同，所有叠层武器均存在该问题，都默认以最大加成计算
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

    else if (name == "图莱杜拉的回忆")
    {
        if (!data->attack_config->background)
            if (data->attack_config->condition->attack_way == "平A")
                data->add_percentage("伤害加成", (0.48 * (0.75 + level * 0.25)), (name + "_extra_special"));
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
        //流浪乐章默认无加成
//    else if (name == "流浪乐章")
//    {
//        //每个各取1/4
//        data->add_percentage("攻击力", (0.15 * (0.75 + level * 0.25)), (name + "_extra_special"));
//        data->add_percentage("伤害加成", (0.12 * (0.75 + level * 0.25)), (name + "_extra_special"));
//        data->add_percentage("元素精通", (60.0 * (0.75 + level * 0.25)), (name + "_extra_special"));
//    }
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

        //TODO:NEW
    else if (name == "苇海信标")
    {
        data->add_percentage("攻击力", (0.2 * (0.75 + level * 0.25)), (name + "_extra_special"));//E命中
        data->add_percentage("攻击力", (0.2 * (0.75 + level * 0.25)), (name + "_extra_special"));//受伤
        if (!data->c_point->args->shield_sustain && !data->team_config->teammate_1->args->shield_sustain && !data->team_config->teammate_2->args->shield_sustain && !data->team_config->teammate_3->args->shield_sustain)
            data->add_percentage("生命值", (0.32 * (0.75 + level * 0.25)), (name + "_extra_special"));//不处于护盾下，条件弱
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
        //TODO:NEW
    else if (name == "饰铁之花")
    {
        //E命中或元素反应
        data->add_percentage("攻击力", (0.12 * (0.75 + level * 0.25)), (name + "_extra_special"));
        data->add_percentage("元素精通", (48.0 * (0.75 + level * 0.25)), (name + "_extra_special"));
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
    else if (data->w_point->name == "裁叶萃光" && (data->attack_config->condition->attack_way == "平A" || data->attack_config->condition->attack_way == "E")) data->data_list[4]->useful = true;
    else if (data->w_point->name == "圣显之钥" && data->c_point->args->sword_shengxian_level > 0 && data->data_list[4]->useful) data->data_list[0]->useful = true;
    else if (data->w_point->name == "辰砂之纺锤" && data->attack_config->condition->attack_way == "E") data->data_list[2]->useful = true;
    else if (data->w_point->name == "西福斯的月光" && (data->data_list[5]->useful || data->need_to_satisfy_recharge)) data->data_list[4]->useful = true;
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
    artifact_list.push_back(new Artifact("EMPTY", "EMPTY", nullptr, nullptr));

    artifact_list.push_back(new Artifact("流浪大地的乐团", "wandererstroupe", new Set(new Condition("ALL", "ALL", "ALL"), "元素精通", 80.0),
                                         new Set(new Condition("ALL", "法器|弓", "重A"), "伤害加成", 0.35)));
    artifact_list.push_back(new Artifact("角斗士的终幕礼", "gladiatorsfinale", new Set(new Condition("ALL", "ALL", "ALL"), "攻击力", 0.18),
                                         new Set(new Condition("ALL", "单手剑|双手剑|长柄武器", "平A"), "伤害加成", 0.35)));
    artifact_list.push_back(new Artifact("冰风迷途的勇士", "blizzardstrayer", new Set(new Condition("冰", "ALL", "ALL"), "伤害加成", 0.15),
                                         nullptr));//(special)
    artifact_list.push_back(new Artifact("沉沦之心", "heartofdepth", new Set(new Condition("水", "ALL", "ALL"), "伤害加成", 0.15),
                                         new Set(new Condition("ALL", "ALL", "平A|重A"), "伤害加成", 0.3)));
    artifact_list.push_back(new Artifact("悠古的磐岩", "archaicpetra", new Set(new Condition("岩", "ALL", "ALL"), "伤害加成", 0.15),
                                         nullptr));//(get_team)
    artifact_list.push_back(new Artifact("逆飞的流星", "retracingbolide", new Set(new Condition("ALL", "ALL", "ALL"), "护盾强效", 0.35),
                                         nullptr));//(special)
    artifact_list.push_back(new Artifact("染血的骑士道", "bloodstainedchivalry", new Set(new Condition("物理", "ALL", "ALL"), "伤害加成", 0.25),
                                         nullptr));//new Set(new Condition("ALL", "ALL", "重A"), "伤害加成", 0.5)));//击败敌人
    artifact_list.push_back(new Artifact("昔日宗室之仪", "noblesseoblige", new Set(new Condition("ALL", "ALL", "Q"), "伤害加成", 0.2),
                                         new Set(new Condition("ALL", "ALL", "ALL"), "攻击力", 0.2)));//(get_team) 高估
    artifact_list.push_back(new Artifact("翠绿之影", "viridescentvenerer", new Set(new Condition("风", "ALL", "ALL"), "伤害加成", 0.15),
                                         nullptr));//(react) (get_team)
    artifact_list.push_back(new Artifact("被怜爱的少女", "maidenbeloved", new Set(new Condition("ALL", "ALL", "ALL"), "治疗加成", 0.15),
                                         nullptr));//受治疗加成
    artifact_list.push_back(new Artifact("苍白之火", "paleflame", new Set(new Condition("物理", "ALL", "ALL"), "伤害加成", 0.25),
                                         nullptr));//(special)
    artifact_list.push_back(new Artifact("千岩牢固", "tenacityofthemillelith", new Set(new Condition("ALL", "ALL", "ALL"), "生命值", 0.2),
                                         nullptr));//(special) (get_team)
    artifact_list.push_back(new Artifact("炽烈的炎之魔女", "crimsonwitchofflames", new Set(new Condition("火", "ALL", "ALL"), "伤害加成", 0.15),
                                         nullptr));//(special) (react)
    artifact_list.push_back(new Artifact("渡过烈火的贤人", "lavawalker", nullptr, nullptr));//(special)
    artifact_list.push_back(new Artifact("如雷的盛怒", "thunderingfury", new Set(new Condition("雷", "ALL", "ALL"), "伤害加成", 0.15),
                                         nullptr));//(react) 减CD
    artifact_list.push_back(new Artifact("平息鸣雷的尊者", "thundersoother", nullptr, nullptr));//(special)
    artifact_list.push_back(new Artifact("追忆之注连", "shimenawasreminiscence", new Set(new Condition("ALL", "ALL", "ALL"), "攻击力", 0.18),
                                         nullptr));//new Set(new Condition("ALL", "ALL", "平A|重A|下落A"), "伤害加成", 0.5)));//15能量
    artifact_list.push_back(new Artifact("绝缘之旗印", "emblemofseveredfate", new Set(new Condition("ALL", "ALL", "ALL"), "元素充能效率", 0.2),
                                         nullptr));//(get_convert)
    artifact_list.push_back(new Artifact("华馆梦醒形骸记", "huskofopulentdreams", new Set(new Condition("ALL", "ALL", "ALL"), "防御力", 0.3),
                                         nullptr));//(special)
    artifact_list.push_back(new Artifact("海染砗磲", "oceanhuedclam", new Set(new Condition("ALL", "ALL", "ALL"), "治疗加成", 0.15),
                                         nullptr));//额外物理伤害
    artifact_list.push_back(new Artifact("辰砂往生录", "vermillionhereafter", new Set(new Condition("ALL", "ALL", "ALL"), "攻击力", 0.18),
                                         nullptr));//(special)
    artifact_list.push_back(new Artifact("来歆余响", "echoesofanoffering", new Set(new Condition("ALL", "ALL", "ALL"), "攻击力", 0.18),
                                         new Set(new Condition("ALL", "ALL", "平A"), "额外倍率", 0.35)));
    artifact_list.push_back(new Artifact("深林的记忆", "deepwoodmemories", new Set(new Condition("草", "ALL", "ALL"), "伤害加成", 0.15),
                                         nullptr));//(special) (get_team)
    artifact_list.push_back(new Artifact("饰金之梦", "gildeddreams", new Set(new Condition("ALL", "ALL", "ALL"), "元素精通", 80.0),
                                         nullptr));//(special)
    artifact_list.push_back(new Artifact("沙上楼阁史话", "desertpavilionchronicle", new Set(new Condition("风", "ALL", "ALL"), "伤害加成", 0.15),
                                         nullptr));
    artifact_list.push_back(new Artifact("乐园遗落之花", "flowerofparadiselost", new Set(new Condition("ALL", "ALL", "ALL"), "元素精通", 80.0),
                                         nullptr));//(react)
}

//build new artifact(all) 保证二件套效果和四件套效果分开
//TODO:important 技能获取的叠层不同，所有叠层圣遗物均存在该问题，都默认以最大加成计算
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
    else if (if_4_piece && name == "沙上楼阁史话")
    {
        if (!data->attack_config->background)
            if (data->attack_config->condition->attack_way == "平A" || data->attack_config->condition->attack_way == "重A" || data->attack_config->condition->attack_way == "下落A")
                data->add_percentage("伤害加成", 0.4, (name + "_extra_special"));
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
        else if (data->suit1->name == "乐园遗落之花")
        {
            if (data->attack_config->react_type.find("绽放") != string::npos)
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
        //只允许 角斗+其他/追忆 染血+其他/苍白 少女+其他/海染 乐团+其他/饰金 风套+其他/楼阁
        //suit1
        if ((data->suit1->name == "追忆之注连" && data->suit2->name != "角斗士的终幕礼") || data->suit1->name == "辰砂往生录" || data->suit1->name == "来歆余响") suit1_valid = false;//原来肯定-现在否定；原来否定-现在否定
        if (data->suit1->name == "苍白之火" && data->suit2->name != "染血的骑士道") suit1_valid = false;//原来肯定-现在否定；原来否定-现在否定
        if (data->suit1->name == "海染砗磲" && data->suit2->name != "被怜爱的少女") suit1_valid = false;//原来肯定-现在否定；原来否定-现在否定
        if ((data->suit1->name == "饰金之梦" && data->suit2->name != "流浪大地的乐团") || data->suit1->name == "乐园遗落之花") suit1_valid = false;//原来肯定-现在否定；原来否定-现在否定
        if (data->suit1->name == "沙上楼阁史话" && data->suit2->name != "翠绿之影") suit1_valid = false;//原来肯定-现在否定；原来否定-现在否定
        //suit2
        if ((data->suit2->name == "追忆之注连" && data->suit1->name != "角斗士的终幕礼") || data->suit2->name == "辰砂往生录" || data->suit2->name == "来歆余响") suit2_valid = false;//原来肯定-现在否定；原来否定-现在否定
        if (data->suit2->name == "苍白之火" && data->suit1->name != "染血的骑士道") suit2_valid = false;//原来肯定-现在否定；原来否定-现在否定
        if (data->suit2->name == "海染砗磲" && data->suit1->name != "被怜爱的少女") suit2_valid = false;//原来肯定-现在否定；原来否定-现在否定
        if ((data->suit2->name == "饰金之梦" && data->suit1->name != "流浪大地的乐团") || data->suit2->name == "乐园遗落之花") suit2_valid = false;//原来肯定-现在否定；原来否定-现在否定
        if (data->suit2->name == "沙上楼阁史话" && data->suit1->name != "翠绿之影") suit2_valid = false;//原来肯定-现在否定；原来否定-现在否定
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
    else if (w_point->name == "裁叶萃光")
    {
        if (base_skillrate != 0)
            if ((data_list[4]->value_per_entry * 1.2 * (0.75 + w_point->level * 0.25) / (base_skillrate * base_atk)) < data_list[1]->value_per_entry)
                data_list[4]->useful = attack_config->useful_attributes[4];
        if (attack_config->condition->attack_way != "平A" && attack_config->condition->attack_way != "E")
            data_list[4]->useful = attack_config->useful_attributes[4];
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
        if (attack_config->condition->attack_way != "E")
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
        if (attack_config->condition->attack_way != "平A")
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
        if (attack_config->condition->attack_way != "重A")
            data_list[4]->useful = attack_config->useful_attributes[4];
    }
    else if (w_point->name == "赤角石溃杵")
    {
        if (base_skillrate != 0)
            if ((data_list[2]->value_per_entry * 0.4 * (0.75 + w_point->level * 0.25) * base_def / (base_skillrate * base_atk)) < data_list[1]->value_per_entry)
                data_list[2]->useful = attack_config->useful_attributes[2];
        if (attack_config->condition->attack_way != "平A" && attack_config->condition->attack_way != "重A")
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
    if (c_point->name == "钟离") data_list[1]->useful = false;//考虑盾
//    if (c_point->name == "艾尔海森") data_list[1]->useful = false;//精通>攻击，除非有攻击转什么
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
        if (attack_config->condition->ele_type == "火" && (!(attack_config->background && !attack_config->lockface)))
            add_percentage("伤害加成", 0.15, "team_班尼特");
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
        add_percentage("伤害加成", 0.6, "team_莫娜");
        //constellation>=4 Q
        add_percentage("暴击率", 0.15, "team_莫娜");
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
    if (team_config->teammate_all.find("久岐忍") != string::npos)
    {
        Electro_num++;
    }
    if (team_config->teammate_all.find("艾尔海森") != string::npos)
    {
        //constellation>=4
//        add_percentage("元素精通", 90, "team_艾尔海森");
        Dendro_num++;
    }
    if (team_config->teammate_all.find("提纳里") != string::npos)
    {
        //constellation>=4
//        add_percentage("元素精通", 120, "team_提纳里");
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
    int electro_num = 0;
    if (c_point->ele_type == "雷") electro_num++;
    if (team_config->teammate_1->ele_type == "雷") electro_num++;
    if (team_config->teammate_2->ele_type == "雷") electro_num++;
    if (team_config->teammate_3->ele_type == "雷") electro_num++;
    if (electro_num >= 2) energy += (3 * back + 1 * front) * ((team_config->teammate_3->ele_type == "雷") ? same : diff);

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
        energy += ((double_E_per_round.find(c_point->name) != string::npos) ? 2 : 1) * c_point->E_energy * front * same;

        if (w_point->name == "西风长枪") energy += 3 * front * white;
        else if (w_point->name == "喜多院十文字") Q_energy_modify -= 12;
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
        energy = ((double_E_per_round.find(c_point->name) != string::npos) ? 2 : 1) * c_point->E_energy * front * same;

        if (w_point->name == "西风长枪") energy += 3 * front * white;
        if (w_point->name == "喜多院十文字") Q_energy_modify -= 12;
    }
    else if (c_point->name == "纳西妲")
    {
        //Q 50 E 6b 2E/Q
        energy += ((double_E_per_round.find(c_point->name) != string::npos) ? 2 : 1) * c_point->E_energy * back * same;

        if (w_point->name == "西风秘典") energy += 3 * front * white;
        else if (w_point->name == "祭礼残章") energy += 0;
        else if (w_point->name == "试作金珀") Q_energy_modify -= 18;
        //"不灭月华" 0.6/A Q后12s内 不吃充能
    }
    else if (c_point->name == "艾尔海森")
    {
        //Q 70 E 5f 1E/Q
        energy += ((double_E_per_round.find(c_point->name) != string::npos) ? 2 : 1) * c_point->E_energy * front * same;

        if (w_point->name == "西风剑") energy += 3 * front * white;
        else if (w_point->name == "祭礼剑") energy += 0;
        else if (w_point->name == "天目影打刀") Q_energy_modify -= 12;
        else if (w_point->name == "西福斯的月光") converted_recharge += data_list[4]->percentage * 0.00036 * (0.75 + w_point->level * 0.25);
    }
    else if (c_point->name == "提纳里")
    {
        //Q 40 E 3f 1E/Q
        energy += ((double_E_per_round.find(c_point->name) != string::npos) ? 2 : 1) * c_point->E_energy * front * same;

        if (w_point->name == "西风猎弓") energy += 3 * front * white;
        else if (w_point->name == "祭礼弓") energy += c_point->E_energy * front * same;
    }
    else energy = Q_energy_modify;

    min_recharge_num = max(0, (int) ((Q_energy_modify / energy - data_list[5]->percentage - data_list[5]->converted_percentage - converted_recharge) / data_list[5]->value_per_entry));
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
    else if (c_point->name == "纳西妲" && attack_config->condition->attack_way == "E")//精通->E暴击增伤
    {
        damplus_add += min((mastery + data_list[4]->converted_percentage - 200.0), 800.0) * 0.001;
        critrate_add += min((mastery + data_list[4]->converted_percentage - 200.0), 800.0) * 0.0003;
    }
    else if (c_point->name == "艾尔海森" && (attack_config->condition->attack_way == "E" || attack_config->condition->attack_way == "Q"))//精通->EQ增伤
    {
        damplus_add += min((mastery + data_list[4]->converted_percentage) * 0.001, 1.0);
    }
    else if (c_point->name == "提纳里" && (attack_config->condition->attack_way == "重A" || attack_config->condition->attack_way == "Q"))//精通->重AQ增伤
    {
        damplus_add += min((mastery + data_list[4]->converted_percentage) * 0.0006, 0.6);
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
    if (w_point->name == "裁叶萃光" && (attack_config->condition->attack_way == "平A" || attack_config->condition->attack_way == "E"))
        extrarate += 1.2 * (0.75 + w_point->level * 0.25) * mastery;
    else if (w_point->name == "辰砂之纺锤" && attack_config->condition->attack_way == "E")
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
        else if (attack_config->condition->attack_way == "E")//LV10
            extrarate += 0.407 * life * base_life;
        else if (attack_config->condition->attack_way == "Q")//LV10
        {
            if (c_point->constellation >= 2)
                extrarate += ((0.1315 + 0.0877 * 3 * 15 + 0.14 * 7) / 53) * life * base_life;
            else
                extrarate += ((0.1315 + 0.0877 * 3 * 15) / 46) * life * base_life;
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
    else if (c_point->name == "纳西妲")
    {
        if (attack_config->condition->attack_way == "E")
            extrarate += 3.715 * mastery;//LV10
    }
    else if (c_point->name == "艾尔海森")
    {
        if (attack_config->condition->attack_way == "E")
            extrarate += 2.419 * mastery;//LV10
        else if (attack_config->condition->attack_way == "Q")
            extrarate += 1.751 * mastery;//LV10
    }
}

//build new character(needed)||build new weapon(all)||build new artifact(all)
void Deployment::get_react_value(double mastery, double &extrarate, double &growrate, double &extra_damage)
{
    //扩散（风+水火雷冰），结晶（岩+水火雷冰），绽放（草水+火雷），激化（草雷），燃烧（草火），蒸发（水火），融化（火冰），冻结（水冰），感电（雷水），超载（雷火），超导（雷冰）
    //默认抗性固定为0.1
    if (attack_config->react_type.find("no_add_damage") != string::npos) return;
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
//            if ((suit1->name == suit2->name) && (suit1->name == "乐园遗落之花")) extra_damplus += 0.8;
//            //extra_damage += 6.0 * 723.4 * (1.0 + (16.0 * mastery) / (mastery + 2000.0) + extra_damplus);
//            extra_damage += 6.0 * 723.4 * (1.0 + (16.0 * mastery) / (mastery + 2000.0) + extra_damplus) * ((team_config->teammate_all.find("纳西妲") != string::npos) ? 1.2 : 1);
//            //6.0 * 723.4 * (1.0 + (16.0 * mastery) / (mastery + 2000.0) + 如雷/魔女等) * resistance;
//            //草伤，吃草抗
//        }
//        else if (attack_config->react_type.find("超绽放") != string::npos)
//        {
//            double extra_damplus = 0;
//            if ((suit1->name == suit2->name) && (suit1->name == "如雷的盛怒")) extra_damplus += 0.4;
//            if ((suit1->name == suit2->name) && (suit1->name == "乐园遗落之花")) extra_damplus += 0.8;
//            //extra_damage += 6.0 * 723.4 * (1.0 + (16.0 * mastery) / (mastery + 2000.0) + extra_damplus);
//            extra_damage += 6.0 * 723.4 * (1.0 + (16.0 * mastery) / (mastery + 2000.0) + extra_damplus) * ((team_config->teammate_all.find("纳西妲") != string::npos) ? 1.2 : 1);
//            //6.0 * 723.4 * (1.0 + (16.0 * mastery) / (mastery + 2000.0) + 如雷/魔女等) * resistance;
//            //草伤，吃草抗
//        }
//        else
//        {
//            double extra_damplus = 0;
//            if ((suit1->name == suit2->name) && (suit1->name == "乐园遗落之花")) extra_damplus += 0.8;
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

ofstream outfile_result;
ofstream outfile_debug;
bool out_header = true;
string double_E_per_round = "神里绫华甘雨温迪夜兰枫原万叶班尼特";
int max_up_num_per_base = 4;
double max_attribute_num_per_pos = 2.5;
int max_entry_num = 30;
int artifact_2_2_max_entry_bonus = 2;

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
    bool need_to_satisfy_recharge;

    Combination(Weapon *w_point_,
                Artifact *suit1_,
                Artifact *suit2_,
                string a_main3_,
                string a_main4_,
                string a_main5_,
                Team_config *team_config_,
                vector<Attack_config *> attack_config_list_,
                bool need_to_satisfy_recharge_)
    {
        w_point = w_point_;
        suit1 = suit1_;
        suit2 = suit2_;
        a_main3 = a_main3_;
        a_main4 = a_main4_;
        a_main5 = a_main5_;
        team_config = team_config_;
        attack_config_list = attack_config_list_;
        need_to_satisfy_recharge = need_to_satisfy_recharge_;
    }
};

bool out_debug = false;
double out_filter_percentage = 0.95;//0.95*(1+2.5%)*(1+2.5%)=1 2词条
string config_cal_enable = "ALL";//ALL全部启用
string add_character = "";
string add_weapon = "";
string add_artifact = "";

//注意圣遗物限定2+2有顺序，所有的限定条件只能是单个限定
void get_all_config(string c_name, vector<Combination *> &combination_list, string mode)
{
    //"白辰之环" "苍古自由之誓" "松籁响起之时" "圣显之钥" "西福斯的月光" "流浪的晚星" "玛海菈的水色"

    //"终末嗟叹之诗" "讨龙" "原木刀" "森林王器" "贯月矢" "千夜浮梦"
    //"悠古的磐岩" "昔日宗室之仪" "翠绿之影" "千岩牢固" "深林的记忆"

    //"胡桃" "夜兰" "行秋" "钟离" "终末嗟叹之诗_昔日宗室之仪" "水" "水"
    //"胡桃" "夜兰" "冰_test" "枫原万叶" "终末嗟叹之诗_翠绿之影" "水冰" "水冰"
    //"神里绫华" "甘雨" "莫娜" "温迪"/"枫原万叶" "昔日宗室之仪_翠绿之影" "冰水" "冰水"
    //"甘雨" "钟离" "香菱" "班尼特" "昔日宗室之仪" "火" "冰火"
    //"雷电将军" "行秋" "香菱" "班尼特" "昔日宗室之仪" "水雷" "水雷火"
    //"艾尔海森" "八重神子" "纳西妲" "钟离"/"久岐忍" "千夜浮梦_深林的记忆_昔日宗室之仪" "草" ""

    if (config_cal_enable.find(c_name) == string::npos && config_cal_enable != "ALL") return;

    if (c_name == "提纳里")
    {
        auto *tc1 = new Team_config(find_character_by_name("纳西妲"), find_character_by_name("八重神子"), find_character_by_name("钟离"),
                                    "千夜浮梦_深林的记忆_昔日宗室之仪", "草", "");

        vector<Attack_config *> ac1;//EZZZQ
        ac1.push_back(new Attack_config(new Condition("草", "弓", "重A"), false, false, "蔓激化_no_add_damage",
                                        false, true, false, true, false,
                                        true, true, true, false, false, 9));
        ac1.push_back(new Attack_config(new Condition("草", "弓", "重A"), false, false, "蔓激化",
                                        false, true, false, true, false,
                                        true, true, true, false, false, 6));
        ac1.push_back(new Attack_config(new Condition("草", "弓", "E"), false, false, "蔓激化",
                                        false, true, false, true, false,
                                        true, true, true, false, false, 1));
        ac1.push_back(new Attack_config(new Condition("草", "弓", "Q"), false, false, "蔓激化_no_add_damage",
                                        false, true, false, true, false,
                                        true, true, true, false, false, 8));
        ac1.push_back(new Attack_config(new Condition("草", "弓", "Q"), false, false, "蔓激化",
                                        false, true, false, true, false,
                                        true, true, true, false, false, 4));

        if (mode == "cal_deployment")
        {
            combination_list.push_back(new Combination(find_weapon_by_name(""), find_artifact_by_name(""), find_artifact_by_name(""),
                                                       "", "", "", tc1, ac1, true));
        }
        else if (mode == "cal_optimal_artifact")
        {
            combination_list.push_back(new Combination(find_weapon_by_name("天空之翼"), find_artifact_by_name(""), find_artifact_by_name(""),
                                                       "", "", "", tc1, ac1, true));
        }
        else if (mode == "generate_gcsim_script")
        {
            //猎人之径
            combination_list.push_back(new Combination(find_weapon_by_name("天空之翼"), find_artifact_by_name("饰金之梦"), find_artifact_by_name("饰金之梦"),
                                                       "元素精通", "伤害加成", "暴击率", tc1, ac1, true));
            combination_list.push_back(new Combination(find_weapon_by_name("天空之翼"), find_artifact_by_name("流浪大地的乐团"), find_artifact_by_name("饰金之梦"),
                                                       "元素精通", "伤害加成", "暴击率", tc1, ac1, true));
        }
    }

    if (c_name == "胡桃")
    {
        auto *tc1 = new Team_config(find_character_by_name("钟离"), find_character_by_name("行秋"), find_character_by_name("夜兰"),
                                    "终末嗟叹之诗_昔日宗室之仪", "水", "水");
        auto *tc2 = new Team_config(find_character_by_name("枫原万叶"), find_character_by_name("冰_test"), find_character_by_name("夜兰"),
                                    "终末嗟叹之诗_翠绿之影", "水冰", "水冰");

        vector<Attack_config *> ac1;//10AZ
        ac1.push_back(new Attack_config(new Condition("火", "长柄武器", "平A"), false, false, "蒸发_no_add_damage",
                                        true, true, false, false, false,
                                        true, true, true, false, false, 6));
        ac1.push_back(new Attack_config(new Condition("火", "长柄武器", "平A"), false, false, "蒸发",
                                        true, true, false, true, false,
                                        true, true, true, false, false, 4));
        ac1.push_back(new Attack_config(new Condition("火", "长柄武器", "重A"), false, false, "蒸发",
                                        true, true, false, true, false,
                                        true, true, true, false, false, 10));

        if (mode == "cal_deployment")
        {
            combination_list.push_back(new Combination(find_weapon_by_name(""), find_artifact_by_name(""), find_artifact_by_name(""),
                                                       "", "", "", tc1, ac1, false));
            combination_list.push_back(new Combination(find_weapon_by_name(""), find_artifact_by_name(""), find_artifact_by_name(""),
                                                       "", "", "", tc2, ac1, false));
        }
        else if (mode == "cal_optimal_artifact")
        {
            combination_list.push_back(new Combination(find_weapon_by_name("护摩之杖"), find_artifact_by_name(""), find_artifact_by_name(""),
                                                       "", "", "", tc1, ac1, false));
            combination_list.push_back(new Combination(find_weapon_by_name("护摩之杖"), find_artifact_by_name(""), find_artifact_by_name(""),
                                                       "", "", "", tc2, ac1, false));
        }
        else if (mode == "generate_gcsim_script")
        {
            combination_list.push_back(new Combination(find_weapon_by_name("护摩之杖"), find_artifact_by_name("饰金之梦"), find_artifact_by_name("饰金之梦"),
                                                       "生命值", "伤害加成", "暴击率", tc1, ac1, false));
            combination_list.push_back(new Combination(find_weapon_by_name("护摩之杖"), find_artifact_by_name("炽烈的炎之魔女"), find_artifact_by_name("炽烈的炎之魔女"),
                                                       "生命值", "伤害加成", "暴击率", tc1, ac1, false));
            combination_list.push_back(new Combination(find_weapon_by_name("护摩之杖"), find_artifact_by_name("追忆之注连"), find_artifact_by_name("追忆之注连"),
                                                       "元素精通", "伤害加成", "暴击率", tc1, ac1, false));

            combination_list.push_back(new Combination(find_weapon_by_name("护摩之杖"), find_artifact_by_name("饰金之梦"), find_artifact_by_name("饰金之梦"),
                                                       "生命值", "伤害加成", "暴击率", tc2, ac1, false));
            combination_list.push_back(new Combination(find_weapon_by_name("护摩之杖"), find_artifact_by_name("炽烈的炎之魔女"), find_artifact_by_name("炽烈的炎之魔女"),
                                                       "生命值", "伤害加成", "暴击率", tc2, ac1, false));
            combination_list.push_back(new Combination(find_weapon_by_name("护摩之杖"), find_artifact_by_name("追忆之注连"), find_artifact_by_name("追忆之注连"),
                                                       "元素精通", "伤害加成", "暴击率", tc2, ac1, false));
        }
    }
    if (c_name == "神里绫华")
    {
        auto *tc1 = new Team_config(find_character_by_name("甘雨"), find_character_by_name("莫娜"), find_character_by_name("温迪"),
                                    "昔日宗室之仪_翠绿之影", "冰水", "冰水");
        auto *tc2 = new Team_config(find_character_by_name("甘雨"), find_character_by_name("莫娜"), find_character_by_name("枫原万叶"),
                                    "昔日宗室之仪_翠绿之影", "冰水", "冰水");

        vector<Attack_config *> ac1;//EAQ AZAZ
        ac1.push_back(new Attack_config(new Condition("冰", "单手剑", "平A"), false, false, "冻结",
                                        false, true, false, false, false,
                                        true, true, true, false, false, 1 + 1 + 1));
        ac1.push_back(new Attack_config(new Condition("冰", "单手剑", "重A"), false, false, "冻结",
                                        false, true, false, false, false,
                                        true, true, true, false, false, 3 * 2));
        ac1.push_back(new Attack_config(new Condition("冰", "单手剑", "E"), false, false, "冻结",
                                        false, true, false, false, false,
                                        true, true, true, false, false, 2));
        ac1.push_back(new Attack_config(new Condition("冰", "单手剑", "Q"), false, true, "冻结",
                                        false, true, false, false, false,
                                        true, true, true, false, false, 20));

        if (mode == "cal_deployment")
        {
            combination_list.push_back(new Combination(find_weapon_by_name(""), find_artifact_by_name(""), find_artifact_by_name(""),
                                                       "", "", "", tc1, ac1, true));
            combination_list.push_back(new Combination(find_weapon_by_name(""), find_artifact_by_name(""), find_artifact_by_name(""),
                                                       "", "", "", tc2, ac1, true));
        }
        else if (mode == "cal_optimal_artifact")
        {
            combination_list.push_back(new Combination(find_weapon_by_name("雾切之回光"), find_artifact_by_name(""), find_artifact_by_name(""),
                                                       "", "", "", tc1, ac1, true));
            combination_list.push_back(new Combination(find_weapon_by_name("雾切之回光"), find_artifact_by_name(""), find_artifact_by_name(""),
                                                       "", "", "", tc2, ac1, true));
        }
        else if (mode == "generate_gcsim_script")
        {
            combination_list.push_back(new Combination(find_weapon_by_name("雾切之回光"), find_artifact_by_name("冰风迷途的勇士"), find_artifact_by_name("冰风迷途的勇士"),
                                                       "攻击力", "伤害加成", "暴击伤害", tc1, ac1, true));

            combination_list.push_back(new Combination(find_weapon_by_name("雾切之回光"), find_artifact_by_name("冰风迷途的勇士"), find_artifact_by_name("冰风迷途的勇士"),
                                                       "攻击力", "伤害加成", "暴击伤害", tc2, ac1, true));
        }
    }
    if (c_name == "雷电将军")
    {
        auto *tc1 = new Team_config(find_character_by_name("香菱"), find_character_by_name("班尼特"), find_character_by_name("行秋"),
                                    "昔日宗室之仪", "水雷", "水雷火");

        //无法准确建立反应次数和倍率之间的关系（Q的后续攻击）
        vector<Attack_config *> ac1;
        ac1.push_back(new Attack_config(new Condition("雷", "长柄武器", "Q"), false, false, "感电_no_add_damage",
                                        false, true, false, false, true,
                                        true, true, true, false, false, 1));

        if (mode == "cal_deployment")
        {
            combination_list.push_back(new Combination(find_weapon_by_name(""), find_artifact_by_name(""), find_artifact_by_name(""),
                                                       "", "", "", tc1, ac1, true));
        }
        else if (mode == "cal_optimal_artifact")
        {
            combination_list.push_back(new Combination(find_weapon_by_name("渔获"), find_artifact_by_name(""), find_artifact_by_name(""),
                                                       "", "", "", tc1, ac1, true));
        }
        else if (mode == "generate_gcsim_script")
        {
            //薙草之稻光
            combination_list.push_back(new Combination(find_weapon_by_name("渔获"), find_artifact_by_name("绝缘之旗印"), find_artifact_by_name("绝缘之旗印"),
                                                       "元素充能效率", "伤害加成", "暴击率", tc1, ac1, true));
            combination_list.push_back(new Combination(find_weapon_by_name("渔获"), find_artifact_by_name("绝缘之旗印"), find_artifact_by_name("绝缘之旗印"),
                                                       "元素充能效率", "攻击力", "暴击率", tc1, ac1, true));
        }
    }
    if (c_name == "甘雨")
    {
        auto *tc1 = new Team_config(find_character_by_name("神里绫华"), find_character_by_name("莫娜"), find_character_by_name("温迪"),
                                    "昔日宗室之仪_翠绿之影", "冰水", "冰水");
        auto *tc2 = new Team_config(find_character_by_name("神里绫华"), find_character_by_name("莫娜"), find_character_by_name("枫原万叶"),
                                    "昔日宗室之仪_翠绿之影", "冰水", "冰水");

        auto *tc3 = new Team_config(find_character_by_name("钟离"), find_character_by_name("香菱"), find_character_by_name("班尼特"),
                                    "昔日宗室之仪", "火", "冰火");

        vector<Attack_config *> ac1;//Q EZ EZ
        ac1.push_back(new Attack_config(new Condition("冰", "弓", "重A"), false, false, "冻结",
                                        false, true, false, false, false,
                                        true, true, true, false, false, 2 * 2));
        ac1.push_back(new Attack_config(new Condition("冰", "弓", "E"), false, true, "冻结",
                                        false, true, false, false, false,
                                        true, true, true, false, false, 2 * 2));
        ac1.push_back(new Attack_config(new Condition("冰", "弓", "Q"), true, true, "冻结",
                                        false, true, false, false, false,
                                        true, true, true, false, false, 17));

        vector<Attack_config *> ac2;//Z
        ac2.push_back(new Attack_config(new Condition("冰", "弓", "重A"), false, false, "融化",
                                        false, true, false, true, false,
                                        true, true, true, false, false, 2 * 1));

        if (mode == "cal_deployment")
        {
            combination_list.push_back(new Combination(find_weapon_by_name(""), find_artifact_by_name(""), find_artifact_by_name(""),
                                                       "", "", "", tc1, ac1, true));
            combination_list.push_back(new Combination(find_weapon_by_name(""), find_artifact_by_name(""), find_artifact_by_name(""),
                                                       "", "", "", tc2, ac1, true));

            combination_list.push_back(new Combination(find_weapon_by_name(""), find_artifact_by_name(""), find_artifact_by_name(""),
                                                       "", "", "", tc3, ac2, false));
        }
        else if (mode == "cal_optimal_artifact")
        {
            combination_list.push_back(new Combination(find_weapon_by_name("飞雷之弦振"), find_artifact_by_name(""), find_artifact_by_name(""),
                                                       "", "", "", tc1, ac1, true));
            combination_list.push_back(new Combination(find_weapon_by_name("飞雷之弦振"), find_artifact_by_name(""), find_artifact_by_name(""),
                                                       "", "", "", tc2, ac1, true));

            combination_list.push_back(new Combination(find_weapon_by_name("天空之翼"), find_artifact_by_name(""), find_artifact_by_name(""),
                                                       "", "", "", tc3, ac2, false));
        }
        else if (mode == "generate_gcsim_script")
        {
            combination_list.push_back(new Combination(find_weapon_by_name("飞雷之弦振"), find_artifact_by_name("冰风迷途的勇士"), find_artifact_by_name("冰风迷途的勇士"),
                                                       "攻击力", "伤害加成", "暴击伤害", tc1, ac1, true));
            combination_list.push_back(new Combination(find_weapon_by_name("飞雷之弦振"), find_artifact_by_name("冰风迷途的勇士"), find_artifact_by_name("冰风迷途的勇士"),
                                                       "攻击力", "伤害加成", "暴击伤害", tc2, ac1, true));

            //阿莫斯
            combination_list.push_back(new Combination(find_weapon_by_name("天空之翼"), find_artifact_by_name("流浪大地的乐团"), find_artifact_by_name("流浪大地的乐团"),
                                                       "元素精通", "伤害加成", "暴击率", tc3, ac2, false));
        }
    }
    if (c_name == "艾尔海森")
    {
        auto *tc1 = new Team_config(find_character_by_name("纳西妲"), find_character_by_name("八重神子"), find_character_by_name("钟离"),
                                    "千夜浮梦_深林的记忆_昔日宗室之仪", "草", "");
        auto *tc2 = new Team_config(find_character_by_name("纳西妲"), find_character_by_name("八重神子"), find_character_by_name("久岐忍"),
                                    "千夜浮梦_深林的记忆_昔日宗室之仪", "草", "");

        vector<Attack_config *> ac1;//4Q 18A 23333332E 1Z
        ac1.push_back(new Attack_config(new Condition("草", "单手剑", "平A"), false, false, "蔓激化_no_add_damage",
                                        false, true, false, false, false,
                                        true, true, true, false, false, 12));
        ac1.push_back(new Attack_config(new Condition("草", "单手剑", "平A"), false, false, "蔓激化",
                                        false, true, false, true, false,
                                        true, true, true, false, false, 6));
        ac1.push_back(new Attack_config(new Condition("草", "单手剑", "重A"), false, false, "蔓激化",
                                        false, true, false, true, false,
                                        true, true, true, false, false, 1));
        ac1.push_back(new Attack_config(new Condition("草", "单手剑", "E"), false, false, "蔓激化_no_add_damage",
                                        false, true, false, true, false,
                                        true, true, true, false, false, 18));
        ac1.push_back(new Attack_config(new Condition("草", "单手剑", "E"), false, false, "蔓激化",
                                        false, true, false, true, false,
                                        true, true, true, false, false, 4));
        ac1.push_back(new Attack_config(new Condition("草", "单手剑", "Q"), false, false, "蔓激化_no_add_damage",
                                        false, true, false, true, false,
                                        true, true, true, false, false, 2));
        ac1.push_back(new Attack_config(new Condition("草", "单手剑", "Q"), false, false, "蔓激化",
                                        false, true, false, true, false,
                                        true, true, true, false, false, 2));

        if (mode == "cal_deployment")
        {
            combination_list.push_back(new Combination(find_weapon_by_name(""), find_artifact_by_name(""), find_artifact_by_name(""),
                                                       "", "", "", tc1, ac1, true));
            combination_list.push_back(new Combination(find_weapon_by_name(""), find_artifact_by_name(""), find_artifact_by_name(""),
                                                       "", "", "", tc2, ac1, true));
        }
        else if (mode == "cal_optimal_artifact")
        {
            combination_list.push_back(new Combination(find_weapon_by_name("雾切之回光"), find_artifact_by_name(""), find_artifact_by_name(""),
                                                       "", "", "", tc1, ac1, true));
            combination_list.push_back(new Combination(find_weapon_by_name("雾切之回光"), find_artifact_by_name(""), find_artifact_by_name(""),
                                                       "", "", "", tc2, ac1, true));
        }
        else if (mode == "generate_gcsim_script")
        {
            //裁叶翠光
            combination_list.push_back(new Combination(find_weapon_by_name("雾切之回光"), find_artifact_by_name("饰金之梦"), find_artifact_by_name("饰金之梦"),
                                                       "元素精通", "伤害加成", "暴击率", tc1, ac1, true));
            combination_list.push_back(new Combination(find_weapon_by_name("雾切之回光"), find_artifact_by_name("流浪大地的乐团"), find_artifact_by_name("饰金之梦"),
                                                       "元素精通", "伤害加成", "暴击率", tc1, ac1, true));

            combination_list.push_back(new Combination(find_weapon_by_name("雾切之回光"), find_artifact_by_name("饰金之梦"), find_artifact_by_name("饰金之梦"),
                                                       "元素精通", "伤害加成", "暴击率", tc2, ac1, true));
            combination_list.push_back(new Combination(find_weapon_by_name("雾切之回光"), find_artifact_by_name("流浪大地的乐团"), find_artifact_by_name("饰金之梦"),
                                                       "元素精通", "伤害加成", "暴击率", tc2, ac1, true));
        }
    }
    if (c_name == "纳西妲")
    {
        auto *tc1 = new Team_config(find_character_by_name("艾尔海森"), find_character_by_name("八重神子"), find_character_by_name("钟离"),
                                    "昔日宗室之仪", "草", "");
        auto *tc2 = new Team_config(find_character_by_name("艾尔海森"), find_character_by_name("八重神子"), find_character_by_name("久岐忍"),
                                    "昔日宗室之仪", "草", "");

        vector<Attack_config *> ac1;
        ac1.push_back(new Attack_config(new Condition("草", "法器", "E"), true, false, "蔓激化",
                                        false, true, false, true, false,
                                        true, true, true, false, false, 1));

        if (mode == "cal_deployment")
        {
            combination_list.push_back(new Combination(find_weapon_by_name(""), find_artifact_by_name("深林的记忆"), find_artifact_by_name("深林的记忆"),
                                                       "", "", "", tc1, ac1, true));
            combination_list.push_back(new Combination(find_weapon_by_name(""), find_artifact_by_name("深林的记忆"), find_artifact_by_name("深林的记忆"),
                                                       "", "", "", tc2, ac1, true));
        }
        else if (mode == "cal_optimal_artifact")
        {
            combination_list.push_back(new Combination(find_weapon_by_name("千夜浮梦"), find_artifact_by_name("深林的记忆"), find_artifact_by_name("深林的记忆"),
                                                       "", "", "", tc1, ac1, true));
            combination_list.push_back(new Combination(find_weapon_by_name("千夜浮梦"), find_artifact_by_name("深林的记忆"), find_artifact_by_name("深林的记忆"),
                                                       "", "", "", tc2, ac1, true));
        }
        else if (mode == "generate_gcsim_script")
        {
            combination_list.push_back(new Combination(find_weapon_by_name("千夜浮梦"), find_artifact_by_name("深林的记忆"), find_artifact_by_name("深林的记忆"),
                                                       "元素精通", "元素精通", "暴击率", tc1, ac1, true));
            combination_list.push_back(new Combination(find_weapon_by_name("千夜浮梦"), find_artifact_by_name("深林的记忆"), find_artifact_by_name("深林的记忆"),
                                                       "元素精通", "伤害加成", "暴击率", tc1, ac1, true));

            combination_list.push_back(new Combination(find_weapon_by_name("千夜浮梦"), find_artifact_by_name("深林的记忆"), find_artifact_by_name("深林的记忆"),
                                                       "元素精通", "元素精通", "暴击率", tc2, ac1, true));
            combination_list.push_back(new Combination(find_weapon_by_name("千夜浮梦"), find_artifact_by_name("深林的记忆"), find_artifact_by_name("深林的记忆"),
                                                       "元素精通", "伤害加成", "暴击率", tc2, ac1, true));
        }
    }
    if (c_name == "夜兰")
    {
        auto *tc1 = new Team_config(find_character_by_name("胡桃"), find_character_by_name("钟离"), find_character_by_name("行秋"),
                                    "昔日宗室之仪", "水", "水");
        auto *tc2 = new Team_config(find_character_by_name("胡桃"), find_character_by_name("枫原万叶"), find_character_by_name("冰_test"),
                                    "翠绿之影", "水冰", "水冰");

        vector<Attack_config *> ac1;//EEQ
        ac1.push_back(new Attack_config(new Condition("水", "弓", "E"), false, false, "NONE",
                                        true, false, false, false, false,
                                        true, true, true, false, false, 2));
        ac1.push_back(new Attack_config(new Condition("水", "弓", "Q"), true, false, "NONE",
                                        true, false, false, false, false,
                                        true, true, true, false, false, 53));

        if (mode == "cal_deployment")
        {
            combination_list.push_back(new Combination(find_weapon_by_name(""), find_artifact_by_name(""), find_artifact_by_name(""),
                                                       "", "", "", tc1, ac1, true));
            combination_list.push_back(new Combination(find_weapon_by_name(""), find_artifact_by_name(""), find_artifact_by_name(""),
                                                       "", "", "", tc2, ac1, true));
        }
        else if (mode == "cal_optimal_artifact")
        {
            combination_list.push_back(new Combination(find_weapon_by_name("终末嗟叹之诗"), find_artifact_by_name(""), find_artifact_by_name(""),
                                                       "", "", "", tc1, ac1, true));
            combination_list.push_back(new Combination(find_weapon_by_name("终末嗟叹之诗"), find_artifact_by_name(""), find_artifact_by_name(""),
                                                       "", "", "", tc2, ac1, true));
        }
        else if (mode == "generate_gcsim_script")
        {
            //若水
            combination_list.push_back(new Combination(find_weapon_by_name("终末嗟叹之诗"), find_artifact_by_name("绝缘之旗印"), find_artifact_by_name("绝缘之旗印"),
                                                       "生命值", "伤害加成", "暴击率", tc1, ac1, true));

            combination_list.push_back(new Combination(find_weapon_by_name("终末嗟叹之诗"), find_artifact_by_name("绝缘之旗印"), find_artifact_by_name("绝缘之旗印"),
                                                       "生命值", "伤害加成", "暴击率", tc2, ac1, true));
        }
    }
    if (c_name == "行秋")
    {
        auto *tc1 = new Team_config(find_character_by_name("胡桃"), find_character_by_name("钟离"), find_character_by_name("夜兰"),
                                    "终末嗟叹之诗_昔日宗室之仪", "水", "水");

        auto *tc2 = new Team_config(find_character_by_name("雷电将军"), find_character_by_name("香菱"), find_character_by_name("班尼特"),
                                    "昔日宗室之仪", "水雷", "水雷火");

        vector<Attack_config *> ac1;//EQ
        ac1.push_back(new Attack_config(new Condition("水", "单手剑", "E"), false, false, "NONE",
                                        false, true, false, false, false,
                                        true, true, true, false, false, 2 * 1));
        ac1.push_back(new Attack_config(new Condition("水", "单手剑", "Q"), true, false, "NONE",
                                        false, true, false, false, false,
                                        true, true, true, false, false, 45));

        vector<Attack_config *> ac2;//EQ
        ac2.push_back(new Attack_config(new Condition("水", "单手剑", "E"), false, false, "感电_no_add_damage",
                                        false, true, false, false, false,
                                        true, true, true, false, false, 2 * 1));
        ac2.push_back(new Attack_config(new Condition("水", "单手剑", "Q"), true, false, "感电_no_add_damage",
                                        false, true, false, false, false,
                                        true, true, true, false, false, 45));

        if (mode == "cal_deployment")
        {
            combination_list.push_back(new Combination(find_weapon_by_name(""), find_artifact_by_name(""), find_artifact_by_name(""),
                                                       "", "", "", tc1, ac1, true));
            combination_list.push_back(new Combination(find_weapon_by_name(""), find_artifact_by_name(""), find_artifact_by_name(""),
                                                       "", "", "", tc2, ac2, true));
        }
        else if (mode == "cal_optimal_artifact")
        {
            combination_list.push_back(new Combination(find_weapon_by_name("祭礼剑"), find_artifact_by_name(""), find_artifact_by_name(""),
                                                       "", "", "", tc1, ac1, true));
            combination_list.push_back(new Combination(find_weapon_by_name("祭礼剑"), find_artifact_by_name(""), find_artifact_by_name(""),
                                                       "", "", "", tc2, ac2, true));
        }
        else if (mode == "generate_gcsim_script")
        {
            //磐岩结绿
            combination_list.push_back(new Combination(find_weapon_by_name("祭礼剑"), find_artifact_by_name("沉沦之心"), find_artifact_by_name("昔日宗室之仪"),
                                                       "攻击力", "伤害加成", "暴击率", tc1, ac1, true));
            combination_list.push_back(new Combination(find_weapon_by_name("祭礼剑"), find_artifact_by_name("沉沦之心"), find_artifact_by_name("昔日宗室之仪"),
                                                       "攻击力", "伤害加成", "暴击率", tc2, ac2, true));
        }
    }
    if (c_name == "香菱")
    {
        auto *tc1 = new Team_config(find_character_by_name("雷电将军"), find_character_by_name("班尼特"), find_character_by_name("行秋"),
                                    "昔日宗室之仪", "水雷", "水雷火");

        auto *tc2 = new Team_config(find_character_by_name("甘雨"), find_character_by_name("班尼特"), find_character_by_name("钟离"),
                                    "昔日宗室之仪", "火", "冰火");

        vector<Attack_config *> ac1;
        ac1.push_back(new Attack_config(new Condition("火", "长柄武器", "E"), true, true, "蒸发",
                                        false, true, false, true, false,
                                        true, true, true, false, false, 4));
        ac1.push_back(new Attack_config(new Condition("火", "长柄武器", "Q"), true, true, "蒸发_超载_no_add_damage",
                                        false, true, false, false, false,
                                        true, true, true, false, false, 3));
        ac1.push_back(new Attack_config(new Condition("火", "长柄武器", "Q"), true, true, "蒸发_超载",
                                        false, true, false, true, false,
                                        true, true, true, false, false, 12));

        vector<Attack_config *> ac2;
        ac2.push_back(new Attack_config(new Condition("火", "长柄武器", "E"), true, true, "NONE",
                                        false, true, false, false, false,
                                        true, true, true, false, false, 4));
        ac2.push_back(new Attack_config(new Condition("火", "长柄武器", "Q"), true, true, "NONE",
                                        false, true, false, false, false,
                                        true, true, true, false, false, 15));

        if (mode == "cal_deployment")
        {
            combination_list.push_back(new Combination(find_weapon_by_name(""), find_artifact_by_name(""), find_artifact_by_name(""),
                                                       "", "", "", tc1, ac1, true));
            combination_list.push_back(new Combination(find_weapon_by_name(""), find_artifact_by_name(""), find_artifact_by_name(""),
                                                       "", "", "", tc2, ac2, true));
        }
        else if (mode == "cal_optimal_artifact")
        {
            combination_list.push_back(new Combination(find_weapon_by_name("断浪长鳍"), find_artifact_by_name(""), find_artifact_by_name(""),
                                                       "", "", "", tc1, ac1, true));
            combination_list.push_back(new Combination(find_weapon_by_name("渔获"), find_artifact_by_name(""), find_artifact_by_name(""),
                                                       "", "", "", tc2, ac2, true));
        }
        else if (mode == "generate_gcsim_script")
        {
            //赤沙之杖
            combination_list.push_back(new Combination(find_weapon_by_name("断浪长鳍"), find_artifact_by_name("绝缘之旗印"), find_artifact_by_name("绝缘之旗印"),
                                                       "元素充能效率", "伤害加成", "暴击率", tc1, ac1, true));
            combination_list.push_back(new Combination(find_weapon_by_name("断浪长鳍"), find_artifact_by_name("炽烈的炎之魔女"), find_artifact_by_name("炽烈的炎之魔女"),
                                                       "元素充能效率", "伤害加成", "暴击率", tc1, ac1, true));
            combination_list.push_back(new Combination(find_weapon_by_name("断浪长鳍"), find_artifact_by_name("饰金之梦"), find_artifact_by_name("饰金之梦"),
                                                       "元素充能效率", "伤害加成", "暴击率", tc1, ac1, true));

            combination_list.push_back(new Combination(find_weapon_by_name("匣里灭辰"), find_artifact_by_name("绝缘之旗印"), find_artifact_by_name("绝缘之旗印"),
                                                       "元素充能效率", "伤害加成", "暴击率", tc1, ac1, true));
            combination_list.push_back(new Combination(find_weapon_by_name("匣里灭辰"), find_artifact_by_name("炽烈的炎之魔女"), find_artifact_by_name("炽烈的炎之魔女"),
                                                       "元素充能效率", "伤害加成", "暴击率", tc1, ac1, true));
            combination_list.push_back(new Combination(find_weapon_by_name("匣里灭辰"), find_artifact_by_name("饰金之梦"), find_artifact_by_name("饰金之梦"),
                                                       "元素充能效率", "伤害加成", "暴击率", tc1, ac1, true));

            //薙草之稻光
            combination_list.push_back(new Combination(find_weapon_by_name("渔获"), find_artifact_by_name("绝缘之旗印"), find_artifact_by_name("绝缘之旗印"),
                                                       "元素充能效率", "伤害加成", "暴击率", tc2, ac2, true));
        }
    }
    if (c_name == "八重神子")
    {
        auto *tc1 = new Team_config(find_character_by_name("纳西妲"), find_character_by_name("艾尔海森"), find_character_by_name("钟离"),
                                    "千夜浮梦_深林的记忆_昔日宗室之仪", "草", "");
        auto *tc2 = new Team_config(find_character_by_name("纳西妲"), find_character_by_name("艾尔海森"), find_character_by_name("久岐忍"),
                                    "千夜浮梦_深林的记忆_昔日宗室之仪", "草", "");

        vector<Attack_config *> ac1;
        ac1.push_back(new Attack_config(new Condition("雷", "法器", "E"), true, false, "超激化",
                                        false, true, false, true, false,
                                        true, true, true, false, false, 10));
        ac1.push_back(new Attack_config(new Condition("雷", "法器", "E"), true, false, "超激化_no_add_damage",
                                        false, true, false, true, false,
                                        true, true, true, false, false, 18));
        ac1.push_back(new Attack_config(new Condition("雷", "法器", "Q"), false, true, "超激化",
                                        false, true, false, true, false,
                                        true, true, true, false, false, 4));

        if (mode == "cal_deployment")
        {
            combination_list.push_back(new Combination(find_weapon_by_name(""), find_artifact_by_name(""), find_artifact_by_name(""),
                                                       "", "", "", tc1, ac1, false));//两轮一大
            combination_list.push_back(new Combination(find_weapon_by_name(""), find_artifact_by_name(""), find_artifact_by_name(""),
                                                       "", "", "", tc2, ac1, false));//两轮一大
        }
        else if (mode == "cal_optimal_artifact")
        {
            combination_list.push_back(new Combination(find_weapon_by_name("神乐之真意"), find_artifact_by_name(""), find_artifact_by_name(""),
                                                       "", "", "", tc1, ac1, false));//两轮一大
            combination_list.push_back(new Combination(find_weapon_by_name("神乐之真意"), find_artifact_by_name(""), find_artifact_by_name(""),
                                                       "", "", "", tc2, ac1, false));//两轮一大
        }
        else if (mode == "generate_gcsim_script")
        {
            combination_list.push_back(new Combination(find_weapon_by_name("神乐之真意"), find_artifact_by_name("流浪大地的乐团"), find_artifact_by_name("角斗士的终幕礼"),
                                                       "攻击力", "伤害加成", "暴击率", tc1, ac1, false));//两轮一大
            combination_list.push_back(new Combination(find_weapon_by_name("神乐之真意"), find_artifact_by_name("流浪大地的乐团"), find_artifact_by_name("饰金之梦"),
                                                       "攻击力", "伤害加成", "暴击率", tc1, ac1, false));//两轮一大
            combination_list.push_back(new Combination(find_weapon_by_name("神乐之真意"), find_artifact_by_name("饰金之梦"), find_artifact_by_name("饰金之梦"),
                                                       "攻击力", "伤害加成", "暴击率", tc1, ac1, false));//两轮一大

            combination_list.push_back(new Combination(find_weapon_by_name("神乐之真意"), find_artifact_by_name("流浪大地的乐团"), find_artifact_by_name("角斗士的终幕礼"),
                                                       "攻击力", "伤害加成", "暴击率", tc2, ac1, false));//两轮一大
            combination_list.push_back(new Combination(find_weapon_by_name("神乐之真意"), find_artifact_by_name("流浪大地的乐团"), find_artifact_by_name("饰金之梦"),
                                                       "攻击力", "伤害加成", "暴击率", tc2, ac1, false));//两轮一大
            combination_list.push_back(new Combination(find_weapon_by_name("神乐之真意"), find_artifact_by_name("饰金之梦"), find_artifact_by_name("饰金之梦"),
                                                       "攻击力", "伤害加成", "暴击率", tc2, ac1, false));//两轮一大
        }
    }
    if (c_name == "温迪")
    {
        auto *tc1 = new Team_config(find_character_by_name("岩_test"), find_character_by_name("雷_test"), find_character_by_name("草_test"),
                                    "", "", "");

        vector<Attack_config *> ac1;//默认多目标扩散加倍
        ac1.push_back(new Attack_config(new Condition("风", "弓", "E"), false, false, "扩散",
                                        false, true, false, true, false,
                                        true, true, true, false, false, 2));
        ac1.push_back(new Attack_config(new Condition("风", "弓", "Q"), true, true, "扩散",
                                        false, true, false, true, false,
                                        true, true, true, false, false, 7 * 2 + 2));
        ac1.push_back(new Attack_config(new Condition("风", "弓", "Q"), true, true, "扩散_no_add_damage",
                                        false, true, false, false, false,
                                        true, true, true, false, false, 4));

        if (mode == "cal_deployment")
        {
            combination_list.push_back(new Combination(find_weapon_by_name(""), find_artifact_by_name("翠绿之影"), find_artifact_by_name("翠绿之影"),
                                                       "元素精通", "元素精通", "元素精通", tc1, ac1, true));
            combination_list.push_back(new Combination(find_weapon_by_name(""), find_artifact_by_name("翠绿之影"), find_artifact_by_name("翠绿之影"),
                                                       "攻击力", "伤害加成", "暴击率", tc1, ac1, true));
        }
        else if (mode == "cal_optimal_artifact")
        {
            combination_list.push_back(new Combination(find_weapon_by_name("天空之翼"), find_artifact_by_name("翠绿之影"), find_artifact_by_name("翠绿之影"),
                                                       "", "", "", tc1, ac1, true));
        }
        else if (mode == "generate_gcsim_script")
        {
            //终末嗟叹之诗
            combination_list.push_back(new Combination(find_weapon_by_name("天空之翼"), find_artifact_by_name("翠绿之影"), find_artifact_by_name("翠绿之影"),
                                                       "元素精通", "元素精通", "元素精通", nullptr, ac1, true));
            combination_list.push_back(new Combination(find_weapon_by_name("天空之翼"), find_artifact_by_name("翠绿之影"), find_artifact_by_name("翠绿之影"),
                                                       "攻击力", "伤害加成", "暴击率", nullptr, ac1, true));
        }
    }
    if (c_name == "莫娜")
    {
        auto *tc1 = new Team_config(find_character_by_name("神里绫华"), find_character_by_name("甘雨"), find_character_by_name("温迪"),
                                    "翠绿之影", "冰水", "冰水");
        auto *tc2 = new Team_config(find_character_by_name("神里绫华"), find_character_by_name("甘雨"), find_character_by_name("枫原万叶"),
                                    "翠绿之影", "冰水", "冰水");

        vector<Attack_config *> ac1;
        ac1.push_back(new Attack_config(new Condition("水", "法器", "E"), true, false, "冻结",
                                        false, true, false, false, true,
                                        true, true, true, false, false, 5));
        ac1.push_back(new Attack_config(new Condition("水", "法器", "Q"), true, false, "冻结",
                                        false, true, false, false, true,
                                        true, true, true, false, false, 1));

        if (mode == "cal_deployment")
        {
            combination_list.push_back(new Combination(find_weapon_by_name(""), find_artifact_by_name("昔日宗室之仪"), find_artifact_by_name("昔日宗室之仪"),
                                                       "元素充能效率", "", "", tc1, ac1, true));
            combination_list.push_back(new Combination(find_weapon_by_name(""), find_artifact_by_name("昔日宗室之仪"), find_artifact_by_name("昔日宗室之仪"),
                                                       "元素充能效率", "", "", tc2, ac1, true));
        }
        else if (mode == "cal_optimal_artifact")
        {
            combination_list.push_back(new Combination(find_weapon_by_name("试作金珀"), find_artifact_by_name("昔日宗室之仪"), find_artifact_by_name("昔日宗室之仪"),
                                                       "元素充能效率", "", "", tc1, ac1, true));
            combination_list.push_back(new Combination(find_weapon_by_name("试作金珀"), find_artifact_by_name("昔日宗室之仪"), find_artifact_by_name("昔日宗室之仪"),
                                                       "元素充能效率", "", "", tc2, ac1, true));
        }
        else if (mode == "generate_gcsim_script")
        {
            combination_list.push_back(new Combination(find_weapon_by_name("试作金珀"), find_artifact_by_name("昔日宗室之仪"), find_artifact_by_name("昔日宗室之仪"),
                                                       "元素充能效率", "攻击力", "暴击率", tc1, ac1, true));
            combination_list.push_back(new Combination(find_weapon_by_name("试作金珀"), find_artifact_by_name("昔日宗室之仪"), find_artifact_by_name("昔日宗室之仪"),
                                                       "元素充能效率", "攻击力", "暴击率", tc2, ac1, true));

//            combination_list.push_back(new Combination(find_weapon_by_name("讨龙英杰谭"), find_artifact_by_name("昔日宗室之仪"), find_artifact_by_name("昔日宗室之仪"), "元素充能效率", "攻击力", "暴击率"));
        }
    }
    if (c_name == "钟离")
    {
        auto *tc1 = new Team_config(find_character_by_name("风_test"), find_character_by_name("雷_test"), find_character_by_name("草_test"),
                                    "", "", "");

        vector<Attack_config *> ac1;
        ac1.push_back(new Attack_config(new Condition("岩", "长柄武器", "Q"), false, false, "结晶",
                                        true, true, false, false, false,
                                        true, true, true, false, false, 1));

        if (mode == "cal_deployment")
        {
            combination_list.push_back(new Combination(find_weapon_by_name(""), find_artifact_by_name("昔日宗室之仪"), find_artifact_by_name("昔日宗室之仪"),
                                                       "生命值", "生命值", "", tc1, ac1, false));
            combination_list.push_back(new Combination(find_weapon_by_name(""), find_artifact_by_name("昔日宗室之仪"), find_artifact_by_name("昔日宗室之仪"),
                                                       "生命值", "伤害加成", "", tc1, ac1, false));
        }
        else if (mode == "cal_optimal_artifact")
        {
            combination_list.push_back(new Combination(find_weapon_by_name("西风长枪"), find_artifact_by_name("昔日宗室之仪"), find_artifact_by_name("昔日宗室之仪"),
                                                       "生命值", "", "暴击率", tc1, ac1, false));
        }
        else if (mode == "generate_gcsim_script")
        {
            //护摩之杖
            combination_list.push_back(new Combination(find_weapon_by_name("西风长枪"), find_artifact_by_name("昔日宗室之仪"), find_artifact_by_name("昔日宗室之仪"),
                                                       "生命值", "生命值", "暴击率", nullptr, ac1, false));
//            combination_list.push_back(new Combination(find_weapon_by_name("决斗之枪"), find_artifact_by_name("昔日宗室之仪"), find_artifact_by_name("昔日宗室之仪"), "生命值", "伤害加成", "暴击伤害"));
        }
    }
    if (c_name == "枫原万叶")
    {
        if (mode == "generate_gcsim_script")
        {
            //苍古自由之誓
            combination_list.push_back(new Combination(find_weapon_by_name("东花坊时雨"), find_artifact_by_name("翠绿之影"), find_artifact_by_name("翠绿之影"),
                                                       "元素精通", "元素精通", "元素精通", nullptr, vector<Attack_config *>(), true));
        }
    }
    if (c_name == "班尼特")
    {
        if (mode == "generate_gcsim_script")
        {
            combination_list.push_back(new Combination(find_weapon_by_name("天空之刃"), find_artifact_by_name("昔日宗室之仪"), find_artifact_by_name("昔日宗室之仪"),
                                                       "元素充能效率", "生命值", "治疗加成", nullptr, vector<Attack_config *>(), true));
        }
    }
    if (c_name == "久岐忍")
    {
        if (mode == "generate_gcsim_script")
        {
            //苍古自由之誓 圣显之钥
            combination_list.push_back(new Combination(find_weapon_by_name("原木刀"), find_artifact_by_name("千岩牢固"), find_artifact_by_name("千岩牢固"),
                                                       "生命值", "生命值", "治疗加成", nullptr, vector<Attack_config *>(), false));
//            combination_list.push_back(new Combination(find_weapon_by_name("铁锋刺"), find_artifact_by_name("千岩牢固"), find_artifact_by_name("千岩牢固"),
//                                                       "元素精通", "元素精通", "元素精通", nullptr, vector<Attack_config *>(), false));
//            combination_list.push_back(new Combination(find_weapon_by_name("铁锋刺"), find_artifact_by_name("饰金之梦"), find_artifact_by_name("饰金之梦"),
//                                                       "元素精通", "元素精通", "元素精通", nullptr, vector<Attack_config *>(), false));
        }
    }
}

//cal_(add_)deployment

bool cmp_func(Group *a, Group *b)
{
    if (a->total_damage > b->total_damage) return true;
    else return false;
};

void cal_deployment(string mode)
{
    auto total_start = chrono::system_clock::now();
    for (auto &c_index: character_list)
    {
        //没有武器和圣遗物更新则不用计算未更新角色的任何配置
        if (mode == "ADD" && add_weapon.empty() && add_artifact.empty() && add_character.find(c_index->name) == string::npos) continue;

        vector<Combination *> combination_list;
        get_all_config(c_index->name, combination_list, "cal_deployment");
        for (auto &comb_index: combination_list)
        {
            vector<Group *> comb_out_data;

            for (auto &w_index: weapon_list)
            {
                //没有圣遗物更新则不用计算未更新武器的任何配置
                if (mode == "ADD" && add_artifact.empty() && add_weapon.find(w_index->name) == string::npos) continue;

                if (c_index->weapon_type != w_index->weapon_type) continue;

                if (comb_index->w_point != nullptr && comb_index->w_point != w_index) continue;

                vector<Group *> c_w_pair;
                vector<thread> ths;

                auto start = chrono::system_clock::now();

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
                                                           a_main4[main4_index], a_main5[main5_index], comb_index->team_config, comb_index->attack_config_list, comb_index->need_to_satisfy_recharge);
                                    int check_num = temp->init_check_data(9);
                                    if (check_num == 0)//pass
                                    {
                                        c_w_pair.push_back(temp);
                                        ths.emplace_back(&Group::cal_damage_entry_num, temp);
                                        //temp->cal_damage_entry_num();
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
                                    else if (check_num == 6)//error:recharge
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

                for (auto &th: ths) th.join();

                chrono::duration<double> time = chrono::system_clock::now() - start;

                if (!c_w_pair.empty())
                {
                    Group *optimal = c_w_pair[0];
                    for (auto &i: c_w_pair)
                        if (i->total_damage > optimal->total_damage)
                            optimal = i;
                    for (auto &i: c_w_pair)
                    {
                        if (i->total_damage / optimal->total_damage >= out_filter_percentage) comb_out_data.push_back(i);
                        else delete i;
                    }
                }
                c_w_pair.clear();

                cout << c_index->name << " " << w_index->name << " " << " time=" << time.count() << "s" << ((time.count() > 30) ? "!!!" : "") << " ";
                for (auto &i: comb_index->attack_config_list)
                    cout << to_string(i->attack_time) + "*" + i->condition->attack_way + "_" + (i->background ? "后台" : "前台") + "_" + i->react_type + "/";
                cout << " " << comb_index->team_config->teammate_all + "_" + comb_index->team_config->team_weapon_artifact << "\n";
            }

            if (!comb_out_data.empty())
            {
                stable_sort(comb_out_data.begin(), comb_out_data.end(), cmp_func);
                double total_damage_baseline = comb_out_data[0]->total_damage;
                for (auto &i: comb_out_data)
                {
                    i->out(total_damage_baseline);
                    delete i;
                }
            }
            comb_out_data.clear();
        }
        combination_list.clear();
    }
    chrono::duration<double> total_time = chrono::system_clock::now() - total_start;
    cout << "total_time:" << total_time.count() << "s" << endl;
}

//cal_optimal_artifact

vector<vector<Reinforced_Artifact *>> reinforced_artifact_list;

void read_artifact(string filename)
{
    ifstream infile;
    infile.open(filename);
    vector<Reinforced_Artifact *> pos1;
    vector<Reinforced_Artifact *> pos2;
    vector<Reinforced_Artifact *> pos3;
    vector<Reinforced_Artifact *> pos4;
    vector<Reinforced_Artifact *> pos5;

    while (!infile.eof())
    {
        int pos;
        string suit_name;
        string main_type;
        string entry1_type;
        double entry1_value;
        string entry2_type;
        double entry2_value;
        string entry3_type;
        double entry3_value;
        string entry4_type;
        double entry4_value;
        string character_belong;

        infile >> pos >> suit_name >> main_type >> entry1_type >> entry1_value >> entry2_type >> entry2_value >> entry3_type >> entry3_value >> entry4_type >> entry4_value >> character_belong;
        auto *temp = new Reinforced_Artifact(pos, suit_name, main_type, entry1_type, entry1_value, entry2_type, entry2_value,
                                             entry3_type, entry3_value, entry4_type, entry4_value, character_belong);
        if (pos == 1) pos1.push_back(temp);
        else if (pos == 2) pos2.push_back(temp);
        else if (pos == 3) pos3.push_back(temp);
        else if (pos == 4) pos4.push_back(temp);
        else if (pos == 5) pos5.push_back(temp);
    }
    reinforced_artifact_list.push_back(pos1);
    reinforced_artifact_list.push_back(pos2);
    reinforced_artifact_list.push_back(pos3);
    reinforced_artifact_list.push_back(pos4);
    reinforced_artifact_list.push_back(pos5);
}

void get_suit(Reinforced_Artifact *pos1, Reinforced_Artifact *pos2, Reinforced_Artifact *pos3, Reinforced_Artifact *pos4, Reinforced_Artifact *pos5, Artifact *&suit1, Artifact *&suit2)
{
    int pos1_count = 1;
    int pos2_count = 1;
    int pos3_count = 1;
    int pos4_count = 1;
    int pos5_count = 1;

    if (pos1_count != 0)
    {
        if (pos2->suit_name == pos1->suit_name)
        {
            pos1_count += pos2_count;
            pos2_count = 0;
        }
        if (pos3->suit_name == pos1->suit_name)
        {
            pos1_count += pos3_count;
            pos3_count = 0;
        }
        if (pos4->suit_name == pos1->suit_name)
        {
            pos1_count += pos4_count;
            pos4_count = 0;
        }
        if (pos5->suit_name == pos1->suit_name)
        {
            pos1_count += pos5_count;
            pos5_count = 0;
        }
    }
    if (pos1_count >= 4)
    {
        suit1 = suit2 = find_artifact_by_name(pos1->suit_name);
        return;
    }
    else if (pos1_count >= 2)
    {
        if (suit1 == nullptr) suit1 = find_artifact_by_name(pos1->suit_name);
        else if (suit2 == nullptr)
        {
            suit2 = find_artifact_by_name(pos1->suit_name);
            return;
        }
    }

    if (pos2_count != 0)
    {
        if (pos3->suit_name == pos2->suit_name)
        {
            pos2_count += pos3_count;
            pos3_count = 0;
        }
        if (pos4->suit_name == pos2->suit_name)
        {
            pos2_count += pos4_count;
            pos4_count = 0;
        }
        if (pos5->suit_name == pos2->suit_name)
        {
            pos2_count += pos5_count;
            pos5_count = 0;
        }
    }
    if (pos2_count >= 4)
    {
        suit1 = suit2 = find_artifact_by_name(pos2->suit_name);
        return;
    }
    else if (pos2_count >= 2)
    {
        if (suit1 == nullptr) suit1 = find_artifact_by_name(pos2->suit_name);
        else if (suit2 == nullptr)
        {
            suit2 = find_artifact_by_name(pos2->suit_name);
            return;
        }
    }

    if (pos3_count != 0)
    {
        if (pos4->suit_name == pos3->suit_name)
        {
            pos3_count += pos4_count;
            pos4_count = 0;
        }
        if (pos5->suit_name == pos3->suit_name)
        {
            pos3_count += pos5_count;
            pos5_count = 0;
        }
    }
    if (pos3_count >= 4)
    {
        suit1 = suit2 = find_artifact_by_name(pos3->suit_name);
        return;
    }
    else if (pos3_count >= 2)
    {
        if (suit1 == nullptr) suit1 = find_artifact_by_name(pos3->suit_name);
        else if (suit2 == nullptr)
        {
            suit2 = find_artifact_by_name(pos3->suit_name);
            return;
        }
    }

    if (pos4_count != 0)
    {
        if (pos5->suit_name == pos4->suit_name)
        {
            pos4_count += pos5_count;
            pos5_count = 0;
        }
    }
    if (pos4_count >= 4)
    {
        suit1 = suit2 = find_artifact_by_name(pos4->suit_name);
        return;
    }
    else if (pos4_count >= 2)
    {
        if (suit1 == nullptr) suit1 = find_artifact_by_name(pos4->suit_name);
        else if (suit2 == nullptr)
        {
            suit2 = find_artifact_by_name(pos4->suit_name);
            return;
        }
    }

    if (suit1 == nullptr) suit1 = find_artifact_by_name("EMPTY");
    if (suit2 == nullptr) suit2 = find_artifact_by_name("EMPTY");
}

struct cmp
{
    bool operator()(Group *a, Group *b)
    {
        if (a->total_damage > b->total_damage) return true;
        else return false;
    }
};

void cal_optimal_artifact(string c_name)
{
    vector<Combination *> combination_list;
    get_all_config(c_name, combination_list, "cal_optimal_artifact");
    Character *c_point = find_character_by_name(c_name);
    for (auto &comb_index: combination_list)
    {
        Group *optimal = nullptr;
        priority_queue<Group *, vector<Group *>, cmp> c_w_pair;

        auto start = chrono::system_clock::now();

        for (int pos1_index = 0; pos1_index < reinforced_artifact_list[0].size(); ++pos1_index)
        {
            if (reinforced_artifact_list[0][pos1_index]->character_belong != "none") continue;
            for (int pos2_index = 0; pos2_index < reinforced_artifact_list[1].size(); ++pos2_index)
            {
                if (reinforced_artifact_list[1][pos2_index]->character_belong != "none") continue;
                for (int pos3_index = 0; pos3_index < reinforced_artifact_list[2].size(); ++pos3_index)
                {
                    if (reinforced_artifact_list[2][pos3_index]->character_belong != "none") continue;
                    for (int pos4_index = 0; pos4_index < reinforced_artifact_list[3].size(); ++pos4_index)
                    {
                        if (reinforced_artifact_list[3][pos4_index]->character_belong != "none") continue;
                        if (reinforced_artifact_list[3][pos4_index]->main_type.find("伤害加成") != string::npos && reinforced_artifact_list[3][pos4_index]->main_type != (c_point->ele_type + "伤害加成")) continue;
                        for (int pos5_index = 0; pos5_index < reinforced_artifact_list[4].size(); ++pos5_index)
                        {
                            if (reinforced_artifact_list[4][pos5_index]->character_belong != "none") continue;

                            Artifact *suit1 = nullptr;
                            Artifact *suit2 = nullptr;
                            get_suit(reinforced_artifact_list[0][pos1_index], reinforced_artifact_list[1][pos2_index], reinforced_artifact_list[2][pos3_index],
                                     reinforced_artifact_list[3][pos4_index], reinforced_artifact_list[4][pos5_index], suit1, suit2);
                            string main3 = reinforced_artifact_list[2][pos3_index]->main_type;
                            string main4 = (reinforced_artifact_list[3][pos4_index]->main_type.find("伤害加成") != string::npos) ? "伤害加成" : reinforced_artifact_list[3][pos4_index]->main_type;
                            string main5 = reinforced_artifact_list[4][pos5_index]->main_type;

                            //check config restrictions
                            bool valid = false;
                            if ((comb_index->suit1 == nullptr || comb_index->suit1 == suit1) && (comb_index->suit2 == nullptr || comb_index->suit2 == suit2)) valid = true;
                            if ((comb_index->suit1 == nullptr || comb_index->suit1 == suit2) && (comb_index->suit2 == nullptr || comb_index->suit2 == suit1)) valid = true;
                            if (!valid) continue;
                            if (!comb_index->a_main3.empty() && comb_index->a_main3 != main3) continue;
                            if (!comb_index->a_main4.empty() && comb_index->a_main4 != main4) continue;
                            if (!comb_index->a_main5.empty() && comb_index->a_main5 != main5) continue;

                            auto *temp = new Group(c_point, comb_index->w_point, suit1, suit2, main3, main4, main5,
                                                   comb_index->team_config, comb_index->attack_config_list, comb_index->need_to_satisfy_recharge);
                            temp->data[0] = reinforced_artifact_list[0][pos1_index];
                            temp->data[1] = reinforced_artifact_list[1][pos2_index];
                            temp->data[2] = reinforced_artifact_list[2][pos3_index];
                            temp->data[3] = reinforced_artifact_list[3][pos4_index];
                            temp->data[4] = reinforced_artifact_list[4][pos5_index];

                            int check_num = temp->init_check_data(2);
                            if (check_num == 0)//pass
                            {
                                temp->cal_assigned_artifact_damage();
                                c_w_pair.push(temp);

                                if (optimal == nullptr || optimal->total_damage < temp->total_damage) optimal = temp;
                                while (!c_w_pair.empty())
                                {
                                    Group *c_w = c_w_pair.top();
                                    if (c_w->total_damage / optimal->total_damage < out_filter_percentage)
                                    {
                                        c_w_pair.pop();
                                        delete c_w;
                                    }
                                    else break;
                                }
                            }
                            else if (check_num == 1)//error:suit1
                            {
                                delete temp;
                                goto NEXTPOS5;
                            }
                            else if (check_num == 2)//error:suit2
                            {
                                delete temp;
                                goto NEXTPOS5;
                            }
                            else if (check_num == 3)//error:pos3
                            {
                                delete temp;
                                goto NEXTPOS3;
                            }
                            else if (check_num == 4)//error:pos4
                            {
                                delete temp;
                                goto NEXTPOS4;
                            }
                            else if (check_num == 5)//error:pos5
                            {
                                delete temp;
                                goto NEXTPOS5;
                            }
                            else if (check_num == 6)//error:recharge
                            {
                                delete temp;
                                goto NEXTPOS5;
                            }
                            NEXTPOS5:;
                        }
                        NEXTPOS4:;
                    }
                    NEXTPOS3:;
                }
            }
        }

        chrono::duration<double> time = chrono::system_clock::now() - start;

        while (!c_w_pair.empty())
        {
            Group *c_w = c_w_pair.top();
            c_w_pair.pop();
            c_w->out_assigned_artifact();
            delete c_w;
        }

        cout << c_point->name << " " << comb_index->w_point->name << " " << " time=" << time.count() << "s" << ((time.count() > 30) ? "!!!" : "") << " ";
        for (auto &i: comb_index->attack_config_list)
            cout << to_string(i->attack_time) + "*" + i->condition->attack_way + "_" + (i->background ? "后台" : "前台") + "_" + i->react_type + "/";
        cout << " " << comb_index->team_config->teammate_all + "_" + comb_index->team_config->team_weapon_artifact << "\n";
    }
    combination_list.clear();
}

//generate_gcsim_script

pair<vector<Character *>, vector<string>> team_script;

size_t replace_all(string &inout, string_view what, string_view with)
{
    size_t count{};
    for (string::size_type pos{}; string::npos != (pos = inout.find(what.data(), pos, what.length())); pos += with.length(), ++count)
        inout.replace(pos, what.length(), with.data(), with.length());
    return count;
}

void read_team_script(string filename)
{
    ifstream infile;
    infile.open(filename);

    team_script.first.clear();
    string c_name;
    for (int i = 0; i < 4; ++i)
    {
        infile >> c_name;
        team_script.first.push_back(find_character_by_name(c_name));
    }

    team_script.second.clear();
    string info;
    getline(infile, info);
    while (!infile.eof())
    {
        getline(infile, info);
        replace_all(info, team_script.first[0]->name, team_script.first[0]->english_name);
        replace_all(info, team_script.first[1]->name, team_script.first[1]->english_name);
        replace_all(info, team_script.first[2]->name, team_script.first[2]->english_name);
        replace_all(info, team_script.first[3]->name, team_script.first[3]->english_name);
        replace_all(info, "平A", "attack");
        replace_all(info, "重A", "charge");
        replace_all(info, "下落A", "plunge");
        replace_all(info, "E", "skill");
        replace_all(info, "Q", "burst");
        replace_all(info, "冲刺", "dash");
        replace_all(info, "跳跃", "jump");
        team_script.second.push_back(info);
    }

    infile.close();
}

pair<string, string> main_convert(string ele_type, string main)
{
    if (main == "生命值") return make_pair("hp%", "0.466");
    if (main == "攻击力") return make_pair("atk%", "0.466");
    if (main == "防御力") return make_pair("def%", "0.583");
    if (main == "元素充能效率") return make_pair("er", "0.518");
    if (main == "元素精通") return make_pair("em", "187");
    if (main == "暴击率") return make_pair("cr", "0.311");
    if (main == "暴击伤害") return make_pair("cd", "0.622");
    if (main == "治疗加成") return make_pair("heal", "0.359");
    if (main == "伤害加成" && ele_type == "火") return make_pair("pyro%", "0.466");
    if (main == "伤害加成" && ele_type == "水") return make_pair("hydro%", "0.466");
    if (main == "伤害加成" && ele_type == "雷") return make_pair("electro%", "0.466");
    if (main == "伤害加成" && ele_type == "冰") return make_pair("cryo%", "0.466");
    if (main == "伤害加成" && ele_type == "风") return make_pair("anemo%", "0.466");
    if (main == "伤害加成" && ele_type == "岩") return make_pair("geo%", "0.466");
    if (main == "伤害加成" && ele_type == "草") return make_pair("dendro%", "0.466");
    if (main == "物理元素伤害加成" && ele_type == "物理") return make_pair("phys%", "0.583");
    return make_pair("", "");
}

void generate_gcsim_script(string filepath, string teamname)
{
    vector<Combination *> combination_list_1;
    get_all_config(team_script.first[0]->name, combination_list_1, "generate_gcsim_script");
    vector<Combination *> combination_list_2;
    get_all_config(team_script.first[1]->name, combination_list_2, "generate_gcsim_script");
    vector<Combination *> combination_list_3;
    get_all_config(team_script.first[2]->name, combination_list_3, "generate_gcsim_script");
    vector<Combination *> combination_list_4;
    get_all_config(team_script.first[3]->name, combination_list_4, "generate_gcsim_script");

    int filecount = 1;
    for (auto &combination_1: combination_list_1)
        if (combination_1->team_config == nullptr ||
            (combination_1->team_config->teammate_all.find(team_script.first[1]->name) != string::npos &&
             combination_1->team_config->teammate_all.find(team_script.first[2]->name) != string::npos &&
             combination_1->team_config->teammate_all.find(team_script.first[3]->name) != string::npos))
            for (auto &combination_2: combination_list_2)
                if (combination_2->team_config == nullptr ||
                    (combination_2->team_config->teammate_all.find(team_script.first[0]->name) != string::npos &&
                     combination_2->team_config->teammate_all.find(team_script.first[2]->name) != string::npos &&
                     combination_2->team_config->teammate_all.find(team_script.first[3]->name) != string::npos))
                    for (auto &combination_3: combination_list_3)
                        if (combination_3->team_config == nullptr ||
                            (combination_3->team_config->teammate_all.find(team_script.first[1]->name) != string::npos &&
                             combination_3->team_config->teammate_all.find(team_script.first[0]->name) != string::npos &&
                             combination_3->team_config->teammate_all.find(team_script.first[3]->name) != string::npos))
                            for (auto &combination_4: combination_list_4)
                                if (combination_4->team_config == nullptr ||
                                    (combination_4->team_config->teammate_all.find(team_script.first[1]->name) != string::npos &&
                                     combination_4->team_config->teammate_all.find(team_script.first[2]->name) != string::npos &&
                                     combination_4->team_config->teammate_all.find(team_script.first[0]->name) != string::npos))
                                {
                                    //ps1
                                    outfile_debug << "./run_substat_optimizer_full.bat " << (teamname + "_" + to_string(filecount) + ".txt") << " > ./logs/" << (teamname + "_" + to_string(filecount) + ".txt") << endl;

                                    //config
                                    outfile_result.open(filepath + teamname + "_" + to_string(filecount) + ".txt");
                                    outfile_result << "#./run_substat_optimizer_full.bat " << (teamname + "_" + to_string(filecount) + ".txt") << " > ./logs/" << (teamname + "_" + to_string(filecount) + ".txt") << endl;
                                    outfile_result << endl;

                                    //head
                                    outfile_result << "options iteration=1000 duration=120 swap_delay=4;" << endl;
                                    outfile_result << "target lvl=95 resist=0.1 particle_threshold=150000 particle_drop_count=1;" << endl;

                                    //character1
                                    outfile_result << "#" << team_script.first[0]->name << " " << combination_1->w_point->name << " " << combination_1->suit1->name << " " << combination_1->suit2->name << " "
                                                   << combination_1->a_main3 << " " << combination_1->a_main4 << " " << combination_1->a_main5 << endl;

                                    outfile_result << team_script.first[0]->english_name << " char lvl=90/90 cons=" << to_string(team_script.first[0]->constellation)
                                                   << " talent=" << to_string(team_script.first[0]->A_level) << ","
                                                   << to_string((team_script.first[0]->E_level > 10) ? (team_script.first[0]->E_level - 3) : team_script.first[0]->E_level) << ","
                                                   << to_string((team_script.first[0]->Q_level > 10) ? (team_script.first[0]->Q_level - 3) : team_script.first[0]->Q_level) << ";" << endl;
                                    outfile_result << team_script.first[0]->english_name << " add weapon=\"" << combination_1->w_point->english_name << "\" refine=" << to_string(combination_1->w_point->level) << " lvl=90/90;" << endl;
                                    if (combination_1->suit1 == combination_1->suit2)
                                    {
                                        outfile_result << team_script.first[0]->english_name << " add set=\"" << combination_1->suit1->english_name << "\" count=4;" << endl;
                                    }
                                    else
                                    {
                                        outfile_result << team_script.first[0]->english_name << " add set=\"" << combination_1->suit1->english_name << "\" count=2;" << endl;
                                        outfile_result << team_script.first[0]->english_name << " add set=\"" << combination_1->suit2->english_name << "\" count=2;" << endl;
                                    }
                                    outfile_result << team_script.first[0]->english_name << " add stats hp=4780 atk=311 "
                                                   << main_convert(team_script.first[0]->ele_type, combination_1->a_main3).first << "=" << main_convert(team_script.first[0]->ele_type, combination_1->a_main3).second << " "
                                                   << main_convert(team_script.first[0]->ele_type, combination_1->a_main4).first << "=" << main_convert(team_script.first[0]->ele_type, combination_1->a_main4).second << " "
                                                   << main_convert(team_script.first[0]->ele_type, combination_1->a_main5).first << "=" << main_convert(team_script.first[0]->ele_type, combination_1->a_main5).second << ";" << endl;
                                    outfile_result << endl;

                                    //character2
                                    outfile_result << "#" << team_script.first[1]->name << " " << combination_2->w_point->name << " " << combination_2->suit1->name << " " << combination_2->suit2->name << " "
                                                   << combination_2->a_main3 << " " << combination_2->a_main4 << " " << combination_2->a_main5 << endl;

                                    outfile_result << team_script.first[1]->english_name << " char lvl=90/90 cons=" << to_string(team_script.first[1]->constellation)
                                                   << " talent=" << to_string(team_script.first[1]->A_level) << ","
                                                   << to_string((team_script.first[1]->E_level > 10) ? (team_script.first[1]->E_level - 3) : team_script.first[1]->E_level) << ","
                                                   << to_string((team_script.first[1]->Q_level > 10) ? (team_script.first[1]->Q_level - 3) : team_script.first[1]->Q_level) << ";" << endl;
                                    outfile_result << team_script.first[1]->english_name << " add weapon=\"" << combination_2->w_point->english_name << "\" refine=" << to_string(combination_2->w_point->level) << " lvl=90/90;" << endl;
                                    if (combination_2->suit1 == combination_2->suit2)
                                    {
                                        outfile_result << team_script.first[1]->english_name << " add set=\"" << combination_2->suit1->english_name << "\" count=4;" << endl;
                                    }
                                    else
                                    {
                                        outfile_result << team_script.first[1]->english_name << " add set=\"" << combination_2->suit1->english_name << "\" count=2;" << endl;
                                        outfile_result << team_script.first[1]->english_name << " add set=\"" << combination_2->suit2->english_name << "\" count=2;" << endl;
                                    }
                                    outfile_result << team_script.first[1]->english_name << " add stats hp=4780 atk=311 "
                                                   << main_convert(team_script.first[1]->ele_type, combination_2->a_main3).first << "=" << main_convert(team_script.first[1]->ele_type, combination_2->a_main3).second << " "
                                                   << main_convert(team_script.first[1]->ele_type, combination_2->a_main4).first << "=" << main_convert(team_script.first[1]->ele_type, combination_2->a_main4).second << " "
                                                   << main_convert(team_script.first[1]->ele_type, combination_2->a_main5).first << "=" << main_convert(team_script.first[1]->ele_type, combination_2->a_main5).second << ";" << endl;
                                    outfile_result << endl;

                                    //character3
                                    outfile_result << "#" << team_script.first[2]->name << " " << combination_3->w_point->name << " " << combination_3->suit1->name << " " << combination_3->suit2->name << " "
                                                   << combination_3->a_main3 << " " << combination_3->a_main4 << " " << combination_3->a_main5 << endl;

                                    outfile_result << team_script.first[2]->english_name << " char lvl=90/90 cons=" << to_string(team_script.first[2]->constellation)
                                                   << " talent=" << to_string(team_script.first[2]->A_level) << ","
                                                   << to_string((team_script.first[2]->E_level > 10) ? (team_script.first[2]->E_level - 3) : team_script.first[2]->E_level) << ","
                                                   << to_string((team_script.first[2]->Q_level > 10) ? (team_script.first[2]->Q_level - 3) : team_script.first[2]->Q_level) << ";" << endl;
                                    outfile_result << team_script.first[2]->english_name << " add weapon=\"" << combination_3->w_point->english_name << "\" refine=" << to_string(combination_3->w_point->level) << " lvl=90/90;" << endl;
                                    if (combination_3->suit1 == combination_3->suit2)
                                    {
                                        outfile_result << team_script.first[2]->english_name << " add set=\"" << combination_3->suit1->english_name << "\" count=4;" << endl;
                                    }
                                    else
                                    {
                                        outfile_result << team_script.first[2]->english_name << " add set=\"" << combination_3->suit1->english_name << "\" count=2;" << endl;
                                        outfile_result << team_script.first[2]->english_name << " add set=\"" << combination_3->suit2->english_name << "\" count=2;" << endl;
                                    }
                                    outfile_result << team_script.first[2]->english_name << " add stats hp=4780 atk=311 "
                                                   << main_convert(team_script.first[2]->ele_type, combination_3->a_main3).first << "=" << main_convert(team_script.first[2]->ele_type, combination_3->a_main3).second << " "
                                                   << main_convert(team_script.first[2]->ele_type, combination_3->a_main4).first << "=" << main_convert(team_script.first[2]->ele_type, combination_3->a_main4).second << " "
                                                   << main_convert(team_script.first[2]->ele_type, combination_3->a_main5).first << "=" << main_convert(team_script.first[2]->ele_type, combination_3->a_main5).second << ";" << endl;
                                    outfile_result << endl;

                                    //character4
                                    outfile_result << "#" << team_script.first[3]->name << " " << combination_4->w_point->name << " " << combination_4->suit1->name << " " << combination_4->suit2->name << " "
                                                   << combination_4->a_main3 << " " << combination_4->a_main4 << " " << combination_4->a_main5 << endl;

                                    outfile_result << team_script.first[3]->english_name << " char lvl=90/90 cons=" << to_string(team_script.first[3]->constellation)
                                                   << " talent=" << to_string(team_script.first[3]->A_level) << ","
                                                   << to_string((team_script.first[3]->E_level > 10) ? (team_script.first[3]->E_level - 3) : team_script.first[3]->E_level) << ","
                                                   << to_string((team_script.first[3]->Q_level > 10) ? (team_script.first[3]->Q_level - 3) : team_script.first[3]->Q_level) << ";" << endl;
                                    outfile_result << team_script.first[3]->english_name << " add weapon=\"" << combination_4->w_point->english_name << "\" refine=" << to_string(combination_4->w_point->level) << " lvl=90/90;" << endl;
                                    if (combination_4->suit1 == combination_4->suit2)
                                    {
                                        outfile_result << team_script.first[3]->english_name << " add set=\"" << combination_4->suit1->english_name << "\" count=4;" << endl;
                                    }
                                    else
                                    {
                                        outfile_result << team_script.first[3]->english_name << " add set=\"" << combination_4->suit1->english_name << "\" count=2;" << endl;
                                        outfile_result << team_script.first[3]->english_name << " add set=\"" << combination_4->suit2->english_name << "\" count=2;" << endl;
                                    }
                                    outfile_result << team_script.first[3]->english_name << " add stats hp=4780 atk=311 "
                                                   << main_convert(team_script.first[3]->ele_type, combination_4->a_main3).first << "=" << main_convert(team_script.first[3]->ele_type, combination_4->a_main3).second << " "
                                                   << main_convert(team_script.first[3]->ele_type, combination_4->a_main4).first << "=" << main_convert(team_script.first[3]->ele_type, combination_4->a_main4).second << " "
                                                   << main_convert(team_script.first[3]->ele_type, combination_4->a_main5).first << "=" << main_convert(team_script.first[3]->ele_type, combination_4->a_main5).second << ";" << endl;
                                    outfile_result << endl;

                                    //active
                                    outfile_result << "active " << team_script.second[0].substr(0, team_script.second[0].find_first_of(' ')) << ";" << endl;
                                    outfile_result << endl;

                                    //attack_list
                                    outfile_result << "let x = 5;" << endl;
                                    outfile_result << "while x {" << endl;
                                    for (auto &i: team_script.second)
                                    {
                                        if (i[i.length() - 1] == '{' || i[i.length() - 1] == '}')
                                            outfile_result << "  " << i << endl;
                                        else
                                            outfile_result << "  " << i << ";" << endl;
                                    }
                                    outfile_result << "  x = x - 1;" << endl;
                                    outfile_result << "}";
                                    outfile_result.close();

                                    filecount++;
                                }
}

int main()
{
    init_character_data();
    init_weapon_data();
    init_artifact_data();

    int mode = 0;
    cout << "计算模式(1:cal_deployment 2:cal_add_deployment 3:cal_optimal_artifact 4:generate_gcsim_script):";
    cin >> mode;
    if (mode == 1)
    {
        outfile_result.open("./RESULTS/data.csv");
        if (out_debug) outfile_debug.open("./RESULTS/log_data.csv");
        cal_deployment("ALL");
        outfile_result.close();
        outfile_debug.close();
    }
    else if (mode == 2)
    {
        outfile_result.open("./RESULTS/data_add.csv");
        if (out_debug) outfile_debug.open("./RESULTS/log_data_add.csv");
        cal_deployment("ADD");
        outfile_result.close();
        outfile_debug.close();
    }
    else if (mode == 3)
    {
        string c_name;
        cout << "人物:";
        cin >> c_name;

        outfile_result.open("./RESULTS/assign_artifact_data_" + c_name + ".csv");
        if (out_debug) outfile_debug.open("./RESULTS/assign_artifact_log_" + c_name + ".csv");
        read_artifact("./RESULTS/artifacts.txt");
        cal_optimal_artifact(c_name);
        outfile_result.close();
        outfile_debug.close();
    }
    else if (mode == 4)
    {
        vector<string> team_list;
        string team_name;
        getline(cin, team_name);
        while (true)
        {
            cout << "输入中文配置文件名(ENTER结束):";
            getline(cin, team_name);
            if (team_name.empty()) break;
            team_list.push_back(team_name);
        }

        outfile_debug.open("./RESULTS/calculate_all.ps1");
        for (auto &i: team_list)
        {
            read_team_script("./RESULTS/chinese_config/" + i + ".txt");
            generate_gcsim_script("./RESULTS/config/", i);
        }
        outfile_debug.close();
    }
    return 0;
}