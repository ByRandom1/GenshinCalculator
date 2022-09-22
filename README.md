# GenshinCalculator
一个用于计算原神人物-武器-圣遗物（套装、主词条、副词条）最佳搭配的计算器。

### 人物建构

具体资料以原神wiki：https://wiki.biligame.com/ys/ 和 https://genshin.honeyhunterworld.com 为准。

目前仅构建了胡桃1、神里绫华0、雷电将军0、甘雨0、夜兰0、行秋6、香菱6、八重神子1、枫原万叶0、班尼特5、温迪0、莫娜1、钟离0。

将这些人物的属性分为自身加成init_character_data（特殊加成get_extra_special)、有关转化类的加成（modify_useful_attribute、get_convert_value、get_extra_rate_value），对队友的加成（get_team_data)，充能考虑（check_recharge_requirement）

### 武器建构

具体资料以原神wiki：https://wiki.biligame.com/ys/ 和 https://genshin.honeyhunterworld.com 为准。

构建了所有武器，构建时忽略了持续时间的限制，而仅考虑触发条件的限制，并总是取最大的可能数值加成。

将这些武器的属性分为自身加成init_weapon_data（特殊加成get_extra_special)、有关转化类的加成（modify_useful_attribute、get_convert_value、get_extra_rate_value），对队友的加成（get_team_data)

### 圣遗物建构

具体资料以原神wiki：https://wiki.biligame.com/ys/ 和 https://genshin.honeyhunterworld.com 为准。

构建了所有圣遗物，忽略了染血4件套和追忆4件套的加成（条件苛刻，效果太强）。

将这些圣遗物的属性分为自身加成init_artifact_data（特殊加成get_extra_special)、有关转化类的加成（modify_useful_attribute、get_convert_value、get_extra_rate_value），对队友的加成（get_team_data)

### 如何更新数据

在每个版本的第一天，需要做如下工作：

1、检查上个版本的预览数据的正确性

2、更新下个版本的预览角色、武器、圣遗物等数据

### 最佳搭配条件限制

首先，针对武器、圣遗物套装、圣遗物主词条，在init_check_data做了限制，不提供任何有效词条加成的内容被剔除，这一功能可以通过设置filter_type进行开关。

其次，不满足充能要求的配置被剔除。

最后，针对圣遗物副词条，采用如下几点限制：

1、不考虑小生命、小攻击、小防御

2、总词条被分为base和up，base指的是最开始的4个基准词条，up指的是在基准词条上强化的词条

​	  有$\sum base\le 20$，$\sum up\le 20$，每类词条的$base\le$可能出现的位置，每类词条的$up\le \min(base*4,可能出现位置*3-base)$，$\sum base+\sum up\le 配置设定的最大词条数$。

​	  例如：采用攻伤暴的圣遗物主词条配置，给定副词条最大数量30，则攻击、暴击最多$3*4=12$条，其中$base\le 4$，$up\le \min(base*4,4*3-base)$，生命、防御、元素精通、元素充能效率、爆伤最多$3*5=15$条，其中$base\le 5$，$up\le \min(base*4,5*3-base)$，最后还要保证$\sum base\le 20$，$\sum up\le 20$，$\sum base+\sum up\le 30$）

### 计算算法

首先获得有效配置（人物-武器-圣遗物套装1-圣遗物套装2-圣遗物主词条），其次采用遍历的方法，结合上述限制，对每一个可能的副词条数量，按照 https://nga.178.com/read.php?tid=25564438 中所述的伤害计算公式进行计算，最后取伤害最大值作为该配置的结果。

根据用户定义的topk，保留配置（人物-武器）中伤害最大的topk个配置作为结果。

### 计算配置

用户需要按如下规则定义人物的攻击状态：

（攻击元素、攻击者武器类型、攻击方式）反应类型、可能的怪物附着元素、最大词条数、生命有效？、攻击有效？、防御有效？、精通有效？、充能有效？、暴击有效？、爆伤有效？、伤害加成有效？、治疗加成有效？、护盾强效有效？、队友1、队友2、队友3、队友提供加成的武器/圣遗物

### 运行注意事项

使用clion，在不同系统下需要首先建立c++项目（pure c++），之后仅需要将.cpp文件、.h文件、cmake文件保留。

在MAC中，请将文件的编码改为UTF-8，即可正常运行。

在WIN中，请首先在设置中将项目的文件编码改为UTF-8，并在编辑界面将所有文件的编码改为GBK，并使用ctrl+alt+shift+.打开控制台注册表界面，并勾选run.process.with.pty，如此可解决运行过程中中文乱码的问题。但是在调试过程中仍会出现中文乱码和显示string为16进制字符的问题。

### 未来工作

1、考虑组合伤害的计算（技能组、技能+剧变）

2、添加更多角色（主要是5星）

3、配置文件化

4、GUI
