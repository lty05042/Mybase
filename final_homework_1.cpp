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

// ȫ�ֱ�������

const int MAXSPACE = 1010; // �յ����������� // ��ʱ���1010��

int CurrentLevel; // ��ǰ�ؿ�
int MaxLevel = 0; //���ͨ���ؿ�

int InputBelt[MAXSPACE]; // ��������
vector <int> TargetOutputBelt; // Ŀ���������

int AvaibleOpenSpaceNumber; // ���ÿյ���
string AvailableOrderSet[10]; // ����ָ�

char info_title[4][20] = {"�� һ ��","�� �� ��","�� �� ��","�� �� ��"};
char info_input[4][100] = {"�������У�1, 2","�������У�3,9,5,1,-2,-2,9,-9"," �������У�6,2,7,7,-9,3,-3,-3"," "};
char info_task[4][200] = {"�ؿ������û�����ȡ�����������ϵ�ÿ����ľ�������������","�ؿ����񣺶������������е�ÿ�����������Ȱѵ� 1 ����ȥ�� 2 �������ѽ��������������У�Ȼ��ѵ� 2 ����ȥ�� 1 �����ٰѽ��������������У��ظ���","�ؿ����񣺴���������������ȡ 2 �����֣�������������һ������������ӵ����ظ��������ֱ�����봫�ʹ�Ϊ�ա�"," "};
char info_openspace[4][100] = {"���ÿյ�����0","���ÿյ�����3","���ÿյ�����3"," "};
char info_command[4][100] = {"����ָ���inbox,outbox","����ָ���inbox,outbox,copyfrom,copyto,add,sub,jump,jumpifzero","����ָ���inbox,outbox,copyfrom,copyto,add,sub,jump,jumpifzero"," "};


// �����˺ʹ��ʹ�����

class Level{
    public:
        // �����˺ʹ��ʹ��Ĳ���
        
        bool CurrentBlockExist; // �Ƿ���ڵ�ǰ��ľ
        int CurrentBlock; // ��ǰ��ľ����
        vector <int> InputBeltBlocks; // ���봫�ʹ��ϻ�ľ
        vector <int> OutputBeltBlocks; // ������ʹ��ϻ�ľ
		int OpenSpaceBlocks[10]; // �յ��ϻ�ľ ��Ŵ�0��ʼ
        bool OpenSpaceBlocksExist[MAXSPACE]; // �Ƿ���ڿյ��ϻ�ľ
        int SpaceNum; // �յ���
        int CurrentInstruction; // ��ǰָ��
        int InstructionNum; // ָ���� ��Ŵ�1��ʼ
        bool EndState; // �Ƿ������Ϸ trueΪ��

        void ErrorProcess() // �쳣�������
        {
			system("cls");
			gotoxy(50,12);
            cout << "Error on instruction  " << CurrentInstruction+1 << endl; // ����쳣ָ��
            EndState = true; // ������Ϸ
			Sleep(3000);
            return ;
        }

        // ָ�

        void EndJudgeByInput() // �ж���Ϸ�Ƿ������봫�ʹ�����
        {
            if (InputBeltBlocks.empty()) // ������봫�ʹ�û�л�ľ
                EndState = true; // ��Ϸ����
            return ;
        }

        void inbox() // inbox����ǰӦ���EndjudgeByInput
        {
            EndJudgeByInput();
            CurrentBlock = InputBeltBlocks[0]; // �������봫�ʹ��ĵ�һ����ľ����ԭ�е�ǰ��ľ������
            InputBeltBlocks.erase(InputBeltBlocks.begin()); // ���ʹ��ϻ�ľ����
            CurrentBlockExist = true; // ��ǰ��ľ����
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
			if (floor(AddPlaceNumber) != AddPlaceNumber) // �����������
			{
				ErrorProcess();
                return ;
			}
            if (CurrentBlockExist == false) // ���û�е�ǰ��ľ
            {
                ErrorProcess();
                return ;
            }
            if (AddPlaceNumber > SpaceNum || AddPlaceNumber <= 0) // ���X�ſյز�����
            {
                ErrorProcess();
                return ;
            }
            if (OpenSpaceBlocksExist[(int)AddPlaceNumber] == false) // ����յ���û�л�ľ
            {
                ErrorProcess();
                return ;
            }
            CurrentBlock += OpenSpaceBlocks[(int)AddPlaceNumber]; // ��ǰ��ľ���ּ��Ͽյػ�ľ����
        }

