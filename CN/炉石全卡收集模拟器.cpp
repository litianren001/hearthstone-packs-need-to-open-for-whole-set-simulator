#include <iostream>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include<fstream>
#define setn 7
using namespace std;

const int dustcr[8]= {40,100,400,1600,400,800,1600,3200}; //[isgolden(0~1)*4+rarity(0~3)], craft
const int dustde[8]= {5,20,100,400,50,100,400,1600}; //disenchant
const int cardmax[8]= {2,2,2,1,2,2,2,1}; //max amount of each rarity
const int cardp[8]= {697474,217533,41902,9213,16493,13670,2526,1189}; //card possibility based on 1/1000000, Kripparian Dec 2016 1346 packs, http://hearthstone.gamepedia.com/Card_pack_statistics
const int cardnarr[setn][8]={{1,1,1,1,1,1,1,1},
						 {94,81,37,33,94,81,37,33},
						 {40,37,26,20,40,37,26,20},
						 {49,36,27,20,49,36,27,20},
						 {49,36,27,20,50,36,27,21},
						 {49,36,27,20,49,36,27,20},
						 {49,36,27,23,49,36,27,23}};//amount of cards of each rarity of each set
const string setname[setn]={"测试","标准","地精大战侏儒","冠军的试炼","古神的低语","龙争虎斗加基森","勇闯安戈洛"};
const string rarityname[8]={"普通","稀有","史诗","传说","金色普通","金色稀有","金色史诗","金色传说"};

int cardcurrentarr[setn][8][3];//current amount of cards of 0-2 collections of each rarity of each set
int cardn[8],cardcurrent[8][3];//amount of cards of each rarity, current amount of cards of 0-2 collections of each rarity
int simn=100000; //simulate times 
int cardpagg[8]; //accumulated possibility of each rarity
int cardnagg[8]; //accumulated card number range of each rarity
int card[1000],cardinit[1000]; //amount of each card
int cardsum,dustsum,dustneed,dustneedinit,dustinit,i,j,k,tmp,tp1,tp2,newcardtype,newcardnum,option,timeflag,origintime,flag;
long long s;
string strtmp;


void CollectionProgressReset(); //reset collection progress to empty
void CollectionProgressLoad(); //load collection progress from hs_card_simulator.txt
void Init();
int Sim(); //return card amount drawn for full non-golden collection
int SimGolden(); //for golden collection
int SimAll(); // for full collection
void NewCard(); //return newcardtype,newcardnum

void GenerateInitSim();
void GenerateInitSimGolden();
void GenerateInitSimAll();



ifstream fin;

int main()
{
	cout<<"炉石传说全卡收集模拟器 by 节操何在@NGA\n";
	CollectionProgressReset();
	while (true)
	{
		cout<<"输入11-63以模拟不同的卡包和策略\n十位数: 1:标准 2: 地精大战侏儒 3:冠军的试炼 4:古神的低语 5:龙争虎斗加基森 6:勇闯安戈洛\n个位数: 1:收集普卡 2: 收集金卡 3: 收集全卡\n输入0以从炉石全卡收集模拟器.txt中读取收集进度，或-1以重置进度为初始值\n输入超过999的数字以改变模拟次数(默认值100000)\n";
		cin>>option;
		if (option==0)
		{
			CollectionProgressLoad();
			if (flag==1)
			{
				CollectionProgressReset();
				cout<<"收集进度已重置。按回车以继续。\n===============\n";
				getline(cin,strtmp);
				getline(cin,strtmp);
			}
			else cout<<"收集进度已载入。\n===============\n";
			continue;
		}
		if (option==-1)
		{
			CollectionProgressReset();
			cout<<"收集进度已重置。\n===============\n";
			continue;
		}
		if (option>999)
		{
			simn=option;
			cout<<"模拟次数已改变。\n===============\n";
			continue;
		}
		Init();
		s=0;
		timeflag=0;
		switch(option%10)
		{
			case 1:
			{
				GenerateInitSim();
				cout<<"模拟进行中...\n";
				origintime=time(NULL);
				for(j=0;j<simn;j++)
				{
					if (((time(NULL)-origintime)%10==9)&&(time(NULL)!=timeflag))
					{
						timeflag=time(NULL);
						cout<<double(j)/simn*100<<"%, "<<(time(NULL)-origintime)*(simn-j)/j<<" 秒剩余"<<endl;
					}
					s+=Sim();
				}
				break;
			}
			case 2:
			{
				GenerateInitSimGolden();
				cout<<"模拟进行中...\n";
				origintime=time(NULL);
				for(j=0;j<simn;j++)
				{
					if (((time(NULL)-origintime)%10==9)&&(time(NULL)!=timeflag))
					{
						timeflag=time(NULL);
						cout<<double(j)/simn*100<<"%, "<<(time(NULL)-origintime)*(simn-j)/j<<" 秒剩余"<<endl;
					}
					s+=SimGolden();
				}
				break;
			}
			case 3:
			{
				GenerateInitSimAll();
				cout<<"模拟进行中...\n";
				origintime=time(NULL);
				for(j=0;j<simn;j++)
				{
					if (((time(NULL)-origintime)%10==9)&&(time(NULL)!=timeflag))
					{
						timeflag=time(NULL);
						cout<<double(j)/simn*100<<"%, "<<(time(NULL)-origintime)*(simn-j)/j<<" 秒剩余"<<endl;
					}
					s+=SimAll();
				}
				break;
			}
			default:
			{
				goto endit;
			}
		}		
		cout<<"结果：\n"<<double(s)/simn/5<<"\n===============\n";
	}
	endit: return 0;
}

