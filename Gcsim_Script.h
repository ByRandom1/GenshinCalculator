//
// Created by 陈劭彬 on 2022/12/22.
//

#ifndef GENSHINCALCULATOR_GCSIM_SCRIPT_H
#define GENSHINCALCULATOR_GCSIM_SCRIPT_H

#include <iostream>
#include <fstream>
#include "Condition.h"
#include "Character.h"
#include "Weapon.h"
#include "Artifact.h"

extern ofstream outfile_result;

class Build_up
{
public:
    Character *c;
    Weapon *w;
    Artifact *a1;
    Artifact *a2;
    string main3;
    string main4;
    string main5;

    Build_up(Character *c_,
             Weapon *w_,
             Artifact *a1_,
             Artifact *a2_,
             string main3_,
             string main4_,
             string main5_);

    pair<string, string> main_convert(string main);

    void out();
};

#endif //GENSHINCALCULATOR_GCSIM_SCRIPT_H
