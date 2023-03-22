# GenshinCalculator
一个用于计算原神人物-武器-圣遗物（套装、主词条、副词条）最佳搭配的计算器。

### 人物建构

具体资料以原神wiki：https://wiki.biligame.com/ys/ 和 https://genshin.honeyhunterworld.com 为准。

目前仅构建了胡桃1、艾尔海森0、雷电将军0、神里绫华0、甘雨0、纳西妲2、夜兰2、八重神子2、行秋6、香菱6、枫原万叶0、钟离0、温迪0、莫娜4、班尼特6。

将这些人物的属性分为自身加成init_character_data（特殊加成get_extra_special)、有关转化类的加成（get_convert_value、get_extra_rate_value），对队友的加成（get_team_data)，充能考虑（check_recharge_requirement）。

### 武器建构

具体资料以原神wiki：https://wiki.biligame.com/ys/ 和 https://genshin.honeyhunterworld.com 为准。

构建了所有武器，构建时忽略了持续时间的限制，而仅考虑触发条件的限制，并总是取最大的可能数值加成。

将这些武器的属性分为自身加成init_weapon_data（特殊加成get_extra_special)、有关转化类的加成（modify_useful_attribute、get_convert_value、get_extra_rate_value），对队友的加成（get_team_data)。

### 圣遗物建构

具体资料以原神wiki：https://wiki.biligame.com/ys/ 和 https://genshin.honeyhunterworld.com 为准。

构建了所有圣遗物，忽略了染血4件套和追忆4件套的加成（条件不好量化，效果太强），仅考虑触发条件的限制，并总是取最大的可能数值加成。

将这些圣遗物的属性分为自身加成init_artifact_data（特殊加成get_extra_special)、有关转化类的加成（modify_useful_attribute、get_convert_value、get_extra_rate_value），对队友的加成（get_team_data)。

### 如何更新数据

在每个池子更新的当天，需要更新当前版本的新增的角色、武器、圣遗物等数据。

### 最佳搭配条件限制

首先，针对圣遗物套装、圣遗物主词条，在init_check_data做了限制，不提供任何有效词条加成的内容被剔除。

其次，不满足充能要求的配置被剔除。

最后，针对圣遗物副词条，采用如下几点限制：

1、不考虑小生命、小攻击、小防御。

2、总词条被分为base和up，base指的是最开始的4个基准词条，up指的是在基准词条上强化的词条。

​	  有$\sum base\le 20$，$\sum up\le 20$，每类词条的$base\le$可能出现的位置，每类词条的$up\le \min(base*4,可能出现位置*3-base)$，$\sum base+\sum up\le 配置设定的最大词条数$。

​	  例如：采用攻伤暴的圣遗物主词条配置，给定副词条最大数量30，则攻击、暴击最多$3*4=12$条，其中$base\le 4$，$up\le \min(base*4,4*3-base)$，生命、防御、元素精通、元素充能效率、爆伤最多$3*5=15$条，其中$base\le 5$，$up\le \min(base*4,5*3-base)$，最后还要保证$\sum base\le 20$，$\sum up\le 20$，$\sum base+\sum up\le 30$）。

### 计算算法

首先获得有效配置（人物-武器-圣遗物套装1-圣遗物套装2-圣遗物主词条），其次采用遍历的方法，结合上述限制，对每一个可能的副词条数量，按照 https://nga.178.com/read.php?tid=25564438 中所述的伤害计算公式进行计算，最后取伤害最大值作为该配置的结果。

2022/10/13更新：

添加了技能组计算的功能，现在的计算方法如下：

1、按照计算配置定义人物的队伍配置、攻击序列。

2、利用遍历的方法获取每一个可能的武器-圣遗物-主属性组合，并根据上述组合、队伍配置、攻击方式获取数据，如果每个攻击方式都获取不到加成，就检查下一个组合。

3、对每一个可能的副词条数量，对每一个攻击方式，按照 https://nga.178.com/read.php?tid=25564438 中所述的伤害计算公式进行计算，最后取伤害和的最大值作为该配置的结果。

2022/2/3更新：已经添加多线程计算，生成组合后创建新线程计算最佳副词条并保存

4、根据用户定义的percentage，保留配置（人物-武器）中相比于最大伤害，在[percentage,1]区间内的所有配置。

### 计算配置

2023/3/16日更新：

为了更精确地计算副词条分配，本程序需要结合gcsim使用，整体流程如下：

1、标准配置文件生成：运行程序，使用功能1生成sample_config文件。

