# GenshinCalculator
一个用于计算原神人物-武器-圣遗物（套装、主词条、副词条）最佳搭配的计算器。

### 人物建构

具体资料以原神wiki：https://wiki.biligame.com/ys/ 和 https://genshin.honeyhunterworld.com 为准。

目前仅构建了胡桃1、艾尔海森0、雷电将军0、神里绫华0、甘雨0、纳西妲2、夜兰2、八重神子2、行秋6、香菱6、枫原万叶0、钟离0、温迪0、莫娜4、班尼特6。

将这些人物的属性分为自身加成init_character_data（特殊加成get_extra_special)、有关转化类的加成（get_convert_value、get_extra_rate_value），对队友的加成（get_team_data)，充能考虑（check_recharge_requirement）

### 武器建构

具体资料以原神wiki：https://wiki.biligame.com/ys/ 和 https://genshin.honeyhunterworld.com 为准。

构建了所有武器，构建时忽略了持续时间的限制，而仅考虑触发条件的限制，并总是取最大的可能数值加成。

将这些武器的属性分为自身加成init_weapon_data（特殊加成get_extra_special)、有关转化类的加成（modify_useful_attribute、get_convert_value、get_extra_rate_value），对队友的加成（get_team_data)

### 圣遗物建构

具体资料以原神wiki：https://wiki.biligame.com/ys/ 和 https://genshin.honeyhunterworld.com 为准。

构建了所有圣遗物，忽略了染血4件套和追忆4件套的加成（条件不好量化，效果太强），仅考虑触发条件的限制，并总是取最大的可能数值加成。

将这些圣遗物的属性分为自身加成init_artifact_data（特殊加成get_extra_special)、有关转化类的加成（modify_useful_attribute、get_convert_value、get_extra_rate_value），对队友的加成（get_team_data)

### 如何更新数据

在每个池子更新的当天，需要更新当前版本的新增的角色、武器、圣遗物等数据

### 最佳搭配条件限制

首先，针对圣遗物套装、圣遗物主词条，在init_check_data做了限制，不提供任何有效词条加成的内容被剔除。

其次，不满足充能要求的配置被剔除。

最后，针对圣遗物副词条，采用如下几点限制：

1、不考虑小生命、小攻击、小防御

2、总词条被分为base和up，base指的是最开始的4个基准词条，up指的是在基准词条上强化的词条

​	  有$\sum base\le 20$，$\sum up\le 20$，每类词条的$base\le$可能出现的位置，每类词条的$up\le \min(base*4,可能出现位置*3-base)$，$\sum base+\sum up\le 配置设定的最大词条数$。

​	  例如：采用攻伤暴的圣遗物主词条配置，给定副词条最大数量30，则攻击、暴击最多$3*4=12$条，其中$base\le 4$，$up\le \min(base*4,4*3-base)$，生命、防御、元素精通、元素充能效率、爆伤最多$3*5=15$条，其中$base\le 5$，$up\le \min(base*4,5*3-base)$，最后还要保证$\sum base\le 20$，$\sum up\le 20$，$\sum base+\sum up\le 30$）

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

    1）在gcsim_config:后，按照队伍成员的顺序填入现有的武器、圣遗物配置。

    2）在options:后，按照gcsim中对options设置的定义，填入需要更改或添加的选项（默认options iteration=10000 swap_delay=4 duration=106;）。

    3）在target:后，按照gcsim中对options设置的定义，填入需要更改或添加的选项（默认target lvl=95 resist=0.1 particle_threshold=150000 particle_drop_count=1;）。

    4）在attack_list:后，按照gcsim中对options设置的定义以及大致的输出流程，填入中文描述的动作序列

将sample_config文件重命名为队伍名称，并放置在队伍名文件夹内，并创建config文件夹。

3、初步脚本生成：运行程序，使用功能2生成初步的gcsim脚本文件。

4、脚本流程检查：在网页端gcsim模拟器中运行模拟，调整输出流程（设置iteration=1000, particle=10以保证循环），推荐检查如下元素：CD对接、吃球情况、元素附着、buff及伤害情况。

5、手动数据填充：根据gcsim网页端的Details页面，在restrictions/team_config/attack_config:后，按按照队伍成员的顺序填入以下内容：

    1）计算最佳副词条时对于武器圣遗物的限制，以及该人物是否要保证单轮循环充能

    2）一轮释放E的次数E_release_times，能够获取的队友武器和圣遗物增益team_weapon_artifact，怪物附着的元素ele_attach_type，允许扩散结晶的元素ele_allow_spread

    3）多个攻击配置：攻击方式attack_way，倍率索引rate_pos，是否后台background，反应类型react_type，攻击次数attack_time，苍白层数cangbai_level，千岩触发qianyan_enable，魔女层数monv_level，辰砂触发chensha_enable，深林触发shenlin_enable，水仙层数shuixian_level，对应武器层数wuqie_shenle_feilei_humo，对应武器层数shengxian_biluo_dongji_chisha，对应武器层数none_sifeng_pomo_shizuo。

6、最佳副词条结果生成：运行程序，使用功能3生成最佳副词条结果。需要注意的是，该结果并不为最精确的结果，由于"已知问题"的存在，更为精确的结果请继续后续步骤。

7、手动配置确定：根据生成的csv文件，确定希望gcsim计算的，每个人物较好的武器、圣遗物搭配，并类似2、1）填入gcsim_config:后的行。

