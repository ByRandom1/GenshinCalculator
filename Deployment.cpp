//
// Created by 陈劭彬 on 2022/8/10.
//

#include "Deployment.h"

Deployment::Deployment(Character *c_point_,
                       Weapon *w_point_,
                       Artifact *suit1_,
                       Artifact *suit2_,
                       string a_main3_,
                       string a_main4_,
                       string a_main5_,
                       Config *config_)
{
    c_point = c_point_;
    w_point = w_point_;
    suit1 = suit1_;
    suit2 = suit2_;
    a_main3 = a_main3_;
    a_main4 = a_main4_;
    a_main5 = a_main5_;
    config = config_;

    base_life = c_point->get_life();
    base_atk = c_point->get_atk() + w_point->get_atk();
    base_def = c_point->get_def();
    if (config->condition->attack_way == "平A")
    {
        base_skillrate = c_point->get_normal_A();
        config->lockface = false;
    }
    else if (config->condition->attack_way == "重A")
    {
        base_skillrate = c_point->get_heavy_A();
        config->lockface = false;
    }
    else if (config->condition->attack_way == "下落A")
    {
        base_skillrate = c_point->get_down_A();
        config->lockface = false;
    }
    else if (config->condition->attack_way == "E")
    {
        base_skillrate = c_point->get_E();
        config->lockface = c_point->E_lockface;
    }
    else if (config->condition->attack_way == "Q")
    {
        base_skillrate = c_point->get_Q();
        config->lockface = c_point->Q_lockface;
    }

    data_list.push_back(new attribute("生命值", 1.0, 0, 0.05, config->useful_attributes[0]));
    data_list.push_back(new attribute("攻击力", 1.0, 0, 0.05, config->useful_attributes[1]));
    data_list.push_back(new attribute("防御力", 1.0, 0, 0.062, config->useful_attributes[2]));
    data_list.push_back(new attribute("额外倍率", 0, 0, 0, config->useful_attributes[3]));
    data_list.push_back(new attribute("元素精通", 0, 0, 20.0, config->useful_attributes[4]));
    data_list.push_back(new attribute("元素充能效率", 1.0, 0, 0.055, config->useful_attributes[5]));
    data_list.push_back(new attribute("暴击率", 0.05, 0, 0.033, config->useful_attributes[6]));
    data_list.push_back(new attribute("暴击伤害", 0.5, 0, 0.066, config->useful_attributes[7]));
    data_list.push_back(new attribute("伤害加成", 1, 0, 0, config->useful_attributes[8]));
    data_list.push_back(new attribute("抗性削弱", 0, 0, 0, config->useful_attributes[9]));
    data_list.push_back(new attribute("防御削弱", 0, 0, 0, config->useful_attributes[10]));
    data_list.push_back(new attribute("防御无视", 0, 0, 0, config->useful_attributes[11]));
    data_list.push_back(new attribute("治疗加成", 0, 0, 0, config->useful_attributes[12]));
    data_list.push_back(new attribute("护盾强效", 0, 0, 0, config->useful_attributes[13]));
    damage = 0;
}

Deployment::~Deployment()
{
    for (auto &i: data_list) delete i;
}

