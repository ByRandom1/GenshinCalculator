//
// Created by 陈劭彬 on 2022/10/11.
//

#include "Group.h"

Deployment::Deployment(Character *c_point_,
                       Weapon *w_point_,
                       Artifact *suit1_,
                       Artifact *suit2_,
                       string a_main3_,
                       string a_main4_,
                       string a_main5_,
                       Team_config *team_config_,
                       Attack_config *attack_config_)
{
    c_point = c_point_;
    w_point = w_point_;
    suit1 = suit1_;
    suit2 = suit2_;
    a_main3 = a_main3_;
    a_main4 = a_main4_;
    a_main5 = a_main5_;
    team_config = team_config_;
    attack_config = attack_config_;

    base_life = c_point->get_life();
    base_atk = c_point->get_atk() + w_point->get_atk();
    base_def = c_point->get_def();

    if (attack_config->condition->attack_way == "平A")
    {
        base_skillrate = c_point->get_normal_A();
        attack_config->lockface = false;
    }
    else if (attack_config->condition->attack_way == "重A")
    {
        base_skillrate = c_point->get_heavy_A();
        attack_config->lockface = false;
    }
    else if (attack_config->condition->attack_way == "下落A")
    {
        base_skillrate = c_point->get_down_A();
        attack_config->lockface = false;
    }
    else if (attack_config->condition->attack_way == "E")
    {
        base_skillrate = c_point->get_E();
        attack_config->lockface = c_point->E_lockface;
    }
    else if (attack_config->condition->attack_way == "Q")
    {
        base_skillrate = c_point->get_Q();
        attack_config->lockface = c_point->Q_lockface;
    }

    data_list["生命值"] = new attribute(1.0, 0, 0, 0.05, attack_config->useful_attributes["生命值"]);
    data_list["攻击力"] = new attribute(1.0, 0, 0, 0.05, attack_config->useful_attributes["攻击力"]);
    data_list["防御力"] = new attribute(1.0, 0, 0, 0.062, attack_config->useful_attributes["防御力"]);
    data_list["额外倍率"] = new attribute(0, 0, 0, 0, attack_config->useful_attributes["额外倍率"]);
    data_list["元素精通"] = new attribute(0, 0, 0, 20.0, attack_config->useful_attributes["元素精通"]);
    data_list["元素充能效率"] = new attribute(1.0, 0, 0, 0.055, attack_config->useful_attributes["元素充能效率"]);
    data_list["暴击率"] = new attribute(0.05, 0, 0, 0.033, attack_config->useful_attributes["暴击率"]);
    data_list["暴击伤害"] = new attribute(0.5, 0, 0, 0.066, attack_config->useful_attributes["暴击伤害"]);
    data_list["伤害加成"] = new attribute(1, 0, 0, 0, attack_config->useful_attributes["伤害加成"]);
    data_list["抗性削弱"] = new attribute(0, 0, 0, 0, attack_config->useful_attributes["抗性削弱"]);
    data_list["防御削弱"] = new attribute(0, 0, 0, 0, attack_config->useful_attributes["防御削弱"]);
    data_list["防御无视"] = new attribute(0, 0, 0, 0, attack_config->useful_attributes["防御无视"]);
    data_list["治疗加成"] = new attribute(0, 0, 0, 0, attack_config->useful_attributes["治疗加成"]);
    data_list["护盾强效"] = new attribute(0, 0, 0, 0, attack_config->useful_attributes["护盾强效"]);
}

Deployment::~Deployment()
{
    for (auto &i: data_list) delete i.second;
}