        void sub(float SubPlaceNumber)
        {
			if (floor(SubPlaceNumber) != SubPlaceNumber) // �����������
			{
				ErrorProcess();
                return ;
			}
            if (CurrentBlockExist == false) // ���û�е�ǰ��ľ
            {
                ErrorProcess();
                return ;
            }
            if (SubPlaceNumber > SpaceNum || SubPlaceNumber <= 0) // ���X�ſյز�����
            {
                ErrorProcess();
                return ;
            }
            if (OpenSpaceBlocksExist[(int)SubPlaceNumber] == false) // ����յ���û�л�ľ
            {
                ErrorProcess();
                return ;
            }
            CurrentBlock -= OpenSpaceBlocks[(int)SubPlaceNumber]; // ��ǰ��ľ���ּ�ȥ�յػ�ľ����
        }

        void Copyto(float CopytoPlaceNumber)
        {
			if (floor(CopytoPlaceNumber) != CopytoPlaceNumber) // �����������
			{
				ErrorProcess();
                return ;
			}
            if (CurrentBlockExist == false) // ���û�е�ǰ��ľ
            {
                ErrorProcess();
                return ;
            }
            if (CopytoPlaceNumber > SpaceNum || CopytoPlaceNumber <= 0) // ���X�ſյز�����
            {
                ErrorProcess();
                return ;
            }
            OpenSpaceBlocks[(int)CopytoPlaceNumber] = CurrentBlock; // ���Ƶ�ǰ��ľ��X�ſյ�
        }

        void Copyfrom(float CopyfromPlaceNumber)
        {
			if (floor(CopyfromPlaceNumber) != CopyfromPlaceNumber) // �����������
			{
				ErrorProcess();
                return ;
			}
            if (CopyfromPlaceNumber > SpaceNum || CopyfromPlaceNumber <= 0) // ���X�ſյز�����
            {
                ErrorProcess();
                return ;
            }
            if (OpenSpaceBlocksExist[(int)CopyfromPlaceNumber] == false) // X�ſյ�û�л�ľ
            {
                ErrorProcess();
                return ;
            }
            CurrentBlock = OpenSpaceBlocks[(int)CopyfromPlaceNumber]; // ����X�ſյصĻ�ľ����ǰ��ľ
        }

        void jump(float JumpToOrderNumber)
        {
			if (floor(JumpToOrderNumber) != JumpToOrderNumber) // �����������
			{
				ErrorProcess();
                return ;
			}
            if (JumpToOrderNumber > InstructionNum || JumpToOrderNumber <= 0) // �����ڵ�X��ָ��
            {
                ErrorProcess();
                return ;
            }
            CurrentInstruction = (int)JumpToOrderNumber; // �޸ĵ�ǰָ��Ϊ��X��
        }

        void jumpifzero(int JumpToOrderNumber)
        {
			if (floor(JumpToOrderNumber) != JumpToOrderNumber) // �����������
			{
				ErrorProcess();
                return ;
			}
            if (CurrentBlockExist == false) // û�е�ǰ��ľ
            {
                ErrorProcess();
                return ;
            }
            if (JumpToOrderNumber > InstructionNum || JumpToOrderNumber <= 0) // �����ڵ�X��ָ��
            {
                ErrorProcess();
                return ;
            }
            if (CurrentBlock != 0) // �����ǰ��ľ��Ϊ0
                return ;
            CurrentInstruction = (int)JumpToOrderNumber; // �޸ĵ�ǰָ��Ϊ��X��
        }