int Deployment::init_check_data()
{
    //LOG START
    double prevalue[data_list.size()];

    outfile_debug << (c_point->name + "--" + w_point->name + "--" + suit1->name + "--" + suit2->name + "--" + a_main3 + "--" + a_main4 + "--" + a_main5 + ",");

    Weapon::modify_useful_attribute(this);
    Artifact::modify_useful_attribute(this);

    //character get
    c_point->get_break(this);
    c_point->get_extra(this);
    //weapon get
    w_point->get_vice(this);
    w_point->get_extra(this);

    //update
    for (int i = 0; i < data_list.size(); i++) prevalue[i] = data_list[i]->percentage;

    //artifact
    bool suit1_valid;
    bool suit2_valid;
    if (suit1 == suit2)
    {
        suit1->get_extra(this, true);
        //check
        suit1_valid = suit2_valid = false;
        for (int i = 0; i < data_list.size(); i++)
            if (data_list[i]->useful && data_list[i]->percentage > prevalue[i])
            {
                suit1_valid = suit2_valid = true;
                break;
            }
        if (config->condition->attack_way == "Q" && data_list[5]->percentage > prevalue[5]) suit1_valid = suit2_valid = true;

        check_artifact_special(suit1_valid, suit2_valid, true);

        if (!suit1_valid || !suit2_valid)
        {
            outfile_debug << (suit1->name + "_piece4_failure\n");
            return 2;
        }

        suit2->get_extra(this, false);
    }
    else
    {
        suit1->get_extra(this, false);
        //check
        suit1_valid = false;
        for (int i = 0; i < data_list.size(); i++)
            if (data_list[i]->useful && data_list[i]->percentage > prevalue[i])
            {
                suit1_valid = true;
                break;
            }
        if (config->condition->attack_way == "Q" && data_list[5]->percentage > prevalue[5]) suit1_valid = true;
        //update
        for (int i = 0; i < data_list.size(); i++) prevalue[i] = data_list[i]->percentage;

        suit2->get_extra(this, false);
        suit2_valid = false;
        for (int i = 0; i < data_list.size(); i++)
            if (data_list[i]->useful && data_list[i]->percentage > prevalue[i])
            {
                suit2_valid = true;
                break;
            }
        if (config->condition->attack_way == "Q" && data_list[5]->percentage > prevalue[5]) suit2_valid = true;

        check_artifact_special(suit1_valid, suit2_valid, false);

        if (!suit1_valid)
        {
            outfile_debug << (suit1->name + "_piece2_failure\n");
            return 2;
        }
        else if (!suit2_valid)
        {
            outfile_debug << (suit2->name + "_piece2_failure\n");
            return 2;
        }
    }
    //update
//    for (int i = 0; i < data_list.size(); i++) prevalue[i] = data_list[i]->percentage;

    check_useful_attributes();

    //main
    //get
    Artifact::get_main(this);
    //check
    if ((a_main3 == "生命值" && !data_list[0]->useful) || (a_main3 == "攻击力" && !data_list[1]->useful) || (a_main3 == "防御力" && !data_list[2]->useful) ||
        (a_main3 == "元素精通" && !data_list[4]->useful) || (a_main3 == "元素充能效率" && (!data_list[5]->useful && config->condition->attack_way != "Q")))//Q充能考虑
    {
        outfile_debug << (a_main3 + "_main3_failure\n");
        return 3;
    }
    if ((a_main4 == "生命值" && !data_list[0]->useful) || (a_main4 == "攻击力" && !data_list[1]->useful) || (a_main4 == "防御力" && !data_list[2]->useful) ||
        (a_main4 == "元素精通" && !data_list[4]->useful) || (a_main4 == "伤害加成" && !data_list[8]->useful))
    {
        outfile_debug << (a_main4 + "_main4_failure\n");
        return 4;
    }
    if ((a_main5 == "生命值" && !data_list[0]->useful) || (a_main5 == "攻击力" && !data_list[1]->useful) || (a_main5 == "防御力" && !data_list[2]->useful) ||
        (a_main5 == "元素精通" && !data_list[4]->useful) || (a_main5 == "暴击率" && !data_list[6]->useful) || (a_main5 == "暴击伤害" && !data_list[7]->useful) ||
        (a_main5 == "治疗加成" && !data_list[12]->useful))
    {
        outfile_debug << (a_main5 + "_main5_failure\n");
        return 5;
    }
    //update
//    for (int i = 0; i < data_list.size(); i++) prevalue[i] = data_list[i]->percentage;

    get_team_data();

    if (cal_enable_recharge_check) satisfy_recharge_requirement();

    outfile_debug << "\n";

    return 0;
}

bool Deployment::add_percentage(string type_, double value_, string source)
{
    for (auto &i: data_list)
        if (type_ == i->name)
        {
            i->percentage += value_;
            outfile_debug << (source + "_" + type_ + "_" + to_string(value_) + ",");
            return true;
        }
    outfile_debug << (source + "--" + type_ + "--" + to_string(value_) + "_failure" + ",");
    return false;
}

