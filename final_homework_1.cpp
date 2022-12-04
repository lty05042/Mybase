#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <conio.h>
#include <string.h>
#include <iomanip>
#include <algorithm>
#include <math.h>

using namespace std;

void gotoxy(int x,int y)
{
    COORD p ;
    p.X=x;
    p.Y=y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),p);
}

// 全局变量声明

const int MAXSPACE = 1010; // 空地数可能上限 // 暂时设个1010（

int CurrentLevel; // 当前关卡
int MaxLevel = 0; //最高通过关卡

int InputBelt[MAXSPACE]; // 输入序列
vector <int> TargetOutputBelt; // 目标输出序列

int AvaibleOpenSpaceNumber; // 可用空地数
string AvailableOrderSet[10]; // 可用指令集

char info_title[4][20] = {"第 一 关","第 二 关","第 三 关","第 四 关"};
char info_input[4][100] = {"输入序列：1, 2","输入序列：3,9,5,1,-2,-2,9,-9"," 输入序列：6,2,7,7,-9,3,-3,-3"," "};
char info_task[4][200] = {"关卡任务：让机器人取出输入序列上的每个积木放入输出序列中","关卡任务：对于输入序列中的每两个东西，先把第 1 个减去第 2 个，并把结果放在输出序列中，然后把第 2 个减去第 1 个，再把结果放在输出序列中，重复。","关卡任务：从输入序列中依次取 2 个数字，如果相等则将其中一个输出，否则扔掉。重复这个过程直到输入传送带为空。"," "};
char info_openspace[4][100] = {"可用空地数：0","可用空地数：3","可用空地数：3"," "};
char info_command[4][100] = {"可用指令集：inbox,outbox","可用指令集：inbox,outbox,copyfrom,copyto,add,sub,jump,jumpifzero","可用指令集：inbox,outbox,copyfrom,copyto,add,sub,jump,jumpifzero"," "};


// 机器人和传送带设置

class Level{
    public:
        // 机器人和传送带的参数
        
        bool CurrentBlockExist; // 是否存在当前积木
        int CurrentBlock; // 当前积木数字
        vector <int> InputBeltBlocks; // 输入传送带上积木
        vector <int> OutputBeltBlocks; // 输出传送带上积木
		int OpenSpaceBlocks[10]; // 空地上积木 编号从0开始
        bool OpenSpaceBlocksExist[MAXSPACE]; // 是否存在空地上积木
        int SpaceNum; // 空地数
        int CurrentInstruction; // 当前指令
        int InstructionNum; // 指令数 编号从1开始
        bool EndState; // 是否结束游戏 true为是

        void ErrorProcess() // 异常情况处理
        {
			system("cls");
			gotoxy(50,12);
            cout << "Error on instruction  " << CurrentInstruction+1 << endl; // 输出异常指令
            EndState = true; // 结束游戏
			Sleep(3000);
            return ;
        }

        // 指令集

        void EndJudgeByInput() // 判断游戏是否因输入传送带结束
        {
            if (InputBeltBlocks.empty()) // 如果输入传送带没有积木
                EndState = true; // 游戏结束
            return ;
        }

        void inbox() // inbox操作前应检查EndjudgeByInput
        {
            EndJudgeByInput();
            CurrentBlock = InputBeltBlocks[0]; // 拿起输入传送带的第一个积木，且原有当前积木被舍弃
            InputBeltBlocks.erase(InputBeltBlocks.begin()); // 传送带上积木减少
            CurrentBlockExist = true; // 当前积木存在
        }

        void outbox()
        {
            if (CurrentBlockExist == false)
            {
                ErrorProcess();
                return ;
            }
            OutputBeltBlocks.push_back(CurrentBlock);
            CurrentBlock = 0;
            CurrentBlockExist = false;
        }

        void add(float AddPlaceNumber)
        {
			if (floor(AddPlaceNumber) != AddPlaceNumber) // 如果不是整数
			{
				ErrorProcess();
                return ;
			}
            if (CurrentBlockExist == false) // 如果没有当前积木
            {
                ErrorProcess();
                return ;
            }
            if (AddPlaceNumber > SpaceNum || AddPlaceNumber <= 0) // 如果X号空地不存在
            {
                ErrorProcess();
                return ;
            }
            if (OpenSpaceBlocksExist[(int)AddPlaceNumber] == false) // 如果空地中没有积木
            {
                ErrorProcess();
                return ;
            }
            CurrentBlock += OpenSpaceBlocks[(int)AddPlaceNumber]; // 当前积木数字加上空地积木数字
        }

