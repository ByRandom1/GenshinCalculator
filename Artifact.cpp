//
// Created by 陈劭彬 on 2022/8/10.
//

#include "Artifact.h"
#include "Group.h"

Artifact::Artifact(string name_,
                   string english_name_,
                   Set *piece_2_,
                   Set *piece_4_)
{
    name = name_;
    english_name = english_name_;
    piece_2 = piece_2_;
    piece_4 = piece_4_;
}

bool Artifact::get_main(Deployment *data)
{
    data->add_percentage("生命值", 4780.0 / data->base_life, "main1");
    data->add_percentage("攻击力", 311.0 / data->base_atk, "main2");
    if (data->a_main3 == "生命值") data->add_percentage("生命值", 0.466, "main3");
    else if (data->a_main3 == "攻击力") data->add_percentage("攻击力", 0.466, "main3");
    else if (data->a_main3 == "防御力") data->add_percentage("防御力", 0.583, "main3");
    else if (data->a_main3 == "元素精通") data->add_percentage("元素精通", 187.0, "main3");
    else if (data->a_main3 == "元素充能效率") data->add_percentage("元素充能效率", 0.518, "main3");
    if (data->a_main4 == "生命值") data->add_percentage("生命值", 0.466, "main4");
    else if (data->a_main4 == "攻击力") data->add_percentage("攻击力", 0.466, "main4");
    else if (data->a_main4 == "防御力") data->add_percentage("防御力", 0.583, "main4");
    else if (data->a_main4 == "元素精通") data->add_percentage("元素精通", 187.0, "main4");
    else if (data->a_main4 == "伤害加成")
    {
        if (data->attack_config->condition->ele_type == "物理") data->add_percentage("伤害加成", 0.583, "main4");
        else data->add_percentage("伤害加成", 0.466, "main4");
    }
    if (data->a_main5 == "生命值") data->add_percentage("生命值", 0.466, "main5");
    else if (data->a_main5 == "攻击力") data->add_percentage("攻击力", 0.466, "main5");
    else if (data->a_main5 == "防御力") data->add_percentage("防御力", 0.583, "main5");
    else if (data->a_main5 == "元素精通") data->add_percentage("元素精通", 187.0, "main5");
    else if (data->a_main5 == "暴击率") data->add_percentage("暴击率", 0.311, "main5");
    else if (data->a_main5 == "暴击伤害") data->add_percentage("暴击伤害", 0.622, "main5");
    else if (data->a_main5 == "治疗加成") data->add_percentage("治疗加成", 0.359, "main5");
    return true;
}

bool Artifact::get_extra(Deployment *data, bool if_4_piece) const //二件套效果和四件套效果分开获取
{
    if (!if_4_piece && piece_2 != nullptr && *data->attack_config->condition <= *piece_2->condition)
        data->add_percentage(piece_2->type, piece_2->value, (name + "_piece2"));
    if (if_4_piece && piece_4 != nullptr && *data->attack_config->condition <= *piece_4->condition)
        data->add_percentage(piece_4->type, piece_4->value, (name + "_piece4"));

    get_extra_special(data, if_4_piece);

    return true;
}