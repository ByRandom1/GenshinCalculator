//
// Created by 陈劭彬 on 2022/8/10.
//

#include "Weapon.h"
#include "Group.h"

Weapon::Weapon(string name_,
               string weapon_type_,
               int atk_,
               string vice_type_,
               double vice_value_,
               int level_,
               vector<Set *> extra_value_)
{
    name = name_;
    weapon_type = weapon_type_;
    atk = atk_;
    vice_type = vice_type_;
    vice_value = vice_value_;
    level = level_;
    extra_value = extra_value_;
}

int Weapon::get_atk()
{ return atk; }

bool Weapon::get_vice(Deployment *data)
{
    if (!(vice_type == "伤害加成" && data->attack_config->condition->ele_type != "物理"))
        data->add_percentage(vice_type, vice_value, (name+"_vice"));
    return true;
}

bool Weapon::get_extra(Deployment *data)
{
    for (auto &i: extra_value)
        if (*data->attack_config->condition <= *i->condition)
            data->add_percentage(i->type, i->value * (0.75 + level * 0.25), (name+"_extra"));

    get_extra_special(data);

    return true;
}