void Deployment::init_check_data(bool &suit1_valid, bool &suit2_valid, bool &main3_valid, bool &main4_valid, bool &main5_valid)
{
    //LOG START
    outfile_debug << (attack_config->condition->attack_way + "_" + (attack_config->background ? "后台" : "前台") + "_" + (attack_config->lockface ? "锁面板" : "不锁面板") + "_" + attack_config->react_type + ",");

    map<string, double> prevalue;
    suit1_valid = false;
    suit2_valid = false;
    main3_valid = true;
    main4_valid = true;
    main5_valid = true;

    Weapon::modify_useful_attribute(this);
    Artifact::modify_useful_attribute(this);

    //character get
    c_point->get_break(this);
    c_point->get_extra(this);
    //weapon get
    w_point->get_vice(this);
    w_point->get_extra(this);

    //update
    for (auto &i: data_list) prevalue[i.first] = i.second->percentage;

    //artifact
    if (suit1 == suit2)
    {
        suit1->get_extra(this, true);
        //check
        suit1_valid = suit2_valid = false;
        for (auto &i: data_list)
            if (i.second->useful && i.second->percentage > prevalue[i.first])
            {
                suit1_valid = suit2_valid = true;
                break;
            }
        if (attack_config->condition->attack_way == "Q" && data_list["元素充能效率"]->percentage > prevalue["元素充能效率"]) suit1_valid = suit2_valid = true;

        Artifact::check_artifact_special(this, suit1_valid, suit2_valid, true);

        if (!suit1_valid || !suit2_valid) outfile_debug << (suit1->name + "_piece4_failure,");

        suit2->get_extra(this, false);
    }
    else
    {
        suit1->get_extra(this, false);
        //check
        suit1_valid = false;
        for (auto &i: data_list)
            if (i.second->useful && i.second->percentage > prevalue[i.first])
            {
                suit1_valid = true;
                break;
            }
        if (attack_config->condition->attack_way == "Q" && data_list["元素充能效率"]->percentage > prevalue["元素充能效率"]) suit1_valid = true;
        //update
        for (auto &i: data_list) prevalue[i.first] = i.second->percentage;

        suit2->get_extra(this, false);
        suit2_valid = false;
        for (auto &i: data_list)
            if (i.second->useful && i.second->percentage > prevalue[i.first])
            {
                suit2_valid = true;
                break;
            }
        if (attack_config->condition->attack_way == "Q" && data_list["元素充能效率"]->percentage > prevalue["元素充能效率"]) suit2_valid = true;

        Artifact::check_artifact_special(this, suit1_valid, suit2_valid, false);

        if (!suit1_valid) outfile_debug << (suit1->name + "_piece2_failure,");
        if (!suit2_valid) outfile_debug << (suit2->name + "_piece2_failure,");
    }

    check_useful_attribute();

    //main
    //get
    Artifact::get_main(this);
    //check
    if (!data_list[a_main3]->useful && !(a_main3 == "元素充能效率" && attack_config->condition->attack_way == "Q"))
    {
        outfile_debug << (a_main3 + "_main3_failure,");
        main3_valid = false;
    }
    if (!data_list[a_main4]->useful)
    {
        outfile_debug << (a_main4 + "_main4_failure,");
        main4_valid = false;
    }
    if (!data_list[a_main5]->useful)
    {
        outfile_debug << (a_main5 + "_main5_failure,");
        main5_valid = false;
    }

    get_team_data();

    if (cal_enable_recharge_check) satisfy_recharge_requirement();

    outfile_debug << "\n";
}

bool Deployment::add_percentage(string type_, double value_, string source)
{
    if (data_list.find(type_) != data_list.end())
    {
        data_list[type_]->percentage += value_;
        outfile_debug << (source + "_" + type_ + "_" + to_string(value_) + ",");
        return true;
    }
    else
    {
        outfile_debug << (source + "_" + type_ + "_" + to_string(value_) + "_failure" + ",");
        return false;
    }
}

bool Deployment::add_converted_percentage(string type_, double value_, string source)
{
    if (data_list.find(type_) != data_list.end())
    {
        data_list[type_]->converted_percentage += value_;
        outfile_debug << (source + "_" + type_ + "_" + to_string(value_) + ",");
        return true;
    }
    else
    {
        outfile_debug << (source + "_" + type_ + "_" + to_string(value_) + "_failure" + ",");
        return false;
    }
}