void CollectionProgressReset()
{
	dustinit=0;
	memset(cardcurrentarr,0,sizeof(cardcurrentarr));
	for(i=0;i<setn;i++)
		for(j=0;j<8;j++)
		{
			cardcurrentarr[i][j][0]=cardnarr[i][j];
		}
}
void CollectionProgressLoad()
{
	flag=0;
	fin.open("炉石全卡收集模拟器.txt");
	getline(fin,strtmp);
	for(i=1;i<setn;i++)
	{
		for(j=0;j<8;j++)
		{
			tmp=0;
			for(k=0;k<3;k++)
			{
				fin>>cardcurrentarr[i][j][k];
				tmp+=cardcurrentarr[i][j][k];
			}
			if (tmp!=cardnarr[i][j])
			{
				cout<<"错误的"<<setname[i]<<""<<rarityname[j]<<"卡数总和，应该是"<<cardnarr[i][j]<<"而不是"<<cardcurrentarr[i][j][0]<<"+"<<cardcurrentarr[i][j][1]<<"+"<<cardcurrentarr[i][j][2]<<"="<<tmp<<"。\n";
				flag=1;
			}
			if (j%4==3 && cardcurrentarr[i][j][2]!=0)
			{
				cout<<"错误的有两张"<<setname[i]<<""<<rarityname[j]<<"的卡数, 有两张的传奇的卡数应始终为0而不是"<<cardcurrentarr[i][j][2]<<"。\n";
				flag=1;				
			}
	 	}
		getline(fin,strtmp);
	}
	fin>>dustinit;
	fin.close();
}

void Init()
{
	srand(unsigned(time(NULL)));
	tmp=0;
	for(i=0; i<8; i++)
		{
			tmp+=cardp[i];
			cardpagg[i]=tmp;
		}
	tmp=0;
	memcpy(&cardn,&cardnarr[option/10],sizeof(cardnarr[option/10]));
	memcpy(&cardcurrent,&cardcurrentarr[option/10],sizeof(cardcurrentarr[option/10]));
	for(i=0; i<8; i++)
		{
			tmp+=cardn[i];
			cardnagg[i]=tmp;
		}
}

void GenerateInitSim()
{
	dustneedinit=-dustinit;
	tmp=0;
	for(i=0;i<4;i++)
	{
		for(j=0;j<3;j++)
		{
			for(k=0;k<cardcurrent[i][j];k++)
			{
				cardinit[tmp]=j;
				dustneedinit+=dustcr[i]*(cardmax[i]-j);
				tmp++;
			}
		}
	}
	for(i=4;i<8;i++)
	{
		for(j=0;j<3;j++)
		{
			for(k=0;k<cardcurrent[i][j];k++)
			{
				cardinit[tmp]=0;
				dustneedinit-=dustde[i]*j;
				tmp++;
			}
		}
	}	
}