		int checkinput(string input) // ����input�пո���
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
				if (checkinput(input) != 1) // ������������
				{
					ErrorProcess();
					return ;
				}
				Copyfrom(stof(input.substr(9, input.length()))); // ֱ��ĩβ������
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

// ���ֶ�ȡ

vector<int> ArchiveFileLoading() // ��ȡ�浵
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

vector<string> CommandFileLoading() // ���ļ���ȡָ�
{
	string file_path = "";
	file_exception:
	cout << "������ָ���ļ�·����";
	cin >> file_path;
	ifstream fin;
	fin.open(file_path);

	if(!fin)
	{
		cout << "�ļ�·������" << endl;
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

vector<string> CommandInput() // �ü��������ȡָ�
{
	int command_num = 0;
	cout << "����������ָ������:";
	cin >> command_num;

	vector<string> command(command_num);
	cout << "������ָ�";
	for(int i = 0;i < command_num;i++ )
	{
		cin >> command[i];
	}

	return command;
}

vector<string> (*pf_CommandInput[2])() = {CommandFileLoading,CommandInput};		//ָ�����뺯����ת��

// ����ѡ��ؿ�ָ��, �޸ĵ�ǰ�ؿ�

void SelectionProcess() 
{
	vector<int> game_archive = ArchiveFileLoading();

	char* str_title = "�ؿ�ѡ��";
	int len_title = strlen(str_title);
	char* str_instruct = "��ѡ��ؿ���ͨ���������룩��";
	int len_instruct = strlen(str_instruct);
	char* line = "---------------";
	char* arrow = "- - - - - - - >";

	HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE);    //��ñ�׼����豸���  
    CONSOLE_SCREEN_BUFFER_INFO csbi;    //���崰�ڻ�������Ϣ�ṹ��  
    GetConsoleScreenBufferInfo(handle_out, &csbi);  //��ô��ڻ�������Ϣ
	
	label_select:
	int x = 0,y = 0;
	x = (csbi.dwSize.X - len_title) / 2 - 2;    //��߽磬Ϊ����������ַ�������
	y = 4;
	gotoxy(x,y);
	cout << str_title << endl;					//�������

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
		cout << "�ؿ�-" << i+1 ;
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
	}						//����ؿ����ƺ�ͨ�����

	x = (csbi.dwSize.X - len_instruct-2) / 2 - 2;y = 10;
	gotoxy(x,y);
	cout << str_instruct;

	int level_chosen = 0;					//�ؿ�ѡ��
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
		x = (csbi.dwSize.X - strlen("�޷�ѡ��ùؿ���")-2) / 2 - 2;y += 2;
		gotoxy(x,y);
		SetConsoleTextAttribute(handle_out, FOREGROUND_RED); 
		cout << "�޷�ѡ��ùؿ���";
		SetConsoleTextAttribute(handle_out, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
		Sleep(1*1000);
		system("cls");
		goto label_select;
	}

}

// ���ֽ������

void PrintLevelInfo(char* title,char* input,char* output,char* openspace,char* command) // ����ؿ���Ϣ
{
	HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE);    //��ñ�׼����豸���  
    CONSOLE_SCREEN_BUFFER_INFO csbi;    //���崰�ڻ�������Ϣ�ṹ��  
    GetConsoleScreenBufferInfo(handle_out, &csbi);  //��ô��ڻ�������Ϣ

	int x = 0,y = 0;

	x = (csbi.dwSize.X - strlen(title)) / 2 - 2;    
	y = 4;
	gotoxy(x,y);
	cout << title << endl;					//�������

	x = (csbi.dwSize.X - strlen(output)) / 2 - 2;    
	y += 2;
	gotoxy(x,y);
	cout << output << endl;					//������Ҫ��

	x = (csbi.dwSize.X - strlen(input)) / 2 - 2;    
	y += 2;
	gotoxy(x,y);
	cout << input << endl;					//�����������

	x = (csbi.dwSize.X - strlen(openspace)) / 2 - 2;    
	y += 2;
	gotoxy(x,y);
	cout << openspace << endl;					//������ÿյ�

