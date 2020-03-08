# Vim-ESC
Vim，按键ESC关闭大写和输入法。支持Visual Studio扩展VsVim，SlickEdit。
无条件切换到小写和英文输入，再也不用担心输错命令了。

## VsVim
修改文件VsCommandTarget.cs，具体见文件。可从项目中排除ImeCoordinator.cs。
重新编译得到Vim.VisualStudio.Shared.dll。
VsVim.vsix中更新Vim.VisualStudio.Shared.dll，得到新的压缩包，双击安装扩展。

## SlickEdit
编译capslock.cpp得到capslock.dll。
在SlickEdit中，打开命令行，输入'dload somepath\capslock.dll'。
