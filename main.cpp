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

ofstream outfile_result;
ofstream outfile_debug;
bool out_header = true;
string double_E_per_round = "神里绫华甘雨温迪夜兰枫原万叶班尼特";
int max_up_num_per_base = 4;
double max_attribute_num_per_pos = 2.5;
int max_entry_num = 30;
int artifact_2_2_max_entry_bonus = 2;

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

//TODO:less important 将所有添加的属性分为face_percentage,converted_face_percentage,monster_percentage，增加过程为face(get_convert)->converted->monster，目前来看只有增伤、暴击、爆伤可能不加在面板上，不影响结果（没有增伤/暴击/爆伤转别的）
//TODO:important 部分加成默认触发，应根据攻击序列确定触发
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
                                           10, vector<bool>{true, true, false, false, false, true, true, true}, "火", vector<double>{0.836, 0.861, 1.089, 1.171, 0.594, 0.628, 1.534}, vector<double>{0.789, 0.812, 1.027, 1.104, 0.56, 0.592, 1.446},
                                           "火", vector<double>{2.426}, vector<double>{2.287}, "火", vector<double>{2.92}, vector<double>{2.75},
                                           10, 5, false, vector<bool>{true, true, false, false, false, true, true, true}, vector<double>{1.36}, vector<double>{1.28}, vector<double>{1.15}, vector<double>{1.09},
                                           10, 60, false, vector<bool>{true, true, false, false, false, true, true, true}, vector<double>{7.06}, vector<double>{6.76}, vector<double>{6.17}, vector<double>{5.88},
                                           1, temp, false, false, false));//A4、5连续，E血梅香，Q默认低血量
    temp.clear();
    //E 生命转化攻击6.26%*max_life<400%*base_atk (convert)

    temp.push_back(nullptr);//重击或下落攻击命中敌人时，将产生一枚琢光镜。该效果每12秒至多触发一次。
    temp.push_back(nullptr);//EQ增伤0.001*mastery<100 (convert)
    temp.push_back(nullptr);//光幕攻击命中敌人时，将使共相·理式摹写的冷却时间减少1.2秒。该效果每1秒至多触发一次。->e cd 12s
    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "元素精通", 150));//每1枚产生的琢光镜将使元素精通提升50点，4层，持续8秒，最多4层。默认3层，8s
    temp.push_back(new Set(new Condition("草", "ALL", "ALL"), "伤害加成", 0.3));//Q每消耗1枚琢光镜，使队伍中的其他角色元素精通提升30点，持续15秒，每产生1枚琢光镜，使艾尔海森获得10%草元素伤害加成，持续15秒。默认先Q，3层，15s (get_team)
    temp.push_back(nullptr);//产生琢光镜时，若琢光镜数量已达到上限，艾尔海森的暴击率提升10%，暴击伤害提升70%，持续6秒，默认触发 (special)
    character_list.push_back(new Character("艾尔海森", "alhaitham", "草", "单手剑", 13348, 313, 782, "伤害加成", 0.288,
                                           10, vector<bool>{false, true, false, false, false, true, true, true}, "草", vector<double>{0.979, 1.003, 0.676, 0.676, 1.32, 1.658}, vector<double>{0.91, 0.932, 0.628, 0.628, 1.227, 1.541},
                                           "草", vector<double>{1.092, 1.092}, vector<double>{1.015, 1.015}, "草", vector<double>{3.16}, vector<double>{2.93},
                                           10, 8, false, vector<bool>{false, true, false, true, false, true, true, true}, vector<double>{4.114, 1.482}, vector<double>{3.872, 1.344}, vector<double>{3.485, 1.21}, vector<double>{3.291, 1.142},
                                           10, 70, true, vector<bool>{false, true, false, true, false, true, true, true}, vector<double>{2.584}, vector<double>{2.432}, vector<double>{2.189}, vector<double>{2.067},
                                           0, temp, false, false, true));//A2、3连续，Z0、1连续，E突进、1层、2层、3层，Q单次
    temp.clear();
    //EQ精通倍率 (extra_rate)

    temp.push_back(nullptr);//队伍获得元素微粒增加2层愿力
    temp.push_back(nullptr);//充能转化雷伤(recharge-1)*0.4 (convert) 回能提高(recharge-1)*0.6 (recharge)
    temp.push_back(nullptr);//雷元素角色释放元素爆发后，积攒的愿力提升80%；其他元素类型角色施放元素爆发后，积攒的愿力提升20%。
    temp.push_back(new Set(new Condition("ALL", "ALL", "E|Q"), "防御无视", 0.6));//Q后
    temp.push_back(nullptr);//Q结束 队友30%攻击10s (get_team)
    temp.push_back(nullptr);//Q伤害命中，1次减队友CD1s/5次
    character_list.push_back(new Character("雷电将军", "raiden", "雷", "长柄武器", 12907, 337, 789, "元素充能效率", 0.32,
                                           6, vector<bool>{false, true, false, false, false, true, true, true}, "物理", vector<double>{0.784, 0.785, 0.986, 0.573, 0.573, 1.294}, vector<double>{0.728, 0.73, 0.916, 0.533, 0.533, 1.202},
                                           "物理", vector<double>{1.969}, vector<double>{1.83}, "物理", vector<double>{3.16}, vector<double>{2.93},
                                           10, 10, false, vector<bool>{false, true, false, false, true, true, true, true}, vector<double>{2.491, 0.893}, vector<double>{2.344, 0.84}, vector<double>{2.11, 0.756}, vector<double>{1.992, 0.714},
                                           10, 90, false, vector<bool>{false, true, false, false, true, true, true, true},
                                           vector<double>{8.52 + 0.0826 * 60, 0.935 + 0.0154 * 60, 0.919 + 0.0154 * 60, 1.125 + 0.0154 * 60, 0.646 + 0.0154 * 60, 0.646 + 0.0154 * 60, 1.546 + 0.0154 * 60, 1.288 + 0.0154 * 60, 1.555 + 0.0154 * 60},
                                           vector<double>{8.02 + 0.0778 * 60, 0.89 + 0.0145 * 60, 0.874 + 0.0145 * 60, 1.07 + 0.0145 * 60, 0.614 + 0.0145 * 60, 0.614 + 0.0145 * 60, 1.471 + 0.0145 * 60, 1.225 + 0.0145 * 60, 1.479 + 0.0145 * 60},
                                           vector<double>{7.21 + 0.07 * 60, 0.798 + 0.0131 * 60, 0.784 + 0.0131 * 60, 0.96 + 0.0131 * 60, 0.551 + 0.0131 * 60, 0.551 + 0.0131 * 60, 1.319 + 0.0131 * 60, 1.099 + 0.0131 * 60, 1.327 + 0.0131 * 60},
                                           vector<double>{6.81 + 0.0661 * 60, 0.752 + 0.0123 * 60, 0.739 + 0.0123 * 60, 0.905 + 0.0123 * 60, 0.52 + 0.0123 * 60, 0.52 + 0.0123 * 60, 1.244 + 0.0123 * 60, 1.036 + 0.0123 * 60, 1.251 + 0.0123 * 60},
                                           2, temp, false, false, true));//A3、4连续，E释放、协同，Q一刀，后续平A1-6，4、5连续，重A7-8连续
    temp.clear();
    //E 全体大招增伤0.003*Q_energy (special get_team) Q全体回能12.5 (recharge)
    //TODO:设定为2命

    temp.push_back(new Set(new Condition("ALL", "ALL", "平A|重A"), "伤害加成", 0.3));//E后
    temp.push_back(new Set(new Condition("冰", "ALL", "ALL"), "伤害加成", 0.18));//冲刺命中敌人
    temp.push_back(nullptr);//平A重A50%减少ECD0.3s
    temp.push_back(nullptr);//生成2个20%Q伤害的旋风，不考虑
    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "防御削弱", 0.3));//Q命中 (get_team)
    temp.push_back(nullptr);
    //temp.push_back(new Set(new Condition("ALL", "ALL", "重A"), "伤害加成", 2.98));//CD10s，不考虑
    character_list.push_back(new Character("神里绫华", "ayaka", "冰", "单手剑", 12858, 342, 784, "暴击伤害", 0.384,
                                           10, vector<bool>{false, true, false, false, false, true, true, true}, "冰", vector<double>{0.904, 0.962, 1.238, 0.448, 0.448, 0.448, 1.545}, vector<double>{0.84, 0.894, 1.15, 0.416, 0.416, 0.416, 1.436},
                                           "冰", vector<double>{1.09}, vector<double>{1.013}, "冰", vector<double>{3.16}, vector<double>{2.93},
                                           10, 4.5, false, vector<bool>{false, true, false, false, false, true, true, true}, vector<double>{5.083}, vector<double>{4.784}, vector<double>{4.305}, vector<double>{4.066},
                                           10, 80, true, vector<bool>{false, true, false, false, false, true, true, true}, vector<double>{2.386, 3.58}, vector<double>{2.246, 3.369}, vector<double>{2.021, 3.032}, vector<double>{1.909, 2.864},
                                           0, temp, false, false, false));//A3、4、5连续，Z有三下，Q19+1
    temp.clear();

    temp.push_back(new Set(new Condition("ALL", "ALL", "重A"), "暴击率", 0.2));//重A后
    temp.push_back(nullptr);//Q内前台角色 (special) (get_team)
    temp.push_back(new Set(new Condition("冰", "ALL", "ALL"), "抗性削弱", 0.15));//每次重A命中恢复2点能量 (recharge) (get_team)
    temp.push_back(nullptr);//E变为2次 (recharge)
    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "伤害加成", 0.15));//Q内 (get_team)
    temp.push_back(nullptr);//E后瞬发重A
    character_list.push_back(new Character("甘雨", "ganyu", "冰", "弓", 9797, 335, 630, "暴击伤害", 0.384,
                                           10, vector<bool>{false, true, false, false, false, true, true, true}, "物理", vector<double>{0.627, 0.704, 0.899, 0.899, 0.954, 1.139}, vector<double>{0.583, 0.654, 0.836, 0.836, 0.886, 1.059},
                                           "冰", vector<double>{2.3, 3.92}, vector<double>{2.18, 3.7}, "物理", vector<double>{2.81}, vector<double>{2.61},
                                           10, 4, true, vector<bool>{false, true, false, false, false, true, true, true}, vector<double>{2.81, 2.81}, vector<double>{2.64, 2.64}, vector<double>{2.38, 2.38}, vector<double>{2.24, 2.24},
                                           10, 60, true, vector<bool>{false, true, false, false, false, true, true, true}, vector<double>{1.49}, vector<double>{1.41}, vector<double>{1.26}, vector<double>{1.19},
                                           0, temp, false, false, false));//Z两段，E两段，Q单个
    temp.clear();

    temp.push_back(nullptr);
    temp.push_back(nullptr);
    temp.push_back(nullptr);
    temp.push_back(nullptr);
    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "元素精通", 130));//默认平均
    temp.push_back(nullptr);//与E一致，倍率为200%ATK+400%EM
    character_list.push_back(new Character("纳西妲", "nahida", "草", "法器", 10360, 299, 630, "元素精通", 115.0,
                                           6, vector<bool>{false, true, false, false, false, true, true, true}, "草", vector<double>{0.726, 0.666, 0.826, 1.051}, vector<double>{0.685, 0.629, 0.78, 0.993},
                                           "草", vector<double>{2.376}, vector<double>{2.244}, "草", vector<double>{2.81}, vector<double>{2.61},
                                           10, 7, false, vector<bool>{false, true, false, true, false, true, true, true}, vector<double>{2.771, 2.193}, vector<double>{2.608, 2.064}, vector<double>{2.347, 1.858}, vector<double>{2.217, 1.754},
                                           10, 50, false, vector<bool>{false, false, false, false, false, false, false, false}, vector<double>{}, vector<double>{}, vector<double>{}, vector<double>{},
                                           2, temp, false, false, true));
    temp.clear();
    //E精通倍率(extra_rate) Q(special) 天赋1(special) (get_team) 天赋2(get_convert) 1命(special) 2命(special) (get_team) (react)

    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "生命值", 0.18));//3色队
    temp.push_back(nullptr);//前台角色伤害提升1+3.5x% (special get_team)
    temp.push_back(nullptr);//E变为2次 (recharge)
    temp.push_back(nullptr);//每1.8s Q倍率增加14%*LIFE (extra_rate)
    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "生命值", 0.4));//E后 25s (get_team)
    temp.push_back(nullptr);//Q后平A变为重A，伤害*1.56*5
    character_list.push_back(new Character("夜兰", "yelan", "水", "弓", 14450, 244, 548, "暴击率", 0.192,
                                           6, vector<bool>{false, true, false, false, false, true, true, true}, "物理", vector<double>{0.804, 0.772, 1.02, 0.643, 0.643}, vector<double>{0.747, 0.717, 0.948, 0.597, 0.597},
                                           "水", vector<double>{2.23}, vector<double>{2.11}, "物理", vector<double>{2.81}, vector<double>{2.61},
                                           10, 4, false, vector<bool>{true, false, false, false, false, true, true, true}, vector<double>{0}, vector<double>{0}, vector<double>{0}, vector<double>{0},
                                           10, 70, false, vector<bool>{true, false, false, false, false, true, true, true}, vector<double>{0, 0, 0}, vector<double>{0, 0, 0}, vector<double>{0, 0, 0}, vector<double>{0, 0, 0},
                                           2, temp, false, false, false));//A3、4连续，Z不考虑特殊
    temp.clear();
    //EQ 生命倍率 (extra_rate)

    temp.push_back(nullptr);//Q重置E
    temp.push_back(nullptr);//精通转化E伤害mastery*0.15% (convert)
    temp.push_back(nullptr);//每个E为Q后恢复8点能量，24点 (recharge)
    temp.push_back(nullptr);//E倍率提升
    temp.push_back(new Set(new Condition("雷", "ALL", "ALL"), "伤害加成", 0.2));//E命中 (get_team)
    temp.push_back(new Set(new Condition("ALL", "ALL", "E"), "防御无视", 0.6));
    character_list.push_back(new Character("八重神子", "yaemiko", "雷", "法器", 10372, 340, 569, "暴击率", 0.192,
                                           6, vector<bool>{false, true, false, false, false, true, true, true}, "雷", vector<double>{0.714, 0.693, 1.024}, vector<double>{0.674, 0.655, 0.967},
                                           "雷", vector<double>{2.572}, vector<double>{2.429}, "雷", vector<double>{2.81}, vector<double>{2.61},
                                           10, 5, false, vector<bool>{false, true, false, true, false, true, true, true}, vector<double>{2.518}, vector<double>{2.37}, vector<double>{2.133}, vector<double>{2.015},
                                           10, 90, false, vector<bool>{false, true, false, false, false, true, true, true}, vector<double>{5.53, 7.09}, vector<double>{5.2, 6.68}, vector<double>{4.68, 6.01}, vector<double>{4.42, 5.68},
                                           2, temp, false, false, true));//Q1+3
    temp.clear();

    temp.push_back(nullptr);//E喷火距离提高20%
    temp.push_back(nullptr);//E结束拾取辣椒提高10%攻击 (get_team)，不考虑
    temp.push_back(new Set(new Condition("火", "ALL", "ALL"), "抗性削弱", 0.15));//E伤害 (get_team)
    temp.push_back(nullptr);//平A最后一击2s后造成75%火元素伤害
    temp.push_back(nullptr);//Q变为14s
    temp.push_back(new Set(new Condition("火", "ALL", "平A|重A|下落A|E"), "伤害加成", 0.15));//Q持续期间 (get_team)
    character_list.push_back(new Character("香菱", "xiangling", "火", "长柄武器", 10875, 225, 669, "元素精通", 96,
                                           6, vector<bool>{false, true, false, false, false, true, true, true}, "物理", vector<double>{0.831, 0.833, 0.515, 0.515, 0.279, 0.279, 0.279, 0.279, 1.4}, vector<double>{0.773, 0.774, 0.479, 0.479, 0.259, 0.259, 0.259, 0.259, 1.31},
                                           "物理", vector<double>{2.41}, vector<double>{2.24}, "物理", vector<double>{3.16}, vector<double>{2.93},
                                           9 + 3, 4, true, vector<bool>{false, true, false, false, false, true, true, true}, vector<double>{2.36}, vector<double>{2.23}, vector<double>{2.0}, vector<double>{1.89},
                                           9 + 3, 80, true, vector<bool>{false, true, false, false, false, true, true, true}, vector<double>{1.53, 1.87, 2.33, 2.38}, vector<double>{1.44, 1.76, 2.19, 2.24},
                                           vector<double>{1.3, 1.58, 1.97, 2.02}, vector<double>{1.22, 1.5, 1.86, 1.9},
                                           6, temp, false, false, false));//A2、3连续4、5、6、7连续
    temp.clear();

    temp.push_back(nullptr);//每柄雨帘剑恢复当前角色6%行秋最大生命
    temp.push_back(new Set(new Condition("水", "ALL", "ALL"), "伤害加成", 0.2));
    temp.push_back(nullptr);//雨帘剑变为4柄
    temp.push_back(new Set(new Condition("水", "ALL", "ALL"), "抗性削弱", 0.15));//Q变为18s Q命中 (get_team)
    temp.push_back(nullptr);//Q期间，E倍率*1.5 (special)
    temp.push_back(nullptr);//每2次攻击，下次攻击变为5柄剑，回复3点能量 (recharge)
    character_list.push_back(new Character("行秋", "xingqiu", "水", "单手剑", 10222, 202, 758, "攻击力", 0.24,
                                           6, vector<bool>{false, true, false, false, false, true, true, true}, "物理", vector<double>{0.921, 0.942, 0.564, 0.564, 1.107, 0.709, 0.709}, vector<double>{0.856, 0.875, 0.525, 0.525, 1.029, 0.659, 0.659},
                                           "物理", vector<double>{0.935, 1.11}, vector<double>{0.869, 1.032}, "物理", vector<double>{3.16}, vector<double>{2.93},
                                           9 + 3, 5, false, vector<bool>{false, true, false, false, false, true, true, true}, vector<double>{3.57 * 1.5, 4.06 * 1.5}, vector<double>{3.36 * 1.5, 3.82 * 1.5},
                                           vector<double>{3.02 * 1.5, 3.44 * 1.5}, vector<double>{2.86 * 1.5, 3.25 * 1.5},
                                           9 + 3, 80, false, vector<bool>{false, true, false, false, false, true, true, true}, vector<double>{1.15}, vector<double>{1.09}, vector<double>{0.977}, vector<double>{0.923},
                                           6, temp, false, false, false));//A2、3连续5、6连续，Z0、1连续，E0、1连续
    temp.clear();
    //默认Q后E，触发4命

    temp.push_back(nullptr);//E后蓄力减少
    temp.push_back(nullptr);//E后全队回能
    temp.push_back(nullptr);//Ecd9s
    temp.push_back(nullptr);//E原地30%伤害
    temp.push_back(nullptr);//Q6道雷
    temp.push_back(new Set(new Condition("雷", "ALL", "ALL"), "暴击伤害", 0.6));//E增攻下(get_team)
    character_list.push_back(new Character("九条裟罗", "sara", "雷", "弓", 9570, 195, 628, "攻击力", 0.24,
                                           6, vector<bool>{false, true, false, false, false, true, true, true}, "物理", vector<double>{0.729, 0.765, 0.959, 0.996, 1.148}, vector<double>{0.678, 0.711, 0.891, 0.926, 1.067},
                                           "雷", vector<double>{2.23}, vector<double>{2.11}, "物理", vector<double>{2.81}, vector<double>{2.61},
                                           9 + 3, 3, true, vector<bool>{false, true, false, false, false, true, true, true}, vector<double>{2.672}, vector<double>{2.515}, vector<double>{2.264}, vector<double>{2.138},
                                           9 + 3, 80, false, vector<bool>{false, true, false, false, false, true, true, true}, vector<double>{8.704, 0.725}, vector<double>{8.192, 0.682}, vector<double>{7.373, 0.614}, vector<double>{6.963, 0.58},
                                           6, temp, false, false, false));//A2、3连续5、6连续，Z0、1连续，E0、1连续
    temp.clear();
    //E攻击力加成(special) (get_team) 全队回能(recharge)

    //NO NEED TO CALCULATE
    //BUILD BASIC INFORMATION AND GET_TEAM
    character_list.push_back(new Character("温迪", "venti", "风", "弓", 10531, 263, 669, "元素充能效率", 0.32,
                                           6, vector<bool>{}, "物理", vector<double>{}, vector<double>{},
                                           "风", vector<double>{}, vector<double>{}, "物理", vector<double>{}, vector<double>{},
                                           9, 3, false, vector<bool>{}, vector<double>{}, vector<double>{}, vector<double>{}, vector<double>{},
                                           9, 60, true, vector<bool>{}, vector<double>{}, vector<double>{}, vector<double>{}, vector<double>{},
                                           0, vector<Set *>{}, false, false, false));
    //Q恢复转化元素15能量

    character_list.push_back(new Character("莫娜", "mona", "水", "法器", 10409, 287, 653, "元素充能效率", 0.32,
                                           6, vector<bool>{}, "水", vector<double>{}, vector<double>{},
                                           "水", vector<double>{}, vector<double>{}, "水", vector<double>{}, vector<double>{},
                                           9, 3, true, vector<bool>{}, vector<double>{}, vector<double>{}, vector<double>{}, vector<double>{},
                                           9 + 3, 60, false, vector<bool>{}, vector<double>{}, vector<double>{}, vector<double>{}, vector<double>{},
                                           4, vector<Set *>{}, false, false, false));
    //感电反应造成的伤害提升15%，蒸发反应造成的伤害提升15%，水元素扩散反应造成的伤害提升15%；冻结反应的持续时间延长15%
    //Q后增伤60%，暴击率15%

    character_list.push_back(new Character("钟离", "zhongli", "岩", "长柄武器", 14695, 251, 738, "伤害加成", 0.288,
                                           6, vector<bool>{}, "物理", vector<double>{}, vector<double>{},
                                           "物理", vector<double>{}, vector<double>{}, "物理", vector<double>{}, vector<double>{},
                                           9, 6, true, vector<bool>{}, vector<double>{}, vector<double>{}, vector<double>{}, vector<double>{},
                                           9, 40, false, vector<bool>{}, vector<double>{}, vector<double>{}, vector<double>{}, vector<double>{},
                                           0, vector<Set *>{}, true, false, true));
    //全抗性削弱20%，护盾强效5%*5层

    character_list.push_back(new Character("枫原万叶", "kazuha", "风", "单手剑", 13348, 297, 807, "元素精通", 115.0,
                                           6, vector<bool>{}, "物理", vector<double>{}, vector<double>{},
                                           "物理", vector<double>{}, vector<double>{}, "物理", vector<double>{}, vector<double>{},
                                           9, 4, false, vector<bool>{}, vector<double>{}, vector<double>{}, vector<double>{}, vector<double>{},
                                           9, 60, true, vector<bool>{}, vector<double>{}, vector<double>{}, vector<double>{}, vector<double>{},
                                           0, vector<Set *>{}, false, false, false));
    //扩散增伤mastery*0.0004

    character_list.push_back(new Character("班尼特", "bennett", "火", "单手剑", 12397, 191, 771, "元素充能效率", 0.267,
                                           6, vector<bool>{}, "物理", vector<double>{}, vector<double>{},
                                           "物理", vector<double>{}, vector<double>{}, "物理", vector<double>{}, vector<double>{},
                                           9 + 3, 2, false, vector<bool>{}, vector<double>{}, vector<double>{}, vector<double>{}, vector<double>{},
                                           9 + 3, 60, false, vector<bool>{}, vector<double>{}, vector<double>{}, vector<double>{}, vector<double>{},
                                           6, vector<Set *>{}, false, true, false));
    //Q内增攻(1191+608)*(1.12+0.2)，15%火伤，近战武器火附魔

    //MODEL CHARACTER
    character_list.push_back(new Character("火_test", "", "火", "单手剑", 10000, 300, 700, "暴击率", 0.192,
                                           9, vector<bool>{}, "火", vector<double>{}, vector<double>{},
                                           "火", vector<double>{}, vector<double>{}, "火", vector<double>{}, vector<double>{},
                                           9, 3, false, vector<bool>{}, vector<double>{}, vector<double>{}, vector<double>{}, vector<double>{},
                                           9, 60, false, vector<bool>{}, vector<double>{}, vector<double>{}, vector<double>{}, vector<double>{},
                                           0, vector<Set *>{}, false, false, false));

    character_list.push_back(new Character("水_test", "", "水", "单手剑", 10000, 300, 700, "暴击率", 0.192,
                                           9, vector<bool>{}, "水", vector<double>{}, vector<double>{},
                                           "水", vector<double>{}, vector<double>{}, "水", vector<double>{}, vector<double>{},
                                           9, 3, false, vector<bool>{}, vector<double>{}, vector<double>{}, vector<double>{}, vector<double>{},
                                           9, 60, false, vector<bool>{}, vector<double>{}, vector<double>{}, vector<double>{}, vector<double>{},
                                           0, vector<Set *>{}, false, false, false));

    character_list.push_back(new Character("雷_test", "", "雷", "单手剑", 10000, 300, 700, "暴击率", 0.192,
                                           9, vector<bool>{}, "雷", vector<double>{}, vector<double>{},
                                           "雷", vector<double>{}, vector<double>{}, "雷", vector<double>{}, vector<double>{},
                                           9, 3, false, vector<bool>{}, vector<double>{}, vector<double>{}, vector<double>{}, vector<double>{},
                                           9, 60, false, vector<bool>{}, vector<double>{}, vector<double>{}, vector<double>{}, vector<double>{},
                                           0, vector<Set *>{}, false, false, false));

    character_list.push_back(new Character("冰_test", "", "冰", "单手剑", 10000, 300, 700, "暴击率", 0.192,
                                           9, vector<bool>{}, "冰", vector<double>{}, vector<double>{},
                                           "冰", vector<double>{}, vector<double>{}, "冰", vector<double>{}, vector<double>{},
                                           9, 3, false, vector<bool>{}, vector<double>{}, vector<double>{}, vector<double>{}, vector<double>{},
                                           9, 60, false, vector<bool>{}, vector<double>{}, vector<double>{}, vector<double>{}, vector<double>{},
                                           0, vector<Set *>{}, false, false, false));

    character_list.push_back(new Character("风_test", "", "风", "单手剑", 10000, 300, 700, "暴击率", 0.192,
                                           9, vector<bool>{}, "风", vector<double>{}, vector<double>{},
                                           "风", vector<double>{}, vector<double>{}, "风", vector<double>{}, vector<double>{},
                                           9, 3, false, vector<bool>{}, vector<double>{}, vector<double>{}, vector<double>{}, vector<double>{},
                                           9, 60, false, vector<bool>{}, vector<double>{}, vector<double>{}, vector<double>{}, vector<double>{},
                                           0, vector<Set *>{}, false, false, false));

    character_list.push_back(new Character("岩_test", "", "岩", "单手剑", 10000, 300, 700, "暴击率", 0.192,
                                           9, vector<bool>{}, "岩", vector<double>{}, vector<double>{},
                                           "岩", vector<double>{}, vector<double>{}, "岩", vector<double>{}, vector<double>{},
                                           9, 3, false, vector<bool>{}, vector<double>{}, vector<double>{}, vector<double>{}, vector<double>{},
                                           9, 60, false, vector<bool>{}, vector<double>{}, vector<double>{}, vector<double>{}, vector<double>{},
                                           0, vector<Set *>{}, false, false, false));

    character_list.push_back(new Character("草_test", "", "草", "单手剑", 10000, 300, 700, "暴击率", 0.192,
                                           9, vector<bool>{}, "草", vector<double>{}, vector<double>{},
                                           "草", vector<double>{}, vector<double>{}, "草", vector<double>{}, vector<double>{},
                                           9, 3, false, vector<bool>{}, vector<double>{}, vector<double>{}, vector<double>{}, vector<double>{},
                                           9, 60, false, vector<bool>{}, vector<double>{}, vector<double>{}, vector<double>{}, vector<double>{},
                                           0, vector<Set *>{}, false, false, false));
}

