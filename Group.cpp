//
// Created by 陈劭彬 on 2022/10/11.
//

#include "Group.h"

int str2index(string type_)
{
    if (type_ == "生命值") return 0;
    else if (type_ == "攻击力") return 1;
    else if (type_ == "防御力") return 2;
    else if (type_ == "额外倍率") return 3;
    else if (type_ == "元素精通") return 4;
    else if (type_ == "元素充能效率") return 5;
    else if (type_ == "暴击率") return 6;
    else if (type_ == "暴击伤害") return 7;
    else if (type_ == "伤害加成") return 8;
    else if (type_ == "抗性削弱") return 9;
    else if (type_ == "防御削弱") return 10;
    else if (type_ == "防御无视") return 11;
    else if (type_ == "治疗加成") return 12;
    else if (type_ == "护盾强效") return 13;
    else return -1;
}

Deployment::Deployment(Character *c_point_,
                       Weapon *w_point_,
                       Artifact *suit1_,
                       Artifact *suit2_,
                       string a_main3_,
                       string a_main4_,
                       string a_main5_,
                       Team_config *team_config_,
                       Attack_config *attack_config_,
                       bool need_to_satisfy_recharge_)
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
    need_to_satisfy_recharge = need_to_satisfy_recharge_;

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

    get_data_info = "";
    min_recharge_num = 0;

    data_list[0] = new attribute(0, 0, 0.05, attack_config->useful_attributes[0]);
    data_list[1] = new attribute(0, 0, 0.05, attack_config->useful_attributes[1]);
    data_list[2] = new attribute(0, 0, 0.062, attack_config->useful_attributes[2]);
    data_list[3] = new attribute(0, 0, 0, attack_config->useful_attributes[3]);
    data_list[4] = new attribute(0, 0, 20.0, attack_config->useful_attributes[4]);
    data_list[5] = new attribute(0, 0, 0.055, attack_config->useful_attributes[5]);
    data_list[6] = new attribute(0, 0, 0.033, attack_config->useful_attributes[6]);
    data_list[7] = new attribute(0, 0, 0.066, attack_config->useful_attributes[7]);
    data_list[8] = new attribute(0, 0, 0, attack_config->useful_attributes[8]);
    data_list[9] = new attribute(0, 0, 0, attack_config->useful_attributes[9]);
    data_list[10] = new attribute(0, 0, 0, attack_config->useful_attributes[10]);
    data_list[11] = new attribute(0, 0, 0, attack_config->useful_attributes[11]);
    data_list[12] = new attribute(0, 0, 0, attack_config->useful_attributes[12]);
    data_list[13] = new attribute(0, 0, 0, attack_config->useful_attributes[13]);
}

Deployment::~Deployment()
{
    for (auto &i: data_list) delete i;
}

void Deployment::check_data(bool &suit1_valid, bool &suit2_valid, bool &main3_valid, bool &main4_valid, bool &main5_valid)
{
    suit1_valid = suit2_valid = false;
    main3_valid = main4_valid = main5_valid = true;

    Weapon::modify_useful_attribute(this);
    Artifact::modify_useful_attribute(this);

    if (suit1 == suit2)
    {
        suit1->get_extra(this, true);
        //check
        suit1_valid = suit2_valid = false;
        for (auto &i: data_list)
            if (i->useful && (i->percentage + i->converted_percentage) > 0)
            {
                suit1_valid = suit2_valid = true;
                break;
            }

        if (need_to_satisfy_recharge)
            if ((data_list[5]->percentage + data_list[5]->converted_percentage) > 0)
                suit1_valid = suit2_valid = true;

        Artifact::check_artifact_special(this, suit1_valid, suit2_valid, true);
    }
    else
    {
        suit1->get_extra(this, false);
        //check
        suit1_valid = false;
        for (auto &i: data_list)
            if (i->useful && (i->percentage + i->converted_percentage) > 0)
            {
                suit1_valid = true;
                break;
            }

        if (need_to_satisfy_recharge)
            if ((data_list[5]->percentage + data_list[5]->converted_percentage) > 0)
                suit1_valid = true;
        //clear
        for (auto &i: data_list) i->percentage = i->converted_percentage = 0;

        suit2->get_extra(this, false);
        suit2_valid = false;
        for (auto &i: data_list)
            if (i->useful && (i->percentage + i->converted_percentage) > 0)
            {
                suit2_valid = true;
                break;
            }

        if (need_to_satisfy_recharge)
            if ((data_list[5]->percentage + data_list[5]->converted_percentage) > 0)
                suit2_valid = true;

        Artifact::check_artifact_special(this, suit1_valid, suit2_valid, false);
    }

    check_useful_attribute();

    if (!data_list[str2index(a_main3)]->useful && !(need_to_satisfy_recharge && a_main3 == "元素充能效率")) main3_valid = false;
    if (!data_list[str2index(a_main4)]->useful) main4_valid = false;
    if (!data_list[str2index(a_main5)]->useful) main5_valid = false;
}