8、gcsim运行以及结果检查：请按顺序执行以下步骤：

    1）运行程序，使用功能2生成最终的gcsim脚本文件。

    2）运行run_optimized_config.bat，在optimized_config文件夹中可以获取gcsim优化副词条后的配置。！！！需要重点注意的是，由于存在随机性，即使完全相同的队伍（武器圣遗物不影响充能）的情况下，优化的充能词条仍不一样，会严重影响dps的计算结果

    3）统一各配置的充能词条（或者整体的副词条一致）

    4）运行run_substat_optimizer.bat，在logs中获取具体的计算信息，在viewer_gz中获取可在gcsim.app上查看的json文件。

=> 几个注意事项

1、请严格设置options duration，较大程度影响dps

2、上述步骤5中同一技能的反应类型请务必保持一致，不发生反应的请在之后添加no_add_damage；技能全部不发生反应请填入"NONE"。

3、8.3）中统一充能词条需要格外注意充能武器、圣遗物等会影响充能词条，不同的配置不一定要求相同的充能词条。

### 配置文件详解

Line1：队伍成员A B C D的中文全名（注意顺序，之后的数据顺序对应该顺序）

Line2：提示下面的内容为gcsim中的角色武器、圣遗物配置

Line3：角色A的武器、圣遗物配置（可以通过复制填入多行）

Line4：BREAK，角色A配置结束标志

Line5：角色B的武器、圣遗物配置（可以通过复制填入多行）

Line6：BREAK，角色B配置结束标志

Line7：角色C的武器、圣遗物配置（可以通过复制填入多行）

Line8：BREAK，角色C配置结束标志

Line9：角色D的武器、圣遗物配置（可以通过复制填入多行）

Line10：BREAK，角色D配置结束标志

Line11：提示下面的内容为gcsim中的options参数

Line12：gcsim中的options参数（默认options iteration=10000 swap_delay=4 duration=105）

Line13：提示下面的内容为gcsim中的target参数

Line14：gcsim中的target参数（默认target lvl=95 resist=0.1 particle_threshold=150000 particle_drop_count=1）

Line15：提示下面的内容为gcsim中的动作序列

Line16、17、18：夹在rotation_start和rotation_end的动作序列将被循环执行5次，在rotation_start之前和rotation_end之后的动作序列将在开始和结尾被执行。每行动作请按照gcsim中的定义，用中文角色名+（平A/重A/下落A/E/Q/冲刺/跳跃）完成。

Line19：BREAK，动作序列结束标志

Line20：提示下面的内容为cal_optimal_substats的参数

Line21：计算A的最佳副词条时对武器圣遗物的限制，对每轮循环是否需要保证充能的限制recharge

Line22：A每轮循环释放E技能的次数E_release_times（持续性E技能请填1），所在队伍其他角色武器圣遗物提供的buff team_weapon_artifact，所在队伍怪物头上主要的附着元素ele_attach_type，所在队伍循环过程中能够被扩散/结晶的元素ele_allow_spread。

Line23：A的单个动作：攻击方式attack_way（平A/重A/下落A/E/Q），倍率索引rate_pos，是否后台background（0/1），反应类型react_type，攻击次数attack_time，苍白层数cangbai_level，千岩触发qianyan_enable（0/1），魔女层数monv_level，辰砂触发chensha_enable（0/1），深林触发shenlin_enable（0/1），水仙层数shuixian_level，对应武器层数wuqie_shenle_feilei_humo，对应武器层数shengxian_biluo_dongji_chisha，对应武器层数none_sifeng_pomo_shizuo（可以通过复制填入多行）

Line24：BREAK，角色A参数配置结束标志

Line25-28：与Line21-24定义一致，为B对应的内容

Line29-32：与Line21-24定义一致，为C对应的内容

Line33-36：与Line21-24定义一致，为D对应的内容

### 其他功能

1、gcsim脚本生成：具体见计算配置

2、圣遗物搭配，流程如下：（正在更新中）

    1）使用圣遗物扫描器对数据进行扫描，导出为莫娜占卜铺的格式。

    2）使用Filter_Artifact项目将扫描得到的圣遗物文件转化为本程序能够识别的格式。

    3）运行程序，使用功能4生成最佳的圣遗物搭配。

### 运行注意事项

使用clion，在不同系统下需要首先建立c++项目（pure c++），之后仅需要将.cpp文件、.h文件、cmake文件保留。

在MAC中，请将文件的编码改为UTF-8，即可正常运行。

在WIN中，请首先在设置中将项目的文件编码改为UTF-8，并在编辑界面将所有文件的编码改为GBK，并使用ctrl+alt+shift+.打开控制台注册表界面，并勾选run.process.with.pty，如此可解决运行过程中中文乱码的问题。但是在调试过程中仍会出现中文乱码和显示string为16进制字符的问题。

目前来说，拥有5个有效词条的人物-武器对计算时间在10s，拥有4个有效词条的人物-武器对计算时间在1s（cpu：12400f），随着配置复杂度的不同而有所变化。

### 已知问题

1、角色存在属性转化中，未对角色能够获取的属性加成区分是否能够转化（在面板、不在面板、在怪物），目前主要不能转化的属性集中在增伤、暴击、暴伤上，而这些属性目前不参与转化。

2、所有的叠层武器和具有buff时间限制的武器由于复杂性问题在程序中均忽略，仅考虑触发的条件并且总是以角色能够获得的最大层数进行计算。（该问题目前的解决方法是，采用gcsim_script_generator程序，手动选择较好可行的搭配，转化成脚本并进一步在gcsim中进行最佳副词条和真实的模拟演算）

### 未来工作

1、考虑组合伤害的计算（技能组、技能+剧变） FINISH 2022/10/13

2、添加更多角色（主要是5星）

3、配置文件化 FINISH 2023/3/16

4、GUI

5、多线程并行计算 FINISH 2023/2/2