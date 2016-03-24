# WARNING: THE DEVELOPMENT OF THIS PROJECT HAS BEEN OFFICIALLY STOPPED.

##General Information
Linnya is an excellent multimedia application for *nix users, which
supports music library, playlist, lyrics, and more.

Linnya is a free software, and published under the GNU GPLv3 Licence,
you can easily find a copy of GPLv3 in the same directory of this file,
Or from the official GNU GPL website http://www.gnu.org/licenses/gpl.html.
If you do not understand English well, the translated version of GPL licence (
http://www.gnu.org/licenses/translations.html) may do you a great favor.

__WARNNING: If anybody want to modify or republish Linnya, you MUST go
with the GNU GPLv3. To FULLY understand the licence is your responsibility.__


##About Us
* Official website: [http://www.linnya.org](http://www.linnya.org)
* Contact E-mail: [herenvarno@workmail.com](mailto:herenvarno@workmail.com)


##Installation
1. Configure your system information to create the makefile. Use the command
below:
```
./autogen
./configure --prefix=/usr
```

2. Compile all the source file to excuteable file. Use the command below:
```
make
```

3. Copy files to the proper directory, Usually in /usr/share/linnya/... .
Use the command below(please be root):
```
make install
```

4. Clean up all the temp file for next compilation. Use the command below:
```
./clear
```

5. If you are using any DE other than GNOME 3, You should probably install a 
standard gnome 3 theme -- Adwaita -- to make linnya look normal. To achive this,
Use your package manager to install the "gnome-themes-standard" package. 
For example, If you use Archlinux, just type:
```
pacman -S gnome-themes-standard
```

##DEVELOPMENT
The 2.0 version of Linnya is now STABLE, enjoy!


##简介
聆雅音乐播放器是一款为*nix系统使用者量身定制的多媒体软件。它支持音乐库，播放列表，同步歌词，
去除字符乱码等多种功能。

聆雅音乐播放器是一款在GNU GPLv3的授权下发布的自由软件。你可以在本文件的同目录下找到一份
GPLv3的拷贝，也可以访问GNU的官方网站查询，网址为：
http://www.gnu.org/licenses/gpl.html 。不懂英文的用户可以在这个链接里找到你能看懂的版本：
http://www.gnu.org/licenses/translations.html 。

__警告：如果有谁想要修改或再发布本软件，你必须严格遵照GPLv3的授权执行。你有责任和义务
弄懂GPLv3授权协议的每一条规定。__


##关于
* 官方网站:	[http://www.linnya.org](http://www.linnya.org)
* 联系方式:	[herenvarno@workmail.com](mailto:herenvarno@workmail.com)


##安装
1. 运行配置程序生成makefile文件，命令如下：
```
./autogen
./configure --prefix=/usr
```

2. 编译所有的源文件，命令如下：
```
make
```

3. 安装软件，通常的位置为 /usr/share/linnya/... ，命令如下（请使用root权限）：
```
make install
```

4. 建议你清理不用的交换文件以便下次编译，命令如下：
```
./clear
```

5. 如果你不是使用的GNOME 3桌面环境，你可能需要安装一个标准gnome 3主题（Adwaita）才能让聆雅
外观正常。这可以使用你的软件包管理器安装“gnome-themes-standard”包。例如，如果你使用Archlinux，
只需要在终端输入：
```
pacman -S gnome-themes-standard
```

##开发
聆雅2.0版本已稳定，祝您使用愉快！