void Deployment::init_data()
{
    get_data_info = "";
    min_recharge_num = 0;

    delete data_list[0];
    data_list[0] = new attribute(1.0, 0, 0.05, attack_config->useful_attributes[0]);
    delete data_list[1];
    data_list[1] = new attribute(1.0, 0, 0.05, attack_config->useful_attributes[1]);
    delete data_list[2];
    data_list[2] = new attribute(1.0, 0, 0.062, attack_config->useful_attributes[2]);
    delete data_list[3];
    data_list[3] = new attribute(0, 0, 0, attack_config->useful_attributes[3]);
    delete data_list[4];
    data_list[4] = new attribute(0, 0, 20.0, attack_config->useful_attributes[4]);
    delete data_list[5];
    data_list[5] = new attribute(1.0, 0, 0.055, attack_config->useful_attributes[5]);
    delete data_list[6];
    data_list[6] = new attribute(0.05, 0, 0.033, attack_config->useful_attributes[6]);
    delete data_list[7];
    data_list[7] = new attribute(0.5, 0, 0.066, attack_config->useful_attributes[7]);
    delete data_list[8];
    data_list[8] = new attribute(1, 0, 0, attack_config->useful_attributes[8]);
    delete data_list[9];
    data_list[9] = new attribute(0, 0, 0, attack_config->useful_attributes[9]);
    delete data_list[10];
    data_list[10] = new attribute(0, 0, 0, attack_config->useful_attributes[10]);
    delete data_list[11];
    data_list[11] = new attribute(0, 0, 0, attack_config->useful_attributes[11]);
    delete data_list[12];
    data_list[12] = new attribute(0, 0, 0, attack_config->useful_attributes[12]);
    delete data_list[13];
    data_list[13] = new attribute(0, 0, 0, attack_config->useful_attributes[13]);

    //LOG START
    get_data_info += (attack_config->condition->attack_way + "_" + (attack_config->background ? "后台" : "前台") + "_" + (attack_config->lockface ? "锁面板" : "不锁面板") + "_" + attack_config->react_type + ",");

    Weapon::modify_useful_attribute(this);
    Artifact::modify_useful_attribute(this);

    //character get
    c_point->get_break(this);
    c_point->get_extra(this);
    //weapon get
    w_point->get_vice(this);
    w_point->get_extra(this);
    //artifact get
    suit1->get_extra(this, suit1 == suit2);
    suit2->get_extra(this, false);

    check_useful_attribute();

    Artifact::get_main(this);

    get_team_data();

    if (need_to_satisfy_recharge) satisfy_recharge_requirement();

    get_data_info += "\n";
}

void Deployment::add_percentage(string type_, double value_, string source)
{
    data_list[str2index(type_)]->percentage += value_;
    get_data_info += (source + "_" + type_ + "_" + to_string(value_) + ",");
}

