//
// Created by 陈劭彬 on 2022/11/8.
//

#include "Reinforced_Artifact.h"

Reinforced_Artifact::Reinforced_Artifact(string suit_name_,
                                         string main_type_,
                                         string entry1_type_,
                                         double entry1_value_,
                                         string entry2_type_,
                                         double entry2_value_,
                                         string entry3_type_,
                                         double entry3_value_,
                                         string entry4_type_,
                                         double entry4_value_)
{
    suit_name = suit_name_convert(suit_name_);
    main_type = type_name_convert(main_type_);
    entry.emplace_back(type_name_convert(entry1_type_), entry1_value_);
    entry.emplace_back(type_name_convert(entry2_type_), entry2_value_);
    entry.emplace_back(type_name_convert(entry3_type_), entry3_value_);
    entry.emplace_back(type_name_convert(entry4_type_), entry4_value_);
    entry_num["生命值"] = 0;
    entry_num["攻击力"] = 0;
    entry_num["防御力"] = 0;
    entry_num["元素精通"] = 0;
    entry_num["元素充能效率"] = 0;
    entry_num["暴击率"] = 0;
    entry_num["暴击伤害"] = 0;
    count_entry_num();
}

string Reinforced_Artifact::suit_name_convert(string suit_name_)
{
    if (suit_name_ == "wandererTroupe") return "流浪大地的乐团";
    else if (suit_name_ == "gladiatorFinale") return "角斗士的终幕礼";
    else if (suit_name_ == "blizzardStrayer") return "冰风迷途的勇士";
    else if (suit_name_ == "heartOfDepth") return "沉沦之心";
    else if (suit_name_ == "Archaic Petra") return "悠古的磐岩";//?
    else if (suit_name_ == "Retracing Bolide") return "逆飞的流星";//?
    else if (suit_name_ == "Bloodstained Chivalry") return "染血的骑士道";//?
    else if (suit_name_ == "noblesseOblige") return "昔日宗室之仪";
    else if (suit_name_ == "viridescentVenerer") return "翠绿之影";
    else if (suit_name_ == "maidenBeloved") return "被怜爱的少女";
    else if (suit_name_ == "paleFlame") return "苍白之火";
    else if (suit_name_ == "tenacityOfTheMillelith") return "千岩牢固";
    else if (suit_name_ == "crimsonWitch") return "炽烈的炎之魔女";
    else if (suit_name_ == "lavaWalker") return "渡过烈火的贤人";
    else if (suit_name_ == "thunderingFury") return "如雷的盛怒";//?
    else if (suit_name_ == "Thundersoother") return "平息鸣雷的尊者";//?
    else if (suit_name_ == "shimenawaReminiscence") return "追忆之注连";
    else if (suit_name_ == "emblemOfSeveredFate") return "绝缘之旗印";
    else if (suit_name_ == "huskOfOpulentDreams") return "华馆梦醒形骸记";//?
    else if (suit_name_ == "oceanHuedClam") return "海染砗磲";//?
    else if (suit_name_ == "VermillionHereafter") return "辰砂往生录";//?
    else if (suit_name_ == "EchoesOfAnOffering") return "来歆余响";//?
    else if (suit_name_ == "DeepwoodMemories") return "深林的记忆";
    else if (suit_name_ == "GildedDreams") return "饰金之梦";
    else if (suit_name_ == "desertpavilionchronicle") return "沙上楼阁史话";//?
    else if (suit_name_ == "flowerofparadiselost") return "乐园遗落之花";//?
    else if (suit_name_ == "nymphsdream") return "水仙之梦";//?
    else if (suit_name_ == "dewflowersglow") return "花海甘露之光";//?
    else
    {
        cout << suit_name_ << endl;
        return suit_name_;
    }
}

string Reinforced_Artifact::type_name_convert(string type_name_)
{
    if (type_name_ == "lifeStatic") return "生命值S";
    else if (type_name_ == "lifePercentage") return "生命值";
    else if (type_name_ == "attackStatic") return "攻击力S";
    else if (type_name_ == "attackPercentage") return "攻击力";
    else if (type_name_ == "defendStatic") return "防御力S";
    else if (type_name_ == "defendPercentage") return "防御力";
    else if (type_name_ == "elementalMastery") return "元素精通";
    else if (type_name_ == "recharge") return "元素充能效率";
    else if (type_name_ == "critical") return "暴击率";
    else if (type_name_ == "criticalDamage") return "暴击伤害";
    else if (type_name_ == "waterBonus") return "水伤害加成";
    else if (type_name_ == "iceBonus") return "冰伤害加成";
    else if (type_name_ == "fireBonus") return "火伤害加成";
    else if (type_name_ == "thunderBonus") return "雷伤害加成";
    else if (type_name_ == "windBonus") return "风伤害加成";
    else if (type_name_ == "rockBonus") return "岩伤害加成";
    else if (type_name_ == "dendroBonus") return "草伤害加成";
    else if (type_name_ == "physicalBonus") return "物理伤害加成";
    else if (type_name_ == "cureEffect") return "治疗加成";
    else
    {
        cout << type_name_ << endl;
        return type_name_;
    }
}

void Reinforced_Artifact::count_entry_num()
{
    int life_base_value = 14500;
    int atk_base_value = 950;
    int def_base_value = 800;

    for (auto &i: entry)
    {
        if (i.first == "生命值S") entry_num["生命值"] += (i.second / life_base_value) / 0.05;
        else if (i.first == "生命值") entry_num["生命值"] += i.second / 0.05;
        else if (i.first == "攻击力S") entry_num["攻击力"] += (i.second / atk_base_value) / 0.05;
        else if (i.first == "攻击力") entry_num["攻击力"] += i.second / 0.05;
        else if (i.first == "防御力S") entry_num["防御力"] += (i.second / def_base_value) / 0.062;
        else if (i.first == "防御力") entry_num["防御力"] += i.second / 0.062;
        else if (i.first == "元素精通") entry_num["元素精通"] += i.second / 20.0;
        else if (i.first == "元素充能效率") entry_num["元素充能效率"] += i.second / 0.055;
        else if (i.first == "暴击率") entry_num["暴击率"] += i.second / 0.033;
        else if (i.first == "暴击伤害") entry_num["暴击伤害"] += i.second / 0.066;
    }
}