2、初步队伍设定：确定队伍成员（替换sample_config文件中的A、B、C、D），确定大致的输出流程，并填入以下内容：

    1）在all add options后，按照gcsim中对options设置的定义，填入需要更改或添加的选项。

    2）在all add target后，按照gcsim中对options设置的定义，填入需要更改或添加的选项。

    3）在all add attack_list下面若干行中，按照gcsim中对options设置的定义以及大致的输出流程，填入中文描述的动作序列，以"ATTACK_LIST END"为结束标志。

    4）在X add gcsim_config后，填入对应角色现有的武器、圣遗物配置。

将sample_config文件重命名为队伍名称，并放置在队伍名文件夹内，并创建config文件夹。

3、初步脚本生成：运行程序，使用功能2生成初步的gcsim脚本文件。

4、脚本流程检查：在网页端gcsim模拟器中运行模拟，调整输出流程（设置iteration=1000, particle=10以保证循环），推荐检查如下元素：CD对接、吃球情况、元素附着、buff及伤害情况。

5、手动数据填充：根据gcsim网页端的Details页面，为每个队伍成员选择性添加以下内容：

    1）X add cal_all_restriction：计算最佳副词条时对于武器圣遗物的限制。

    2）X add cal_artifact_restriction：计算已有圣遗物时对于武器圣遗物的限制。

    3）X add team_config：E产生能量球的次数E_energy_times，是否保证充能循环recharge，能够获取的队友武器和圣遗物增益team_weapon_artifact，怪物附着的元素ele_attach_type，允许扩散结晶的元素ele_allow_spread。

    4）X add attack_config：多个攻击配置：攻击方式attack_way，倍率索引rate_pos，是否后台background，反应类型react_type，攻击次数attack_time，苍白层数cangbai_level，千岩触发qianyan_enable，魔女层数monv_level，辰砂触发chensha_enable，深林触发shenlin_enable，水仙层数shuixian_level，对应武器层数wuqie_shenle_feilei_humo，对应武器层数shengxian_biluo_dongji_chisha，对应武器层数none_sifeng_pomo_shizuo。

6、最佳副词条结果生成：运行程序，使用功能3生成最佳副词条结果（保证5.1、5.3、5.4存在）。需要注意的是，该结果并不为最精确的结果，由于"已知问题"的存在，更为精确的结果请继续后续步骤。

7、手动配置确定：根据生成的csv文件，确定希望gcsim计算的，每个人物较好的武器、圣遗物搭配，并类似2.1以X add gcsim_config的形式添加或覆盖。

8、gcsim运行以及结果检查：请按顺序执行以下步骤：

    1）运行程序，使用功能2生成最终的gcsim脚本文件。

    2）运行run_substat_optimizer.bat，在optimized_config文件夹中可以获取gcsim优化副词条后的配置。

    3）由于存在随机性，即使完全相同的队伍（武器圣遗物不影响充能）的情况下，优化的充能词条仍不一样，会严重影响dps的计算结果，你可以选择统一各配置的充能词条（或者整体的副词条一致）。

    4）运行run_optimized_config.bat，在logs中获取具体的计算信息，在viewer_gz中获取可在gcsim.app上查看的json文件。

=> 几个注意事项

1、请严格设置options duration，较大程度影响dps，默认是进行5轮循环的时间，通常取5的倍数。

2、对于武器圣遗物的限制部分，武器、圣遗物主词条可以填写多个（均允许），但圣遗物套装每个位置只能填写1个。

3、E_energy_times部分，可以考虑将西风武器等效到E技能产球次数上。

4、上述步骤5.4中同一技能的反应类型请务必保持一致，不发生反应的请在之后添加no_add_damage；技能全部不发生反应请填入"NONE"。

### 配置文件详解

第一行：队伍成员A B C D的中文全名，必须存在。

Part1：all add options something，gcsim中的options参数（默认options iteration=10000 swap_delay=4 duration=105），格式与gcsim保持一致，只取第一次遇到的行，后面的被忽略，可以省略以不做更改。

Part2：all add target something，gcsim中的target参数（默认target lvl=95 resist=0.1 particle_threshold=150000 particle_drop_count=1）格式与gcsim保持一致，只取第一次遇到的行，后面的被忽略，可以省略以不做更改。

Part3：all add attack_list/something/rotation_start/something/rotation_end/something/ATTACK_LIST END，gcsim中的动作序列，夹在rotation_start和rotation_end的动作序列将被循环执行5次，在rotation_start之前和rotation_end之后的动作序列将在开始和结尾被执行，格式：角色名+空格+n*(动作(平A/重A/下落A/E/Q)+英文逗号)，其他特殊动作直接填写gcsim中对应的英文，必须存在。

