<p>新增数据库（mysql）维护方式:</p>
<p>现在可以使用数据库（mysql）维护数据了</p>
<p>想要从文本文件切换为由数据库维护交通信息只需在subway.cpp文件头部加上#define USE_SQL宏即可，默认已加上</p>
<p>第一次使用数据库时可先将文本文件的数据导入到数据库里，方法是先创建名为“InuseLines”和“UnuseLines”的mysql数据库文件（明天更新完后就不用自行创建了），然后在subway.cpp文件头部加上#define FILE_TO_SQL宏，重新编译，并运行一次即可，注意要关闭USE_SQL宏</p>
<p>USE_SQL和FILE_TO_SQL均没被定义时恢复到文本文件维护方式</p>



<p>下一次将更新</p>
<p>替换SUBWAY::GetAllPaths方法内用到的容器类型</p>
