# vcxproj_cmd_modfier
A tool for modifying visual studio project files on the cmd line.
## Discription
  A tool for modifying visual studio project files on the cmd line. Based on this tool, the vcxproj files based on XML can be modified by cmd commands and parameters. 
## USAGE
  After the source file included in the project is updated, the updated source code can be added automatically by executing the configured batch command.
## USECASE1
  In a C / C + + project based on visual studio, the third-party software model tool generates new code or source file updates in the code base. Then you have to readd the source code file for the project to update project solution. When using visual studio GUI to add or replace more than 200 files, the GUI operation always responds slowly,30 seconds at the fastest and 3 minutes at the slowest. Using this tool with batch script, you can update the file within 10s.
## USECASE2
  In a C / C + + project based on visual studio, a CI/CD system wants to update the project automatically according to the source code or software model changes. Although cmake can generate makefiles automatically to realize automatic integration, it still needs to install corresponding dependent environment, and it will take a lot of effort to cooperate with visual studio. However, in VSC projects in windows, this tool can automatically integrate project files without installing any external environment.
## GOAL
The goal of this tool is to realize the automatic maintenance of the C / C + + project developed by visual studio, so that the project can be integrated automatically.

## 简介
  一个在命令行中对visual studio 项目文件进行修改的工具。 基于该工具可以利用命令行命令和参数对基于xml的工程定义文件进行修改。 
## 用法
  工程中包含的源码文件更新后，执行配置好的批处理命令即可自动添加更新后的源码。

## 应用场景1
  在一个基于Visual Studio开发的C/C++项目中，第三方软件模型工具生成了新的代码或者代码库中源码文件更新。此时工程更新模型或库的方法就是清空工程中指定filter中原来包含的文件，然后重新添加。使用Visual Studio进行文件添加或者替换超过200个以上的文件时，界面总是响应很慢，快则30s，慢则三分钟。而使用本工具配合批处理脚本，可以在10s内搞定文件的更新。

## 应用场景2
  在一个基于Visual Studio开发的C/C++项目中，持续集成系统想要根据源码或者软件模型的变动自动更新工程。虽然CMAKE可以自动生成makefile实现自动集成，但是还得安装相应的依赖环境，想要配合visual studio还得费一番功夫。但是在Windows中 ，本工具可以在没有安装任何外部环境的情况下自动集成工程文件。
## 本工具的目标
  实现对Visual Studio开发C/C++项目工程的自动维护，以至于可以进行工程的自动集成。