void GenerateInitSimGolden()
{
	dustneedinit=-dustinit;
	tmp=0;
	for(i=0;i<4;i++)
	{
		for(j=0;j<3;j++)
		{
			for(k=0;k<cardcurrent[i][j];k++)
			{
				cardinit[tmp]=0;
				dustneedinit-=dustde[i]*j;
				tmp++;
			}
		}
	}
	for(i=4;i<8;i++)
	{
		for(j=0;j<3;j++)
		{
			for(k=0;k<cardcurrent[i][j];k++)
			{
				cardinit[tmp]=j;
				dustneedinit+=dustcr[i]*(cardmax[i]-j);
				tmp++;
			}
		}
	}	
}

void GenerateInitSimAll()
{
	dustneedinit=-dustinit;
	tmp=0;
	for(i=0;i<4;i++)
	{
		for(j=0;j<3;j++)
		{
			for(k=0;k<cardcurrent[i][j];k++)
			{
				cardinit[tmp]=j;
				dustneedinit+=dustcr[i]*(cardmax[i]-j);
				tmp++;
			}
		}
	}
	for(i=4;i<8;i++)
	{
		for(j=0;j<3;j++)
		{
			for(k=0;k<cardcurrent[i][j];k++)
			{
				cardinit[tmp]=j;
				dustneedinit+=dustcr[i]*(cardmax[i]-j);
				tmp++;
			}
		}
	}	
}

int Sim()
{
	cardsum=0;
	dustsum=0;
	dustneed=dustneedinit;
	memcpy(&card,&cardinit,sizeof(cardinit));
	while (dustsum<dustneed)
		{
			NewCard();
			cardsum++;
			if ((newcardtype>3)||(card[newcardnum]==cardmax[newcardtype]))
				{
					dustsum+=dustde[newcardtype];
					continue;
				}
			card[newcardnum]++;
			dustneed-=dustcr[newcardtype];
		}
	return(cardsum);
}

int SimGolden()
{
	cardsum=0;
	dustsum=0;
	dustneed=dustneedinit;
	memcpy(&card,&cardinit,sizeof(cardinit));
	while (dustsum<dustneed)
		{
			NewCard();
			cardsum++;
			if ((newcardtype<=3)||(card[newcardnum]==cardmax[newcardtype]))
				{
					dustsum+=dustde[newcardtype];
					continue;
				}
			card[newcardnum]++;
			dustneed-=dustcr[newcardtype];
		}
	return(cardsum);
}

int SimAll()
{
	cardsum=0;
	dustsum=0;
	dustneed=dustneedinit;
	memcpy(&card,&cardinit,sizeof(cardinit));
	while (dustsum<dustneed)
		{
			NewCard();
			cardsum++;
			if (card[newcardnum]==cardmax[newcardtype])
				{
					dustsum+=dustde[newcardtype];
					continue;
				}
			card[newcardnum]++;
			dustneed-=dustcr[newcardtype];
		}
	return(cardsum);
}

void NewCard()
{
	tp1=((rand()<<16)+(rand()<<1)+rand()%2)%1000000; //(rand()<<16)+(rand()<<1)+rand()%2 return 32bit rand instead of 16bit, http://www.cnblogs.com/heyonggang/archive/2012/12/12/2814271.html
	newcardtype=0;
	while (tp1>=cardpagg[newcardtype]) newcardtype++;
	tp2=((rand()<<16)+(rand()<<1)+rand()%2)%cardn[newcardtype];
	newcardnum=cardnagg[newcardtype]-cardn[newcardtype]+tp2;
}

//462, https://www.reddit.com/r/hearthstone/comments/2mkgu4/a_program_that_calculates_how_many_packs_are/
//497, https://www.reddit.com/r/hearthstone/comments/1wn50k/til_for_a_full_collection_you_need_to_open_497/
//447, https://www.reddit.com/r/hearthstone/comments/2jejd5/the_amount_of_packs_needed_for_a_complete/
//512, http://www.icy-veins.com/forums/topic/9868-card-collection-cost-hearthstone/
//510, http://2p.com/1817915_1/Easy-Math-Helps-You-Get-all-of-the-Cards-in-Hearthstone.htm
//512, http://iam.yellingontheinternet.com/2013/10/10/from-dust-to-dust-the-economy-of-hearthstone/