        void sub(float SubPlaceNumber)
        {
			if (floor(SubPlaceNumber) != SubPlaceNumber) // 如果不是整数
			{
				ErrorProcess();
                return ;
			}
            if (CurrentBlockExist == false) // 如果没有当前积木
            {
                ErrorProcess();
                return ;
            }
            if (SubPlaceNumber > SpaceNum || SubPlaceNumber <= 0) // 如果X号空地不存在
            {
                ErrorProcess();
                return ;
            }
            if (OpenSpaceBlocksExist[(int)SubPlaceNumber] == false) // 如果空地中没有积木
            {
                ErrorProcess();
                return ;
            }
            CurrentBlock -= OpenSpaceBlocks[(int)SubPlaceNumber]; // 当前积木数字减去空地积木数字
        }

        void Copyto(float CopytoPlaceNumber)
        {
			if (floor(CopytoPlaceNumber) != CopytoPlaceNumber) // 如果不是整数
			{
				ErrorProcess();
                return ;
			}
            if (CurrentBlockExist == false) // 如果没有当前积木
            {
                ErrorProcess();
                return ;
            }
            if (CopytoPlaceNumber > SpaceNum || CopytoPlaceNumber <= 0) // 如果X号空地不存在
            {
                ErrorProcess();
                return ;
            }
            OpenSpaceBlocks[(int)CopytoPlaceNumber] = CurrentBlock; // 复制当前积木到X号空地
        }

        void Copyfrom(float CopyfromPlaceNumber)
        {
			if (floor(CopyfromPlaceNumber) != CopyfromPlaceNumber) // 如果不是整数
			{
				ErrorProcess();
                return ;
			}
            if (CopyfromPlaceNumber > SpaceNum || CopyfromPlaceNumber <= 0) // 如果X号空地不存在
            {
                ErrorProcess();
                return ;
            }
            if (OpenSpaceBlocksExist[(int)CopyfromPlaceNumber] == false) // X号空地没有积木
            {
                ErrorProcess();
                return ;
            }
            CurrentBlock = OpenSpaceBlocks[(int)CopyfromPlaceNumber]; // 复制X号空地的积木到当前积木
        }

        void jump(float JumpToOrderNumber)
        {
			if (floor(JumpToOrderNumber) != JumpToOrderNumber) // 如果不是整数
			{
				ErrorProcess();
                return ;
			}
            if (JumpToOrderNumber > InstructionNum || JumpToOrderNumber <= 0) // 不存在第X条指令
            {
                ErrorProcess();
                return ;
            }
            CurrentInstruction = (int)JumpToOrderNumber; // 修改当前指令为第X条
        }

        void jumpifzero(int JumpToOrderNumber)
        {
			if (floor(JumpToOrderNumber) != JumpToOrderNumber) // 如果不是整数
			{
				ErrorProcess();
                return ;
			}
            if (CurrentBlockExist == false) // 没有当前积木
            {
                ErrorProcess();
                return ;
            }
            if (JumpToOrderNumber > InstructionNum || JumpToOrderNumber <= 0) // 不存在第X条指令
            {
                ErrorProcess();
                return ;
            }
            if (CurrentBlock != 0) // 如果当前积木不为0
                return ;
            CurrentInstruction = (int)JumpToOrderNumber; // 修改当前指令为第X条
        }

		int checkinput(string input) // 返回input中空格数
		{
			int ans = 0;
			for (int i=0; i<input.size(); i++)
				if (input[i] == ' ')
					ans++;
			return ans;
		}

        void OrderExecution1(string input)
        {
            if (input == "inbox")
                inbox();
            else if (input == "outbox")
                outbox();
            else
                ErrorProcess();
        }

