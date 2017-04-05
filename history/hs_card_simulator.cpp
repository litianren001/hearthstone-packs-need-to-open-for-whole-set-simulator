#include <iostream>
#include <cstring>
#include <cstdlib>
#include <ctime>
using namespace std;

const int simn=10000; //simulate times 
const int dustcr[8]= {40,100,400,1600,400,800,1600,3200}; //[isgolden(0~1)*4+rarity(0~3)], craft
const int dustde[8]= {5,20,100,400,50,100,400,1600}; //disenchant
const int cardmax[8]= {2,2,2,1,2,2,2,1}; //max amount of each rarity
const int cardp[8]= {699651,213964,42946,10800,14695,13777,3061,1106}; //card possibility based on 1/1000000, http://hearthstone.gamepedia.com/Pack

//int cardn[8]= {94,81,37,33}; // amount of cards of each rarity, Classic, 245 total, 457 dual considered
int cardn[8]={40,37,26,20}; //GVG, 123 total, 226 dual considered
//int cardn[8]={49,36,27,20}; //TGT, 132 total, 244 dual considered
//int cardn[8]={1,1,1,1}; //Debug

int cardpagg[8]= {}; //aggregated possibility
int cardnagg[8]= {}; //aggrgated number
int card[1000]= {}; //amount of each card
int cardsum,dustsum,dustneed,i,j,tmp,tp1,tp2,newcardtype,newcardnum;
long long s;

void init();
int sim(); //return card amount drawn for full non-golden collection
int simgolden(); //for golden collection
int simall(); // for full collection
void newcard(); //return newcardtype,newcardnum


int main()
{
	init();
	s=0;
	for(j=0;j<simn;j++) s+=sim();
	
	/*		avg packs opened
	CLSC	447				
	GVG		279				
	TGT		282				
	*/
	
	//for(j=0;j<simn;j++) s+=simgolden();
	
	/*		avg packs opened
	CLSC	2300			
	GVG		1320			
	TGT		1366			
	*/

	//for(j=0;j<simn;j++) s+=simall();
	
	/*		avg packs opened
	CLSC	2461			
	GVG		1425			
	TGT		1471			
	*/	
	
	cout<<double(s)/simn/5;
	return 0;
}

void init()
{
	srand(unsigned(time(0)));
	tmp=0;
	for(i=0; i<8; i++)
		{
			tmp+=cardp[i];
			cardpagg[i]=tmp;
		}
	for(i=0; i<4; i++) cardn[i+4]=cardn[i];
	tmp=0;
	for(i=0; i<8; i++)
		{
			tmp+=cardn[i];
			cardnagg[i]=tmp;
		}
}

int sim()
{
	cardsum=0;
	dustsum=0;
	dustneed=0;
	for(i=0; i<4; i++) dustneed+=cardn[i]*dustcr[i]*cardmax[i];
	memset(card,0,sizeof(card));
	while (dustsum<dustneed)
		{
			newcard();
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

int simgolden()
{
	cardsum=0;
	dustsum=0;
	dustneed=0;
	for(i=4; i<8; i++) dustneed+=cardn[i]*dustcr[i]*cardmax[i];
	memset(card,0,sizeof(card));
	while (dustsum<dustneed)
		{
			newcard();
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

int simall()
{
	cardsum=0;
	dustsum=0;
	dustneed=0;
	for(i=0; i<8; i++) dustneed+=cardn[i]*dustcr[i]*cardmax[i];
	memset(card,0,sizeof(card));
	while (dustsum<dustneed)
		{
			newcard();
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

void newcard()
{
	tp1=((rand()<<16)+(rand()<<1)+rand()%2)%1000000; //(rand()<<16)+(rand()<<1)+rand()%2 return 32bit rand instead of 16bit, http://www.cnblogs.com/heyonggang/archive/2012/12/12/2814271.html
	newcardtype=0;
	while (tp1>=cardpagg[newcardtype]) newcardtype++;
	tp2=((rand()<<16)+(rand()<<1)+rand()%2)%cardn[newcardtype];
	newcardnum=cardnagg[newcardtype]-cardn[newcardtype]+tp2;
}

//const int cardp[8]= {6997,2143,428,100,146,141,35,11}; // Validation, ans=497.4 as https://www.reddit.com/r/hearthstone/comments/1wn50k/til_for_a_full_collection_you_need_to_open_497/
//const int cardp[8]= {6992,2133,425,108,150,147,33,12};

	/*		avg packs opened	cards got	avg cards drawn for each card
	CLSC	485.3				457			1.062
	GVG		302.7				226			1.339
	TGT
	*/
	
	/*		avg packs opened	cards got	avg cards drawn for each card
	CLSC	2668.4				914			2.922	
	GVG		1543.8				452			3.415
	TGT
	*/

//462, https://www.reddit.com/r/hearthstone/comments/2mkgu4/a_program_that_calculates_how_many_packs_are/
//497, https://www.reddit.com/r/hearthstone/comments/1wn50k/til_for_a_full_collection_you_need_to_open_497/
//447, https://www.reddit.com/r/hearthstone/comments/2jejd5/the_amount_of_packs_needed_for_a_complete/
//512, http://www.icy-veins.com/forums/topic/9868-card-collection-cost-hearthstone/
//510, http://2p.com/1817915_1/Easy-Math-Helps-You-Get-all-of-the-Cards-in-Hearthstone.htm
//512, http://iam.yellingontheinternet.com/2013/10/10/from-dust-to-dust-the-economy-of-hearthstone/
