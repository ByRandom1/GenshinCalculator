//
// Created by 陈劭彬 on 2022/8/10.
//

#ifndef GENSHINCALCULATOR_CONDITION_H
#define GENSHINCALCULATOR_CONDITION_H

#include <iostream>

using namespace std;

class Condition
{
public:
    string ele_type;
    string weapon_type;
    string attack_way;

    Condition(string ele_type_, string weapon_type_, string attack_way_)
    {
        ele_type = ele_type_;
        weapon_type = weapon_type_;
        attack_way = attack_way_;
    }

    bool operator<=(const Condition &other) const
    {
        if (other.ele_type == "ALL" || other.ele_type.find(ele_type) != string::npos)
            if (other.weapon_type == "ALL" || other.weapon_type.find(weapon_type) != string::npos)
                if (other.attack_way == "ALL" || other.attack_way.find(attack_way) != string::npos)
                    return true;
        return false;
    }

    bool operator>=(const Condition &other) const
    {
        if (ele_type == "ALL" || ele_type.find(other.ele_type) != string::npos)
            if (weapon_type == "ALL" || weapon_type.find(other.weapon_type) != string::npos)
                if (attack_way == "ALL" || attack_way.find(other.attack_way) != string::npos)
                    return true;
        return false;
    }

    bool operator==(const Condition &other) const
    {
        if (ele_type == other.ele_type)
            if (weapon_type == other.weapon_type)
                if (attack_way == other.attack_way)
                    return true;
        return false;
    }
//
//    bool operator!=(const Condition &other) const
//    {
//        return !(*this == other);
//    }
//
//    bool operator<(const Condition &other) const
//    {
//        if (*this <= other && *this != other) return true;
//        return false;
//    }
//
//    bool operator>(const Condition &other) const
//    {
//        if (*this >= other && *this != other) return true;
//        return false;
//    }
};

struct Set
{
    Condition *condition;
    string type;
    double value;

    Set(Condition *condition_, string type_, double value_)
    {
        condition = condition_;
        type = type_;
        value = value_;
    }
};

#endif //GENSHINCALCULATOR_CONDITION_H