		void OrderExecution2(string input)
		{
			if (input == "inbox")
                inbox();
            else if (input == "outbox")
                outbox();
			else if (input.substr(0, 8) == "copyfrom")
			{
				if (checkinput(input) != 1) // 检查操作数个数
				{
					ErrorProcess();
					return ;
				}
				Copyfrom(stof(input.substr(9, input.length()))); // 直到末尾的数字
			}
			else if (input.substr(0, 6) == "copyto")
			{
				if (checkinput(input) != 1)
				{
					ErrorProcess();
					return ;
				}
				Copyto(stof(input.substr(7, input.length())));
			}
			else if (input.substr(0, 3) == "add")
			{
				if (checkinput(input) != 1)
				{
					ErrorProcess();
					return ;
				}
				add(stof(input.substr(4, input.length())));
			}
			else if (input.substr(0, 3) == "sub")
			{
				if (checkinput(input) != 1)
				{
					ErrorProcess();
					return ;
				}
				sub(stof(input.substr(4, input.length())));
			}
			else if (input.substr(0, 4) == "jump")
			{
				if (checkinput(input) != 1)
				{
					ErrorProcess();
					return ;
				}
				jump(stof(input.substr(5, input.length())));
			}
			else if (input.substr(0, 10) == "jumpifzero")
			{
				if (checkinput(input) != 1)
				{
					ErrorProcess();
					return ;
				}
				jumpifzero(stof(input.substr(11, input.length())));
			}
            else
                ErrorProcess();
		}
};

// 各种读取

vector<int> ArchiveFileLoading() // 读取存档
{
	ofstream fout;
	fout.open("archive.txt",ios::app);
	ifstream fin("archive.txt");

	vector<int> game_level;
	while(!fin.eof())
	{
		int lv = 0;
		fin >> lv;
		if(lv)
			game_level.push_back(lv);
	}
	vector<int> temp = game_level;
	sort(temp.begin(),temp.end());
	MaxLevel = temp.back();

	return game_level;
}

vector<string> CommandFileLoading() // 用文件读取指令集
{
	string file_path = "";
	file_exception:
	cout << "请输入指令文件路径：";
	cin >> file_path;
	ifstream fin;
	fin.open(file_path);

	if(!fin)
	{
		cout << "文件路径有误！" << endl;
		goto file_exception;
	}
	else
	{
		int command_num = 0;
		fin >> command_num;
		vector<string> command(command_num);

		for(int i = 0;i < command_num;i++)
			fin >> command[i];

		fin.close();
		return command;
	}
}

vector<string> CommandInput() // 用键盘输入读取指令集
{
	int command_num = 0;
	cout << "请输入程序的指令数量:";
	cin >> command_num;

	vector<string> command(command_num);
	cout << "请输入指令：";
	for(int i = 0;i < command_num;i++ )
	{
		cin >> command[i];
	}

	return command;
}

vector<string> (*pf_CommandInput[2])() = {CommandFileLoading,CommandInput};		//指令输入函数跳转表

// 处理选择关卡指令, 修改当前关卡

