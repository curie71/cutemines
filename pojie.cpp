#include <windows.h>  
#include <stdio.h>  

DWORD dwPID = 0; 
HWND hWinmine = 0;
DWORD dwBaseAddr = 0x01005360;//ȥ���߿�������ʼ��λ��
DWORD dwWidthAddr = 0x01005334;//�������
DWORD dwHeightAddr = 0x01005338;//�������
int dwWidth = 0, dwHeight = 0;


void select();

int check()//���ɨ����Ϸ�Ƿ�����
{
	hWinmine = FindWindow(NULL, "Minesweeper");  
    GetWindowThreadProcessId(hWinmine, &dwPID);//�õ�PID  
	if(dwPID == 0)
		return 0;
	return 1;
}

void ask()
{
	int ans = 0;
	printf("����ʹ�����������𣿰�1��������2�˳�\n");
	scanf("%d",&ans);
	if(ans == 1 && check() == 1)select();
	else return;
}

void display()//��ʾ���ײ���
{
	BYTE flag = 0x8E;//����
	DWORD dwflagSize = 0;
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, dwPID);//�õ����
   
    //���ᡢ���������Ŀ
    ReadProcessMemory(hProcess, (LPVOID)dwWidthAddr, &dwWidth, sizeof(DWORD), 0);  
    ReadProcessMemory(hProcess, (LPVOID)dwHeightAddr, &dwHeight, sizeof(DWORD), 0);  


	for(int i = 0;i < dwHeight;i++)
	{
		for(int j = 0;j < dwWidth+2;j++)//��ǽ
		{
			DWORD currentInfo = 0;
			DWORD addr = dwBaseAddr+j+(i*0x20);//��ĳһ�е�һ�е���һ�е�һ�м����0x20��С�ĵ�ַ
			ReadProcessMemory(hProcess, (LPVOID)(addr), &currentInfo, sizeof(DWORD), &dwflagSize);
			currentInfo=currentInfo&0x000000ff;
			if(currentInfo == 0x8f)
			{
				WriteProcessMemory(hProcess, (LPVOID)(addr), &flag, sizeof(BYTE),&dwflagSize);//�������ע
			}
		}
	}
	CloseHandle(hProcess);   
	printf("\n����ʾ�������֣��Ժ��������\n���û����ʾ��������С��ɨ����Ϸ��ʹ�����ػ�...\n");
	printf("-------------------------------\n\n");
	ask();
}
  
void stoptime()//ֹͣ��ʱ��
{
	DWORD dwTimer = 0x01002FF5;//ָ���ַ
	DWORD dwTimeSize = 4;//ָ��FF059C57

	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, dwPID);//�õ����

	BYTE* nopsInfo = new BYTE[4];
	for (size_t i = 0; i < 4; i++)  //����ͬ�����ȵ�NOP
    {  
		nopsInfo[i] = 0x90;
	}
	
	WriteProcessMemory(hProcess, (LPVOID)dwTimer, nopsInfo, sizeof(BYTE), &dwTimeSize);//����
	CloseHandle(hProcess);   

	printf("\n��ֹͣ��ʱ����Ϸ���˳���һֱ��Ч...\n");
	printf("-------------------------------\n\n");
	ask();
}

void nofail()//����ģʽ���㵽��Ҳ���Լ�����
{
	DWORD dwFail = 0x010034D6;//ָ���ַ
	DWORD dwFailSize = 10;//ָ��C7 05 00 50 00 01 10 00 00 00

	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, dwPID);//�õ����

	BYTE* nopsInfo = new BYTE[10];
	for (size_t i = 0; i < 10; i++)  //����ͬ�����ȵ�NOP
    {  
		nopsInfo[i] = 0x90;
	}
	
	WriteProcessMemory(hProcess, (LPVOID)dwFail, nopsInfo, sizeof(BYTE), &dwFailSize);//����
	CloseHandle(hProcess);   

	printf("\n�ѿ�������ģʽ���������㣬ը��Ҳ���Լ���\n��Ϸ���˳���һֱ��Ч...\n");
	printf("-------------------------------\n\n");
	ask();
}

void autogame()//�Զ�ģʽ�������Զ�����Ϸ��������������
{
	DWORD xBegin = 0xC;
	DWORD yBegin = 0x37;//�������
	DWORD mSize = 0x10;//���Ӵ�С

	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, dwPID);//�õ����
   
    //�õ������������Ŀ
    ReadProcessMemory(hProcess, (LPVOID)dwWidthAddr, &dwWidth, sizeof(DWORD), 0);  
    ReadProcessMemory(hProcess, (LPVOID)dwHeightAddr, &dwHeight, sizeof(DWORD), 0); 
	
	BYTE flag = 0x8E;//����

	//����ʾ����ԭ��һ��
	for(int i = 0;i < dwHeight;i++)
	{
		for(int j = 0;j < dwWidth+2;j++)
		{
			DWORD currentInfo = 0;
			DWORD addr = dwBaseAddr+j+(i*0x20);
			//printf("%x\n",i*0x20);
			ReadProcessMemory(hProcess, (LPVOID)(addr), &currentInfo, sizeof(DWORD), 0);
			currentInfo=currentInfo&0x000000ff;
			//printf("%d %d %x %d\n",i,j,currentInfo,i*0x20);

			if(currentInfo == 0x8f)//����������
			{
				continue;
			}

			if(currentInfo == 0x0f)//������ģ����
			{
				//�ڴ�����begin��λ�ÿ϶���ǽ������Ҫ�ӽ���begin��λ������ŲmSize/2����һ��ǽ�ŶԵ���
				SendMessage(hWinmine,WM_LBUTTONDOWN,0,MAKELPARAM(xBegin-mSize/2+mSize*j,
					yBegin+mSize/2+mSize*i));
				SendMessage(hWinmine,WM_LBUTTONUP,0,MAKELPARAM(xBegin-mSize/2+mSize*j,
					yBegin+mSize/2+mSize*i));
			}
			else continue;//��ʾ�����ֵ�Ҳ����
			Sleep(80);	
		}

	}
	CloseHandle(hProcess);   
	printf("\n�ѳɹ�����Զ���Ϸģʽ...\n");
	printf("ע�⣡ע�⣡\n���ɨ�״��ڵ��κβ�λ�������������ص����������ϻ������£������ܵ��³���\n");
	printf("-------------------------------\n\n");
	ask();
}


void exit()
{
	printf("\nллʹ��\n\n");
	return;
}


void select()
{
	printf("��������ţ�\n");
	printf("1 ��ʾ��Ϸ����\n");
	printf("2 ֹͣ��ʱ��\n");
	printf("3 ������Ϸģʽ\n");
	printf("4 �Զ���Ϸģʽ\n");
	printf("0 �˳�\n");
	int op = 0;
	scanf("%d",&op);
	if(op == 1)
		display();
	if(op == 2)
		stoptime();
	if(op == 3)
		nofail();
	if(op == 4)
		autogame();
	if(op == 0)
		exit();
}

void main()  
{
	if(check() == 0)
	{
		printf("��������ɨ����Ϸ...\n");
		getchar();
		return;
	}
	printf("�Ѽ�⵽ɨ����Ϸ...\n\n");
	select();
	system("pause");
}  