bool Deployment::change_useful_attribute(std::string type_, bool value_, std::string source)
{
    if (data_list.find(type_) != data_list.end())
    {
        data_list[type_]->useful = value_;
        return true;
    }
    else return false;
}

double Deployment::cal_damage(int life_num, int atk_num, int def_num, int mastery_num, int recharge_num, int critrate_num, int critdam_num)
{
    double life = data_list["生命值"]->percentage + data_list["生命值"]->value_per_entry * life_num;
    double atk = data_list["攻击力"]->percentage + data_list["攻击力"]->value_per_entry * atk_num;
    double def = data_list["防御力"]->percentage + data_list["防御力"]->value_per_entry * def_num;
    double mastery = data_list["元素精通"]->percentage + data_list["元素精通"]->value_per_entry * mastery_num;
    double recharge = data_list["元素充能效率"]->percentage + data_list["元素充能效率"]->value_per_entry * recharge_num;
    double critrate = data_list["暴击率"]->percentage + data_list["暴击率"]->value_per_entry * critrate_num;
    double critdam = data_list["暴击伤害"]->percentage + data_list["暴击伤害"]->value_per_entry * critdam_num;
    double damplus = data_list["伤害加成"]->percentage;

    //深渊
    critrate += 0.08;
    critdam += 0.15;

    get_convert_value(life, atk, def, mastery, recharge, critrate, critdam, damplus);

    life += data_list["生命值"]->converted_percentage;
    atk += data_list["攻击力"]->converted_percentage;
    def += data_list["防御力"]->converted_percentage;
    mastery += data_list["元素精通"]->converted_percentage;
    recharge += data_list["元素充能效率"]->converted_percentage;
    critrate += data_list["暴击率"]->converted_percentage;
    critdam += data_list["暴击伤害"]->converted_percentage;
    damplus += data_list["伤害加成"]->converted_percentage;

    double extrarate = (data_list["额外倍率"]->percentage + data_list["额外倍率"]->converted_percentage) * (double) base_atk * atk;
    get_extra_rate_value(life, atk, def, mastery, recharge, critrate, critdam, damplus, extrarate);

    double growrate = 1.0;
    double extra_damage = get_react_value(mastery, extrarate, growrate);

    if (critrate > cal_max_critrate_valid) critrate = 1.0;
    if (critrate < cal_min_critrate_valid) critrate = 0.0;

    double resistence_ratio = data_list["抗性削弱"]->percentage + data_list["抗性削弱"]->converted_percentage;
    if (attack_config->react_type.find("超导") && attack_config->condition->ele_type == "物理") resistence_ratio += 0.4;
    if (0.1 - resistence_ratio >= 0.75) resistence_ratio = 1 / (4 * (0.1 - resistence_ratio) + 1);
    else if (0.1 - resistence_ratio < 0) resistence_ratio = 1 - (0.1 - resistence_ratio) / 2;
    else resistence_ratio = 1 - (0.1 - resistence_ratio);

    double defdec = data_list["防御削弱"]->percentage + data_list["防御削弱"]->converted_percentage;
    double defign = data_list["防御无视"]->percentage + data_list["防御无视"]->converted_percentage;
    double c_level = 90, enemy_level = 90;
    double defence_ratio = (c_level + 100) / (c_level + 100 + (1 - defdec) * (1 - defign) * (enemy_level + 100));

    double damage = ((double) base_atk * atk * base_skillrate + extrarate) * damplus * (1.0 + critrate * critdam) * growrate * resistence_ratio * defence_ratio + extra_damage;

    return damage;
}