void SelectionProcess() 
{
	vector<int> game_archive = ArchiveFileLoading();

	char* str_title = "关卡选择";
	int len_title = strlen(str_title);
	char* str_instruct = "请选择关卡（通过键盘输入）：";
	int len_instruct = strlen(str_instruct);
	char* line = "---------------";
	char* arrow = "- - - - - - - >";

	HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE);    //获得标准输出设备句柄  
    CONSOLE_SCREEN_BUFFER_INFO csbi;    //定义窗口缓冲区信息结构体  
    GetConsoleScreenBufferInfo(handle_out, &csbi);  //获得窗口缓冲区信息
	
	label_select:
	int x = 0,y = 0;
	x = (csbi.dwSize.X - len_title) / 2 - 2;    //左边界，为了让输出的字符串居中
	y = 4;
	gotoxy(x,y);
	cout << str_title << endl;					//输出标题

	x = (csbi.dwSize.X - strlen(line)*3) / 2 - 2;y += 2;
	gotoxy(x,y);
	for(int i = 0;i < 4;i++)
	{
		cout << '*';
		gotoxy(x+(strlen(line)+1)*(i+1),y);
	}											
	x -= 2;y++;
	gotoxy(x,y);
	for(int i = 0;i < 4;i++)
	{
		cout << "关卡-" << i+1 ;
		gotoxy(x+strlen(line)*(i+1),y);
	}
	x += 3;y--;
	gotoxy(x,y);
	if(game_archive.size() != 0)
	{
		for(int i = 0;i < game_archive.size()-1;i++)
		{
			cout << line;
			x += strlen(line)+1;
			gotoxy(x,y);
		}
		if(game_archive.size() != 4)
			cout << arrow;
	}						//输出关卡名称和通关情况

	x = (csbi.dwSize.X - len_instruct-2) / 2 - 2;y = 10;
	gotoxy(x,y);
	cout << str_instruct;

	int level_chosen = 0;					//关卡选择
	char temp[10] = "";
	cin >> temp;
	if(strlen(temp) > 1 || (strlen(temp) == 1 && !isdigit(temp[0])));
	else
		level_chosen = atoi(temp);
	if(1 <= level_chosen && level_chosen <= game_archive.size()+1)
	{
		CurrentLevel = level_chosen;
		
		Sleep(300);
		system("cls");

		return;
	}
	else
	{
		x = (csbi.dwSize.X - strlen("无法选择该关卡！")-2) / 2 - 2;y += 2;
		gotoxy(x,y);
		SetConsoleTextAttribute(handle_out, FOREGROUND_RED); 
		cout << "无法选择该关卡！";
		SetConsoleTextAttribute(handle_out, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
		Sleep(1*1000);
		system("cls");
		goto label_select;
	}

}

// 各种界面输出

void PrintLevelInfo(char* title,char* input,char* output,char* openspace,char* command) // 输出关卡信息
{
	HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE);    //获得标准输出设备句柄  
    CONSOLE_SCREEN_BUFFER_INFO csbi;    //定义窗口缓冲区信息结构体  
    GetConsoleScreenBufferInfo(handle_out, &csbi);  //获得窗口缓冲区信息

	int x = 0,y = 0;

	x = (csbi.dwSize.X - strlen(title)) / 2 - 2;    
	y = 4;
	gotoxy(x,y);
	cout << title << endl;					//输出标题

	x = (csbi.dwSize.X - strlen(output)) / 2 - 2;    
	y += 2;
	gotoxy(x,y);
	cout << output << endl;					//输出输出要求

	x = (csbi.dwSize.X - strlen(input)) / 2 - 2;    
	y += 2;
	gotoxy(x,y);
	cout << input << endl;					//输出输入序列

	x = (csbi.dwSize.X - strlen(openspace)) / 2 - 2;    
	y += 2;
	gotoxy(x,y);
	cout << openspace << endl;					//输出可用空地

	x = (csbi.dwSize.X - strlen(command)) / 2 - 2;    
	y += 2;
	gotoxy(x,y);
	cout << command << endl;					//输出可用指令集

	Sleep(500);
	x = (csbi.dwSize.X - strlen("（按回车键继续）")) / 2 - 2;    
	y += 4;
	gotoxy(x,y);
	cout << "（按回车键继续）" << endl;					//输出可用指令集

	INPUT_RECORD keyrec;        //定义输入事件结构体  
	DWORD res;					//定义返回记录  
	for (;;)  
	{
		HANDLE handle_in = GetStdHandle(STD_INPUT_HANDLE);      //获得标准输入设备句柄
		ReadConsoleInput(handle_in, &keyrec, 1, &res);      //读取输入事件  
		if (keyrec.EventType == KEY_EVENT)      //如果当前事件是键盘事件  
		{  
			if (keyrec.Event.KeyEvent.wVirtualKeyCode == VK_RETURN  && keyrec.Event.KeyEvent.bKeyDown == true) //当前事件的虚拟键为Enter键  
			{  
				break;
			}  
		}  
	}
		
	system("cls");
}

