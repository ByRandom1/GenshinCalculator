//
// Created by 陈劭彬 on 2022/8/10.
//

#ifndef GENSHINCALCULATOR_ARTIFACT_H
#define GENSHINCALCULATOR_ARTIFACT_H

#include <iostream>
#include <vector>
#include "Condition.h"

using namespace std;

class Deployment;

class Artifact
{
public:
    string name;
    Set *piece_2;
    Set *piece_4;

    Artifact(string name_,
             Set *piece_2_,
             Set *piece_4_);

    static bool get_main(Deployment *data);

    bool get_extra(Deployment *data, bool if_4_piece) const;

    bool get_extra_special(Deployment *data, bool if_4_piece) const;//在main中实现

    static void modify_useful_attribute(Deployment *data);//在main中实现
};

#endif //GENSHINCALCULATOR_ARTIFACT_H