Group::Group(Character *c_point_,
             Weapon *w_point_,
             Artifact *suit1_,
             Artifact *suit2_,
             string a_main3_,
             string a_main4_,
             string a_main5_,
             Team_config *team_config_,
             vector<Attack_config *> attack_config_list)
{
    c_point = c_point_;
    w_point = w_point_;
    suit1 = suit1_;
    suit2 = suit2_;
    a_main3 = a_main3_;
    a_main4 = a_main4_;
    a_main5 = a_main5_;
    team_config = team_config_;
    for (auto &i: attack_config_list)
    {
        combination.push_back(new Deployment(c_point_, w_point_, suit1_, suit2_, a_main3_, a_main4_, a_main5_, team_config_, i));
        damage.push_back(0.0);
    }
    entry["生命值"] = new attribute(0, 0, 0, 0.05, false);
    entry["攻击力"] = new attribute(0, 0, 0, 0.05, false);
    entry["防御力"] = new attribute(0, 0, 0, 0.062, false);
    entry["额外倍率"] = new attribute(0, 0, 0, 0, false);
    entry["元素精通"] = new attribute(0, 0, 0, 20.0, false);
    entry["元素充能效率"] = new attribute(0, 0, 0, 0.055, false);
    entry["暴击率"] = new attribute(0, 0, 0, 0.033, false);
    entry["暴击伤害"] = new attribute(0, 0, 0, 0.066, false);
    entry["伤害加成"] = new attribute(0, 0, 0, 0, false);
    entry["抗性削弱"] = new attribute(0, 0, 0, 0, false);
    entry["防御削弱"] = new attribute(0, 0, 0, 0, false);
    entry["防御无视"] = new attribute(0, 0, 0, 0, false);
    entry["治疗加成"] = new attribute(0, 0, 0, 0, false);
    entry["护盾强效"] = new attribute(0, 0, 0, 0, false);
    total_damage = 0.0;
}

int Group::init_check_data()
{
    outfile_debug
            << (c_point->name + "_" + w_point->name + "_" + suit1->name + "_" + suit2->name + "_" + a_main3 + "_" + a_main4 + "_" + a_main5 + "_" + team_config->teammate_all + (team_config->team_weapon_artifact.empty() ? "" : ("_" + team_config->team_weapon_artifact)) + "\n");

    bool suit1_valid = false;
    bool suit2_valid = false;
    bool main3_valid = false;
    bool main4_valid = false;
    bool main5_valid = false;

    bool suit1_valid_;
    bool suit2_valid_;
    bool main3_valid_;
    bool main4_valid_;
    bool main5_valid_;

    for (auto &i: combination)
    {
        i->init_check_data(suit1_valid_, suit2_valid_, main3_valid_, main4_valid_, main5_valid_);
        suit1_valid = suit1_valid || suit1_valid_;
        suit2_valid = suit2_valid || suit2_valid_;
        main3_valid = main3_valid || main3_valid_;
        main4_valid = main4_valid || main4_valid_;
        main5_valid = main5_valid || main5_valid_;
    }

    if (!suit1_valid || !suit2_valid)
    {
        if (!suit1_valid && !suit2_valid)
        {
            if (suit1 == suit2) return 2;
            else return 1;
        }
        else if (!suit1_valid) return 1;
        else return 2;
    }
    else if (!main3_valid) return 3;
    else if (!main4_valid) return 4;
    else if (!main5_valid) return 5;
    else
    {
        for (auto &i: combination)
        {
            entry["生命值"]->entry_num = max(entry["生命值"]->entry_num, i->data_list["生命值"]->entry_num);
            entry["攻击力"]->entry_num = max(entry["攻击力"]->entry_num, i->data_list["攻击力"]->entry_num);
            entry["防御力"]->entry_num = max(entry["防御力"]->entry_num, i->data_list["防御力"]->entry_num);
            entry["元素精通"]->entry_num = max(entry["元素精通"]->entry_num, i->data_list["元素精通"]->entry_num);
            entry["元素充能效率"]->entry_num = max(entry["元素充能效率"]->entry_num, i->data_list["元素充能效率"]->entry_num);
            entry["暴击率"]->entry_num = max(entry["暴击率"]->entry_num, i->data_list["暴击率"]->entry_num);
            entry["暴击伤害"]->entry_num = max(entry["暴击伤害"]->entry_num, i->data_list["暴击伤害"]->entry_num);

            entry["生命值"]->useful = entry["生命值"]->useful || i->data_list["生命值"]->useful;
            entry["攻击力"]->useful = entry["攻击力"]->useful || i->data_list["攻击力"]->useful;
            entry["防御力"]->useful = entry["防御力"]->useful || i->data_list["防御力"]->useful;
            entry["元素精通"]->useful = entry["元素精通"]->useful || i->data_list["元素精通"]->useful;
            entry["元素充能效率"]->useful = entry["元素充能效率"]->useful || i->data_list["元素充能效率"]->useful;
            entry["暴击率"]->useful = entry["暴击率"]->useful || i->data_list["暴击率"]->useful;
            entry["暴击伤害"]->useful = entry["暴击伤害"]->useful || i->data_list["暴击伤害"]->useful;
        }
        return 0;
    }
}