void  PrintLevelBackground(vector<string> &cmd)	//输出关卡背景
{
	HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE);    //获得标准输出设备句柄  
    CONSOLE_SCREEN_BUFFER_INFO csbi;    //定义窗口缓冲区信息结构体  
    GetConsoleScreenBufferInfo(handle_out, &csbi);  //获得窗口缓冲区信息

	char* frame = "+---+";
	char* frame_1 = "|   |";

	int x = 0,y = 0;

	x = 8;y = 4;		
	gotoxy(1,y+1);
	cout << "IN";
	for(int i = 0;i < 12;i++)	//输入传送带边框
	{
		gotoxy(x,y);
		cout << frame;
		y++;
		gotoxy(x,y);
		if(y % 4 == 1)
			cout << frame_1;
		y++;
	}

	x = 20;y = 4;		//当前积木边框
	for(int i = 0;i < 2;i++)
	{
		gotoxy(x,y);
		cout << frame;
		y++;
		gotoxy(x,y);
		if(y % 4 == 1)
			cout << frame_1;
		y++;
	}

	gotoxy(x,y);			//机器人
	cout << "@   @";
	y++; gotoxy(x,y);
	cout << "-----";
	y++; gotoxy(x,y);
	cout << "|@ @|";
	y++; gotoxy(x,y);
	cout << "  +  ";
	y++; gotoxy(x,y);
	cout << "/   \\";
	y++; gotoxy(x,y);
	cout << " | | ";

	y = 16; gotoxy(x,y);		//空地
	for(int i = 0;i < 4;i++)
	{
		gotoxy(x,y);
		cout << frame;
		gotoxy(x,y+1);
		cout << frame_1;
		gotoxy(x,y+2);
		cout << frame;
		x += (strlen(frame)+1);
	}

	x = 22;y = 19;gotoxy(x,y);		//空地编号
	for(int i = 0;i < 4;i++)
	{
		cout << i+1;
		x += 6;
		gotoxy(x,y);
	}

	x = 50;y = 4;				//输出传送带边框
	for(int i = 0;i < 12;i++)	
	{
		gotoxy(x,y);
		cout << frame;
		y++;
		gotoxy(x,y);
		if(y % 4 == 1)
			cout << frame_1;
		y++;
	}
	gotoxy(x+7,5);
	cout << "OUT";

	x = 65;y = 4;gotoxy(x,y);		//分隔线
	for(int i = 0;i < 27;i++)
	{
		cout << "|";
		y++; gotoxy(x,y);
	}

	x = 70;y = 4;gotoxy(x,y);
	cout << "===== CODE =====";
	y += 2;
	gotoxy(x-2,y+1);cout << '>';gotoxy(x,y);
	for(int i = 0;i < cmd.size();i++)
	{
		y++;gotoxy(x,y);
		cout << i+1 << ' ' << cmd[i];
		y++;
	}

}

void ScreenRefresh(vector<int> &input_belt,vector<int> &output_belt,int openspace[],int currentblock, int spacenum, Level lvl)		//逐步刷新输出内容
{
	HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE);    //获得标准输出设备句柄  
    CONSOLE_SCREEN_BUFFER_INFO csbi;    //定义窗口缓冲区信息结构体  
    GetConsoleScreenBufferInfo(handle_out, &csbi);  //获得窗口缓冲区信息

	int x = 0,y = 0;

	x = 10;y = 5;gotoxy(x,y);			//修改输入传送带
	for(int i = 0;i < 6;i++)
	{
		if(i < input_belt.size())
		{
			char temp[4] = "";
			itoa(input_belt[i],temp,10);
			if(strlen(temp) == 3)
			{
				gotoxy(x-1,y);
				cout << temp;
				y += 4;gotoxy(x,y);
			}
			else
			{
				cout << temp;
				y += 4;gotoxy(x,y);
			}
		}
		else
		{
			cout << 'X';
			y += 4;gotoxy(x,y);
		}
	}

	x = 52;y = 5;gotoxy(x,y);			//修改输出传送带
	for(int i = 0;i < 6;i++)
	{
		if(i < output_belt.size())
		{
			char temp[4] = "";
			itoa(output_belt[i],temp,10);
			if(strlen(temp) == 3)
			{
				gotoxy(x-1,y);
				cout << temp;
				y += 4;gotoxy(x,y);
			}
			else
			{
				cout << temp;
				y += 4;gotoxy(x,y);
			}
		}
		else
		{
			cout << 'X';
			y += 4;gotoxy(x,y);
		}
	}

	x = 22;y = 17;gotoxy(x,y);			//修改空地
	for(int i = 1;i <= 4;i++)
	{
		if(i <= spacenum)
		{
			char temp[4] = "";
			itoa(openspace[i-1],temp,10);
			if(strlen(temp) == 3)
			{
				gotoxy(x-1,y);
				cout << temp;
				x += 6;gotoxy(x,y);
			}
			else
			{
				cout << temp;
				x += 6;gotoxy(x,y);
			}
		}
		else
		{
			cout << 'X';
			x += 6;gotoxy(x,y);
		}
	}

	x = 22;y = 5;gotoxy(x,y);
    if (lvl.CurrentBlockExist == false)
        cout << 'X';
    else
	    cout << currentblock;
}