void Deployment::cal_damage_entry_num()
{
    int totalbase = 20;
    int totalup = 5 * max_up_num_per_base;
    int totalnum = config->max_entry_all;
    if (suit1 != suit2 && totalnum != 0) totalnum += artifact_2_2_max_entry_bonus;

    int life_pos = 5 - ((a_main3 == "生命值") ? 1 : 0) - ((a_main4 == "生命值") ? 1 : 0) - ((a_main5 == "生命值") ? 1 : 0);
    int atk_pos = 5 - ((a_main3 == "攻击力") ? 1 : 0) - ((a_main4 == "攻击力") ? 1 : 0) - ((a_main5 == "攻击力") ? 1 : 0);
    int def_pos = 5 - ((a_main3 == "防御力") ? 1 : 0) - ((a_main4 == "防御力") ? 1 : 0) - ((a_main5 == "防御力") ? 1 : 0);
    int mastery_pos = 5 - ((a_main3 == "元素精通") ? 1 : 0) - ((a_main4 == "元素精通") ? 1 : 0) - ((a_main5 == "元素精通") ? 1 : 0);
    int recharge_pos = 5 - ((a_main3 == "元素充能效率") ? 1 : 0);
    int critrate_pos = 5 - ((a_main5 == "暴击率") ? 1 : 0);
    int critdam_pos = 5 - ((a_main5 == "暴击伤害") ? 1 : 0);

    int life_num = data_list[0]->entry_num;
    int atk_num = data_list[1]->entry_num;
    int def_num = data_list[2]->entry_num;
    int mastery_num = data_list[4]->entry_num;
    int recharge_num = data_list[5]->entry_num;
    int critrate_num = data_list[6]->entry_num;
    int critdam_num = data_list[7]->entry_num;

    int life_base = (data_list[0]->useful) ? life_pos : min(life_num, life_pos);
    int atk_base = (data_list[1]->useful) ? atk_pos : min(atk_num, atk_pos);
    int def_base = (data_list[2]->useful) ? def_pos : min(def_num, def_pos);
    int mastery_base = (data_list[4]->useful) ? mastery_pos : min(mastery_num, mastery_pos);
    int recharge_base = (data_list[5]->useful) ? recharge_pos : min(recharge_num, recharge_pos);
    int critrate_base = (data_list[6]->useful) ? critrate_pos : min(critrate_num, critrate_pos);
    int critdam_base = (data_list[7]->useful) ? critdam_pos : min(critdam_num, critdam_pos);

    double ownlife = data_list[0]->percentage;
    double ownatk = data_list[1]->percentage;
    double owndef = data_list[2]->percentage;
    double ownextrarate = data_list[3]->percentage;
    double ownmastery = data_list[4]->percentage;
    double ownrecharge = data_list[5]->percentage;
    double owncritrate = data_list[6]->percentage;
    double owncritdam = data_list[7]->percentage;
    double owndamplus = data_list[8]->percentage;
    double ownresistencedec = data_list[9]->percentage;
    double owndefencedec = data_list[10]->percentage;
    double owndefenceign = data_list[11]->percentage;

    for (int lifebase = life_base; lifebase >= 0; lifebase--)
        for (int lifeup = min(max_attribute_num_per_pos * life_pos - lifebase, max_up_num_per_base * lifebase); lifeup >= 0; lifeup--)
        {
            if (!data_list[0]->useful && (lifebase + lifeup != life_num)) continue;
            if (lifebase + lifeup < life_num) continue;
            int leftbase1 = totalbase - lifebase;
            int leftup1 = totalup - lifeup;
            int leftnum1 = totalnum - lifebase - lifeup;
            if (leftbase1 < 0 || leftup1 < 0 || leftnum1 < 0) continue;

            for (int atkbase = atk_base; atkbase >= 0; atkbase--)
                for (int atkup = min(max_attribute_num_per_pos * atk_pos - atkbase, max_up_num_per_base * atkbase); atkup >= 0; atkup--)
                {
                    if (!data_list[1]->useful && (atkbase + atkup != atk_num)) continue;
                    if (atkbase + atkup < atk_num) continue;
                    int leftbase2 = leftbase1 - atkbase;
                    int leftup2 = leftup1 - atkup;
                    int leftnum2 = leftnum1 - atkbase - atkup;
                    if (leftbase2 < 0 || leftup2 < 0 || leftnum2 < 0) continue;

                    for (int defbase = def_base; defbase >= 0; defbase--)
                        for (int defup = min(max_attribute_num_per_pos * def_pos - defbase, max_up_num_per_base * defbase); defup >= 0; defup--)
                        {
                            if (!data_list[2]->useful && (defbase + defup != def_num)) continue;
                            if (defbase + defup < def_num) continue;
                            int leftbase3 = leftbase2 - defbase;
                            int leftup3 = leftup2 - defup;
                            int leftnum3 = leftnum2 - defbase - defup;
                            if (leftbase3 < 0 || leftup3 < 0 || leftnum3 < 0) continue;

                            for (int masterybase = mastery_base; masterybase >= 0; masterybase--)
                                for (int masteryup = min(max_attribute_num_per_pos * mastery_pos - masterybase, max_up_num_per_base * masterybase); masteryup >= 0; masteryup--)
                                {
                                    if (!data_list[4]->useful && (masterybase + masteryup != mastery_num)) continue;
                                    if (masterybase + masteryup < mastery_num) continue;
                                    int leftbase4 = leftbase3 - masterybase;
                                    int leftup4 = leftup3 - masteryup;
                                    int leftnum4 = leftnum3 - masterybase - masteryup;
                                    if (leftbase4 < 0 || leftup4 < 0 || leftnum4 < 0) continue;

                                    for (int rechargebase = recharge_base; rechargebase >= 0; rechargebase--)
                                        for (int rechargeup = min(max_attribute_num_per_pos * recharge_pos - rechargebase, max_up_num_per_base * rechargebase);
                                             rechargeup >= 0; rechargeup--)
                                        {
                                            if (!data_list[5]->useful && (rechargebase + rechargeup != recharge_num)) continue;
                                            if (rechargebase + rechargeup < recharge_num) continue;
                                            int leftbase5 = leftbase4 - rechargebase;
                                            int leftup5 = leftup4 - rechargeup;
                                            int leftnum5 = leftnum4 - rechargebase - rechargeup;
                                            if (leftbase5 < 0 || leftup5 < 0 || leftnum5 < 0) continue;

                                            for (int critratebase = critrate_base; critratebase >= 0; critratebase--)
                                                for (int critrateup = min(max_attribute_num_per_pos * critrate_pos - critratebase, max_up_num_per_base * critratebase);
                                                     critrateup >= 0; critrateup--)
                                                {
                                                    if (!data_list[6]->useful && (critratebase + critrateup != critrate_num)) continue;
                                                    if (critratebase + critrateup < critrate_num) continue;
                                                    int leftbase6 = leftbase5 - critratebase;
                                                    int leftup6 = leftup5 - critrateup;
                                                    int leftnum6 = leftnum5 - critratebase - critrateup;
                                                    if (leftbase6 < 0 || leftup6 < 0 || leftnum6 < 0) continue;

                                                    for (int critdambase = critdam_base; critdambase >= 0; critdambase--)
                                                        for (int critdamup = min(max_attribute_num_per_pos * critdam_pos - critdambase, max_up_num_per_base * critdambase);
                                                             critdamup >= 0; critdamup--)
                                                        {
                                                            if (!data_list[7]->useful && (critdambase + critdamup != critdam_num)) continue;
                                                            if (critdambase + critdamup < critdam_num) continue;
                                                            int leftbase7 = leftbase6 - critdambase;
                                                            int leftup7 = leftup6 - critdamup;
                                                            int leftnum7 = leftnum6 - critdambase - critdamup;
                                                            if (leftbase7 < 0 || leftup7 < 0 || leftnum7 < 0) continue;

                                                            double life = ownlife + data_list[0]->value_per_entry * (lifeup + lifebase);
                                                            double atk = ownatk + data_list[1]->value_per_entry * (atkup + atkbase);
                                                            double def = owndef + data_list[2]->value_per_entry * (defup + defbase);
                                                            double mastery = ownmastery + data_list[4]->value_per_entry * (masteryup + masterybase);
                                                            double recharge = ownrecharge + data_list[5]->value_per_entry * (rechargeup + rechargebase);
                                                            double critrate = owncritrate + data_list[6]->value_per_entry * (critrateup + critratebase);
                                                            double critdam = owncritdam + data_list[7]->value_per_entry * (critdamup + critdambase);
                                                            double damplus = owndamplus;

                                                            //深渊
                                                            critrate += 0.08;
                                                            critdam += 0.15;

                                                            get_convert_value(life, atk, def, mastery, recharge, critrate, critdam, damplus);
                                                            double extrarate = ownextrarate * (double) base_atk * atk;
                                                            get_extra_rate_value(life, atk, def, mastery, recharge, critrate, critdam, damplus, extrarate);
                                                            double growrate = 1.0;
                                                            get_react_value(mastery, extrarate, growrate);

                                                            if (critrate > cal_max_critrate_valid) critrate = 1.0;
                                                            if (critrate < cal_min_critrate_valid) critrate = 0.0;

                                                            double temp = ((double) base_atk * atk * base_skillrate + extrarate) * damplus * (1.0 + critrate * critdam) * growrate;

                                                            if (temp > damage)
                                                            {
                                                                data_list[0]->entry_num = lifeup + lifebase;
                                                                data_list[1]->entry_num = atkup + atkbase;
                                                                data_list[2]->entry_num = defup + defbase;
                                                                data_list[4]->entry_num = masteryup + masterybase;
                                                                data_list[5]->entry_num = rechargeup + rechargebase;
                                                                data_list[6]->entry_num = critrateup + critratebase;
                                                                data_list[7]->entry_num = critdamup + critdambase;
                                                                damage = temp;
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
    double resistence_ratio;
    if (0.1 - ownresistencedec >= 0.75) resistence_ratio = 1 / (4 * (0.1 - ownresistencedec) + 1);
    else if (0.1 - ownresistencedec < 0) resistence_ratio = 1 - (0.1 - ownresistencedec) / 2;
    else resistence_ratio = 1 - (0.1 - ownresistencedec);

    double c_level = 90, enemy_level = 90;
    double defence_ratio = (c_level + 100) / (c_level + 100 + (1 - owndefencedec) * (1 - owndefenceign) * (enemy_level + 100));

    damage = damage * resistence_ratio * defence_ratio;
}