void Group::cal_damage_entry_num()
{
    int totalbase = 20;
    int totalup = 5 * max_up_num_per_base;
    int totalnum = max_entry_num;
    if (suit1 != suit2 && totalnum != 0) totalnum += artifact_2_2_max_entry_bonus;

    int life_pos = 5 - ((a_main3 == "生命值") ? 1 : 0) - ((a_main4 == "生命值") ? 1 : 0) - ((a_main5 == "生命值") ? 1 : 0);
    int atk_pos = 5 - ((a_main3 == "攻击力") ? 1 : 0) - ((a_main4 == "攻击力") ? 1 : 0) - ((a_main5 == "攻击力") ? 1 : 0);
    int def_pos = 5 - ((a_main3 == "防御力") ? 1 : 0) - ((a_main4 == "防御力") ? 1 : 0) - ((a_main5 == "防御力") ? 1 : 0);
    int mastery_pos = 5 - ((a_main3 == "元素精通") ? 1 : 0) - ((a_main4 == "元素精通") ? 1 : 0) - ((a_main5 == "元素精通") ? 1 : 0);
    int recharge_pos = 5 - ((a_main3 == "元素充能效率") ? 1 : 0);
    int critrate_pos = 5 - ((a_main5 == "暴击率") ? 1 : 0);
    int critdam_pos = 5 - ((a_main5 == "暴击伤害") ? 1 : 0);

    int life_num = entry["生命值"]->entry_num;
    int atk_num = entry["攻击力"]->entry_num;
    int def_num = entry["防御力"]->entry_num;
    int mastery_num = entry["元素精通"]->entry_num;
    int recharge_num = entry["元素充能效率"]->entry_num;
    int critrate_num = entry["暴击率"]->entry_num;
    int critdam_num = entry["暴击伤害"]->entry_num;

    int life_base = (entry["生命值"]->useful) ? life_pos : min(life_num, life_pos);
    int atk_base = (entry["攻击力"]->useful) ? atk_pos : min(atk_num, atk_pos);
    int def_base = (entry["防御力"]->useful) ? def_pos : min(def_num, def_pos);
    int mastery_base = (entry["元素精通"]->useful) ? mastery_pos : min(mastery_num, mastery_pos);
    int recharge_base = (entry["元素充能效率"]->useful) ? recharge_pos : min(recharge_num, recharge_pos);
    int critrate_base = (entry["暴击率"]->useful) ? critrate_pos : min(critrate_num, critrate_pos);
    int critdam_base = (entry["暴击伤害"]->useful) ? critdam_pos : min(critdam_num, critdam_pos);

    for (int lifebase = life_base; lifebase >= 0; lifebase--)
        for (int lifeup = min(max_attribute_num_per_pos * life_pos - lifebase, max_up_num_per_base * lifebase); lifeup >= 0; lifeup--)
        {
            if (!entry["生命值"]->useful && (lifebase + lifeup != life_num)) continue;
            if (lifebase + lifeup < life_num) continue;
            int leftbase1 = totalbase - lifebase;
            int leftup1 = totalup - lifeup;
            int leftnum1 = totalnum - lifebase - lifeup;
            if (leftbase1 < 0 || leftup1 < 0 || leftnum1 < 0) continue;

            for (int atkbase = atk_base; atkbase >= 0; atkbase--)
                for (int atkup = min(max_attribute_num_per_pos * atk_pos - atkbase, max_up_num_per_base * atkbase); atkup >= 0; atkup--)
                {
                    if (!entry["攻击力"]->useful && (atkbase + atkup != atk_num)) continue;
                    if (atkbase + atkup < atk_num) continue;
                    int leftbase2 = leftbase1 - atkbase;
                    int leftup2 = leftup1 - atkup;
                    int leftnum2 = leftnum1 - atkbase - atkup;
                    if (leftbase2 < 0 || leftup2 < 0 || leftnum2 < 0) continue;

                    for (int defbase = def_base; defbase >= 0; defbase--)
                        for (int defup = min(max_attribute_num_per_pos * def_pos - defbase, max_up_num_per_base * defbase); defup >= 0; defup--)
                        {
                            if (!entry["防御力"]->useful && (defbase + defup != def_num)) continue;
                            if (defbase + defup < def_num) continue;
                            int leftbase3 = leftbase2 - defbase;
                            int leftup3 = leftup2 - defup;
                            int leftnum3 = leftnum2 - defbase - defup;
                            if (leftbase3 < 0 || leftup3 < 0 || leftnum3 < 0) continue;

                            for (int masterybase = mastery_base; masterybase >= 0; masterybase--)
                                for (int masteryup = min(max_attribute_num_per_pos * mastery_pos - masterybase, max_up_num_per_base * masterybase); masteryup >= 0; masteryup--)
                                {
                                    if (!entry["元素精通"]->useful && (masterybase + masteryup != mastery_num)) continue;
                                    if (masterybase + masteryup < mastery_num) continue;
                                    int leftbase4 = leftbase3 - masterybase;
                                    int leftup4 = leftup3 - masteryup;
                                    int leftnum4 = leftnum3 - masterybase - masteryup;
                                    if (leftbase4 < 0 || leftup4 < 0 || leftnum4 < 0) continue;

                                    for (int rechargebase = recharge_base; rechargebase >= 0; rechargebase--)
                                        for (int rechargeup = min(max_attribute_num_per_pos * recharge_pos - rechargebase, max_up_num_per_base * rechargebase);
                                             rechargeup >= 0; rechargeup--)
                                        {
                                            if (!entry["元素充能效率"]->useful && (rechargebase + rechargeup != recharge_num)) continue;
                                            if (rechargebase + rechargeup < recharge_num) continue;
                                            int leftbase5 = leftbase4 - rechargebase;
                                            int leftup5 = leftup4 - rechargeup;
                                            int leftnum5 = leftnum4 - rechargebase - rechargeup;
                                            if (leftbase5 < 0 || leftup5 < 0 || leftnum5 < 0) continue;

                                            for (int critratebase = critrate_base; critratebase >= 0; critratebase--)
                                                for (int critrateup = min(max_attribute_num_per_pos * critrate_pos - critratebase, max_up_num_per_base * critratebase);
                                                     critrateup >= 0; critrateup--)
                                                {
                                                    if (!entry["暴击率"]->useful && (critratebase + critrateup != critrate_num)) continue;
                                                    if (critratebase + critrateup < critrate_num) continue;
                                                    int leftbase6 = leftbase5 - critratebase;
                                                    int leftup6 = leftup5 - critrateup;
                                                    int leftnum6 = leftnum5 - critratebase - critrateup;
                                                    if (leftbase6 < 0 || leftup6 < 0 || leftnum6 < 0) continue;

                                                    for (int critdambase = critdam_base; critdambase >= 0; critdambase--)
                                                        for (int critdamup = min(max_attribute_num_per_pos * critdam_pos - critdambase, max_up_num_per_base * critdambase);
                                                             critdamup >= 0; critdamup--)
                                                        {
                                                            if (!entry["暴击伤害"]->useful && (critdambase + critdamup != critdam_num)) continue;
                                                            if (critdambase + critdamup < critdam_num) continue;
                                                            int leftbase7 = leftbase6 - critdambase;
                                                            int leftup7 = leftup6 - critdamup;
                                                            int leftnum7 = leftnum6 - critdambase - critdamup;
                                                            if (leftbase7 < 0 || leftup7 < 0 || leftnum7 < 0) continue;

                                                            vector<double> temp_damage;
                                                            double temp_total_damage = 0;
                                                            for (auto &i: combination)
                                                            {
                                                                double temp =
                                                                        i->attack_config->attack_time * i->cal_damage(lifeup + lifebase, atkup + atkbase, defup + defbase, masteryup + masterybase, rechargeup + rechargebase, critrateup + critratebase, critdamup + critdambase);
                                                                temp_damage.push_back(temp);
                                                                temp_total_damage += temp;
                                                            }

                                                            if (temp_total_damage > total_damage)
                                                            {
                                                                entry["生命值"]->entry_num = lifeup + lifebase;
                                                                entry["攻击力"]->entry_num = atkup + atkbase;
                                                                entry["防御力"]->entry_num = defup + defbase;
                                                                entry["元素精通"]->entry_num = masteryup + masterybase;
                                                                entry["元素充能效率"]->entry_num = rechargeup + rechargebase;
                                                                entry["暴击率"]->entry_num = critrateup + critratebase;
                                                                entry["暴击伤害"]->entry_num = critdamup + critdambase;
                                                                damage = temp_damage;
                                                                total_damage = temp_total_damage;
                                                            }
                                                            goto NEXT_ROUND;//第一次得到的critdam一定是最大的数值
                                                        }
                                                    NEXT_ROUND:;
                                                }
                                        }
                                }
                        }
                }
        }
}