void move_arrow(int instruction_idx, int instruction_num)
{
	HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE);    //获得标准输出设备句柄  
	CONSOLE_SCREEN_BUFFER_INFO csbi;    //定义窗口缓冲区信息结构体  
	GetConsoleScreenBufferInfo(handle_out, &csbi);  //获得窗口缓冲区信息

	SMALL_RECT scroll;      //定义移动区域  
    COORD pos = {68 ,6 + (instruction_idx+1)*2};     //移动位置  
    CHAR_INFO chFill;       //定义填充字符
	
	//定义填充字符的各个参数及属性
	chFill.Char.AsciiChar = ' ';  
    chFill.Attributes = csbi.wAttributes;

	//确定区域
	scroll.Top = 6 + (instruction_idx)*2;
	scroll.Bottom = scroll.Top+1;
	scroll.Left = 68;
	scroll.Right = scroll.Left+1;

	if(instruction_idx < instruction_num - 1)
		ScrollConsoleScreenBuffer(handle_out, &scroll, NULL, pos, &chFill); //移动文本
}


void PrintEndScreen(string prompt)			//关卡结束界面 
{
	HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE);    //获得标准输出设备句柄  
	CONSOLE_SCREEN_BUFFER_INFO csbi;    //定义窗口缓冲区信息结构体  
	GetConsoleScreenBufferInfo(handle_out, &csbi);  //获得窗口缓冲区信息

	SetConsoleTextAttribute(handle_out, FOREGROUND_RED); 
	int x = (csbi.dwSize.X - prompt.length()) / 2 - 2;
	int y = 12;
	gotoxy(x,y);
	cout << prompt;
	SetConsoleTextAttribute(handle_out, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);

	x = (csbi.dwSize.X - strlen("（按回车键返回）")) / 2 - 2;
	y += 2;
	gotoxy(x,y);
	cout << "（按回车键返回）";

	INPUT_RECORD keyrec;        //定义输入事件结构体  
	DWORD res;					//定义返回记录  
	for (;;)  
	{
		HANDLE handle_in = GetStdHandle(STD_INPUT_HANDLE);      //获得标准输入设备句柄
		ReadConsoleInput(handle_in, &keyrec, 1, &res);      //读取输入事件  
		if (keyrec.EventType == KEY_EVENT)      //如果当前事件是键盘事件  
		{  
			if (keyrec.Event.KeyEvent.wVirtualKeyCode == VK_RETURN  && keyrec.Event.KeyEvent.bKeyDown == true) //当前事件的虚拟键为Enter键  
			{  
				break;
			}  
		}  
	}

	system("cls");
}

// 正确性检测

bool CheckCorrectness(Level lvl)
{
    vector<int>::iterator it1 = lvl.OutputBeltBlocks.begin();
    vector<int>::iterator it2 = TargetOutputBelt.begin();
    if (lvl.OutputBeltBlocks.size() != TargetOutputBelt.size())
        return false;
    while (it1 != lvl.OutputBeltBlocks.end() && it2 != TargetOutputBelt.end())
    {
        if ((*it1) != (*it2))
            return false;
        it1++, it2++;
    }
    return true;
}

// 关卡主程序

void LevelOnePresetting(Level &lvl)
{
    lvl.CurrentBlock = 0;
    lvl.CurrentBlockExist = false;

    lvl.CurrentInstruction = 0;

    lvl.EndState = false;

    lvl.InputBeltBlocks.push_back(1);
    lvl.InputBeltBlocks.push_back(2);

    lvl.SpaceNum = 0;

    memset(lvl.OpenSpaceBlocks, 0, sizeof(lvl.OpenSpaceBlocks));

	TargetOutputBelt.clear();
    TargetOutputBelt.push_back(1);
    TargetOutputBelt.push_back(2);
}