Part4：角色计算配置

    1）X add gcsim_config，gcsim中的角色武器、圣遗物配置，格式：中文全名，一一对应，可以复制多行，不可省略。

    2）X add cal_all_restriction，计算最佳副词条时对武器圣遗物的限制，格式：中文全名，除了圣遗物套装外可以填写多个，逗号分隔，可以复制多行，可以省略（即不计算最佳副词条）。

    3）X add cal_artifact_restriction，计算圣遗物时对武器圣遗物的限制，格式：中文全名，除了圣遗物套装外可以填写多个，逗号分隔，可以复制多行，可以省略（即不计算圣遗物结果）。

    4）X add team_config，队伍信息，格式：E产生能量球的次数E_energy_times（整数），是否保证充能循环recharge（0/1），能够获取的队友武器和圣遗物增益team_weapon_artifact（多个武器圣遗物名称），怪物附着的元素ele_attach_type（多个元素名称），允许扩散结晶的元素ele_allow_spread（多个元素名称），只取X的最前面一行，后面的被忽略，必须存在。

    5）X add attack_config，动作信息，格式：攻击方式attack_way（平A/重A/下落A/E/Q），倍率索引rate_pos（整数），是否后台background（0/1），反应类型react_type（多个反应名称），攻击次数attack_time（整数），苍白层数cangbai_level（0/1/2），千岩触发qianyan_enable（0/1），魔女层数monv_level（0/1/2/3），辰砂触发chensha_enable（0/1），深林触发shenlin_enable（0/1），水仙层数shuixian_level（0/1/2/3），对应武器层数wuqie_shenle_feilei_humo，对应武器层数shengxian_biluo_dongji_chisha，对应武器层数none_sifeng_pomo_shizuo，可以复制多行，可以省略（即不计算任何内容）。

注意：各个Part和Part的小点对应的单行命令均可以任意顺序排列，Part3请作为一个整体连续编写。

### 其他功能

1、gcsim脚本生成：具体见计算配置

2、圣遗物搭配，流程如下：

    1）使用圣遗物扫描器对数据进行扫描，导出为莫娜占卜铺的格式。

    2）在配置文件中填写计算圣遗物时对武器圣遗物的限制。

    2）运行程序，使用功能4生成对圣遗物好坏的判定，以及每个角色在对应配置下，穿戴所有可能圣遗物组合的伤害数据。

    3）对生成的artifacts_results.csv文件，可以按照人物、队友、伤害进行排序，之后按照人物优先级选择圣遗物，已经被选择的圣遗物可以通过结合索引和excel的筛选功能去除，之后再重复该步骤，为下一个角色选择圣遗物。

### 运行注意事项

使用clion，在不同系统下需要首先建立c++项目（pure c++），之后仅需要将.cpp文件、.h文件、cmake文件保留。

在MAC中，请将文件的编码改为UTF-8，即可正常运行。

在WIN中，请首先在设置中将项目的文件编码改为UTF-8，并在编辑界面将所有文件的编码改为GBK，并使用ctrl+alt+shift+.打开控制台注册表界面，并勾选run.process.with.pty，如此可解决运行过程中中文乱码的问题。但是在调试过程中仍会出现中文乱码和显示string为16进制字符的问题。

目前来说，拥有5个有效词条的人物-武器对计算时间在10s，拥有4个有效词条的人物-武器对计算时间在1s（cpu：12400f），随着配置复杂度的不同而有所变化。

### 已知问题

1、角色存在属性转化中，未对角色能够获取的属性加成区分是否能够转化（在面板、不在面板、在怪物），目前主要不能转化的属性集中在增伤、暴击、暴伤上，而这些属性目前不参与转化。

2、所有的叠层武器和具有buff时间限制的武器由于复杂性问题在程序中均忽略，仅考虑触发的条件并且总是以角色能够获得的最大层数进行计算。（该问题目前的解决方法是，手动选择较好可行的搭配，转化成脚本并进一步在gcsim中进行最佳副词条和真实的模拟演算）

### 未来工作

1、考虑组合伤害的计算（技能组、技能+剧变） FINISH 2022/10/13

2、添加更多角色（主要是5星）

3、配置文件化 FINISH 2023/3/16

4、GUI

5、多线程并行计算 FINISH 2023/2/2