	x = (csbi.dwSize.X - strlen(command)) / 2 - 2;    
	y += 2;
	gotoxy(x,y);
	cout << command << endl;					//�������ָ�

	Sleep(500);
	x = (csbi.dwSize.X - strlen("�����س���������")) / 2 - 2;    
	y += 4;
	gotoxy(x,y);
	cout << "�����س���������" << endl;					//�������ָ�

	INPUT_RECORD keyrec;        //���������¼��ṹ��  
	DWORD res;					//���巵�ؼ�¼  
	for (;;)  
	{
		HANDLE handle_in = GetStdHandle(STD_INPUT_HANDLE);      //��ñ�׼�����豸���
		ReadConsoleInput(handle_in, &keyrec, 1, &res);      //��ȡ�����¼�  
		if (keyrec.EventType == KEY_EVENT)      //�����ǰ�¼��Ǽ����¼�  
		{  
			if (keyrec.Event.KeyEvent.wVirtualKeyCode == VK_RETURN  && keyrec.Event.KeyEvent.bKeyDown == true) //��ǰ�¼��������ΪEnter��  
			{  
				break;
			}  
		}  
	}
		
	system("cls");
}

void  PrintLevelBackground(vector<string> &cmd)	//����ؿ�����
{
	HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE);    //��ñ�׼����豸���  
    CONSOLE_SCREEN_BUFFER_INFO csbi;    //���崰�ڻ�������Ϣ�ṹ��  
    GetConsoleScreenBufferInfo(handle_out, &csbi);  //��ô��ڻ�������Ϣ

	char* frame = "+---+";
	char* frame_1 = "|   |";

	int x = 0,y = 0;

	x = 8;y = 4;		
	gotoxy(1,y+1);
	cout << "IN";
	for(int i = 0;i < 12;i++)	//���봫�ʹ��߿�
	{
		gotoxy(x,y);
		cout << frame;
		y++;
		gotoxy(x,y);
		if(y % 4 == 1)
			cout << frame_1;
		y++;
	}

	x = 20;y = 4;		//��ǰ��ľ�߿�
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

	gotoxy(x,y);			//������
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

	y = 16; gotoxy(x,y);		//�յ�
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

	x = 22;y = 19;gotoxy(x,y);		//�յر��
	for(int i = 0;i < 4;i++)
	{
		cout << i+1;
		x += 6;
		gotoxy(x,y);
	}

	x = 50;y = 4;				//������ʹ��߿�
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

	x = 65;y = 4;gotoxy(x,y);		//�ָ���
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

void ScreenRefresh(vector<int> &input_belt,vector<int> &output_belt,int openspace[],int currentblock, int spacenum, Level lvl)		//��ˢ���������
{
	HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE);    //��ñ�׼����豸���  
    CONSOLE_SCREEN_BUFFER_INFO csbi;    //���崰�ڻ�������Ϣ�ṹ��  
    GetConsoleScreenBufferInfo(handle_out, &csbi);  //��ô��ڻ�������Ϣ

	int x = 0,y = 0;

	x = 10;y = 5;gotoxy(x,y);			//�޸����봫�ʹ�
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

	x = 52;y = 5;gotoxy(x,y);			//�޸�������ʹ�
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

	x = 22;y = 17;gotoxy(x,y);			//�޸Ŀյ�
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
	HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE);    //��ñ�׼����豸���  
	CONSOLE_SCREEN_BUFFER_INFO csbi;    //���崰�ڻ�������Ϣ�ṹ��  
	GetConsoleScreenBufferInfo(handle_out, &csbi);  //��ô��ڻ�������Ϣ

	SMALL_RECT scroll;      //�����ƶ�����  
    COORD pos = {68 ,6 + (instruction_idx+1)*2};     //�ƶ�λ��  
    CHAR_INFO chFill;       //��������ַ�
	
	//��������ַ��ĸ�������������
	chFill.Char.AsciiChar = ' ';  
    chFill.Attributes = csbi.wAttributes;

	//ȷ������
	scroll.Top = 6 + (instruction_idx)*2;
	scroll.Bottom = scroll.Top+1;
	scroll.Left = 68;
	scroll.Right = scroll.Left+1;

	if(instruction_idx < instruction_num - 1)
		ScrollConsoleScreenBuffer(handle_out, &scroll, NULL, pos, &chFill); //�ƶ��ı�
}