void LevelOne() // 关卡1主程序
{
	PrintLevelInfo(info_title[0],info_input[0],info_task[0],info_openspace[0],info_command[0]);	//输出关卡信息
	
	cout << "请选择指令输入方式（文件读入--1/命令行输入--2）：";		//输入方式选择
	int idx = 0;
	cin >> idx;
	system("cls");
	vector<string> command = pf_CommandInput[idx-1]();
	
	PrintLevelBackground(command);
    Level lvl1;
    lvl1.InstructionNum = command.size();
    LevelOnePresetting(lvl1);
    ScreenRefresh(lvl1.InputBeltBlocks,lvl1.OutputBeltBlocks,lvl1.OpenSpaceBlocks,lvl1.CurrentBlock, lvl1.SpaceNum, lvl1);
    
	while (true)
    {
		Sleep(2000);
		vector<string>::iterator it=command.begin();
		it += lvl1.CurrentInstruction;
		if (it == command.end())
			break;
		lvl1.OrderExecution1(*it);
        if (lvl1.EndState == true)
            break;
		move_arrow(lvl1.CurrentInstruction,lvl1.InstructionNum);
        ScreenRefresh(lvl1.InputBeltBlocks,lvl1.OutputBeltBlocks,lvl1.OpenSpaceBlocks,lvl1.CurrentBlock, lvl1.SpaceNum, lvl1);
		lvl1.CurrentInstruction++;
    }
	system("cls");
    // 关卡1具体过程

    if (CheckCorrectness(lvl1))
    {	
		PrintEndScreen("Success!");

		ofstream fout;
		fout.open("archive.txt",ios::app);
		if(CurrentLevel > MaxLevel)
			fout << CurrentLevel << endl;
		fout.close();
	}
    else
	{	
		PrintEndScreen("Failed!");
	}
}

void LevelTwoPresetting(Level &lvl)
{
	lvl.CurrentBlock = 0;
    lvl.CurrentBlockExist = false;

    lvl.CurrentInstruction = 0;

    lvl.EndState = false;

    lvl.InputBeltBlocks.push_back(3);
    lvl.InputBeltBlocks.push_back(9);
	lvl.InputBeltBlocks.push_back(5);
	lvl.InputBeltBlocks.push_back(1);
	lvl.InputBeltBlocks.push_back(-2);
	lvl.InputBeltBlocks.push_back(-2);
	lvl.InputBeltBlocks.push_back(9);
	lvl.InputBeltBlocks.push_back(-9);

    lvl.SpaceNum = 3;

    memset(lvl.OpenSpaceBlocks, 0, sizeof(lvl.OpenSpaceBlocks));

	TargetOutputBelt.clear();
    TargetOutputBelt.push_back(-6);
    TargetOutputBelt.push_back(6);
	TargetOutputBelt.push_back(4);
	TargetOutputBelt.push_back(-4);
	TargetOutputBelt.push_back(0);
	TargetOutputBelt.push_back(0);
	TargetOutputBelt.push_back(18);
	TargetOutputBelt.push_back(-18);
}

void LevelTwo() // 关卡2主程序
{
    PrintLevelInfo(info_title[1],info_input[1],info_task[1],info_openspace[1],info_command[1]);	//输出关卡信息
    
	cout << "请选择指令输入方式（文件读入--1/命令行输入--2）：";		//输入方式选择
	int idx = 0;
	cin >> idx;
	system("cls");
	vector<string> command = pf_CommandInput[idx-1]();

    // 关卡2具体过程
	PrintLevelBackground(command);
	Level lvl2;
    lvl2.InstructionNum = command.size();
    LevelTwoPresetting(lvl2);

	ScreenRefresh(lvl2.InputBeltBlocks,lvl2.OutputBeltBlocks,lvl2.OpenSpaceBlocks,lvl2.CurrentBlock, lvl2.SpaceNum, lvl2);
    
	while (true)
    {
		Sleep(2000);
		vector<string>::iterator it=command.begin();
		it += lvl2.CurrentInstruction;
		if (it == command.end())
			break;
        lvl2.OrderExecution2(*it);
        if (lvl2.EndState == true)
            break;
		move_arrow(lvl2.CurrentInstruction,lvl2.InstructionNum);
        ScreenRefresh(lvl2.InputBeltBlocks,lvl2.OutputBeltBlocks,lvl2.OpenSpaceBlocks,lvl2.CurrentBlock, lvl2.SpaceNum, lvl2);
		lvl2.CurrentInstruction++;
    }
	system("cls");

    if (CheckCorrectness(lvl2))
	{
		PrintEndScreen("Success!");

		ofstream fout;
		fout.open("archive.txt",ios::app);
		if(CurrentLevel > MaxLevel)
			fout << CurrentLevel << endl;
		fout.close();
	}
    else
        PrintEndScreen("Failed!");
}