void Group::out()
{
    if (out_header)
    {
        outfile_result << "人物名称" << "," << "攻击序列" << "," << "队友信息" << ","
                       << "武器名称" << "," << "圣遗物1" << "," << "圣遗物2" << "," << "3号位" << "," << "4号位" << "," << "5号位" << "," << "期望伤害" << "," << "伤害占比" << ","
                       << "lifenum" << "," << "atknum" << "," << "defnum" << "," << "masterynum" << "," << "rechargenum" << "," << "critratenum" << "," << "critdamnum" << "\n";
        out_header = false;
    }
    outfile_result << c_point->name << ",";
    for (auto &i: combination)
        outfile_result << to_string(i->attack_config->attack_time) + "*" + i->attack_config->condition->attack_way + "_" + (i->attack_config->background ? "后台" : "前台") + "_" + i->attack_config->react_type + "/";
    outfile_result << ","
                   << team_config->teammate_all + (team_config->team_weapon_artifact.empty() ? "" : ("_" + team_config->team_weapon_artifact)) << ","
                   << w_point->name << ","
                   << suit1->name << ","
                   << suit2->name << ","
                   << a_main3 << ","
                   << a_main4 << ","
                   << a_main5 << ","
                   << total_damage << ",";
    for (int i = 0; i < combination.size(); i++)
        outfile_result << combination[i]->attack_config->condition->attack_way + "_" + combination[i]->attack_config->react_type + "(" + to_string((int) (100.0 * damage[i] / total_damage)) + "%)";
    outfile_result << ","
                   << entry["生命值"]->entry_num << ","
                   << entry["攻击力"]->entry_num << ","
                   << entry["防御力"]->entry_num << ","
                   << entry["元素精通"]->entry_num << ","
                   << entry["元素充能效率"]->entry_num << ","
                   << entry["暴击率"]->entry_num << ","
                   << entry["暴击伤害"]->entry_num << "\n";
}