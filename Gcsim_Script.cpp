//
// Created by 陈劭彬 on 2022/12/22.
//

#include "Gcsim_Script.h"

Build_up::Build_up(Character *c_,
                   Weapon *w_,
                   Artifact *a1_,
                   Artifact *a2_,
                   string main3_,
                   string main4_,
                   string main5_)
{
    c = c_;
    w = w_;
    a1 = a1_;
    a2 = a2_;
    main3 = main3_;
    main4 = main4_;
    main5 = main5_;
}

pair<string, string> Build_up::main_convert(std::string main)
{
    if (main == "生命值") return make_pair("hp%", "0.466");
    if (main == "攻击力") return make_pair("atk%", "0.466");
    if (main == "防御力") return make_pair("def%", "0.583");
    if (main == "元素充能效率") return make_pair("er", "0.518");
    if (main == "元素精通") return make_pair("em", "187");
    if (main == "暴击率") return make_pair("cr", "0.311");
    if (main == "暴击伤害") return make_pair("cd", "0.622");
    if (main == "治疗加成") return make_pair("heal", "0.359");
    if (main == "伤害加成" && c->ele_type == "火") return make_pair("pyro%", "0.466");
    if (main == "伤害加成" && c->ele_type == "水") return make_pair("hydro%", "0.466");
    if (main == "伤害加成" && c->ele_type == "雷") return make_pair("electro%", "0.466");
    if (main == "伤害加成" && c->ele_type == "冰") return make_pair("cryo%", "0.466");
    if (main == "伤害加成" && c->ele_type == "风") return make_pair("anemo%", "0.466");
    if (main == "伤害加成" && c->ele_type == "岩") return make_pair("geo%", "0.466");
    if (main == "伤害加成" && c->ele_type == "草") return make_pair("dendro%", "0.466");
    if (main == "物理元素伤害加成" && c->ele_type == "物理") return make_pair("phys%", "0.583");
    return make_pair("", "");
}

void Build_up::out()
{
    outfile_result << "#" << c->name << " " << w->name << " " << a1->name << " " << a2->name << " " << main3 << " " << main4 << " " << main5 << endl;

    outfile_result << c->english_name << " char lvl=90/90 cons=" << to_string(c->constellation)
                   << " talent=" << to_string(c->A_level) << "," << to_string((c->E_level > 10) ? (c->E_level - 3) : c->E_level) << "," << to_string((c->Q_level > 10) ? (c->Q_level - 3) : c->Q_level) << ";" << endl;
    outfile_result << c->english_name << " add weapon=\"" << w->english_name << "\" refine=" << to_string(w->level) << " lvl=90/90;" << endl;
    if (a1 == a2)
    {
        outfile_result << c->english_name << " add set=\"" << a1->english_name << "\" count=4;" << endl;
    }
    else
    {
        outfile_result << c->english_name << " add set=\"" << a1->english_name << "\" count=2;" << endl;
        outfile_result << c->english_name << " add set=\"" << a2->english_name << "\" count=2;" << endl;
    }
    outfile_result << c->english_name << " add stats hp=4780 atk=311 "
                   << main_convert(main3).first << "=" << main_convert(main3).second << " "
                   << main_convert(main4).first << "=" << main_convert(main4).second << " "
                   << main_convert(main5).first << "=" << main_convert(main5).second << ";" << endl;
    outfile_result << endl;
}