void PrintEndScreen(string prompt)			//�ؿ��������� 
{
	HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE);    //��ñ�׼����豸���  
	CONSOLE_SCREEN_BUFFER_INFO csbi;    //���崰�ڻ�������Ϣ�ṹ��  
	GetConsoleScreenBufferInfo(handle_out, &csbi);  //��ô��ڻ�������Ϣ

	SetConsoleTextAttribute(handle_out, FOREGROUND_RED); 
	int x = (csbi.dwSize.X - prompt.length()) / 2 - 2;
	int y = 12;
	gotoxy(x,y);
	cout << prompt;
	SetConsoleTextAttribute(handle_out, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);

	x = (csbi.dwSize.X - strlen("�����س������أ�")) / 2 - 2;
	y += 2;
	gotoxy(x,y);
	cout << "�����س������أ�";

	INPUT_RECORD keyrec;        //���������¼��ṹ��  
	DWORD res;					//���巵�ؼ�¼  
	for (;;)  
	{
		HANDLE handle_in = GetStdHandle(STD_INPUT_HANDLE);      //��ñ�׼�����豸���
		ReadConsoleInput(handle_in, &keyrec, 1, &res);      //��ȡ�����¼�  
		if (keyrec.EventType == KEY_EVENT)      //�����ǰ�¼��Ǽ����¼�  
		{  
			if (keyrec.Event.KeyEvent.wVirtualKeyCode == VK_RETURN  && keyrec.Event.KeyEvent.bKeyDown == true) //��ǰ�¼��������ΪEnter��  
			{  
				break;
			}  
		}  
	}

	system("cls");
}

// ��ȷ�Լ��

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

// �ؿ�������

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

void LevelOne() // �ؿ�1������
{
	PrintLevelInfo(info_title[0],info_input[0],info_task[0],info_openspace[0],info_command[0]);	//����ؿ���Ϣ
	
	cout << "��ѡ��ָ�����뷽ʽ���ļ�����--1/����������--2����";		//���뷽ʽѡ��
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
    // �ؿ�1�������

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

void LevelTwo() // �ؿ�2������
{
    PrintLevelInfo(info_title[1],info_input[1],info_task[1],info_openspace[1],info_command[1]);	//����ؿ���Ϣ
    
	cout << "��ѡ��ָ�����뷽ʽ���ļ�����--1/����������--2����";		//���뷽ʽѡ��
	int idx = 0;
	cin >> idx;
	system("cls");
	vector<string> command = pf_CommandInput[idx-1]();

    // �ؿ�2�������
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

void LevelThree() // �ؿ�3������
{
    PrintLevelInfo(info_title[2],info_input[2],info_task[2],info_openspace[2],info_command[2]);	//����ؿ���Ϣ
    
	cout << "��ѡ��ָ�����뷽ʽ���ļ�����--1/����������--2����";		//���뷽ʽѡ��
	int idx = 0;
	cin >> idx;
	system("cls");
	vector<string> command = pf_CommandInput[idx-1]();

    // �ؿ�2�������
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

void LevelFour() // �ؿ�4������
{
   // PrintLevelInfo();
    // CommandFileLoading(); or CommandInput()

    // �ؿ�4�������

    //CheckCorrectness();
}

// ������

int main()
{
/*******************************************************************/

	//SelectionProcess();
	//LevelOne();
	//system("pause");
/*******************************************************************/

    while (true)
    {
        SelectionProcess(); // ѡ��ؿ�
        switch (CurrentLevel) // �жϵ�ǰ�ؿ�
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