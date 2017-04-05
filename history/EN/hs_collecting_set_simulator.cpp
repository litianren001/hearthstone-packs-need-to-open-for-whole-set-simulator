#include <iostream>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include<fstream>
#define setn 5
using namespace std;

const int dustcr[8]= {40,100,400,1600,400,800,1600,3200}; //[isgolden(0~1)*4+rarity(0~3)], craft
const int dustde[8]= {5,20,100,400,50,100,400,1600}; //disenchant
const int cardmax[8]= {2,2,2,1,2,2,2,1}; //max amount of each rarity
const int cardp[8]= {703567,216017,40850,9398,14826,12668,1933,741}; //card possibility based on 1/1000000, http://hearthstone.gamepedia.com/Pack
const int cardnarr[setn][8]={{1,1,1,1,1,1,1,1},
						 {94,81,37,33,94,81,37,33},
						 {40,37,26,20,40,37,26,20},
						 {49,36,27,20,49,36,27,20},
						 {49,36,27,20,50,36,27,21}};//amount of cards of each rarity of each set
const string setname[5]={"debug","classic","GVG","TGT","WOG"};
const string rarityname[8]={"common","rare","epic","legendary","golden common","golden rare","golden epic","golden legendary"};

//int cardn[8]={1,1,1,1}; //Debug
//int cardn[8]= {94,81,37,33}; // Classic, 245 total
//int cardn[8]={40,37,26,20}; //GVG, 123 total
//int cardn[8]={49,36,27,20}; //TGT, 132 total
//int cardn[8]={50,36,27,21}; //WOG, 134 total, note that non-golden C'Thun and Beckoner of Evil are free

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
void CollectionProgressLoad(); //load collection progress from hs_collecting_set_simulator.txt
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
	cout<<"Hearthstone Collecting Set Simulator by litianren001@github\n";
	CollectionProgressReset();
	while (true)
	{
		cout<<"input number 11-43 to simulate based on different sets and strategies\ntens place: 1:classic 2: GVG 3:TGT 4:WOG\nones place: 1:Collect normal 2: Collect golden 3: Collect all\ninput 0 to load collection progress from hs_collecting_set_simulator.txt or -1 to reset\ninput bigger than 999 to change simulation times (default is 100000)\n";
		cin>>option;
		if (option==0)
		{
			CollectionProgressLoad();
			if (flag==1)
			{
				CollectionProgressReset();
				cout<<"Collection progress is reset. Press enter to continue.\n===============\n";
				getline(cin,strtmp);
				getline(cin,strtmp);
			}
			else cout<<"Collection pregress is loaded.\n===============\n";
			continue;
		}
		if (option==-1)
		{
			CollectionProgressReset();
			cout<<"Collection pregress is reset.\n===============\n";
			continue;
		}
		if (option>999)
		{
			simn=option;
			cout<<"Simulation times is changed.\n===============\n";
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
				cout<<"Simulation in progress...\n";
				origintime=time(NULL);
				for(j=0;j<simn;j++)
				{
					if (((time(NULL)-origintime)%10==9)&&(time(NULL)!=timeflag))
					{
						timeflag=time(NULL);
						cout<<double(j)/simn*100<<"%, "<<(time(NULL)-origintime)*(simn-j)/j<<" seconds remain."<<endl;
					}
					s+=Sim();
				}
				break;
				/*		avg packs opened
				CLSC	491				
				GVG		306			
				TGT		310	
				WOG		310		
				*/
			}
			case 2:
			{
				GenerateInitSimGolden();
				cout<<"Simulation in progress...\n";
				origintime=time(NULL);
				for(j=0;j<simn;j++)
				{
					if (((time(NULL)-origintime)%10==9)&&(time(NULL)!=timeflag))
					{
						timeflag=time(NULL);
						cout<<double(j)/simn*100<<"%, "<<(time(NULL)-origintime)*(simn-j)/j<<" seconds remain."<<endl;
					}
					s+=SimGolden();
				}
				break;
				/*		avg packs opened
				CLSC	2598			
				GVG		1502			
				TGT		1551			
				WOG		1579		
				*/				
			}
			case 3:
			{
				GenerateInitSimAll();
				cout<<"Simulation in progress...\n";
				origintime=time(NULL);
				for(j=0;j<simn;j++)
				{
					if (((time(NULL)-origintime)%10==9)&&(time(NULL)!=timeflag))
					{
						timeflag=time(NULL);
						cout<<double(j)/simn*100<<"%, "<<(time(NULL)-origintime)*(simn-j)/j<<" seconds remain."<<endl;
					}
					s+=SimAll();
				}
				break;
				/*		avg packs opened
				CLSC	2782			
				GVG		1622			
				TGT		1672			
				WOG		1700		
				*/	
			}
			default:
			{
				goto endit;
			}
		}		
		cout<<"Result:\n"<<double(s)/simn/5<<"\n===============\n";
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
	fin.open("hs_collecting_set_simulator.txt");
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
				cout<<"Error in the amount of "<<setname[i]<<" "<<rarityname[j]<<", which should be "<<cardnarr[i][j]<<" rather than "<<cardcurrentarr[i][j][0]<<"+"<<cardcurrentarr[i][j][1]<<"+"<<cardcurrentarr[i][j][2]<<"="<<tmp<<".\n";
				flag=1;
			}
			if (j%4==3 && cardcurrentarr[i][j][2]!=0)
			{
				cout<<"Error in the amount of "<<setname[i]<<" "<<rarityname[j]<<" with 2 copies, which should be 0 forever rather than "<<cardcurrentarr[i][j][2]<<".\n";
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