void LevelThreePresetting(Level &lvl)
{
	lvl.CurrentBlock = 0;
    lvl.CurrentBlockExist = false;

    lvl.CurrentInstruction = 0;

    lvl.EndState = false;

    lvl.InputBeltBlocks.push_back(6);
    lvl.InputBeltBlocks.push_back(2);
	lvl.InputBeltBlocks.push_back(7);
	lvl.InputBeltBlocks.push_back(7);
	lvl.InputBeltBlocks.push_back(-9);
	lvl.InputBeltBlocks.push_back(3);
	lvl.InputBeltBlocks.push_back(-3);
	lvl.InputBeltBlocks.push_back(-3);

    lvl.SpaceNum = 3;

    memset(lvl.OpenSpaceBlocks, 0, sizeof(lvl.OpenSpaceBlocks));

	TargetOutputBelt.clear();
    TargetOutputBelt.push_back(7);
    TargetOutputBelt.push_back(-3);
	
}

void LevelThree() // 关卡3主程序
{
    PrintLevelInfo(info_title[2],info_input[2],info_task[2],info_openspace[2],info_command[2]);	//输出关卡信息
    
	cout << "请选择指令输入方式（文件读入--1/命令行输入--2）：";		//输入方式选择
	int idx = 0;
	cin >> idx;
	system("cls");
	vector<string> command = pf_CommandInput[idx-1]();

    // 关卡2具体过程
	PrintLevelBackground(command);
	Level lvl3;
    lvl3.InstructionNum = command.size();
    LevelTwoPresetting(lvl3);

	ScreenRefresh(lvl3.InputBeltBlocks,lvl3.OutputBeltBlocks,lvl3.OpenSpaceBlocks,lvl3.CurrentBlock, lvl3.SpaceNum, lvl3);
    
	while (true)
    {
		Sleep(2000);
		vector<string>::iterator it=command.begin();
		it += lvl3.CurrentInstruction;
		if (it == command.end())
			break;
        lvl3.OrderExecution2(*it);
        if (lvl3.EndState == true)
            break;
		move_arrow(lvl3.CurrentInstruction,lvl3.InstructionNum);
        ScreenRefresh(lvl3.InputBeltBlocks,lvl3.OutputBeltBlocks,lvl3.OpenSpaceBlocks,lvl3.CurrentBlock, lvl3.SpaceNum, lvl3);
		lvl3.CurrentInstruction++;
    }
	system("cls");

    if (CheckCorrectness(lvl3))
	{
		PrintEndScreen("Success!");

		ofstream fout;
		fout.open("archive.txt",ios::app);
		if(CurrentLevel > MaxLevel)
			fout << CurrentLevel << endl;
		fout.close();
	}
    else
        PrintEndScreen("Failed!");
}

void LevelFour() // 关卡4主程序
{
   // PrintLevelInfo();
    // CommandFileLoading(); or CommandInput()

    // 关卡4具体过程

    //CheckCorrectness();
}

// 主程序

int main()
{
/*******************************************************************/

	//SelectionProcess();
	//LevelOne();
	//system("pause");
/*******************************************************************/

    while (true)
    {
        SelectionProcess(); // 选择关卡
        switch (CurrentLevel) // 判断当前关卡
        {
            case 1 :
            {
                LevelOne();
                break;
            }
            case 2 : 
            {
                LevelTwo();
                break;
            }
            case 3:
            {
                LevelThree();
                break;
            }
            case 4:
            {
                LevelFour();
                break;
            }
            default :
                break;
        }
    }
}