//build new character(needed)
bool Character::get_extra_special(Deployment *data) const
{
    if (data->c_point->name == "艾尔海森")
    {
        if (data->c_point->constellation >= 6)
        {
            data->add_percentage("暴击率", 0.1, (name + "_extra_special"));
            data->add_percentage("暴击伤害", 0.7, (name + "_extra_special"));
        }
    }
    else if (data->c_point->name == "雷电将军")
    {
        if (data->attack_config->condition->attack_way == "Q")
            data->add_percentage("伤害加成", 0.27, (name + "_extra_special"));
    }
    else if (data->c_point->name == "甘雨")
    {
        if (!(data->attack_config->background && !data->attack_config->lockface) && data->attack_config->condition->attack_way != "Q")
            data->add_percentage("伤害加成", 0.2, (name + "_extra_special"));
    }
    else if (data->c_point->name == "纳西妲")
    {
        //Q 1命
        int pyro_num = 0;
        if (data->team_config->teammate_1->ele_type == "火") pyro_num++;
        if (data->team_config->teammate_2->ele_type == "火") pyro_num++;
        if (data->team_config->teammate_3->ele_type == "火") pyro_num++;
        if (data->c_point->constellation >= 1) pyro_num++;
        if (data->attack_config->condition->attack_way == "E")
        {
            if (pyro_num == 1)
            {
                if (Q_level == 13) data->add_percentage("伤害加成", 0.316, (name + "_extra_special"));
                else if (Q_level == 12) data->add_percentage("伤害加成", 0.298, (name + "_extra_special"));
                else if (Q_level == 10) data->add_percentage("伤害加成", 0.268, (name + "_extra_special"));
                else if (Q_level == 9) data->add_percentage("伤害加成", 0.253, (name + "_extra_special"));
            }
            else if (pyro_num >= 2)
            {
                if (Q_level == 13) data->add_percentage("伤害加成", 0.474, (name + "_extra_special"));
                else if (Q_level == 12) data->add_percentage("伤害加成", 0.446, (name + "_extra_special"));
                else if (Q_level == 10) data->add_percentage("伤害加成", 0.402, (name + "_extra_special"));
                else if (Q_level == 9) data->add_percentage("伤害加成", 0.379, (name + "_extra_special"));
            }
        }
        //天赋1
        if (!(data->attack_config->background && !data->attack_config->lockface))
            data->add_converted_percentage("元素精通", 200, (name + "_extra_special"));
        //2命
        if (data->c_point->constellation >= 2 && data->attack_config->react_type.find("激化") != string::npos)
            data->add_percentage("防御削弱", 0.3, (name + "_extra_special"));
    }
    else if (data->c_point->name == "夜兰")
    {
        if (!data->attack_config->background)
            data->add_percentage("伤害加成", 0.25, (name + "_extra_special"));
    }
    else if (data->c_point->name == "九条裟罗")
    {
        if (data->c_point->E_level == 13) data->add_percentage("攻击力", 0.91, (name + "_extra_special"));
        else if (data->c_point->E_level == 12) data->add_percentage("攻击力", 0.86, (name + "_extra_special"));
        else if (data->c_point->E_level == 10) data->add_percentage("攻击力", 0.77, (name + "_extra_special"));
        else if (data->c_point->E_level == 9) data->add_percentage("攻击力", 0.73, (name + "_extra_special"));
    }
    return true;
}

class weapon_artifact_related_arguments
{
public:
    //weapon
    int sword_wuqie_level = -1;//雾切，普通攻击造成元素伤害时，持续5秒；施放元素爆发时，持续10秒；角色元素能量低于100%时
    int sword_shengxian_level = -1;//圣显之钥，E命中。20s，3层
    int catalyst_shenle_level = -1;//神乐铃，释放E，16s，3层
    bool catalyst_biluo_enable = false;//碧落之珑，创造护盾，3s
    int catalyst_sifeng_level = -1;
    int bow_feilei_level = -1;//飞雷，普通攻击造成伤害时，持续5秒；施放元素战技时，持续10秒；此外，角色元素能量低于100%时
    int bow_dongji_level = -1;//冬极，普通攻击、重击、元素战技或元素爆发命中敌人
    bool bow_pomo_fullenergy = false;//破魔
    bool polearm_humo_halflife = false;//护摩
    int polearm_chisha_level = -1;//赤砂之杖，E命中，10s，3层
    int polearm_shizuo_level = -1;//试作星镰，释放E，12s，2层
    //artifact
    int cangbai_level;//苍白，E命中，7s，2层
    bool qianyan_enable;//千岩，E命中，3s
    int monv_level;//魔女，释放E，10s，3层
    bool chensha_enable;//辰砂
    bool shenlin_enable;//草套，EQ命中，8s
    int shuixian_level;//水仙，A重A下落AEQ命中，8s

    weapon_artifact_related_arguments(int cangbai_level_,
                                      bool qianyan_enable_,
                                      int monv_level_,
                                      bool chensha_enable_,
                                      bool shenlin_enable_,
                                      int shuixian_level_,

                                      string weapon_type,
                                      int wuqie_shenle_feilei_humo,
                                      int shengxian_biluo_dongji_chisha,
                                      int none_sifeng_pomo_shizuo)
    {
        cangbai_level = cangbai_level_;
        qianyan_enable = qianyan_enable_;
        monv_level = monv_level_;
        chensha_enable = chensha_enable_;
        shenlin_enable = shenlin_enable_;
        shuixian_level = shuixian_level_;

        if (weapon_type == "单手剑")
        {
            sword_wuqie_level = wuqie_shenle_feilei_humo;
            sword_shengxian_level = shengxian_biluo_dongji_chisha;
        }
        else if (weapon_type == "法器")
        {
            catalyst_shenle_level = wuqie_shenle_feilei_humo;
            catalyst_biluo_enable = shengxian_biluo_dongji_chisha;
            catalyst_sifeng_level = none_sifeng_pomo_shizuo;
        }
        else if (weapon_type == "弓")
        {
            bow_feilei_level = wuqie_shenle_feilei_humo;
            bow_dongji_level = shengxian_biluo_dongji_chisha;
            bow_pomo_fullenergy = none_sifeng_pomo_shizuo;
        }
        else if (weapon_type == "双手剑")
        {

        }
        else if (weapon_type == "长柄武器")
        {
            polearm_humo_halflife = wuqie_shenle_feilei_humo;
            polearm_chisha_level = shengxian_biluo_dongji_chisha;
            polearm_shizuo_level = none_sifeng_pomo_shizuo;
        }
    }
};

//build new weapon(all)
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
    //(get_team)
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

    //(get_team)
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