void Deployment::add_converted_percentage(string type_, double value_, string source)
{
    data_list[str2index(type_)]->converted_percentage += value_;
    get_data_info += (source + "_" + type_ + "_" + to_string(value_) + ",");
}

double Deployment::cal_damage(double life_value, double atk_value, double def_value, double mastery_value, double recharge_value, double critrate_value, double critdam_value)
{
    double life = data_list[0]->percentage + life_value;
    double atk = data_list[1]->percentage + atk_value;
    double def = data_list[2]->percentage + def_value;
    double mastery = data_list[4]->percentage + mastery_value;
    double recharge = data_list[5]->percentage + recharge_value;
    double critrate = data_list[6]->percentage + critrate_value;
    double critdam = data_list[7]->percentage + critdam_value;
    double damplus = data_list[8]->percentage;

    //深渊
    critrate += 0.08;
    critdam += 0.15;

    get_convert_value(life, atk, def, mastery, recharge, critrate, critdam, damplus);

    life += data_list[0]->converted_percentage;
    atk += data_list[1]->converted_percentage;
    def += data_list[2]->converted_percentage;
    mastery += data_list[4]->converted_percentage;
    recharge += data_list[5]->converted_percentage;
    critrate += data_list[6]->converted_percentage;
    critdam += data_list[7]->converted_percentage;
    damplus += data_list[8]->converted_percentage;

    double extrarate = (data_list[3]->percentage + data_list[3]->converted_percentage) * (double) base_atk * atk;
    get_extra_rate_value(life, atk, def, mastery, recharge, critrate, critdam, damplus, extrarate);

    double growrate = 1.0;
    double extra_damage = 0.0;
    get_react_value(mastery, extrarate, growrate, extra_damage);

    if (critrate > 1.0) critrate = 1.0;
    if (critrate < 0.0) critrate = 0.0;

    double resistence_ratio = data_list[9]->percentage + data_list[9]->converted_percentage;
    if (attack_config->react_type.find("超导") && attack_config->condition->ele_type == "物理") resistence_ratio += 0.4;
    if (0.1 - resistence_ratio >= 0.75) resistence_ratio = 1 / (4 * (0.1 - resistence_ratio) + 1);
    else if (0.1 - resistence_ratio < 0) resistence_ratio = 1 - (0.1 - resistence_ratio) / 2;
    else resistence_ratio = 1 - (0.1 - resistence_ratio);

    double defdec = data_list[10]->percentage + data_list[10]->converted_percentage;
    double defign = data_list[11]->percentage + data_list[11]->converted_percentage;
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
             vector<Attack_config *> attack_config_list,
             bool need_to_satisfy_recharge_)
{
    c_point = c_point_;
    w_point = w_point_;
    suit1 = suit1_;
    suit2 = suit2_;
    a_main3 = a_main3_;
    a_main4 = a_main4_;
    a_main5 = a_main5_;
    team_config = team_config_;
    need_to_satisfy_recharge = need_to_satisfy_recharge_;
    for (auto &i: attack_config_list)
        combination.push_back(new Deployment(c_point_, w_point_, suit1_, suit2_, a_main3_, a_main4_, a_main5_, team_config_, i, need_to_satisfy_recharge_));

    for (int i = 0; i < 7; ++i)
    {
        useful[i] = false;
        entry[i] = 0;
    }
    temp_damage = new double[combination.size()]();
    damage = new double[combination.size()]();
    total_damage = 0.0;
}

Group::~Group()
{
    for (auto &i: combination) delete i;
    delete temp_damage;
    delete damage;
}

