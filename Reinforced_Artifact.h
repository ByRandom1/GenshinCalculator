//
// Created by 陈劭彬 on 2022/11/8.
//

#ifndef GENSHINCALCULATOR_REINFORCED_ARTIFACT_H
#define GENSHINCALCULATOR_REINFORCED_ARTIFACT_H

#include <iostream>
#include <vector>
#include <map>

using namespace std;

class Reinforced_Artifact
{
public:
    string suit_name;
    string main_type;
    vector<pair<string, double>> entry;
    map<string, double> entry_num;

    Reinforced_Artifact(string suit_name_,
                        string main_type_,
                        string entry1_type_,
                        double entry1_value_,
                        string entry2_type_,
                        double entry2_value_,
                        string entry3_type_,
                        double entry3_value_,
                        string entry4_type_,
                        double entry4_value_);

    static string suit_name_convert(string suit_name_);

    static string type_name_convert(string type_name_);

    void count_entry_num();
};


#endif //GENSHINCALCULATOR_REINFORCED_ARTIFACT_H