//    //TODO:NEW
//    //(recharge)
//    //(convert)
//    weapon_list.push_back(new Weapon("碧落之珑", "jadefallssplendor", "法器", 608, "生命值", 0.496, 1, temp));
//    temp.clear();

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

    //(special)
    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "攻击力", 0.2));//E命中
    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "攻击力", 0.2));//受伤
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

    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "攻击力", 0.12));//E命中或元素反应
    temp.push_back(new Set(new Condition("ALL", "ALL", "ALL"), "元素精通", 48.0));//E命中或元素反应
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
//TODO:important 叠层类圣遗物触发后能快速自动叠层的总取最大层数，否则显式指定层数
bool Weapon::get_extra_special(Deployment *data) const
{
    if (name == "雾切之回光")
    {
        if (data->attack_config->condition->ele_type != "物理")
        {
            if (data->attack_config->args->sword_wuqie_level == 1) data->add_percentage("伤害加成", (0.08 * (0.75 + level * 0.25)), (name + "_extra_special"));
            else if (data->attack_config->args->sword_wuqie_level == 2) data->add_percentage("伤害加成", (0.16 * (0.75 + level * 0.25)), (name + "_extra_special"));
            else if (data->attack_config->args->sword_wuqie_level >= 3) data->add_percentage("伤害加成", (0.28 * (0.75 + level * 0.25)), (name + "_extra_special"));
        }
    }
    else if (name == "斫峰之刃")
    {
        if (!data->attack_config->background)
        {
            data->add_percentage("攻击力", (0.04 * 5 * (0.75 + level * 0.25)), (name + "_extra_special"));
            if (data->c_point->shield_sustain || data->team_config->teammate_1->shield_sustain || data->team_config->teammate_2->shield_sustain || data->team_config->teammate_3->shield_sustain)
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
        if (!data->attack_config->background)
        {
            data->add_percentage("攻击力", (0.04 * 4 * (0.75 + level * 0.25)), (name + "_extra_special"));
            data->add_percentage("防御力", (0.04 * 4 * (0.75 + level * 0.25)), (name + "_extra_special"));
        }
    }
        //默认给队友
//    else if (name == "原木刀")
//    {
//        if (data->attack_config->react_type.find("燃烧") != string::npos || data->attack_config->react_type.find("激化") != string::npos || data->attack_config->react_type.find("绽放") != string::npos)
//            data->add_percentage("元素精通", (60.0 * (0.75 + level * 0.25)), (name + "_extra_special"));
//    }
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
            {
                if (data->attack_config->args->catalyst_sifeng_level >= 4) data->add_percentage("伤害加成", (0.32 * (0.75 + level * 0.25)), (name + "_extra_special"));
                else if (data->attack_config->args->catalyst_sifeng_level == 3) data->add_percentage("伤害加成", (0.24 * (0.75 + level * 0.25)), (name + "_extra_special"));
                else if (data->attack_config->args->catalyst_sifeng_level == 2) data->add_percentage("伤害加成", (0.16 * (0.75 + level * 0.25)), (name + "_extra_special"));
                else if (data->attack_config->args->catalyst_sifeng_level == 1) data->add_percentage("伤害加成", (0.08 * (0.75 + level * 0.25)), (name + "_extra_special"));
            }
    }
    else if (name == "神乐之真意")
    {
        if (data->attack_config->args->catalyst_shenle_level >= 3)
        {
            if (data->attack_config->condition->attack_way == "E")
                data->add_percentage("伤害加成", (0.12 * 3 * (0.75 + level * 0.25)), (name + "_extra_special"));
            if (data->attack_config->condition->ele_type != "物理")
                data->add_percentage("伤害加成", (0.12 * (0.75 + level * 0.25)), (name + "_extra_special"));
        }
        else if (data->attack_config->args->catalyst_shenle_level == 2)
        {
            if (data->attack_config->condition->attack_way == "E")
                data->add_percentage("伤害加成", (0.12 * 2 * (0.75 + level * 0.25)), (name + "_extra_special"));
        }
        else if (data->attack_config->args->catalyst_shenle_level == 1)
        {
            if (data->attack_config->condition->attack_way == "E")
                data->add_percentage("伤害加成", (0.12 * (0.75 + level * 0.25)), (name + "_extra_special"));
        }
    }
    else if (name == "尘世之锁")
    {
        if (!data->attack_config->background)
        {
            data->add_percentage("攻击力", (0.04 * 5 * (0.75 + level * 0.25)), (name + "_extra_special"));
            if (data->c_point->shield_sustain || data->team_config->teammate_1->shield_sustain || data->team_config->teammate_2->shield_sustain || data->team_config->teammate_3->shield_sustain)
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
            (data->attack_config->react_type.find("超导") != string::npos && (data->attack_config->condition->ele_type == "雷" || data->attack_config->condition->ele_type == "冰")) ||
            (data->attack_config->react_type.find("扩散") != string::npos && data->team_config->ele_allow_spread.find("雷") != string::npos && (data->attack_config->condition->ele_type == "雷" || data->attack_config->condition->ele_type == "风")) ||
            (data->attack_config->react_type.find("结晶") != string::npos && data->team_config->ele_allow_spread.find("雷") != string::npos && (data->attack_config->condition->ele_type == "雷" || data->attack_config->condition->ele_type == "岩")))
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
            if (data->attack_config->condition->attack_way == "平A" || data->attack_config->condition->attack_way == "E" || data->attack_config->condition->attack_way == "Q")
                data->add_percentage("伤害加成", (0.2 * (0.75 + level * 0.25)), (name + "_extra_special"));
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
        if (data->attack_config->args->bow_dongji_level == 1) data->add_percentage("攻击力", (0.1 * (0.75 + level * 0.25)), (name + "_extra_special"));
        else if (data->attack_config->args->bow_dongji_level == 2) data->add_percentage("攻击力", (0.2 * (0.75 + level * 0.25)), (name + "_extra_special"));
        else if (data->attack_config->args->bow_dongji_level == 3) data->add_percentage("攻击力", (0.3 * (0.75 + level * 0.25)), (name + "_extra_special"));
        else if (data->attack_config->args->bow_dongji_level >= 4) data->add_percentage("攻击力", (0.48 * (0.75 + level * 0.25)), (name + "_extra_special"));
    }
    else if (name == "飞雷之弦振")
    {
        if (data->attack_config->condition->attack_way == "平A")
        {
            if (data->attack_config->args->bow_feilei_level == 1) data->add_percentage("伤害加成", (0.12 * (0.75 + level * 0.25)), (name + "_extra_special"));
            else if (data->attack_config->args->bow_feilei_level == 2) data->add_percentage("伤害加成", (0.24 * (0.75 + level * 0.25)), (name + "_extra_special"));
            else if (data->attack_config->args->bow_feilei_level >= 3) data->add_percentage("伤害加成", (0.40 * (0.75 + level * 0.25)), (name + "_extra_special"));
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
            data->add_percentage("伤害加成", min(334.0, data->c_point->Q_energy + data->team_config->teammate_1->Q_energy + data->team_config->teammate_2->Q_energy + data->team_config->teammate_3->Q_energy)
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
            if (data->attack_config->condition->attack_way == "平A" || data->attack_config->condition->attack_way == "E")
                data->add_percentage("伤害加成", (0.2 * (0.75 + level * 0.25)), (name + "_extra_special"));
    }
    else if (name == "破魔之弓")
    {
        if (data->attack_config->condition->attack_way == "平A")
            data->add_percentage("伤害加成", ((data->attack_config->args->bow_pomo_fullenergy ? 2 : 1) * 0.16 * (0.75 + level * 0.25)), (name + "_extra_special"));
        if (data->attack_config->condition->attack_way == "重A")
            data->add_percentage("伤害加成", ((data->attack_config->args->bow_pomo_fullenergy ? 2 : 1) * 0.12 * (0.75 + level * 0.25)), (name + "_extra_special"));
    }
    else if (name == "王下近侍")
    {
        if (!data->attack_config->background)
            data->add_percentage("元素精通", (40.0 + level * 20.0), (name + "_extra_special"));//EQ后
    }
    else if (name == "钢轮弓")
    {
        if (!data->attack_config->background)
            data->add_percentage("攻击力", (0.04 * 4 * (0.75 + level * 0.25)), (name + "_extra_special"));
    }

    else if (name == "苇海信标")
    {
        if (!data->c_point->shield_sustain && !data->team_config->teammate_1->shield_sustain && !data->team_config->teammate_2->shield_sustain && !data->team_config->teammate_3->shield_sustain)
            data->add_percentage("生命值", (0.32 * (0.75 + level * 0.25)), (name + "_extra_special"));//不处于护盾下，条件弱
    }
    else if (name == "无工之剑")
    {
        if (!data->attack_config->background)
        {
            data->add_percentage("攻击力", (0.04 * 5 * (0.75 + level * 0.25)), (name + "_extra_special"));
            if (data->c_point->shield_sustain || data->team_config->teammate_1->shield_sustain || data->team_config->teammate_2->shield_sustain || data->team_config->teammate_3->shield_sustain)
                data->add_percentage("攻击力", (0.04 * 5 * (0.75 + level * 0.25)), (name + "_extra_special"));
        }
    }
        //默认给队友
//    else if (name == "森林王器")
//    {
//        if (data->attack_config->react_type.find("燃烧") != string::npos || data->attack_config->react_type.find("激化") != string::npos || data->attack_config->react_type.find("绽放") != string::npos)
//            data->add_percentage("元素精通", (60.0 * (0.75 + level * 0.25)), (name + "_extra_special"));
//    }
    else if (name == "螭骨剑")
    {
        data->add_percentage("伤害加成", (0.25 + level * 0.05), (name + "_extra_special"));
    }
    else if (name == "白影剑")
    {
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
            data->add_percentage("伤害加成", min(334.0, data->c_point->Q_energy + data->team_config->teammate_1->Q_energy + data->team_config->teammate_2->Q_energy + data->team_config->teammate_3->Q_energy)
                                             * 0.0012 * (0.75 + level * 0.25), (name + "_extra_special"));
    }
    else if (name == "钟剑")
    {
        if (data->c_point->shield_sustain || data->team_config->teammate_1->shield_sustain || data->team_config->teammate_2->shield_sustain || data->team_config->teammate_3->shield_sustain)
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
        if (data->attack_config->background)
            data->add_percentage("攻击力", (0.192 * (0.75 + level * 0.25)), (name + "_extra_special"));
    }
    else if (name == "和璞鸢")
    {
        if (!data->attack_config->background)
        {
            data->add_percentage("攻击力", (0.025 + level * 0.007) * 7, (name + "_extra_special"));//默认满层
            data->add_percentage("伤害加成", (0.75 + level * 0.25) * 0.12, (name + "_extra_special"));//默认满层
        }
    }
    else if (name == "贯虹之槊")
    {
        if (!data->attack_config->background)
        {
            data->add_percentage("攻击力", (0.04 * 5 * (0.75 + level * 0.25)), (name + "_extra_special"));
            if (data->c_point->shield_sustain || data->team_config->teammate_1->shield_sustain || data->team_config->teammate_2->shield_sustain || data->team_config->teammate_3->shield_sustain)
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
            data->add_percentage("伤害加成", min(334.0, data->c_point->Q_energy + data->team_config->teammate_1->Q_energy + data->team_config->teammate_2->Q_energy + data->team_config->teammate_3->Q_energy)
                                             * 0.0012 * (0.75 + level * 0.25), (name + "_extra_special"));
    }
    else if (name == "千岩长枪")
    {
        data->add_percentage("攻击力", (0.24 + level * 0.04), (name + "_extra_special"));//默认3层
        data->add_percentage("暴击率", (0.08 + level * 0.04), (name + "_extra_special"));//默认3层
    }
        //默认给队友
//    else if (name == "贯月矢")
//    {
//        if (data->attack_config->react_type.find("燃烧") != string::npos || data->attack_config->react_type.find("激化") != string::npos || data->attack_config->react_type.find("绽放") != string::npos)
//            data->add_percentage("攻击力", (0.16 * (0.75 + level * 0.25)), (name + "_extra_special"));
//    }
    else if (name == "试作星镰")
    {
        if (data->attack_config->args->polearm_shizuo_level >= 2)
        {
            if (data->attack_config->condition->attack_way == "平A" || data->attack_config->condition->attack_way == "重A")
                data->add_percentage("伤害加成", (0.08 * 2 * (0.75 + level * 0.25)), (name + "_extra_special"));
        }
        else if (data->attack_config->args->polearm_shizuo_level == 1)
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
    if (data->w_point->name == "磐岩结绿" && data->data_list[str2index_full("攻击力")]->useful)
        data->data_list[str2index_full("生命值")]->useful = true;
    else if (data->w_point->name == "裁叶萃光" && (data->attack_config->condition->attack_way == "平A" || data->attack_config->condition->attack_way == "E"))
        data->data_list[str2index_full("元素精通")]->useful = true;
    else if (data->w_point->name == "圣显之钥" && data->attack_config->args->sword_shengxian_level > 0 && data->data_list[str2index_full("元素精通")]->useful)
        data->data_list[str2index_full("生命值")]->useful = true;
    else if (data->w_point->name == "辰砂之纺锤" && data->attack_config->condition->attack_way == "E")
        data->data_list[str2index_full("防御力")]->useful = true;
    else if (data->w_point->name == "西福斯的月光" && (data->data_list[str2index_full("元素充能效率")]->useful || data->need_to_satisfy_recharge))
        data->data_list[str2index_full("元素精通")]->useful = true;
    else if (data->w_point->name == "不灭月华" && data->attack_config->condition->attack_way == "平A")
        data->data_list[str2index_full("生命值")]->useful = true;
//        //TODO:NEW
//    else if (data->w_point->name == "碧落之珑" && data->attack_config->args->catalyst_biluo_enable)
//        data->data_list[str2index_full("生命值")]->useful = true;
    else if (data->w_point->name == "流浪的晚星" && data->data_list[str2index_full("攻击力")]->useful)
        data->data_list[str2index_full("元素精通")]->useful = true;
    else if (data->w_point->name == "猎人之径" && data->attack_config->condition->attack_way == "重A")
        data->data_list[str2index_full("元素精通")]->useful = true;
    else if (data->w_point->name == "赤角石溃杵" && (data->attack_config->condition->attack_way == "平A" || data->attack_config->condition->attack_way == "重A"))
        data->data_list[str2index_full("防御力")]->useful = true;
    else if (data->w_point->name == "玛海菈的水色" && data->data_list[str2index_full("攻击力")]->useful)
        data->data_list[str2index_full("元素精通")]->useful = true;
    else if (data->w_point->name == "护摩之杖" && data->data_list[str2index_full("攻击力")]->useful)
        data->data_list[str2index_full("生命值")]->useful = true;
    else if (data->w_point->name == "薙草之稻光" && data->data_list[str2index_full("攻击力")]->useful)
        data->data_list[str2index_full("元素充能效率")]->useful = true;
    else if (data->w_point->name == "赤沙之杖" && data->data_list[str2index_full("攻击力")]->useful)
        data->data_list[str2index_full("元素精通")]->useful = true;
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
//    //TODO:NEW
//    artifact_list.push_back(new Artifact("水仙之梦", "nymphsdream", new Set(new Condition("水", "ALL", "ALL"), "伤害加成", 0.15),
//                                         nullptr));//(special)
//    artifact_list.push_back(new Artifact("花海甘露之光", "dewflowersglow", new Set(new Condition("ALL", "ALL", "ALL"), "生命值", 0.2),
//                                         nullptr));//(special)
}

//build new artifact(all) 保证二件套效果和四件套效果分开
//TODO:important 叠层类圣遗物触发后能快速自动叠层的总取最大层数，否则显式指定层数
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
        if (data->c_point->shield_sustain || data->team_config->teammate_1->shield_sustain || data->team_config->teammate_2->shield_sustain || data->team_config->teammate_3->shield_sustain)
            if (data->attack_config->condition->attack_way == "平A" || data->attack_config->condition->attack_way == "重A")
                data->add_percentage("伤害加成", 0.4, (name + "_extra_special"));
    }
    else if (if_4_piece && name == "苍白之火")
    {
        if (data->attack_config->args->cangbai_level >= 2)
        {
            data->add_percentage("攻击力", 0.18, name + "_extra_special");
            if (data->attack_config->condition->ele_type == "物理") data->add_percentage("伤害加成", 0.25, (name + "_extra_special"));
        }
        else if (data->attack_config->args->cangbai_level == 1)
        {
            data->add_percentage("攻击力", 0.09, name + "_extra_special");
        }
    }
    else if (if_4_piece && name == "千岩牢固")
    {
        if (data->attack_config->args->qianyan_enable)
        {
            data->add_percentage("攻击力", 0.2, (name + "_extra_special"));
            data->add_percentage("护盾强效", 0.3, (name + "_extra_special"));
        }
    }
    else if (if_4_piece && name == "炽烈的炎之魔女")
    {
        if (data->attack_config->condition->ele_type == "火")
        {
            if (data->attack_config->args->monv_level >= 3) data->add_percentage("伤害加成", 0.225, (name + "_extra_special"));
            else if (data->attack_config->args->monv_level == 2) data->add_percentage("伤害加成", 0.15, (name + "_extra_special"));
            else if (data->attack_config->args->monv_level == 1) data->add_percentage("伤害加成", 0.075, (name + "_extra_special"));
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
        if (data->attack_config->args->chensha_enable)
            data->add_percentage("攻击力", 0.48, (name + "_extra_special"));
    }
    else if (if_4_piece && name == "深林的记忆")
    {
        if (data->attack_config->args->shenlin_enable)
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
//        //TODO:NEW
//    else if (if_4_piece && name == "水仙之梦")
//    {
//        if (data->attack_config->args->shuixian_level == 1)
//        {
//            data->add_percentage("攻击力", 0.07, (name + "_extra_special"));
//            if (data->attack_config->condition->ele_type == "水") data->add_percentage("伤害加成", 0.04, (name + "_extra_special"));
//        }
//        else if (data->attack_config->args->shuixian_level == 2)
//        {
//            data->add_percentage("攻击力", 0.16, (name + "_extra_special"));
//            if (data->attack_config->condition->ele_type == "水") data->add_percentage("伤害加成", 0.09, (name + "_extra_special"));
//        }
//        else if (data->attack_config->args->shuixian_level >= 3)
//        {
//            data->add_percentage("攻击力", 0.25, (name + "_extra_special"));
//            if (data->attack_config->condition->ele_type == "水") data->add_percentage("伤害加成", 0.15, (name + "_extra_special"));
//        }
//    }
//    else if (if_4_piece && name == "花海甘露之光")
//    {
//        if (data->attack_config->condition->attack_way == "E" || data->attack_config->condition->attack_way == "Q")
//        {
//            if (!data->attack_config->background && !data->c_point->shield_sustain && !data->team_config->teammate_1->shield_sustain && !data->team_config->teammate_2->shield_sustain && !data->team_config->teammate_3->shield_sustain)
//                data->add_percentage("伤害加成", 0.5, (name + "_extra_special"));
//            else
//                data->add_percentage("伤害加成", 0.1, (name + "_extra_special"));
//        }
//    }
    return true;
}

//build new artifact(all) 被转化的属性有效
//考虑Q充能
void Artifact::modify_useful_attribute(Deployment *data)
{
    if (data->suit1->name == "绝缘之旗印" && data->suit2->name == "绝缘之旗印" && data->attack_config->condition->attack_way == "Q")
        data->data_list[str2index_full("元素充能效率")]->useful = true;
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
            if (data->c_point->heal_sustain)
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
            if (data->attack_config->args->qianyan_enable)
                suit1_valid = suit2_valid = true;//原来肯定-现在肯定；原来否定-现在肯定；
        }
        else if (data->suit1->name == "深林的记忆")
        {
            if (data->attack_config->args->shenlin_enable)
                suit1_valid = suit2_valid = true;//原来肯定-现在肯定；原来否定-现在肯定；
        }

        //recharge
    }
    else
    {
        //2+2
        //攻击：角斗+（追忆） 物理：染血+（苍白） 治疗：少女+（海染） 精通：乐团+（饰金） 风伤：翠绿+（沙上） 水伤：沉沦+（水仙） 生命：千岩+（花海）
        //suit1
        if ((data->suit1->name == "追忆之注连" && data->suit2->name != "角斗士的终幕礼") || data->suit1->name == "辰砂往生录" || data->suit1->name == "来歆余响") suit1_valid = false;//原来肯定-现在否定；原来否定-现在否定
        if (data->suit1->name == "苍白之火" && data->suit2->name != "染血的骑士道") suit1_valid = false;//原来肯定-现在否定；原来否定-现在否定
        if (data->suit1->name == "海染砗磲" && data->suit2->name != "被怜爱的少女") suit1_valid = false;//原来肯定-现在否定；原来否定-现在否定
        if ((data->suit1->name == "饰金之梦" && data->suit2->name != "流浪大地的乐团") || data->suit1->name == "乐园遗落之花") suit1_valid = false;//原来肯定-现在否定；原来否定-现在否定
        if (data->suit1->name == "沙上楼阁史话" && data->suit2->name != "翠绿之影") suit1_valid = false;//原来肯定-现在否定；原来否定-现在否定
//        //TODO:NEW
//        if (data->suit1->name == "水仙之梦" && data->suit2->name != "沉沦之心") suit1_valid = false;//原来肯定-现在否定；原来否定-现在否定
//        if (data->suit1->name == "花海甘露之光" && data->suit2->name != "千岩牢固") suit1_valid = false;//原来肯定-现在否定；原来否定-现在否定
        //suit2
        if ((data->suit2->name == "追忆之注连" && data->suit1->name != "角斗士的终幕礼") || data->suit2->name == "辰砂往生录" || data->suit2->name == "来歆余响") suit2_valid = false;//原来肯定-现在否定；原来否定-现在否定
        if (data->suit2->name == "苍白之火" && data->suit1->name != "染血的骑士道") suit2_valid = false;//原来肯定-现在否定；原来否定-现在否定
        if (data->suit2->name == "海染砗磲" && data->suit1->name != "被怜爱的少女") suit2_valid = false;//原来肯定-现在否定；原来否定-现在否定
        if ((data->suit2->name == "饰金之梦" && data->suit1->name != "流浪大地的乐团") || data->suit2->name == "乐园遗落之花") suit2_valid = false;//原来肯定-现在否定；原来否定-现在否定
        if (data->suit2->name == "沙上楼阁史话" && data->suit1->name != "翠绿之影") suit2_valid = false;//原来肯定-现在否定；原来否定-现在否定
//        //TODO:NEW
//        if (data->suit2->name == "水仙之梦" && data->suit1->name != "沉沦之心") suit2_valid = false;//原来肯定-现在否定；原来否定-现在否定
//        if (data->suit2->name == "花海甘露之光" && data->suit1->name != "千岩牢固") suit2_valid = false;//原来肯定-现在否定；原来否定-现在否定
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
        if ((data_list[str2index_full("生命值")]->value_per_entry * 0.012 * (0.75 + w_point->level * 0.25) * base_life / base_atk) < data_list[str2index_full("攻击力")]->value_per_entry)
            data_list[str2index_full("生命值")]->useful = attack_config->useful_attributes[str2index_full("生命值")];
    }
    else if (w_point->name == "裁叶萃光")
    {
        if (base_skillrate != 0)
            if ((data_list[str2index_full("元素精通")]->value_per_entry * 1.2 * (0.75 + w_point->level * 0.25) / (base_skillrate * base_atk)) < data_list[str2index_full("攻击力")]->value_per_entry)
                data_list[str2index_full("元素精通")]->useful = attack_config->useful_attributes[str2index_full("元素精通")];
    }
    else if (w_point->name == "圣显之钥")
    {
        if (attack_config->args->sword_shengxian_level >= 3)
        {
            if ((data_list[str2index_full("生命值")]->value_per_entry * 0.0056 * (0.75 + w_point->level * 0.25) * base_life) < data_list[str2index_full("元素精通")]->value_per_entry)
                data_list[str2index_full("生命值")]->useful = attack_config->useful_attributes[str2index_full("生命值")];
        }
        else if (attack_config->args->sword_shengxian_level == 2)
        {
            if ((data_list[str2index_full("生命值")]->value_per_entry * 0.0024 * (0.75 + w_point->level * 0.25) * base_life) < data_list[str2index_full("元素精通")]->value_per_entry)
                data_list[str2index_full("生命值")]->useful = attack_config->useful_attributes[str2index_full("生命值")];
        }
        else if (attack_config->args->sword_shengxian_level == 1)
        {
            if ((data_list[str2index_full("生命值")]->value_per_entry * 0.0012 * (0.75 + w_point->level * 0.25) * base_life) < data_list[str2index_full("元素精通")]->value_per_entry)
                data_list[str2index_full("生命值")]->useful = attack_config->useful_attributes[str2index_full("生命值")];
        }
    }
    else if (w_point->name == "辰砂之纺锤")
    {
        if (base_skillrate != 0)
            if ((data_list[str2index_full("防御力")]->value_per_entry * 0.4 * (0.75 + w_point->level * 0.25) * base_def / (base_skillrate * base_atk)) < data_list[str2index_full("攻击力")]->value_per_entry)
                data_list[str2index_full("防御力")]->useful = attack_config->useful_attributes[str2index_full("防御力")];
    }
    else if (w_point->name == "西福斯的月光")
    {
        if ((data_list[str2index_full("元素精通")]->value_per_entry * 0.00036 * (0.75 + w_point->level * 0.25)) < data_list[str2index_full("元素充能效率")]->value_per_entry)
            data_list[str2index_full("元素精通")]->useful = attack_config->useful_attributes[str2index_full("元素精通")];
    }
    else if (w_point->name == "不灭月华")
    {
        if (base_skillrate != 0)
            if ((data_list[str2index_full("生命值")]->value_per_entry * (0.005 + w_point->level * 0.005) * base_life / (base_skillrate * base_atk)) < data_list[str2index_full("攻击力")]->value_per_entry)
                data_list[str2index_full("生命值")]->useful = attack_config->useful_attributes[str2index_full("生命值")];
    }
    else if (w_point->name == "流浪的晚星")
    {
        if ((data_list[str2index_full("元素精通")]->value_per_entry * 0.24 * (0.75 + w_point->level * 0.25) / base_atk) < data_list[str2index_full("攻击力")]->value_per_entry)
            data_list[str2index_full("元素精通")]->useful = attack_config->useful_attributes[str2index_full("元素精通")];
    }
    else if (w_point->name == "猎人之径")
    {
        if (base_skillrate != 0)
            if ((data_list[str2index_full("元素精通")]->value_per_entry * 1.6 * (0.75 + w_point->level * 0.25) / (base_skillrate * base_atk)) < data_list[str2index_full("攻击力")]->value_per_entry)
                data_list[str2index_full("元素精通")]->useful = attack_config->useful_attributes[str2index_full("元素精通")];
    }
    else if (w_point->name == "赤角石溃杵")
    {
        if (base_skillrate != 0)
            if ((data_list[str2index_full("防御力")]->value_per_entry * 0.4 * (0.75 + w_point->level * 0.25) * base_def / (base_skillrate * base_atk)) < data_list[str2index_full("攻击力")]->value_per_entry)
                data_list[str2index_full("防御力")]->useful = attack_config->useful_attributes[str2index_full("防御力")];
    }
    else if (w_point->name == "玛海菈的水色")
    {
        if ((data_list[str2index_full("元素精通")]->value_per_entry * 0.24 * (0.75 + w_point->level * 0.25) / base_atk) < data_list[str2index_full("攻击力")]->value_per_entry)
            data_list[str2index_full("元素精通")]->useful = attack_config->useful_attributes[str2index_full("元素精通")];
    }
    else if (w_point->name == "护摩之杖")
    {
        if (attack_config->args->polearm_humo_halflife)
        {
            if ((data_list[str2index_full("生命值")]->value_per_entry * (0.008 * (0.75 + w_point->level * 0.25) + (0.008 + w_point->level * 0.002)) * base_life / base_atk) < data_list[str2index_full("攻击力")]->value_per_entry)
                data_list[str2index_full("生命值")]->useful = attack_config->useful_attributes[str2index_full("生命值")];
        }
        else
        {
            if ((data_list[str2index_full("生命值")]->value_per_entry * 0.008 * (0.75 + w_point->level * 0.25) * base_life / base_atk) < data_list[str2index_full("攻击力")]->value_per_entry)
                data_list[str2index_full("生命值")]->useful = attack_config->useful_attributes[str2index_full("生命值")];
        }
    }
    else if (w_point->name == "薙草之稻光")
    {
        if ((data_list[str2index_full("元素充能效率")]->value_per_entry * 0.28 * (0.75 + w_point->level * 0.25)) < data_list[str2index_full("攻击力")]->value_per_entry)
            data_list[str2index_full("元素充能效率")]->useful = attack_config->useful_attributes[str2index_full("元素充能效率")];
    }
    else if (w_point->name == "赤沙之杖")
    {
        if (attack_config->args->polearm_chisha_level >= 3)
        {
            if ((data_list[str2index_full("元素精通")]->value_per_entry * 1.36 * (0.75 + w_point->level * 0.25) / base_atk) < data_list[str2index_full("攻击力")]->value_per_entry)
                data_list[str2index_full("元素精通")]->useful = attack_config->useful_attributes[str2index_full("元素精通")];
        }
        else if (attack_config->args->polearm_chisha_level == 2)
        {
            if ((data_list[str2index_full("元素精通")]->value_per_entry * 1.08 * (0.75 + w_point->level * 0.25) / base_atk) < data_list[str2index_full("攻击力")]->value_per_entry)
                data_list[str2index_full("元素精通")]->useful = attack_config->useful_attributes[str2index_full("元素精通")];
        }
        else if (attack_config->args->polearm_chisha_level == 1)
        {
            if ((data_list[str2index_full("元素精通")]->value_per_entry * 0.8 * (0.75 + w_point->level * 0.25) / base_atk) < data_list[str2index_full("攻击力")]->value_per_entry)
                data_list[str2index_full("元素精通")]->useful = attack_config->useful_attributes[str2index_full("元素精通")];
        }
        else
        {
            if ((data_list[str2index_full("元素精通")]->value_per_entry * 0.52 * (0.75 + w_point->level * 0.25) / base_atk) < data_list[str2index_full("攻击力")]->value_per_entry)
                data_list[str2index_full("元素精通")]->useful = attack_config->useful_attributes[str2index_full("元素精通")];
        }
    }

    //artifact
    //绝缘肯定有效（增伤效益认为无穷）
    if (suit1->name == "绝缘之旗印" && suit2->name == "绝缘之旗印" && attack_config->condition->attack_way == "Q")
        data_list[str2index_full("元素充能效率")]->useful = true;

    //character
    if (c_point->name == "胡桃") data_list[str2index_full("攻击力")]->useful = false;//生命>攻击，除非有攻击转什么
    if (c_point->name == "艾尔海森") data_list[str2index_full("攻击力")]->useful = false;//TODO:加速计算
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
            (attack_config->react_type.find("超导") != string::npos && (attack_config->condition->ele_type == "雷" || attack_config->condition->ele_type == "冰")) ||
            (attack_config->react_type.find("扩散") != string::npos && team_config->ele_allow_spread.find("雷") != string::npos && (attack_config->condition->ele_type == "雷" || attack_config->condition->ele_type == "风")) ||
            (attack_config->react_type.find("结晶") != string::npos && team_config->ele_allow_spread.find("雷") != string::npos && (attack_config->condition->ele_type == "雷" || attack_config->condition->ele_type == "岩")))
            add_percentage("伤害加成", 0.2, "team_白辰之环");
    }
    if (team_config->team_weapon_artifact.find("讨龙英杰谭") != string::npos)
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
        add_converted_percentage("元素精通", 100.0, "team_圣显之钥");//50000HP
    }
    if (team_config->team_weapon_artifact.find("西福斯的月光") != string::npos)
    {
        add_converted_percentage("元素充能效率", 0.054, "team_西福斯的月光");//500EM
    }
    if (team_config->team_weapon_artifact.find("流浪的晚星") != string::npos)
    {
        add_converted_percentage("攻击力", 36.0 / base_atk, "team_流浪的晚星");//500EM
    }
    if (team_config->team_weapon_artifact.find("玛海菈的水色") != string::npos)
    {
        add_converted_percentage("攻击力", 36.0 / base_atk, "team_玛海菈的水色");//500EM
    }

    //artifact
    if (team_config->team_weapon_artifact.find("悠古的磐岩") != string::npos && (suit1->name != "悠古的磐岩" || suit2->name != "悠古的磐岩"))
    {
        if (attack_config->condition->ele_type == "水" || attack_config->condition->ele_type == "火" || attack_config->condition->ele_type == "雷" || attack_config->condition->ele_type == "冰")
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
    if (team_config->teammate_all.find("艾尔海森") != string::npos)
    {
        //constellation>=4
//        add_percentage("元素精通", 90, "team_艾尔海森");
        Dendro_num++;
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
    if (team_config->teammate_all.find("神里绫华") != string::npos)
    {
        //constellation>=4 Q
//        add_percentage("防御削弱", 0.3, "team_神里绫华");
        Cryo_num++;
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
    if (team_config->teammate_all.find("纳西妲") != string::npos)
    {
        //talent Q后
        //转化类
        if (!(attack_config->background && !attack_config->lockface))
            add_converted_percentage("元素精通", 200, "team_纳西妲");
        //constellation>=2 激化后
        if (attack_config->react_type.find("激化") != string::npos)
            add_percentage("防御削弱", 0.3, "team_纳西妲");
        Dendro_num++;
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
    if (team_config->teammate_all.find("八重神子") != string::npos)
    {
        //constellation>=4 E命中
//        if (config->condition->ele_type == "雷")
//            add_percentage("伤害加成", 0.2, "team_八重神子");
        Electro_num++;
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
    if (team_config->teammate_all.find("行秋") != string::npos)
    {
        //constellation>=2
        if (attack_config->condition->ele_type == "水")
            add_percentage("抗性削弱", 0.15, "team_行秋");
        Hydro_num++;
    }
    if (team_config->teammate_all.find("温迪") != string::npos)
    {
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
    if (team_config->teammate_all.find("枫原万叶") != string::npos)
    {
        //talent 扩散
        //转化类
        if (attack_config->condition->ele_type == "水" || attack_config->condition->ele_type == "火" || attack_config->condition->ele_type == "雷" || attack_config->condition->ele_type == "冰")
            if (team_config->ele_allow_spread.find(attack_config->condition->ele_type) != string::npos)//扩散与元素附着
                add_converted_percentage("伤害加成", 0.36, "team_枫原万叶");
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
    if (team_config->teammate_all.find("九条裟罗") != string::npos)
    {
        //E
        if (!(attack_config->background && !attack_config->lockface))
            add_percentage("攻击力", (195 + 674) * 0.86 / base_atk, "team_九条裟罗");
        //constellation>=6 E下
        if (attack_config->condition->ele_type == "雷" && (!(attack_config->background && !attack_config->lockface)))
            add_percentage("暴击伤害", 0.6, "team_九条裟罗");
        Pyro_num++;
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
        if (c_point->shield_sustain || team_config->teammate_1->shield_sustain || team_config->teammate_2->shield_sustain || team_config->teammate_3->shield_sustain)
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
    if (team_config->teammate_all.find("九条裟罗") != string::npos) Q_energy_modify -= 2.4;

    double energy = monster_drop;
    energy += team_config->teammate_1->E_energy * back * ((team_config->teammate_1->ele_type == c_point->ele_type) ? same : diff) * ((double_E_per_round.find(team_config->teammate_1->name) != string::npos) ? 2 : 1);
    energy += team_config->teammate_2->E_energy * back * ((team_config->teammate_2->ele_type == c_point->ele_type) ? same : diff) * ((double_E_per_round.find(team_config->teammate_2->name) != string::npos) ? 2 : 1);
    energy += team_config->teammate_3->E_energy * back * ((team_config->teammate_3->ele_type == c_point->ele_type) ? same : diff) * ((double_E_per_round.find(team_config->teammate_3->name) != string::npos) ? 2 : 1);
    int electro_num = 0;
    if (c_point->ele_type == "雷") electro_num++;
    if (team_config->teammate_1->ele_type == "雷") electro_num++;
    if (team_config->teammate_2->ele_type == "雷") electro_num++;
    if (team_config->teammate_3->ele_type == "雷") electro_num++;
    if (electro_num >= 2) energy += (3 * back + 1 * front) * ((c_point->ele_type == "雷") ? same : diff);

    double converted_recharge = 0;

    //"天目影打刀" 12/E 不吃充能
    //"西风剑" 3*2/6s
    //"祭礼剑" 1E/16s
    //"西福斯的月光" 充能效率 mastery * 0.00036 * (0.75 + w_point->level * 0.25)

    //"不灭月华" 0.6/A Q后12s内 不吃充能
    //"碧落之珑" 4/Q 4/2.5s
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
    else if (c_point->name == "艾尔海森")
    {
        //Q 70 E 5f 1E/Q
        energy += ((double_E_per_round.find(c_point->name) != string::npos) ? 2 : 1) * c_point->E_energy * front * same;

        if (w_point->name == "西风剑") energy += 3 * front * white;
        else if (w_point->name == "祭礼剑") energy += 0;
        else if (w_point->name == "天目影打刀") Q_energy_modify -= 12;
        else if (w_point->name == "西福斯的月光") converted_recharge += data_list[str2index_full("元素精通")]->percentage * 0.00036 * (0.75 + w_point->level * 0.25);
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
    else if (c_point->name == "神里绫华")
    {
        //Q 80 E 4.5f 2E/Q
        energy += ((double_E_per_round.find(c_point->name) != string::npos) ? 2 : 1) * c_point->E_energy * front * same;

        if (w_point->name == "西风剑") energy += 3 * front * white;
        else if (w_point->name == "祭礼剑") energy += c_point->E_energy * front * same;
        else if (w_point->name == "天目影打刀") Q_energy_modify -= 12;
        else if (w_point->name == "西福斯的月光") converted_recharge += data_list[str2index_full("元素精通")]->percentage * 0.00036 * (0.75 + w_point->level * 0.25);
    }
    else if (c_point->name == "甘雨")
    {
        //Q 60 E 2f+2b 2E/Q
        energy += ((double_E_per_round.find(c_point->name) != string::npos) ? 2 : 1) * c_point->E_energy * (front / 2 + back / 2) * same;

        if (w_point->name == "西风猎弓") energy += 3 * front * white;
        else if (w_point->name == "祭礼弓") energy += c_point->E_energy * (front / 2 + back / 2) * same;
    }
    else if (c_point->name == "纳西妲")
    {
        //Q 50 E 6b 2E/Q
        energy += ((double_E_per_round.find(c_point->name) != string::npos) ? 2 : 1) * c_point->E_energy * back * same;

        if (w_point->name == "西风秘典") energy += 3 * front * white;
        else if (w_point->name == "祭礼残章") energy += 0;
        else if (w_point->name == "试作金珀") Q_energy_modify -= 18;
        else if (w_point->name == "不灭月华") Q_energy_modify -= 0;
//            //TODO:NEW
//        else if (w_point->name == "碧落之珑") Q_energy_modify -= 4;
    }
    else if (c_point->name == "夜兰")
    {
        //Q 70 E 4f 1E/Q
        energy += ((double_E_per_round.find(c_point->name) != string::npos) ? 2 : 1) * c_point->E_energy * front * same;

        if (w_point->name == "西风猎弓") energy += 3 * front * white;
        else if (w_point->name == "祭礼弓") energy += c_point->E_energy * front * same;
    }
    else if (c_point->name == "八重神子")
    {
        //Q 90-24=66 E 5b 1E/Q
        Q_energy_modify -= 24;
        energy += ((double_E_per_round.find(c_point->name) != string::npos) ? 2 : 1) * c_point->E_energy * back * same;

        if (w_point->name == "西风秘典") energy += 3 * front * white;
        else if (w_point->name == "祭礼残章") energy += 0;
        else if (w_point->name == "试作金珀") Q_energy_modify -= 18;
        else if (w_point->name == "不灭月华") Q_energy_modify -= 0;
//            //TODO:NEW
//        else if (w_point->name == "碧落之珑") Q_energy_modify -= 4;
    }
    else if (c_point->name == "香菱")
    {
        //Q 80 E 4b 1E/Q
        energy += ((double_E_per_round.find(c_point->name) != string::npos) ? 2 : 1) * c_point->E_energy * back * same;

        if (w_point->name == "西风长枪") energy += 3 * front * white;
        else if (w_point->name == "喜多院十文字") Q_energy_modify -= 12;
    }
    else if (c_point->name == "行秋")
    {
        //Q 80-3*5=65 E 5f 1E/Q
        Q_energy_modify -= 12;
        energy += ((double_E_per_round.find(c_point->name) != string::npos) ? 2 : 1) * c_point->E_energy * front * same;

        if (w_point->name == "西风剑") energy += 3 * front * white;
        else if (w_point->name == "祭礼剑") energy += c_point->E_energy * front * same;
        else if (w_point->name == "天目影打刀") Q_energy_modify -= 12;
        else if (w_point->name == "西福斯的月光") converted_recharge += data_list[str2index_full("元素精通")]->percentage * 0.00036 * (0.75 + w_point->level * 0.25);
    }
    else energy = Q_energy_modify;

    min_recharge_num = max(0, (int) ((Q_energy_modify / energy - data_list[str2index_full("元素充能效率")]->percentage - data_list[str2index_full("元素充能效率")]->converted_percentage - converted_recharge) / data_list[str2index_full("元素充能效率")]->value_per_entry));
}

//build new character(needed)||build new weapon(all)||build new artifact(all)
void Deployment::get_convert_value(double &life, double &atk, double &def, double &mastery, double &recharge, double &critrate, double &critdam, double &damplus)
{
    //先计算转化，最后再执行加成，不加到面板上的转化可以吃converted_percentage
    double life_add = 0, atk_add = 0, def_add = 0, mastery_add = 0, recharge_add = 0, critrate_add = 0, critdam_add = 0, damplus_add = 0;
    //character
    if (c_point->name == "胡桃")//生命->攻击
        atk_add += min(life * 0.0626 * base_life / base_atk, 4.0);
    else if (c_point->name == "艾尔海森" && (attack_config->condition->attack_way == "E" || attack_config->condition->attack_way == "Q"))//精通->EQ增伤
        damplus_add += min((mastery + data_list[str2index_full("元素精通")]->converted_percentage) * 0.001, 1.0);
    else if (c_point->name == "雷电将军" && attack_config->condition->ele_type == "雷")//充能->增伤
        damplus_add += (recharge - 1) * 0.4;
    else if (c_point->name == "纳西妲" && attack_config->condition->attack_way == "E")//精通->E暴击增伤
    {
        damplus_add += min((mastery + data_list[str2index_full("元素精通")]->converted_percentage - 200.0), 800.0) * 0.001;
        critrate_add += min((mastery + data_list[str2index_full("元素精通")]->converted_percentage - 200.0), 800.0) * 0.0003;
    }
    else if (c_point->name == "八重神子" && attack_config->condition->attack_way == "E")//精通->E增伤
        damplus_add += (mastery + data_list[str2index_full("元素精通")]->converted_percentage) * 0.0015;

    //weapon
    if (w_point->name == "磐岩结绿")//生命->攻击
        atk_add += life * 0.012 * (0.75 + w_point->level * 0.25) * base_life / base_atk;//生命->攻击
    else if (w_point->name == "圣显之钥")//生命->精通
    {
        if (attack_config->args->sword_shengxian_level >= 3)
        {
            mastery_add += life * 0.0012 * 3 * (0.75 + w_point->level * 0.25) * base_life;//生命->精通
            mastery_add += life * 0.002 * (0.75 + w_point->level * 0.25) * base_life;//生命->精通
        }
        else if (attack_config->args->sword_shengxian_level == 2)
        {
            mastery_add += life * 0.0012 * 2 * (0.75 + w_point->level * 0.25) * base_life;//生命->精通
        }
        else if (attack_config->args->sword_shengxian_level == 1)
        {
            mastery_add += life * 0.0012 * (0.75 + w_point->level * 0.25) * base_life;//生命->精通
        }
    }
    else if (w_point->name == "西福斯的月光")//精通->充能
        recharge_add += mastery * 0.00036 * (0.75 + w_point->level * 0.25);//精通->充能
//        //TODO:NEW
//    else if (w_point->name == "碧落之珑" && attack_config->args->catalyst_biluo_enable)//生命->增伤
//        damplus_add += min(life * (0.0015 + w_point->level * 0.0015) * base_life / 1000, 0.06 + w_point->level * 0.06);//生命->增伤
    else if (w_point->name == "流浪的晚星")//精通->攻击
        atk_add += mastery * 0.24 * (0.75 + w_point->level * 0.25) / base_atk;//精通->攻击
    else if (w_point->name == "玛海菈的水色")//精通->攻击
        atk_add += mastery * 0.24 * (0.75 + w_point->level * 0.25) / base_atk;//精通->攻击
    else if (w_point->name == "护摩之杖")//生命->攻击
    {
        atk_add += life * 0.008 * (0.75 + w_point->level * 0.25) * base_life / base_atk;//生命->攻击
        if (attack_config->args->polearm_humo_halflife)
            atk_add += life * (0.008 + w_point->level * 0.002) * base_life / base_atk;//生命->攻击
    }
    else if (w_point->name == "薙草之稻光")//充能->攻击
        atk_add += min((recharge - 1) * (0.75 + w_point->level * 0.25) * 0.28, (0.7 + w_point->level * 0.1));
    else if (w_point->name == "赤沙之杖")//精通->攻击
    {
        atk_add += mastery * 0.52 * (0.75 + w_point->level * 0.25) / base_atk;//精通->攻击
        if (attack_config->args->polearm_chisha_level >= 3)
            atk_add += mastery * 0.28 * 3 * (0.75 + w_point->level * 0.25) / base_atk;//精通->攻击
        else if (attack_config->args->polearm_chisha_level == 2)
            atk_add += mastery * 0.28 * 2 * (0.75 + w_point->level * 0.25) / base_atk;//精通->攻击
        else if (attack_config->args->polearm_chisha_level == 1)
            atk_add += mastery * 0.28 * (0.75 + w_point->level * 0.25) / base_atk;//精通->攻击
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
    else if (c_point->name == "艾尔海森")
    {
        if (attack_config->condition->attack_way == "E")
        {
            if (c_point->E_level == 13)
            {
                if (attack_config->rate_pos == 0) extrarate += 3.291 * mastery;
                else if (attack_config->rate_pos == 1) extrarate += 2.856 * mastery;
            }
            else if (c_point->E_level == 12)
            {
                if (attack_config->rate_pos == 0) extrarate += 3.098 * mastery;
                else if (attack_config->rate_pos == 1) extrarate += 2.688 * mastery;
            }
            else if (c_point->E_level == 10)
            {
                if (attack_config->rate_pos == 0) extrarate += 2.788 * mastery;
                else if (attack_config->rate_pos == 1) extrarate += 2.419 * mastery;
            }
            else if (c_point->E_level == 9)
            {
                if (attack_config->rate_pos == 0) extrarate += 2.633 * mastery;
                else if (attack_config->rate_pos == 1) extrarate += 2.285 * mastery;
            }
        }
        else if (attack_config->condition->attack_way == "Q")
        {
            if (c_point->Q_level == 13) extrarate += 2.067 * mastery;
            else if (c_point->Q_level == 12) extrarate += 1.946 * mastery;
            else if (c_point->Q_level == 10) extrarate += 1.751 * mastery;
            else if (c_point->Q_level == 9) extrarate += 1.654 * mastery;
        }
    }
    else if (c_point->name == "纳西妲")
    {
        if (attack_config->condition->attack_way == "E" && attack_config->rate_pos == 1)
        {
            if (c_point->E_level == 13) extrarate += 4.386 * mastery;
            else if (c_point->E_level == 12) extrarate += 4.128 * mastery;
            else if (c_point->E_level == 10) extrarate += 3.715 * mastery;
            else if (c_point->E_level == 9) extrarate += 3.509 * mastery;
        }
    }
    else if (c_point->name == "夜兰")
    {
        if (attack_config->condition->attack_way == "E")
        {
            if (c_point->E_level == 13) extrarate += 0.481 * life * base_life;
            else if (c_point->E_level == 12) extrarate += 0.452 * life * base_life;
            else if (c_point->E_level == 10) extrarate += 0.407 * life * base_life;
            else if (c_point->E_level == 9) extrarate += 0.384 * life * base_life;
        }
        else if (attack_config->condition->attack_way == "Q")
        {
            if (c_point->Q_level == 13)
            {
                if (attack_config->rate_pos == 0) extrarate += 0.1553 * life * base_life;
                else if (attack_config->rate_pos == 1) extrarate += 0.1035 * life * base_life;
                else if (c_point->constellation >= 2 && attack_config->rate_pos == 2) extrarate += 0.14 * life * base_life;
            }
            else if (c_point->Q_level == 12)
            {
                if (attack_config->rate_pos == 0) extrarate += 0.1462 * life * base_life;
                else if (attack_config->rate_pos == 1) extrarate += 0.0974 * life * base_life;
                else if (c_point->constellation >= 2 && attack_config->rate_pos == 2) extrarate += 0.14 * life * base_life;
            }
            else if (c_point->Q_level == 10)
            {
                if (attack_config->rate_pos == 0) extrarate += 0.1315 * life * base_life;
                else if (attack_config->rate_pos == 1) extrarate += 0.0877 * life * base_life;
                else if (c_point->constellation >= 2 && attack_config->rate_pos == 2) extrarate += 0.14 * life * base_life;
            }
            else if (c_point->Q_level == 9)
            {
                if (attack_config->rate_pos == 0) extrarate += 0.1242 * life * base_life;
                else if (attack_config->rate_pos == 1) extrarate += 0.0828 * life * base_life;
                else if (c_point->constellation >= 2 && attack_config->rate_pos == 2) extrarate += 0.14 * life * base_life;
            }
        }
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
        double extra_damplus = 0;
        if (this->attack_config->condition->ele_type == "草") extrarate += 1447.0 * 1.25 * (1.0 + (5.0 * mastery) / (mastery + 1200.0) + extra_damplus);
        else if (this->attack_config->condition->ele_type == "雷")
        {
            if ((suit1->name == suit2->name) && (suit1->name == "如雷的盛怒")) extra_damplus += 0.2;
            extrarate += 1447.0 * 1.15 * (1.0 + (5.0 * mastery) / (mastery + 1200.0) + extra_damplus);
        }
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
    //"神里绫华" "甘雨" "莫娜" "温迪"/"枫原万叶" "昔日宗室之仪_翠绿之影" "冰水" "冰水"
    //"雷电将军" "行秋" "香菱" "班尼特" "昔日宗室之仪" "水雷" "水雷火"
    //"雷电将军" "九条裟罗" "枫原万叶" "班尼特" "昔日宗室之仪_翠绿之影" "雷火" "雷火"
    //"艾尔海森" "八重神子" "纳西妲" "钟离" "千夜浮梦_深林的记忆_昔日宗室之仪" "草" ""

    if (config_cal_enable.find(c_name) == string::npos && config_cal_enable != "ALL") return;

    if (c_name == "胡桃")
    {
        auto *tc1 = new Team_config(find_character_by_name("钟离"), find_character_by_name("行秋"), find_character_by_name("夜兰"),
                                    "终末嗟叹之诗_昔日宗室之仪", "水", "水");

        //9AZ 4E
        vector<Attack_config *> ac1{
                new Attack_config(find_character_by_name(c_name), "平A", 0, false, "蒸发_no_add_damage", 6,
                                  new weapon_artifact_related_arguments(1, true, 1, false, true, 3,
                                                                        "长柄武器", 1, 1, 1)),
                new Attack_config(find_character_by_name(c_name), "平A", 0, false, "蒸发", 3,
                                  new weapon_artifact_related_arguments(1, true, 1, false, true, 3,
                                                                        "长柄武器", 1, 1, 1)),
                new Attack_config(find_character_by_name(c_name), "重A", 0, false, "蒸发", 9,
                                  new weapon_artifact_related_arguments(1, true, 1, false, true, 3,
                                                                        "长柄武器", 1, 1, 1)),
                new Attack_config(find_character_by_name(c_name), "E", 0, false, "蒸发", 4,
                                  new weapon_artifact_related_arguments(1, true, 1, false, true, 3,
                                                                        "长柄武器", 1, 1, 1))
        };

        if (mode == "cal_deployment")
        {
            combination_list.push_back(new Combination(find_weapon_by_name(""), find_artifact_by_name(""), find_artifact_by_name(""),
                                                       "", "", "", tc1, ac1, false));
        }
        else if (mode == "cal_optimal_artifact")
        {
            combination_list.push_back(new Combination(find_weapon_by_name("护摩之杖"), find_artifact_by_name(""), find_artifact_by_name(""),
                                                       "", "", "", tc1, ac1, false));
        }
        else if (mode == "generate_gcsim_script")
        {
            combination_list.push_back(new Combination(find_weapon_by_name("护摩之杖"), find_artifact_by_name("炽烈的炎之魔女"), find_artifact_by_name("炽烈的炎之魔女"),
                                                       "生命值", "伤害加成", "暴击率", tc1, ac1, false));
            combination_list.push_back(new Combination(find_weapon_by_name("护摩之杖"), find_artifact_by_name("追忆之注连"), find_artifact_by_name("追忆之注连"),
                                                       "元素精通", "伤害加成", "暴击率", tc1, ac1, false));
        }
    }
    if (c_name == "艾尔海森")
    {
        auto *tc1 = new Team_config(find_character_by_name("纳西妲"), find_character_by_name("八重神子"), find_character_by_name("钟离"),
                                    "千夜浮梦_深林的记忆_昔日宗室之仪", "草", "");

        //4Q 233332E AAA AAA AAA Z AAA AAA AAA
        vector<Attack_config *> ac1{
                new Attack_config(find_character_by_name(c_name), "Q", 0, false, "蔓激化", 2,
                                  new weapon_artifact_related_arguments(0, false, 0, false, true, 3,
                                                                        "单手剑", 2, 3, -1)),
                new Attack_config(find_character_by_name(c_name), "Q", 0, false, "蔓激化_no_add_damage", 2,
                                  new weapon_artifact_related_arguments(0, false, 0, false, true, 3,
                                                                        "单手剑", 2, 3, -1)),

                new Attack_config(find_character_by_name(c_name), "E", 0, false, "蔓激化", 1,
                                  new weapon_artifact_related_arguments(0, false, 1, false, true, 3,
                                                                        "单手剑", 2, 3, -1)),
                new Attack_config(find_character_by_name(c_name), "E", 1, false, "蔓激化", 8,
                                  new weapon_artifact_related_arguments(2, true, 1, false, true, 3,
                                                                        "单手剑", 3, 3, -1)),
                new Attack_config(find_character_by_name(c_name), "E", 1, false, "蔓激化_no_add_damage", 8,
                                  new weapon_artifact_related_arguments(2, true, 1, false, true, 3,
                                                                        "单手剑", 3, 3, -1)),

                new Attack_config(find_character_by_name(c_name), "平A", 0, false, "蔓激化", 2,
                                  new weapon_artifact_related_arguments(2, true, 1, false, true, 3,
                                                                        "单手剑", 3, 3, -1)),
                new Attack_config(find_character_by_name(c_name), "平A", 0, false, "蔓激化_no_add_damage", 4,
                                  new weapon_artifact_related_arguments(2, true, 1, false, true, 3,
                                                                        "单手剑", 3, 3, -1)),
                new Attack_config(find_character_by_name(c_name), "平A", 1, false, "蔓激化_no_add_damage", 6,
                                  new weapon_artifact_related_arguments(2, true, 1, false, true, 3,
                                                                        "单手剑", 3, 3, -1)),
                new Attack_config(find_character_by_name(c_name), "平A", 2, false, "蔓激化", 3,
                                  new weapon_artifact_related_arguments(2, true, 1, false, true, 3,
                                                                        "单手剑", 3, 3, -1)),
                new Attack_config(find_character_by_name(c_name), "平A", 2, false, "蔓激化_no_add_damage", 3,
                                  new weapon_artifact_related_arguments(2, true, 1, false, true, 3,
                                                                        "单手剑", 3, 3, -1)),
                new Attack_config(find_character_by_name(c_name), "平A", 3, false, "蔓激化", 4,
                                  new weapon_artifact_related_arguments(2, true, 1, false, true, 3,
                                                                        "单手剑", 3, 3, -1)),
                new Attack_config(find_character_by_name(c_name), "平A", 3, false, "蔓激化_no_add_damage", 2,
                                  new weapon_artifact_related_arguments(2, true, 1, false, true, 3,
                                                                        "单手剑", 3, 3, -1)),

                new Attack_config(find_character_by_name(c_name), "重A", 0, false, "蔓激化", 1,
                                  new weapon_artifact_related_arguments(2, true, 1, false, true, 3,
                                                                        "单手剑", 3, 3, -1)),
                new Attack_config(find_character_by_name(c_name), "重A", 1, false, "蔓激化_no_add_damage", 1,
                                  new weapon_artifact_related_arguments(2, true, 1, false, true, 3,
                                                                        "单手剑", 3, 3, -1))
        };

        if (mode == "cal_deployment")
        {
            combination_list.push_back(new Combination(find_weapon_by_name(""), find_artifact_by_name(""), find_artifact_by_name(""),
                                                       "", "", "", tc1, ac1, true));
        }
        else if (mode == "cal_optimal_artifact")
        {
            combination_list.push_back(new Combination(find_weapon_by_name("雾切之回光"), find_artifact_by_name(""), find_artifact_by_name(""),
                                                       "", "", "", tc1, ac1, true));
        }
        else if (mode == "generate_gcsim_script")
        {
            //裁叶翠光
            combination_list.push_back(new Combination(find_weapon_by_name("雾切之回光"), find_artifact_by_name("饰金之梦"), find_artifact_by_name("饰金之梦"),
                                                       "元素精通", "伤害加成", "暴击率", tc1, ac1, true));
            combination_list.push_back(new Combination(find_weapon_by_name("雾切之回光"), find_artifact_by_name("流浪大地的乐团"), find_artifact_by_name("饰金之梦"),
                                                       "元素精通", "伤害加成", "暴击率", tc1, ac1, true));
        }
    }
    if (c_name == "雷电将军")
    {
        auto *tc1 = new Team_config(find_character_by_name("香菱"), find_character_by_name("班尼特"), find_character_by_name("行秋"),
                                    "昔日宗室之仪", "水雷", "水雷火");
        auto *tc2 = new Team_config(find_character_by_name("九条裟罗"), find_character_by_name("枫原万叶"), find_character_by_name("班尼特"),
                                    "昔日宗室之仪_翠绿之影", "雷火", "雷火");

        //无法准确得知感电反应次数和触发关系
        //Q AAAAA AAAAA AAAAA 18+1E
        vector<Attack_config *> ac1{
                new Attack_config(find_character_by_name(c_name), "Q", 0, false, "感电_no_add_damage", 1,
                                  new weapon_artifact_related_arguments(2, true, 0, false, true, 1,
                                                                        "长柄武器", 0, 3, 0)),
                new Attack_config(find_character_by_name(c_name), "Q", 1, false, "感电_no_add_damage", 3,
                                  new weapon_artifact_related_arguments(2, true, 0, false, true, 2,
                                                                        "长柄武器", 0, 3, 0)),
                new Attack_config(find_character_by_name(c_name), "Q", 2, false, "感电_no_add_damage", 3,
                                  new weapon_artifact_related_arguments(2, true, 0, false, true, 2,
                                                                        "长柄武器", 0, 3, 0)),
                new Attack_config(find_character_by_name(c_name), "Q", 3, false, "感电_no_add_damage", 3,
                                  new weapon_artifact_related_arguments(2, true, 0, false, true, 2,
                                                                        "长柄武器", 0, 3, 0)),
                new Attack_config(find_character_by_name(c_name), "Q", 4, false, "感电_no_add_damage", 3,
                                  new weapon_artifact_related_arguments(2, true, 0, false, true, 2,
                                                                        "长柄武器", 0, 3, 0)),
                new Attack_config(find_character_by_name(c_name), "Q", 5, false, "感电_no_add_damage", 3,
                                  new weapon_artifact_related_arguments(2, true, 0, false, true, 2,
                                                                        "长柄武器", 0, 3, 0)),
                new Attack_config(find_character_by_name(c_name), "Q", 6, false, "感电_no_add_damage", 3,
                                  new weapon_artifact_related_arguments(2, true, 0, false, true, 2,
                                                                        "长柄武器", 0, 3, 0)),
                new Attack_config(find_character_by_name(c_name), "E", 0, false, "感电_no_add_damage", 1,
                                  new weapon_artifact_related_arguments(2, true, 1, false, true, 2,
                                                                        "长柄武器", 0, 3, 1)),
                new Attack_config(find_character_by_name(c_name), "E", 1, false, "感电_no_add_damage", 18,
                                  new weapon_artifact_related_arguments(2, true, 0, false, true, 2,
                                                                        "长柄武器", 0, 3, 0))
        };

        vector<Attack_config *> ac2{
                new Attack_config(find_character_by_name(c_name), "Q", 0, false, "NONE", 1,
                                  new weapon_artifact_related_arguments(2, true, 0, false, true, 1,
                                                                        "长柄武器", 0, 3, 0)),
                new Attack_config(find_character_by_name(c_name), "Q", 1, false, "NONE", 3,
                                  new weapon_artifact_related_arguments(2, true, 0, false, true, 2,
                                                                        "长柄武器", 0, 3, 0)),
                new Attack_config(find_character_by_name(c_name), "Q", 2, false, "NONE", 3,
                                  new weapon_artifact_related_arguments(2, true, 0, false, true, 2,
                                                                        "长柄武器", 0, 3, 0)),
                new Attack_config(find_character_by_name(c_name), "Q", 3, false, "NONE", 3,
                                  new weapon_artifact_related_arguments(2, true, 0, false, true, 2,
                                                                        "长柄武器", 0, 3, 0)),
                new Attack_config(find_character_by_name(c_name), "Q", 4, false, "NONE", 3,
                                  new weapon_artifact_related_arguments(2, true, 0, false, true, 2,
                                                                        "长柄武器", 0, 3, 0)),
                new Attack_config(find_character_by_name(c_name), "Q", 5, false, "NONE", 3,
                                  new weapon_artifact_related_arguments(2, true, 0, false, true, 2,
                                                                        "长柄武器", 0, 3, 0)),
                new Attack_config(find_character_by_name(c_name), "Q", 6, false, "NONE", 3,
                                  new weapon_artifact_related_arguments(2, true, 0, false, true, 2,
                                                                        "长柄武器", 0, 3, 0)),
                new Attack_config(find_character_by_name(c_name), "E", 0, false, "NONE", 1,
                                  new weapon_artifact_related_arguments(2, true, 1, false, true, 2,
                                                                        "长柄武器", 0, 3, 1)),
                new Attack_config(find_character_by_name(c_name), "E", 1, false, "NONE", 18,
                                  new weapon_artifact_related_arguments(2, true, 0, false, true, 2,
                                                                        "长柄武器", 0, 3, 0))
        };

        if (mode == "cal_deployment")
        {
            combination_list.push_back(new Combination(find_weapon_by_name(""), find_artifact_by_name(""), find_artifact_by_name(""),
                                                       "", "", "", tc1, ac1, true));
            combination_list.push_back(new Combination(find_weapon_by_name(""), find_artifact_by_name(""), find_artifact_by_name(""),
                                                       "", "", "", tc2, ac2, true));
        }
        else if (mode == "cal_optimal_artifact")
        {
            combination_list.push_back(new Combination(find_weapon_by_name("渔获"), find_artifact_by_name(""), find_artifact_by_name(""),
                                                       "", "", "", tc1, ac1, true));
            combination_list.push_back(new Combination(find_weapon_by_name("渔获"), find_artifact_by_name(""), find_artifact_by_name(""),
                                                       "", "", "", tc2, ac2, true));
        }
        else if (mode == "generate_gcsim_script")
        {
            //薙草之稻光
            combination_list.push_back(new Combination(find_weapon_by_name("渔获"), find_artifact_by_name("绝缘之旗印"), find_artifact_by_name("绝缘之旗印"),
                                                       "元素充能效率", "伤害加成", "暴击率", tc1, ac1, true));
            combination_list.push_back(new Combination(find_weapon_by_name("渔获"), find_artifact_by_name("绝缘之旗印"), find_artifact_by_name("绝缘之旗印"),
                                                       "元素充能效率", "攻击力", "暴击率", tc1, ac1, true));
            combination_list.push_back(new Combination(find_weapon_by_name("渔获"), find_artifact_by_name("绝缘之旗印"), find_artifact_by_name("绝缘之旗印"),
                                                       "元素充能效率", "伤害加成", "暴击率", tc2, ac2, true));
            combination_list.push_back(new Combination(find_weapon_by_name("渔获"), find_artifact_by_name("绝缘之旗印"), find_artifact_by_name("绝缘之旗印"),
                                                       "元素充能效率", "攻击力", "暴击率", tc2, ac2, true));
        }
    }
    if (c_name == "神里绫华")
    {
        auto *tc1 = new Team_config(find_character_by_name("甘雨"), find_character_by_name("莫娜"), find_character_by_name("温迪"),
                                    "昔日宗室之仪_翠绿之影", "冰水", "冰水");
        auto *tc2 = new Team_config(find_character_by_name("甘雨"), find_character_by_name("莫娜"), find_character_by_name("枫原万叶"),
                                    "昔日宗室之仪_翠绿之影", "冰水", "冰水");

        //EAQ EAZAA
        vector<Attack_config *> ac1{
                new Attack_config(find_character_by_name(c_name), "E", 0, false, "冻结", 1,
                                  new weapon_artifact_related_arguments(0, false, 1, false, false, 2,
                                                                        "单手剑", 1, 2, -1)),
                new Attack_config(find_character_by_name(c_name), "平A", 0, false, "冻结", 1,
                                  new weapon_artifact_related_arguments(1, false, 1, false, false, 3,
                                                                        "单手剑", 1, 2, -1)),
                new Attack_config(find_character_by_name(c_name), "Q", 0, false, "冻结", 19,
                                  new weapon_artifact_related_arguments(1, false, 1, false, false, 2,
                                                                        "单手剑", 3, 2, -1)),
                new Attack_config(find_character_by_name(c_name), "Q", 1, false, "冻结", 1,
                                  new weapon_artifact_related_arguments(1, false, 1, false, false, 2,
                                                                        "单手剑", 3, 2, -1)),
                new Attack_config(find_character_by_name(c_name), "E", 0, false, "冻结", 1,
                                  new weapon_artifact_related_arguments(0, false, 1, false, false, 2,
                                                                        "单手剑", 2, 2, -1)),
                new Attack_config(find_character_by_name(c_name), "平A", 0, false, "冻结", 2,
                                  new weapon_artifact_related_arguments(1, false, 1, false, false, 3,
                                                                        "单手剑", 2, 2, -1)),
                new Attack_config(find_character_by_name(c_name), "平A", 1, false, "冻结", 1,
                                  new weapon_artifact_related_arguments(1, false, 1, false, false, 3,
                                                                        "单手剑", 2, 2, -1)),
                new Attack_config(find_character_by_name(c_name), "重A", 0, false, "冻结", 3,
                                  new weapon_artifact_related_arguments(1, false, 1, false, false, 3,
                                                                        "单手剑", 2, 2, -1))
        };

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
    if (c_name == "甘雨")
    {
        auto *tc1 = new Team_config(find_character_by_name("神里绫华"), find_character_by_name("莫娜"), find_character_by_name("温迪"),
                                    "昔日宗室之仪_翠绿之影", "冰水", "冰水");
        auto *tc2 = new Team_config(find_character_by_name("神里绫华"), find_character_by_name("莫娜"), find_character_by_name("枫原万叶"),
                                    "昔日宗室之仪_翠绿之影", "冰水", "冰水");

        //EQ EZZ
        vector<Attack_config *> ac1{
                new Attack_config(find_character_by_name(c_name), "Q", 0, true, "冻结", 15,
                                  new weapon_artifact_related_arguments(1, false, 1, false, true, 2,
                                                                        "弓", 1, 2, 0)),
                new Attack_config(find_character_by_name(c_name), "E", 0, false, "冻结", 2,
                                  new weapon_artifact_related_arguments(0, false, 1, false, true, 2,
                                                                        "弓", 1, 2, 0)),
                new Attack_config(find_character_by_name(c_name), "E", 1, true, "冻结", 2,
                                  new weapon_artifact_related_arguments(0, false, 1, false, true, 2,
                                                                        "弓", 1, 2, 0)),
                new Attack_config(find_character_by_name(c_name), "重A", 0, false, "冻结", 2,
                                  new weapon_artifact_related_arguments(2, false, 1, false, true, 3,
                                                                        "弓", 2, 3, 0)),
                new Attack_config(find_character_by_name(c_name), "重A", 1, false, "冻结", 2,
                                  new weapon_artifact_related_arguments(2, false, 1, false, true, 3,
                                                                        "弓", 2, 3, 0))
        };

        if (mode == "cal_deployment")
        {
            combination_list.push_back(new Combination(find_weapon_by_name(""), find_artifact_by_name(""), find_artifact_by_name(""),
                                                       "", "", "", tc1, ac1, true));
            combination_list.push_back(new Combination(find_weapon_by_name(""), find_artifact_by_name(""), find_artifact_by_name(""),
                                                       "", "", "", tc2, ac1, true));
        }
        else if (mode == "cal_optimal_artifact")
        {
            combination_list.push_back(new Combination(find_weapon_by_name("飞雷之弦振"), find_artifact_by_name(""), find_artifact_by_name(""),
                                                       "", "", "", tc1, ac1, true));
            combination_list.push_back(new Combination(find_weapon_by_name("飞雷之弦振"), find_artifact_by_name(""), find_artifact_by_name(""),
                                                       "", "", "", tc2, ac1, true));
        }
        else if (mode == "generate_gcsim_script")
        {
            combination_list.push_back(new Combination(find_weapon_by_name("飞雷之弦振"), find_artifact_by_name("冰风迷途的勇士"), find_artifact_by_name("冰风迷途的勇士"),
                                                       "攻击力", "伤害加成", "暴击伤害", tc1, ac1, true));
            combination_list.push_back(new Combination(find_weapon_by_name("飞雷之弦振"), find_artifact_by_name("冰风迷途的勇士"), find_artifact_by_name("冰风迷途的勇士"),
                                                       "攻击力", "伤害加成", "暴击伤害", tc2, ac1, true));
        }
    }
    if (c_name == "纳西妲")
    {
        auto *tc1 = new Team_config(find_character_by_name("艾尔海森"), find_character_by_name("八重神子"), find_character_by_name("钟离"),
                                    "昔日宗室之仪", "草", "");

        vector<Attack_config *> ac1{
                new Attack_config(find_character_by_name(c_name), "E", 0, false, "蔓激化", 1,
                                  new weapon_artifact_related_arguments(2, true, 1, false, true, 1,
                                                                        "法器", 0, 0, 1)),
                new Attack_config(find_character_by_name(c_name), "E", 1, true, "蔓激化", 7,
                                  new weapon_artifact_related_arguments(2, true, 1, false, true, 1,
                                                                        "法器", 1, 0, 0))
        };

        if (mode == "cal_deployment")
        {
            combination_list.push_back(new Combination(find_weapon_by_name(""), find_artifact_by_name("深林的记忆"), find_artifact_by_name("深林的记忆"),
                                                       "", "", "", tc1, ac1, true));
        }
        else if (mode == "cal_optimal_artifact")
        {
            combination_list.push_back(new Combination(find_weapon_by_name("千夜浮梦"), find_artifact_by_name("深林的记忆"), find_artifact_by_name("深林的记忆"),
                                                       "", "", "", tc1, ac1, true));
        }
        else if (mode == "generate_gcsim_script")
        {
            combination_list.push_back(new Combination(find_weapon_by_name("千夜浮梦"), find_artifact_by_name("深林的记忆"), find_artifact_by_name("深林的记忆"),
                                                       "元素精通", "元素精通", "暴击率", tc1, ac1, true));
            combination_list.push_back(new Combination(find_weapon_by_name("千夜浮梦"), find_artifact_by_name("深林的记忆"), find_artifact_by_name("深林的记忆"),
                                                       "元素精通", "伤害加成", "暴击率", tc1, ac1, true));
        }
    }
    if (c_name == "夜兰")
    {
        auto *tc1 = new Team_config(find_character_by_name("胡桃"), find_character_by_name("钟离"), find_character_by_name("行秋"),
                                    "昔日宗室之仪", "水", "水");

        //EEQ
        vector<Attack_config *> ac1{
                new Attack_config(find_character_by_name(c_name), "E", 0, false, "NONE", 2,
                                  new weapon_artifact_related_arguments(1, false, 2, false, true, 2,
                                                                        "弓", 3, 2, 0)),
                new Attack_config(find_character_by_name(c_name), "Q", 0, false, "NONE", 1,
                                  new weapon_artifact_related_arguments(2, false, 2, false, true, 3,
                                                                        "弓", 2, 3, 0)),
                new Attack_config(find_character_by_name(c_name), "Q", 1, true, "NONE", 45,
                                  new weapon_artifact_related_arguments(2, false, 2, false, true, 3,
                                                                        "弓", 2, 3, 0)),
                new Attack_config(find_character_by_name(c_name), "Q", 2, true, "NONE", 7,
                                  new weapon_artifact_related_arguments(2, false, 2, false, true, 3,
                                                                        "弓", 2, 3, 0))
        };

        if (mode == "cal_deployment")
        {
            combination_list.push_back(new Combination(find_weapon_by_name(""), find_artifact_by_name(""), find_artifact_by_name(""),
                                                       "", "", "", tc1, ac1, true));
        }
        else if (mode == "cal_optimal_artifact")
        {
            combination_list.push_back(new Combination(find_weapon_by_name("终末嗟叹之诗"), find_artifact_by_name(""), find_artifact_by_name(""),
                                                       "", "", "", tc1, ac1, true));
        }
        else if (mode == "generate_gcsim_script")
        {
            combination_list.push_back(new Combination(find_weapon_by_name("若水"), find_artifact_by_name("绝缘之旗印"), find_artifact_by_name("绝缘之旗印"),
                                                       "生命值", "伤害加成", "暴击率", tc1, ac1, true));//准备提升
            combination_list.push_back(new Combination(find_weapon_by_name("终末嗟叹之诗"), find_artifact_by_name("绝缘之旗印"), find_artifact_by_name("绝缘之旗印"),
                                                       "生命值", "伤害加成", "暴击率", tc1, ac1, true));
        }
    }
    if (c_name == "八重神子")
    {
        auto *tc1 = new Team_config(find_character_by_name("纳西妲"), find_character_by_name("艾尔海森"), find_character_by_name("钟离"),
                                    "千夜浮梦_深林的记忆_昔日宗室之仪", "草", "");

        vector<Attack_config *> ac1{
                new Attack_config(find_character_by_name(c_name), "E", 0, true, "超激化", 10,
                                  new weapon_artifact_related_arguments(2, true, 3, false, true, 1,
                                                                        "法器", 3, 0, 0)),
                new Attack_config(find_character_by_name(c_name), "E", 0, true, "超激化_no_add_damage", 18,
                                  new weapon_artifact_related_arguments(2, true, 3, false, true, 1,
                                                                        "法器", 3, 0, 0)),
                new Attack_config(find_character_by_name(c_name), "Q", 0, false, "超激化", 1,
                                  new weapon_artifact_related_arguments(2, true, 0, false, true, 1,
                                                                        "法器", 3, 0, 1)),
                new Attack_config(find_character_by_name(c_name), "Q", 1, false, "超激化", 3,
                                  new weapon_artifact_related_arguments(2, true, 0, false, true, 2,
                                                                        "法器", 3, 0, 1))
        };

        if (mode == "cal_deployment")
        {
            combination_list.push_back(new Combination(find_weapon_by_name(""), find_artifact_by_name(""), find_artifact_by_name(""),
                                                       "", "", "", tc1, ac1, false));//两轮一大
        }
        else if (mode == "cal_optimal_artifact")
        {
            combination_list.push_back(new Combination(find_weapon_by_name("神乐之真意"), find_artifact_by_name(""), find_artifact_by_name(""),
                                                       "", "", "", tc1, ac1, false));//两轮一大
        }
        else if (mode == "generate_gcsim_script")
        {
            combination_list.push_back(new Combination(find_weapon_by_name("神乐之真意"), find_artifact_by_name("流浪大地的乐团"), find_artifact_by_name("角斗士的终幕礼"),
                                                       "攻击力", "伤害加成", "暴击率", tc1, ac1, false));//两轮一大
            combination_list.push_back(new Combination(find_weapon_by_name("神乐之真意"), find_artifact_by_name("饰金之梦"), find_artifact_by_name("饰金之梦"),
                                                       "攻击力", "伤害加成", "暴击率", tc1, ac1, false));//两轮一大
        }
    }
    if (c_name == "香菱")
    {
        auto *tc1 = new Team_config(find_character_by_name("雷电将军"), find_character_by_name("班尼特"), find_character_by_name("行秋"),
                                    "昔日宗室之仪", "水雷", "水雷火");

        //QE
        vector<Attack_config *> ac1{
                new Attack_config(find_character_by_name(c_name), "Q", 0, false, "蒸发_超载_no_add_damage", 1,
                                  new weapon_artifact_related_arguments(0, false, 0, false, true, 0,
                                                                        "长柄武器", 0, 0, 0)),
                new Attack_config(find_character_by_name(c_name), "Q", 1, false, "蒸发_超载_no_add_damage", 1,
                                  new weapon_artifact_related_arguments(0, false, 0, false, true, 1,
                                                                        "长柄武器", 0, 0, 0)),
                new Attack_config(find_character_by_name(c_name), "Q", 2, false, "蒸发_超载_no_add_damage", 1,
                                  new weapon_artifact_related_arguments(0, false, 0, false, true, 1,
                                                                        "长柄武器", 0, 0, 0)),
                new Attack_config(find_character_by_name(c_name), "Q", 3, true, "蒸发_超载_no_add_damage", 4,
                                  new weapon_artifact_related_arguments(0, false, 0, false, true, 1,
                                                                        "长柄武器", 0, 0, 0)),
                new Attack_config(find_character_by_name(c_name), "Q", 3, true, "蒸发_超载", 8,
                                  new weapon_artifact_related_arguments(0, false, 0, false, true, 1,
                                                                        "长柄武器", 0, 0, 0)),
                new Attack_config(find_character_by_name(c_name), "E", 0, true, "蒸发", 4,
                                  new weapon_artifact_related_arguments(0, false, 1, false, true, 1,
                                                                        "长柄武器", 0, 0, 0))
        };

        if (mode == "cal_deployment")
        {
            combination_list.push_back(new Combination(find_weapon_by_name(""), find_artifact_by_name(""), find_artifact_by_name(""),
                                                       "", "", "", tc1, ac1, true));
        }
        else if (mode == "cal_optimal_artifact")
        {
            combination_list.push_back(new Combination(find_weapon_by_name("断浪长鳍"), find_artifact_by_name(""), find_artifact_by_name(""),
                                                       "", "", "", tc1, ac1, true));
            combination_list.push_back(new Combination(find_weapon_by_name("决斗之枪"), find_artifact_by_name(""), find_artifact_by_name(""),
                                                       "", "", "", tc1, ac1, true));
            combination_list.push_back(new Combination(find_weapon_by_name("匣里灭辰"), find_artifact_by_name(""), find_artifact_by_name(""),
                                                       "", "", "", tc1, ac1, true));
        }
        else if (mode == "generate_gcsim_script")
        {
            combination_list.push_back(new Combination(find_weapon_by_name("断浪长鳍"), find_artifact_by_name("绝缘之旗印"), find_artifact_by_name("绝缘之旗印"),
                                                       "元素充能效率", "伤害加成", "暴击率", tc1, ac1, true));
            combination_list.push_back(new Combination(find_weapon_by_name("断浪长鳍"), find_artifact_by_name("饰金之梦"), find_artifact_by_name("饰金之梦"),
                                                       "元素充能效率", "伤害加成", "暴击率", tc1, ac1, true));
        }
    }
    if (c_name == "行秋")
    {
        auto *tc1 = new Team_config(find_character_by_name("胡桃"), find_character_by_name("钟离"), find_character_by_name("夜兰"),
                                    "终末嗟叹之诗_昔日宗室之仪", "水", "水");

        auto *tc2 = new Team_config(find_character_by_name("雷电将军"), find_character_by_name("香菱"), find_character_by_name("班尼特"),
                                    "昔日宗室之仪", "水雷", "水雷火");

        //QE
        vector<Attack_config *> ac1{
                new Attack_config(find_character_by_name(c_name), "E", 0, false, "NONE", 1,
                                  new weapon_artifact_related_arguments(0, false, 1, false, true, 2,
                                                                        "单手剑", 2, 1, -1)),
                new Attack_config(find_character_by_name(c_name), "E", 1, false, "NONE", 1,
                                  new weapon_artifact_related_arguments(1, false, 1, false, true, 3,
                                                                        "单手剑", 2, 1, -1)),
                new Attack_config(find_character_by_name(c_name), "Q", 0, true, "NONE", 42,
                                  new weapon_artifact_related_arguments(1, false, 1, false, true, 3,
                                                                        "单手剑", 2, 1, -1))
        };

        vector<Attack_config *> ac2{
                new Attack_config(find_character_by_name(c_name), "E", 0, false, "感电_no_add_damage", 1,
                                  new weapon_artifact_related_arguments(0, false, 1, false, true, 2,
                                                                        "单手剑", 2, 1, -1)),
                new Attack_config(find_character_by_name(c_name), "E", 1, false, "感电_no_add_damage", 1,
                                  new weapon_artifact_related_arguments(1, false, 1, false, true, 3,
                                                                        "单手剑", 2, 1, -1)),
                new Attack_config(find_character_by_name(c_name), "Q", 0, true, "感电_no_add_damage", 45,
                                  new weapon_artifact_related_arguments(1, false, 1, false, true, 3,
                                                                        "单手剑", 2, 1, -1))
        };

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
            combination_list.push_back(new Combination(find_weapon_by_name("磐岩结绿"), find_artifact_by_name("绝缘之旗印"), find_artifact_by_name("绝缘之旗印"),
                                                       "攻击力", "伤害加成", "暴击伤害", tc1, ac1, true));//准备提升
            combination_list.push_back(new Combination(find_weapon_by_name("磐岩结绿"), find_artifact_by_name("绝缘之旗印"), find_artifact_by_name("绝缘之旗印"),
                                                       "攻击力", "伤害加成", "暴击伤害", tc2, ac2, true));//准备提升
            combination_list.push_back(new Combination(find_weapon_by_name("祭礼剑"), find_artifact_by_name("沉沦之心"), find_artifact_by_name("昔日宗室之仪"),
                                                       "攻击力", "伤害加成", "暴击率", tc1, ac1, true));
            combination_list.push_back(new Combination(find_weapon_by_name("祭礼剑"), find_artifact_by_name("沉沦之心"), find_artifact_by_name("昔日宗室之仪"),
                                                       "攻击力", "伤害加成", "暴击率", tc2, ac2, true));
        }
    }
    if (c_name == "九条裟罗")
    {
        auto *tc1 = new Team_config(find_character_by_name("雷电将军"), find_character_by_name("枫原万叶"), find_character_by_name("班尼特"),
                                    "昔日宗室之仪_翠绿之影", "雷火", "雷火");

        //EQ
        vector<Attack_config *> ac1{
                new Attack_config(find_character_by_name(c_name), "Q", 0, false, "NONE", 1,
                                  new weapon_artifact_related_arguments(1, false, 1, false, false, 3,
                                                                        "弓", 2, 3, 0)),
                new Attack_config(find_character_by_name(c_name), "Q", 1, true, "NONE", 12,
                                  new weapon_artifact_related_arguments(1, false, 1, false, false, 3,
                                                                        "弓", 2, 3, 0)),
                new Attack_config(find_character_by_name(c_name), "E", 0, false, "NONE", 1,
                                  new weapon_artifact_related_arguments(1, false, 1, false, false, 2,
                                                                        "弓", 2, 2, 1))
        };

        if (mode == "cal_deployment")
        {
            combination_list.push_back(new Combination(find_weapon_by_name("天空之翼"), find_artifact_by_name(""), find_artifact_by_name(""),
                                                       "", "", "", tc1, ac1, true));
        }
        else if (mode == "cal_optimal_artifact")
        {
            combination_list.push_back(new Combination(find_weapon_by_name("天空之翼"), find_artifact_by_name(""), find_artifact_by_name(""),
                                                       "", "", "", tc1, ac1, true));
        }
        else if (mode == "generate_gcsim_script")
        {
            combination_list.push_back(new Combination(find_weapon_by_name("天空之翼"), find_artifact_by_name("绝缘之旗印"), find_artifact_by_name("绝缘之旗印"),
                                                       "元素充能效率", "伤害加成", "暴击率", tc1, ac1, true));
        }
    }
    if (c_name == "温迪")
    {
        if (mode == "generate_gcsim_script")
        {
            //终末嗟叹之诗
            combination_list.push_back(new Combination(find_weapon_by_name("终末嗟叹之诗"), find_artifact_by_name("翠绿之影"), find_artifact_by_name("翠绿之影"),
                                                       "元素精通", "元素精通", "元素精通", nullptr, vector<Attack_config *>{}, true));
            combination_list.push_back(new Combination(find_weapon_by_name("天空之翼"), find_artifact_by_name("翠绿之影"), find_artifact_by_name("翠绿之影"),
                                                       "攻击力", "伤害加成", "暴击率", nullptr, vector<Attack_config *>{}, true));
        }
    }
    if (c_name == "莫娜")
    {
        if (mode == "generate_gcsim_script")
        {
            combination_list.push_back(new Combination(find_weapon_by_name("试作金珀"), find_artifact_by_name("昔日宗室之仪"), find_artifact_by_name("昔日宗室之仪"),
                                                       "元素充能效率", "攻击力", "暴击率", nullptr, vector<Attack_config *>{}, true));
//            combination_list.push_back(new Combination(find_weapon_by_name("讨龙英杰谭"), find_artifact_by_name("昔日宗室之仪"), find_artifact_by_name("昔日宗室之仪"), "元素充能效率", "攻击力", "暴击率"));
        }
    }
    if (c_name == "钟离")
    {
        if (mode == "generate_gcsim_script")
        {
            //护摩之杖
            combination_list.push_back(new Combination(find_weapon_by_name("西风长枪"), find_artifact_by_name("昔日宗室之仪"), find_artifact_by_name("昔日宗室之仪"),
                                                       "生命值", "生命值", "暴击率", nullptr, vector<Attack_config *>{}, false));
//            combination_list.push_back(new Combination(find_weapon_by_name("决斗之枪"), find_artifact_by_name("昔日宗室之仪"), find_artifact_by_name("昔日宗室之仪"), "生命值", "伤害加成", "暴击伤害"));
        }
    }
    if (c_name == "枫原万叶")
    {
        if (mode == "generate_gcsim_script")
        {
            //苍古自由之誓
            combination_list.push_back(new Combination(find_weapon_by_name("铁蜂刺"), find_artifact_by_name("翠绿之影"), find_artifact_by_name("翠绿之影"),
                                                       "元素精通", "元素精通", "元素精通", nullptr, vector<Attack_config *>{}, true));
        }
    }
    if (c_name == "班尼特")
    {
        if (mode == "generate_gcsim_script")
        {
            combination_list.push_back(new Combination(find_weapon_by_name("天空之刃"), find_artifact_by_name("昔日宗室之仪"), find_artifact_by_name("昔日宗室之仪"),
                                                       "元素充能效率", "生命值", "治疗加成", nullptr, vector<Attack_config *>{}, true));
        }
    }
}

//cal_(add_)deployment

bool cmp_func(Group *a, Group *b)
{
    if (a->total_damage > b->total_damage) return true;
    else return false;
}

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

class gcsim_script
{
public:
    vector<Character *> character_list;
    string option_modify;
    string target_modify;
    vector<string> attack_list;
};

gcsim_script *team_script;

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

    string c_name;
    for (int i = 0; i < 4; ++i)
    {
        infile >> c_name;
        team_script->character_list.push_back(find_character_by_name(c_name));
    }

    string info;
    getline(infile, info);
    while (!infile.eof())
    {
        getline(infile, info);
        if (info.empty()) continue;
        else if (info.find("options") != string::npos) team_script->option_modify = info.substr(info.find_first_of(' ') + 1);
        else if (info.find("target") != string::npos) team_script->target_modify = info.substr(info.find_first_of(' ') + 1);
        else
        {
            replace_all(info, team_script->character_list[0]->name, team_script->character_list[0]->english_name);
            replace_all(info, team_script->character_list[1]->name, team_script->character_list[1]->english_name);
            replace_all(info, team_script->character_list[2]->name, team_script->character_list[2]->english_name);
            replace_all(info, team_script->character_list[3]->name, team_script->character_list[3]->english_name);
            replace_all(info, "平A", "attack");
            replace_all(info, "重A", "charge");
            replace_all(info, "下落A", "high_plunge");
            replace_all(info, "长E", "skill[hold=1]");
            replace_all(info, "E", "skill");
            replace_all(info, "Q", "burst");
            replace_all(info, "冲刺", "dash");
            replace_all(info, "跳跃", "jump");
            team_script->attack_list.push_back(info);
        }
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
    get_all_config(team_script->character_list[0]->name, combination_list_1, "generate_gcsim_script");
    vector<Combination *> combination_list_2;
    get_all_config(team_script->character_list[1]->name, combination_list_2, "generate_gcsim_script");
    vector<Combination *> combination_list_3;
    get_all_config(team_script->character_list[2]->name, combination_list_3, "generate_gcsim_script");
    vector<Combination *> combination_list_4;
    get_all_config(team_script->character_list[3]->name, combination_list_4, "generate_gcsim_script");

    int filecount = 1;
    for (auto &combination_1: combination_list_1)
        if (combination_1->team_config == nullptr ||
            (combination_1->team_config->teammate_all.find(team_script->character_list[1]->name) != string::npos &&
             combination_1->team_config->teammate_all.find(team_script->character_list[2]->name) != string::npos &&
             combination_1->team_config->teammate_all.find(team_script->character_list[3]->name) != string::npos))
            for (auto &combination_2: combination_list_2)
                if (combination_2->team_config == nullptr ||
                    (combination_2->team_config->teammate_all.find(team_script->character_list[0]->name) != string::npos &&
                     combination_2->team_config->teammate_all.find(team_script->character_list[2]->name) != string::npos &&
                     combination_2->team_config->teammate_all.find(team_script->character_list[3]->name) != string::npos))
                    for (auto &combination_3: combination_list_3)
                        if (combination_3->team_config == nullptr ||
                            (combination_3->team_config->teammate_all.find(team_script->character_list[1]->name) != string::npos &&
                             combination_3->team_config->teammate_all.find(team_script->character_list[0]->name) != string::npos &&
                             combination_3->team_config->teammate_all.find(team_script->character_list[3]->name) != string::npos))
                            for (auto &combination_4: combination_list_4)
                                if (combination_4->team_config == nullptr ||
                                    (combination_4->team_config->teammate_all.find(team_script->character_list[1]->name) != string::npos &&
                                     combination_4->team_config->teammate_all.find(team_script->character_list[2]->name) != string::npos &&
                                     combination_4->team_config->teammate_all.find(team_script->character_list[0]->name) != string::npos))
                                {
                                    //ps1
                                    outfile_debug << "./run_substat_optimizer_full.bat " << (teamname + "_" + to_string(filecount) + ".txt") << " > ./logs/" << (teamname + "_" + to_string(filecount) + ".txt") << endl;

                                    //config
                                    outfile_result.open(filepath + teamname + "_" + to_string(filecount) + ".txt");
                                    outfile_result << "#./run_substat_optimizer_full.bat " << (teamname + "_" + to_string(filecount) + ".txt") << " > ./logs/" << (teamname + "_" + to_string(filecount) + ".txt") << endl;
                                    outfile_result << endl;

                                    //character1
                                    outfile_result << "#" << team_script->character_list[0]->name << " " << combination_1->w_point->name << " " << combination_1->suit1->name << " " << combination_1->suit2->name << " "
                                                   << combination_1->a_main3 << " " << combination_1->a_main4 << " " << combination_1->a_main5 << endl;

                                    outfile_result << team_script->character_list[0]->english_name << " char lvl=90/90 cons=" << to_string(team_script->character_list[0]->constellation)
                                                   << " talent=" << to_string(team_script->character_list[0]->A_level) << ","
                                                   << to_string((team_script->character_list[0]->E_level > 10) ? (team_script->character_list[0]->E_level - 3) : team_script->character_list[0]->E_level) << ","
                                                   << to_string((team_script->character_list[0]->Q_level > 10) ? (team_script->character_list[0]->Q_level - 3) : team_script->character_list[0]->Q_level) << ";" << endl;
                                    outfile_result << team_script->character_list[0]->english_name << " add weapon=\"" << combination_1->w_point->english_name << "\" refine=" << to_string(combination_1->w_point->level) << " lvl=90/90;" << endl;
                                    if (combination_1->suit1 == combination_1->suit2)
                                    {
                                        outfile_result << team_script->character_list[0]->english_name << " add set=\"" << combination_1->suit1->english_name << "\" count=4;" << endl;
                                    }
                                    else
                                    {
                                        outfile_result << team_script->character_list[0]->english_name << " add set=\"" << combination_1->suit1->english_name << "\" count=2;" << endl;
                                        outfile_result << team_script->character_list[0]->english_name << " add set=\"" << combination_1->suit2->english_name << "\" count=2;" << endl;
                                    }
                                    outfile_result << team_script->character_list[0]->english_name << " add stats hp=4780 atk=311 "
                                                   << main_convert(team_script->character_list[0]->ele_type, combination_1->a_main3).first << "=" << main_convert(team_script->character_list[0]->ele_type, combination_1->a_main3).second << " "
                                                   << main_convert(team_script->character_list[0]->ele_type, combination_1->a_main4).first << "=" << main_convert(team_script->character_list[0]->ele_type, combination_1->a_main4).second << " "
                                                   << main_convert(team_script->character_list[0]->ele_type, combination_1->a_main5).first << "=" << main_convert(team_script->character_list[0]->ele_type, combination_1->a_main5).second << ";" << endl;
                                    outfile_result << endl;

                                    //character2
                                    outfile_result << "#" << team_script->character_list[1]->name << " " << combination_2->w_point->name << " " << combination_2->suit1->name << " " << combination_2->suit2->name << " "
                                                   << combination_2->a_main3 << " " << combination_2->a_main4 << " " << combination_2->a_main5 << endl;

                                    outfile_result << team_script->character_list[1]->english_name << " char lvl=90/90 cons=" << to_string(team_script->character_list[1]->constellation)
                                                   << " talent=" << to_string(team_script->character_list[1]->A_level) << ","
                                                   << to_string((team_script->character_list[1]->E_level > 10) ? (team_script->character_list[1]->E_level - 3) : team_script->character_list[1]->E_level) << ","
                                                   << to_string((team_script->character_list[1]->Q_level > 10) ? (team_script->character_list[1]->Q_level - 3) : team_script->character_list[1]->Q_level) << ";" << endl;
                                    outfile_result << team_script->character_list[1]->english_name << " add weapon=\"" << combination_2->w_point->english_name << "\" refine=" << to_string(combination_2->w_point->level) << " lvl=90/90;" << endl;
                                    if (combination_2->suit1 == combination_2->suit2)
                                    {
                                        outfile_result << team_script->character_list[1]->english_name << " add set=\"" << combination_2->suit1->english_name << "\" count=4;" << endl;
                                    }
                                    else
                                    {
                                        outfile_result << team_script->character_list[1]->english_name << " add set=\"" << combination_2->suit1->english_name << "\" count=2;" << endl;
                                        outfile_result << team_script->character_list[1]->english_name << " add set=\"" << combination_2->suit2->english_name << "\" count=2;" << endl;
                                    }
                                    outfile_result << team_script->character_list[1]->english_name << " add stats hp=4780 atk=311 "
                                                   << main_convert(team_script->character_list[1]->ele_type, combination_2->a_main3).first << "=" << main_convert(team_script->character_list[1]->ele_type, combination_2->a_main3).second << " "
                                                   << main_convert(team_script->character_list[1]->ele_type, combination_2->a_main4).first << "=" << main_convert(team_script->character_list[1]->ele_type, combination_2->a_main4).second << " "
                                                   << main_convert(team_script->character_list[1]->ele_type, combination_2->a_main5).first << "=" << main_convert(team_script->character_list[1]->ele_type, combination_2->a_main5).second << ";" << endl;
                                    outfile_result << endl;

                                    //character3
                                    outfile_result << "#" << team_script->character_list[2]->name << " " << combination_3->w_point->name << " " << combination_3->suit1->name << " " << combination_3->suit2->name << " "
                                                   << combination_3->a_main3 << " " << combination_3->a_main4 << " " << combination_3->a_main5 << endl;

                                    outfile_result << team_script->character_list[2]->english_name << " char lvl=90/90 cons=" << to_string(team_script->character_list[2]->constellation)
                                                   << " talent=" << to_string(team_script->character_list[2]->A_level) << ","
                                                   << to_string((team_script->character_list[2]->E_level > 10) ? (team_script->character_list[2]->E_level - 3) : team_script->character_list[2]->E_level) << ","
                                                   << to_string((team_script->character_list[2]->Q_level > 10) ? (team_script->character_list[2]->Q_level - 3) : team_script->character_list[2]->Q_level) << ";" << endl;
                                    outfile_result << team_script->character_list[2]->english_name << " add weapon=\"" << combination_3->w_point->english_name << "\" refine=" << to_string(combination_3->w_point->level) << " lvl=90/90;" << endl;
                                    if (combination_3->suit1 == combination_3->suit2)
                                    {
                                        outfile_result << team_script->character_list[2]->english_name << " add set=\"" << combination_3->suit1->english_name << "\" count=4;" << endl;
                                    }
                                    else
                                    {
                                        outfile_result << team_script->character_list[2]->english_name << " add set=\"" << combination_3->suit1->english_name << "\" count=2;" << endl;
                                        outfile_result << team_script->character_list[2]->english_name << " add set=\"" << combination_3->suit2->english_name << "\" count=2;" << endl;
                                    }
                                    outfile_result << team_script->character_list[2]->english_name << " add stats hp=4780 atk=311 "
                                                   << main_convert(team_script->character_list[2]->ele_type, combination_3->a_main3).first << "=" << main_convert(team_script->character_list[2]->ele_type, combination_3->a_main3).second << " "
                                                   << main_convert(team_script->character_list[2]->ele_type, combination_3->a_main4).first << "=" << main_convert(team_script->character_list[2]->ele_type, combination_3->a_main4).second << " "
                                                   << main_convert(team_script->character_list[2]->ele_type, combination_3->a_main5).first << "=" << main_convert(team_script->character_list[2]->ele_type, combination_3->a_main5).second << ";" << endl;
                                    outfile_result << endl;

                                    //character4
                                    outfile_result << "#" << team_script->character_list[3]->name << " " << combination_4->w_point->name << " " << combination_4->suit1->name << " " << combination_4->suit2->name << " "
                                                   << combination_4->a_main3 << " " << combination_4->a_main4 << " " << combination_4->a_main5 << endl;

                                    outfile_result << team_script->character_list[3]->english_name << " char lvl=90/90 cons=" << to_string(team_script->character_list[3]->constellation)
                                                   << " talent=" << to_string(team_script->character_list[3]->A_level) << ","
                                                   << to_string((team_script->character_list[3]->E_level > 10) ? (team_script->character_list[3]->E_level - 3) : team_script->character_list[3]->E_level) << ","
                                                   << to_string((team_script->character_list[3]->Q_level > 10) ? (team_script->character_list[3]->Q_level - 3) : team_script->character_list[3]->Q_level) << ";" << endl;
                                    outfile_result << team_script->character_list[3]->english_name << " add weapon=\"" << combination_4->w_point->english_name << "\" refine=" << to_string(combination_4->w_point->level) << " lvl=90/90;" << endl;
                                    if (combination_4->suit1 == combination_4->suit2)
                                    {
                                        outfile_result << team_script->character_list[3]->english_name << " add set=\"" << combination_4->suit1->english_name << "\" count=4;" << endl;
                                    }
                                    else
                                    {
                                        outfile_result << team_script->character_list[3]->english_name << " add set=\"" << combination_4->suit1->english_name << "\" count=2;" << endl;
                                        outfile_result << team_script->character_list[3]->english_name << " add set=\"" << combination_4->suit2->english_name << "\" count=2;" << endl;
                                    }
                                    outfile_result << team_script->character_list[3]->english_name << " add stats hp=4780 atk=311 "
                                                   << main_convert(team_script->character_list[3]->ele_type, combination_4->a_main3).first << "=" << main_convert(team_script->character_list[3]->ele_type, combination_4->a_main3).second << " "
                                                   << main_convert(team_script->character_list[3]->ele_type, combination_4->a_main4).first << "=" << main_convert(team_script->character_list[3]->ele_type, combination_4->a_main4).second << " "
                                                   << main_convert(team_script->character_list[3]->ele_type, combination_4->a_main5).first << "=" << main_convert(team_script->character_list[3]->ele_type, combination_4->a_main5).second << ";" << endl;
                                    outfile_result << endl;

                                    //options
                                    outfile_result << "options";
                                    if (team_script->option_modify.find("iteration") == string::npos) outfile_result << " iteration=10000";
                                    if (team_script->option_modify.find("duration") == string::npos) outfile_result << " duration=105";
                                    if (team_script->option_modify.find("swap_delay") == string::npos) outfile_result << " swap_delay=4";
                                    if (!team_script->option_modify.empty()) outfile_result << " " << team_script->option_modify;
                                    outfile_result << ";" << endl;

                                    //target
                                    outfile_result << "target";
                                    if (team_script->target_modify.find("lvl") == string::npos) outfile_result << " lvl=95";
                                    if (team_script->target_modify.find("resist") == string::npos) outfile_result << " resist=0.1";
                                    if (team_script->target_modify.find("particle_threshold") == string::npos) outfile_result << " particle_threshold=150000";
                                    if (team_script->target_modify.find("particle_drop_count") == string::npos) outfile_result << " particle_drop_count=1";
                                    if (!team_script->target_modify.empty()) outfile_result << " " << team_script->target_modify;
                                    outfile_result << ";" << endl;

                                    outfile_result << endl;

                                    //active
                                    if (team_script->attack_list[0] == "start:") outfile_result << "active " << team_script->attack_list[1].substr(0, team_script->attack_list[1].find_first_of(' ')) << ";" << endl;
                                    else outfile_result << "active " << team_script->attack_list[0].substr(0, team_script->attack_list[0].find_first_of(' ')) << ";" << endl;
                                    outfile_result << endl;

                                    //attack_list
                                    for (auto &i: team_script->attack_list)
                                    {
                                        if (i == "start:")
                                        {
                                            outfile_result << "for let x=0; x<5; x=x+1 {" << endl;
                                            continue;
                                        }
                                        if (i[i.length() - 1] == '{' || i[i.length() - 1] == '}')
                                            outfile_result << "  " << i << endl;
                                        else
                                            outfile_result << "  " << i << ";" << endl;
                                    }
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
            team_script = new gcsim_script();
            read_team_script("./RESULTS/chinese_config/" + i + ".txt");
            generate_gcsim_script("./RESULTS/config/", i);
            delete team_script;
        }
        outfile_debug.close();
    }
    return 0;
}