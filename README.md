# GenshinCalculator
一个用于计算原神人物-武器-圣遗物（套装、主词条、副词条）最佳搭配的计算器。

### 人物建构

具体资料以原神wiki：https://wiki.biligame.com/ys/ 和 https://genshin.honeyhunterworld.com 为准。

目前仅构建了胡桃1、神里绫华0、雷电将军0、甘雨0、夜兰2、行秋6、香菱6、八重神子2、枫原万叶0、班尼特6、温迪0、莫娜3、钟离0、纳西妲2。

将这些人物的属性分为自身加成init_character_data（特殊加成get_extra_special)、有关转化类的加成（get_convert_value、get_extra_rate_value），对队友的加成（get_team_data)，充能考虑（check_recharge_requirement）

### 武器建构

具体资料以原神wiki：https://wiki.biligame.com/ys/ 和 https://genshin.honeyhunterworld.com 为准。

构建了所有武器，构建时忽略了持续时间的限制，而仅考虑触发条件的限制，并总是取最大的可能数值加成。

将这些武器的属性分为自身加成init_weapon_data（特殊加成get_extra_special)、有关转化类的加成（modify_useful_attribute、get_convert_value、get_extra_rate_value），对队友的加成（get_team_data)

### 圣遗物建构

具体资料以原神wiki：https://wiki.biligame.com/ys/ 和 https://genshin.honeyhunterworld.com 为准。

构建了所有圣遗物，忽略了染血4件套和追忆4件套的加成（条件苛刻，效果太强），仅考虑触发条件的限制，并总是取最大的可能数值加成。

将这些圣遗物的属性分为自身加成init_artifact_data（特殊加成get_extra_special)、有关转化类的加成（modify_useful_attribute、get_convert_value、get_extra_rate_value），对队友的加成（get_team_data)

### 如何更新数据

在每个池子更新的当天，需要做如下工作：

1、检查上个版本的预览数据的正确性

2、更新下个版本的预览角色、武器、圣遗物等数据

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

根据用户定义的topk，保留配置（人物-武器）中伤害最大的topk个配置作为结果。

2022/10/13更新：

添加了技能组计算的功能，现在的计算方法如下：

1、按照计算配置定义人物的队伍配置、攻击序列。

2、利用遍历的方法获取每一个可能的武器-圣遗物-主属性组合，并根据上述组合、队伍配置、攻击方式获取数据，如果每个攻击方式都获取不到加成，就检查下一个组合。

3、对每一个可能的副词条数量，对每一个攻击方式，按照 https://nga.178.com/read.php?tid=25564438 中所述的伤害计算公式进行计算，最后取伤害和的最大值作为该配置的结果。

4、根据用户定义的topk，保留配置（人物-武器）中伤害最大的topk个配置作为结果。

### 计算配置

用户需要按如下规则定义人物的攻击状态：

（攻击元素、攻击者武器类型、攻击方式）反应类型、可能的怪物附着元素、最大词条数、生命有效？、攻击有效？、防御有效？、精通有效？、充能有效？、暴击有效？、爆伤有效？、伤害加成有效？、治疗加成有效？、护盾强效有效？、队友1、队友2、队友3、队友提供加成的武器/圣遗物

2022/10/13更新：

添加了技能组计算的功能，现在的配置定义如下：

1、队伍配置：队友1；队友2；队友3；队伍加成类武器和圣遗物；敌人的元素附着；允许的扩散元素

2、攻击序列：n*攻击方式：（攻击元素、攻击者武器类型、攻击方式）；前台/后台；锁面板/不锁面板；反应类型；生命有效？；攻击有效？；防御有效？；精通有效？；充能有效？；暴击有效？；爆伤有效？；伤害加成有效？；治疗加成有效？；护盾强效有效？；攻击次数

3、计算组合：武器限定；圣遗物1限定；圣遗物2限定；主属性3限定；主属性4限定；主属性5限定；队伍配置；攻击序列；是否保证循环；

2022/12/21更新：

需要特别注意的是：1）是否锁面板可以填任意值 2）同一技能的反应类型请务必保持一致，不发生反应的请在之后添加no_add_damage；技能全部不发生反应请填入"NONE"；

### 其他功能

增量更新：

在每个版本数据更新后，可以对add_character/add_weapon/add_artifact三个参数设定为更新的数据名称，若武器、圣遗物均未更新，则程序只会计算更新的角色；若圣遗物未更新，程序只会计算（原角色-新武器）、（新角色-武器）的配置；若圣遗物更新，程序会全部重新计算

圣遗物搭配：

1、使用圣遗物扫描器对数据进行扫描，导出为莫娜占卜铺的格式。

2、使用Filter_Artifact项目将扫描得到的圣遗物文件转化为本程序能够识别的格式。

3、运行本程序即可根据定义的计算配置和2中的圣遗物数据计算得到最佳的圣遗物搭配。

gcsim脚本生成：

1、在get_build_up中设置角色搭配（建议先使用计算功能考虑较好的搭配以减少生成的比对脚本数量），在RESULTS/chinese_config/中按照特定格式书写队伍搭配和攻击序列。

2、使用程序生成符合gcsim优化的脚本（包括武器、圣遗物、主属性），并进一步按照gcsim的流程进行最优副词条求解以及模拟。

### 运行注意事项

使用clion，在不同系统下需要首先建立c++项目（pure c++），之后仅需要将.cpp文件、.h文件、cmake文件保留。

在MAC中，请将文件的编码改为UTF-8，即可正常运行。

在WIN中，请首先在设置中将项目的文件编码改为UTF-8，并在编辑界面将所有文件的编码改为GBK，并使用ctrl+alt+shift+.打开控制台注册表界面，并勾选run.process.with.pty，如此可解决运行过程中中文乱码的问题。但是在调试过程中仍会出现中文乱码和显示string为16进制字符的问题。

目前来说，拥有5个有效词条的人物-武器对计算时间在5-10s，拥有4个有效词条的人物-武器对计算时间在0.5-1s（cpu：12400f）。

### 已知问题

1、角色存在属性转化中，未对角色能够获取的属性加成区分是否能够转化（在面板、不在面板、在怪物），目前主要不能转化的属性集中在增伤、暴击、暴伤上，而这些属性目前不参与转化。

2、所有的叠层武器和具有buff时间限制的武器由于复杂性问题在程序中均忽略，仅考虑触发的条件并且总是以角色能够获得的最大层数进行计算。（该问题目前的解决方法是，采用gcsim_script_generator程序，手动选择较好可行的搭配，转化成脚本并进一步在gcsim中进行最佳副词条和真实的模拟演算）

### 未来工作

1、考虑组合伤害的计算（技能组、技能+剧变） FINISH 2022/10/13

2、添加更多角色（主要是5星）

3、配置文件化

4、GUI

5、多线程并行计算