//
// Created by 陈劭彬 on 2022/8/10.
//

#ifndef GENSHINCALCULATOR_WEAPON_H
#define GENSHINCALCULATOR_WEAPON_H

#include <iostream>
#include <vector>
#include "Condition.h"

using namespace std;

class Deployment;

class Weapon
{
public:
    string name;
    string english_name;
    string weapon_type;
    int atk;
    string vice_type;
    double vice_value;
    int level;
    vector<Set *> extra_value;

    Weapon(string name_,
           string english_name_,
           string weapon_type_,
           int atk_,
           string vice_type_,
           double vice_value_,
           int level_,
           vector<Set *> extra_value_);

    int get_atk();

    bool get_vice(Deployment *data);

    bool get_extra(Deployment *data);

    bool get_extra_special(Deployment *data) const;//在main中实现

    static void modify_useful_attribute(Deployment *data);//在main中实现
};

#endif //GENSHINCALCULATOR_WEAPON_H
