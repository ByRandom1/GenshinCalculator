//
// Created by 陈劭彬 on 2022/11/8.
//

#ifndef GENSHINCALCULATOR_REINFORCED_ARTIFACT_H
#define GENSHINCALCULATOR_REINFORCED_ARTIFACT_H

#include <iostream>

using namespace std;

class Reinforced_Artifact
{
public:
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

    Reinforced_Artifact(int pos_, string suit_name_, string main_type_, string entry1_type_, double entry1_value_, string entry2_type_, double entry2_value_,
                        string entry3_type_, double entry3_value_, string entry4_type_, double entry4_value_, string character_belong_)
    {
        pos = pos_;
        suit_name = suit_name_;
        main_type = main_type_;
        entry1_type = entry1_type_;
        entry1_value = entry1_value_;
        entry2_type = entry2_type_;
        entry2_value = entry2_value_;
        entry3_type = entry3_type_;
        entry3_value = entry3_value_;
        entry4_type = entry4_type_;
        entry4_value = entry4_value_;
        character_belong = character_belong_;
    };
};


#endif //GENSHINCALCULATOR_REINFORCED_ARTIFACT_H