int Group::init_check_data()
{
    outfile_debug << (c_point->name + "_" + w_point->name + "_" + suit1->name + "_" + suit2->name + "_" + a_main3 + "_" + a_main4 + "_" + a_main5 + "_")
                  << (team_config->teammate_all + "_" + team_config->team_weapon_artifact) + ",";

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
        i->check_data(suit1_valid_, suit2_valid_, main3_valid_, main4_valid_, main5_valid_);
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
            if (suit1 == suit2)
            {
                outfile_debug << suit1->name + "--piece4_failure\n";
                return 2;
            }
            else
            {
                outfile_debug << suit1->name + "--piece2_failure\n";
                return 1;
            }
        }
        else if (!suit1_valid)
        {
            outfile_debug << suit1->name + "--piece2_failure\n";
            return 1;
        }
        else
        {
            outfile_debug << suit2->name + "--piece2_failure\n";
            return 2;
        }
    }
    else if (!main3_valid)
    {
        outfile_debug << "main3--failure\n";
        return 3;
    }
    else if (!main4_valid)
    {
        outfile_debug << "main4--failure\n";
        return 4;
    }
    else if (!main5_valid)
    {
        outfile_debug << "main5--failure\n";
        return 5;
    }

    outfile_debug << "success!\n";

    for (auto &i: combination)
    {
        i->init_data();
        outfile_debug << i->get_data_info;
        useful[0] = useful[0] || i->data_list[0]->useful;
        useful[1] = useful[1] || i->data_list[1]->useful;
        useful[2] = useful[2] || i->data_list[2]->useful;
        useful[3] = useful[3] || i->data_list[4]->useful;
        useful[4] = useful[4] || i->data_list[5]->useful;
        useful[5] = useful[5] || i->data_list[6]->useful;
        useful[6] = useful[6] || i->data_list[7]->useful;
        if (need_to_satisfy_recharge) entry[4] = max(entry[4], i->min_recharge_num);
    }
    return 0;
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

    int life_num = entry[0];
    int atk_num = entry[1];
    int def_num = entry[2];
    int mastery_num = entry[3];
    int recharge_num = entry[4];
    int critrate_num = entry[5];
    int critdam_num = entry[6];

    int life_base = (useful[0]) ? life_pos : min(life_num, life_pos);
    int atk_base = (useful[1]) ? atk_pos : min(atk_num, atk_pos);
    int def_base = (useful[2]) ? def_pos : min(def_num, def_pos);
    int mastery_base = (useful[3]) ? mastery_pos : min(mastery_num, mastery_pos);
    int recharge_base = (useful[4]) ? recharge_pos : min(recharge_num, recharge_pos);
    int critrate_base = (useful[5]) ? critrate_pos : min(critrate_num, critrate_pos);
    int critdam_base = (useful[6]) ? critdam_pos : min(critdam_num, critdam_pos);

    for (int lifebase = life_base; lifebase >= 0; lifebase--)
        for (int lifeup = min(max_attribute_num_per_pos * life_pos - lifebase, max_up_num_per_base * lifebase); lifeup >= 0; lifeup--)
        {
            if (!useful[0] && (lifebase + lifeup != life_num)) continue;
            if (lifebase + lifeup < life_num) continue;
            int leftbase1 = totalbase - lifebase;
            int leftup1 = totalup - lifeup;
            int leftnum1 = totalnum - lifebase - lifeup;
            if (leftbase1 < 0 || leftup1 < 0 || leftnum1 < 0) continue;

            for (int atkbase = atk_base; atkbase >= 0; atkbase--)
                for (int atkup = min(max_attribute_num_per_pos * atk_pos - atkbase, max_up_num_per_base * atkbase); atkup >= 0; atkup--)
                {
                    if (!useful[1] && (atkbase + atkup != atk_num)) continue;
                    if (atkbase + atkup < atk_num) continue;
                    int leftbase2 = leftbase1 - atkbase;
                    int leftup2 = leftup1 - atkup;
                    int leftnum2 = leftnum1 - atkbase - atkup;
                    if (leftbase2 < 0 || leftup2 < 0 || leftnum2 < 0) continue;

                    for (int defbase = def_base; defbase >= 0; defbase--)
                        for (int defup = min(max_attribute_num_per_pos * def_pos - defbase, max_up_num_per_base * defbase); defup >= 0; defup--)
                        {
                            if (!useful[2] && (defbase + defup != def_num)) continue;
                            if (defbase + defup < def_num) continue;
                            int leftbase3 = leftbase2 - defbase;
                            int leftup3 = leftup2 - defup;
                            int leftnum3 = leftnum2 - defbase - defup;
                            if (leftbase3 < 0 || leftup3 < 0 || leftnum3 < 0) continue;

                            for (int masterybase = mastery_base; masterybase >= 0; masterybase--)
                                for (int masteryup = min(max_attribute_num_per_pos * mastery_pos - masterybase, max_up_num_per_base * masterybase); masteryup >= 0; masteryup--)
                                {
                                    if (!useful[3] && (masterybase + masteryup != mastery_num)) continue;
                                    if (masterybase + masteryup < mastery_num) continue;
                                    int leftbase4 = leftbase3 - masterybase;
                                    int leftup4 = leftup3 - masteryup;
                                    int leftnum4 = leftnum3 - masterybase - masteryup;
                                    if (leftbase4 < 0 || leftup4 < 0 || leftnum4 < 0) continue;

                                    for (int rechargebase = recharge_base; rechargebase >= 0; rechargebase--)
                                        for (int rechargeup = min(max_attribute_num_per_pos * recharge_pos - rechargebase, max_up_num_per_base * rechargebase);
                                             rechargeup >= 0; rechargeup--)
                                        {
                                            if (!useful[4] && (rechargebase + rechargeup != recharge_num)) continue;
                                            if (rechargebase + rechargeup < recharge_num) continue;
                                            int leftbase5 = leftbase4 - rechargebase;
                                            int leftup5 = leftup4 - rechargeup;
                                            int leftnum5 = leftnum4 - rechargebase - rechargeup;
                                            if (leftbase5 < 0 || leftup5 < 0 || leftnum5 < 0) continue;

                                            for (int critratebase = critrate_base; critratebase >= 0; critratebase--)
                                                for (int critrateup = min(max_attribute_num_per_pos * critrate_pos - critratebase, max_up_num_per_base * critratebase);
                                                     critrateup >= 0; critrateup--)
                                                {
                                                    if (!useful[5] && (critratebase + critrateup != critrate_num)) continue;
                                                    if (critratebase + critrateup < critrate_num) continue;
                                                    int leftbase6 = leftbase5 - critratebase;
                                                    int leftup6 = leftup5 - critrateup;
                                                    int leftnum6 = leftnum5 - critratebase - critrateup;
                                                    if (leftbase6 < 0 || leftup6 < 0 || leftnum6 < 0) continue;

                                                    for (int critdambase = critdam_base; critdambase >= 0; critdambase--)
                                                        for (int critdamup = min(max_attribute_num_per_pos * critdam_pos - critdambase, max_up_num_per_base * critdambase);
                                                             critdamup >= 0; critdamup--)
                                                        {
                                                            if (!useful[6] && (critdambase + critdamup != critdam_num)) continue;
                                                            if (critdambase + critdamup < critdam_num) continue;
                                                            int leftbase7 = leftbase6 - critdambase;
                                                            int leftup7 = leftup6 - critdamup;
                                                            int leftnum7 = leftnum6 - critdambase - critdamup;
                                                            if (leftbase7 < 0 || leftup7 < 0 || leftnum7 < 0) continue;

                                                            double temp_total_damage = 0;
                                                            for (int i = 0; i < combination.size(); ++i)
                                                            {
                                                                temp_damage[i] = combination[i]->attack_config->attack_time *
                                                                                 combination[i]->cal_damage((lifeup + lifebase) * combination[0]->data_list[0]->value_per_entry, (atkup + atkbase) * combination[0]->data_list[1]->value_per_entry,
                                                                                                            (defup + defbase) * combination[0]->data_list[2]->value_per_entry, (masteryup + masterybase) * combination[0]->data_list[4]->value_per_entry,
                                                                                                            (rechargeup + rechargebase) * combination[0]->data_list[5]->value_per_entry, (critrateup + critratebase) * combination[0]->data_list[6]->value_per_entry,
                                                                                                            (critdamup + critdambase) * combination[0]->data_list[7]->value_per_entry);

                                                                //TODO:TEST
                                                                if (combination[i]->w_point->name.find("祭礼") != string::npos && combination[i]->attack_config->condition->attack_way == "E" && !combination[i]->c_point->args->sustain_E_hit)
                                                                {
                                                                    if (double_E_per_round.find(combination[i]->c_point->name) != string::npos) temp_damage[i] = temp_damage[i] * 3 / 2;
                                                                    else temp_damage[i] = temp_damage[i] * 2 / 1;
                                                                }
                                                                temp_total_damage += temp_damage[i];
                                                            }

                                                            if (temp_total_damage > total_damage)
                                                            {
                                                                entry[0] = lifeup + lifebase;
                                                                entry[1] = atkup + atkbase;
                                                                entry[2] = defup + defbase;
                                                                entry[3] = masteryup + masterybase;
                                                                entry[4] = rechargeup + rechargebase;
                                                                entry[5] = critrateup + critratebase;
                                                                entry[6] = critdamup + critdambase;
                                                                for (int i = 0; i < combination.size(); ++i) damage[i] = temp_damage[i];
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
    outfile_result << (need_to_satisfy_recharge ? "要求循环" : "不要求循环") << ","
                   << team_config->teammate_all + "_" + team_config->team_weapon_artifact << ","
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
                   << entry[0] << ","
                   << entry[1] << ","
                   << entry[2] << ","
                   << entry[3] << ","
                   << entry[4] << ","
                   << entry[5] << ","
                   << entry[6] << "\n";
}

int Group::init_check_assigned_artifact()
{
    outfile_debug << (c_point->name + "_" + w_point->name + "_" + suit1->name + "_" + suit2->name + "_" + a_main3 + "_" + a_main4 + "_" + a_main5 + "_")
                  << (team_config->teammate_all + "_" + team_config->team_weapon_artifact) + ",";

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
        i->check_data(suit1_valid_, suit2_valid_, main3_valid_, main4_valid_, main5_valid_);
        suit1_valid = suit1_valid || suit1_valid_;
        suit2_valid = suit2_valid || suit2_valid_;
        main3_valid = main3_valid || main3_valid_;
        main4_valid = main4_valid || main4_valid_;
        main5_valid = main5_valid || main5_valid_;
    }

    if (!main3_valid)
    {
        outfile_debug << "main3--failure\n";
        return 3;
    }
    else if (!main4_valid)
    {
        outfile_debug << "main4--failure\n";
        return 4;
    }
    else if (!main5_valid)
    {
        outfile_debug << "main5--failure\n";
        return 5;
    }

    outfile_debug << "success!\n";

    for (auto &i: combination)
    {
        i->init_data();
//        outfile_debug << i->get_data_info;
        useful[0] = useful[0] || i->data_list[0]->useful;
        useful[1] = useful[1] || i->data_list[1]->useful;
        useful[2] = useful[2] || i->data_list[2]->useful;
        useful[3] = useful[3] || i->data_list[4]->useful;
        useful[4] = useful[4] || i->data_list[5]->useful;
        useful[5] = useful[5] || i->data_list[6]->useful;
        useful[6] = useful[6] || i->data_list[7]->useful;
        if (need_to_satisfy_recharge) entry[4] = max(entry[4], i->min_recharge_num);
    }
    return 0;
}

void Group::cal_assigned_artifact_damage()
{
    double life_value = 0, atk_value = 0, def_value = 0, mastery_value = 0, recharge_value = 0, critrate_value = 0, critdam_value = 0;
    for (auto &i: data)
    {
        if (i->entry1_type == "生命值") life_value += i->entry1_value / combination[0]->base_life;
        else if (i->entry1_type == "生命值%") life_value += i->entry1_value;
        else if (i->entry1_type == "攻击力") atk_value += i->entry1_value / combination[0]->base_atk;
        else if (i->entry1_type == "攻击力%") atk_value += i->entry1_value;
        else if (i->entry1_type == "防御力") def_value += i->entry1_value / combination[0]->base_def;
        else if (i->entry1_type == "防御力%") def_value += i->entry1_value;
        else if (i->entry1_type == "元素精通") mastery_value += i->entry1_value;
        else if (i->entry1_type == "元素充能效率") recharge_value += i->entry1_value;
        else if (i->entry1_type == "暴击率") critrate_value += i->entry1_value;
        else if (i->entry1_type == "暴击伤害") critdam_value += i->entry1_value;

        if (i->entry2_type == "生命值") life_value += i->entry2_value / combination[0]->base_life;
        else if (i->entry2_type == "生命值%") life_value += i->entry2_value;
        else if (i->entry2_type == "攻击力") atk_value += i->entry2_value / combination[0]->base_atk;
        else if (i->entry2_type == "攻击力%") atk_value += i->entry2_value;
        else if (i->entry2_type == "防御力") def_value += i->entry2_value / combination[0]->base_def;
        else if (i->entry2_type == "防御力%") def_value += i->entry2_value;
        else if (i->entry2_type == "元素精通") mastery_value += i->entry2_value;
        else if (i->entry2_type == "元素充能效率") recharge_value += i->entry2_value;
        else if (i->entry2_type == "暴击率") critrate_value += i->entry2_value;
        else if (i->entry2_type == "暴击伤害") critdam_value += i->entry2_value;

        if (i->entry3_type == "生命值") life_value += i->entry3_value / combination[0]->base_life;
        else if (i->entry3_type == "生命值%") life_value += i->entry3_value;
        else if (i->entry3_type == "攻击力") atk_value += i->entry3_value / combination[0]->base_atk;
        else if (i->entry3_type == "攻击力%") atk_value += i->entry3_value;
        else if (i->entry3_type == "防御力") def_value += i->entry3_value / combination[0]->base_def;
        else if (i->entry3_type == "防御力%") def_value += i->entry3_value;
        else if (i->entry3_type == "元素精通") mastery_value += i->entry3_value;
        else if (i->entry3_type == "元素充能效率") recharge_value += i->entry3_value;
        else if (i->entry3_type == "暴击率") critrate_value += i->entry3_value;
        else if (i->entry3_type == "暴击伤害") critdam_value += i->entry3_value;

        if (i->entry4_type == "生命值") life_value += i->entry4_value / combination[0]->base_life;
        else if (i->entry4_type == "生命值%") life_value += i->entry4_value;
        else if (i->entry4_type == "攻击力") atk_value += i->entry4_value / combination[0]->base_atk;
        else if (i->entry4_type == "攻击力%") atk_value += i->entry4_value;
        else if (i->entry4_type == "防御力") def_value += i->entry4_value / combination[0]->base_def;
        else if (i->entry4_type == "防御力%") def_value += i->entry4_value;
        else if (i->entry4_type == "元素精通") mastery_value += i->entry4_value;
        else if (i->entry4_type == "元素充能效率") recharge_value += i->entry4_value;
        else if (i->entry4_type == "暴击率") critrate_value += i->entry4_value;
        else if (i->entry4_type == "暴击伤害") critdam_value += i->entry4_value;
    }

    for (int i = 0; i < combination.size(); ++i)
    {
        damage[i] = combination[i]->attack_config->attack_time * combination[i]->cal_damage(life_value, atk_value, def_value, mastery_value, recharge_value, critrate_value, critdam_value);

        //TODO:TEST
        if (combination[i]->w_point->name.find("祭礼") != string::npos && combination[i]->attack_config->condition->attack_way == "E" && !combination[i]->c_point->args->sustain_E_hit)
        {
            if (double_E_per_round.find(combination[i]->c_point->name) != string::npos) damage[i] = damage[i] * 3 / 2;
            else damage[i] = damage[i] * 2 / 1;
        }
        total_damage += damage[i];
    }
}

void Group::out_assigned_artifact()
{
    if (out_header)
    {
        outfile_result << "人物名称" << "," << "攻击序列" << "," << "队友信息" << "," << "期望伤害" << "," << "伤害占比" << "," << "圣遗物1" << "," << "圣遗物2" << ","
                       << "圣遗物位置" << "," << "主属性" << "," << "副属性1" << "," << "副属性2" << "," << "副属性3" << "," << "副属性4" << "\n";
        out_header = false;
    }
    outfile_result << c_point->name << ",";
    for (auto &i: combination)
        outfile_result << to_string(i->attack_config->attack_time) + "*" + i->attack_config->condition->attack_way + "_" + (i->attack_config->background ? "后台" : "前台") + "_" + i->attack_config->react_type + "/";
    outfile_result << (need_to_satisfy_recharge ? "要求循环" : "不要求循环") << ","
                   << team_config->teammate_all + "_" + team_config->team_weapon_artifact << ","
                   << total_damage << ",";
    for (int i = 0; i < combination.size(); i++)
        outfile_result << combination[i]->attack_config->condition->attack_way + "_" + combination[i]->attack_config->react_type + "(" + to_string((int) (100.0 * damage[i] / total_damage)) + "%)";
    outfile_result << ","
                   << suit1->name << ","
                   << suit2->name << ",";

    outfile_result << data[0]->pos << ","
                   << data[0]->main_type << ","
                   << data[0]->entry1_type + "_" + to_string(data[0]->entry1_value) << ","
                   << data[0]->entry2_type + "_" + to_string(data[0]->entry2_value) << ","
                   << data[0]->entry3_type + "_" + to_string(data[0]->entry3_value) << ","
                   << data[0]->entry4_type + "_" + to_string(data[0]->entry4_value) << "\n";
    outfile_result << "," << "," << "," << "," << "," << "," << ","
                   << data[1]->pos << ","
                   << data[1]->main_type << ","
                   << data[1]->entry1_type + "_" + to_string(data[1]->entry1_value) << ","
                   << data[1]->entry2_type + "_" + to_string(data[1]->entry2_value) << ","
                   << data[1]->entry3_type + "_" + to_string(data[1]->entry3_value) << ","
                   << data[1]->entry4_type + "_" + to_string(data[1]->entry4_value) << "\n";
    outfile_result << "," << "," << "," << "," << "," << "," << ","
                   << data[2]->pos << ","
                   << data[2]->main_type << ","
                   << data[2]->entry1_type + "_" + to_string(data[2]->entry1_value) << ","
                   << data[2]->entry2_type + "_" + to_string(data[2]->entry2_value) << ","
                   << data[2]->entry3_type + "_" + to_string(data[2]->entry3_value) << ","
                   << data[2]->entry4_type + "_" + to_string(data[2]->entry4_value) << "\n";
    outfile_result << "," << "," << "," << "," << "," << "," << ","
                   << data[3]->pos << ","
                   << data[3]->main_type << ","
                   << data[3]->entry1_type + "_" + to_string(data[3]->entry1_value) << ","
                   << data[3]->entry2_type + "_" + to_string(data[3]->entry2_value) << ","
                   << data[3]->entry3_type + "_" + to_string(data[3]->entry3_value) << ","
                   << data[3]->entry4_type + "_" + to_string(data[3]->entry4_value) << "\n";
    outfile_result << "," << "," << "," << "," << "," << "," << ","
                   << data[4]->pos << ","
                   << data[4]->main_type << ","
                   << data[4]->entry1_type + "_" + to_string(data[4]->entry1_value) << ","
                   << data[4]->entry2_type + "_" + to_string(data[4]->entry2_value) << ","
                   << data[4]->entry3_type + "_" + to_string(data[4]->entry3_value) << ","
                   << data[4]->entry4_type + "_" + to_string(data[4]->entry4_value) << "\n";
}