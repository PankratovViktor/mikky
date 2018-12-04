#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <vector>
#include <time.h>
#include <fcntl.h>
#include <iostream>
using namespace std;
struct part
{
	string from;
	string now;
	string to;
	int doub;
	int ifto;
	int iffrom;
};
vector <string> split (vector <string> a,string word)
{
	std::string div1 = " ";
	std::string c;
	char div2 = '\t';
	size_t pos =0;
	while (((pos = word.find(div1)) != std::string::npos)||(word.find(div2)!=std::string::npos))
	{
		if ((word.find(div2)!= std::string::npos)&&(word.find(div2) < pos))
		{
			pos = word.find(div2);
		}
		if ((word[0] == ' ') ||(word[0] == '\t')) 
		{
			word.erase(0,1);
		}
		else 
		{
			c = word.substr(0,pos);
			a.push_back(c);
			word.erase(0,pos + 1);
		}
	}
	a.push_back(word);
	return a;	
}
struct rusage timebegin()
{
	struct rusage a;
	int b = getrusage(RUSAGE_CHILDREN , &a);
	return a;
}
void timeend(struct rusage mytime, struct timeval time1 , struct timeval time2)
{
	struct timeval utime;
	struct timeval stime;
	if ((getrusage(RUSAGE_CHILDREN , &mytime ))!=0)
	{}
	utime = mytime.ru_utime;
	stime = mytime.ru_stime;
	cout << "\n user	"<<( (double) (utime.tv_sec - time1.tv_sec) + (double)(utime.tv_usec - time1.tv_usec)/1000000) << "\n sys	" << ((double)(stime.tv_sec - time2.tv_sec) +(double) (stime.tv_usec - time2.tv_usec)/1000000) << "\n";
}
vector <struct part> convsplit(string word)
{
	vector <struct part> letswork;
	vector <struct part> notworking;
	int j = 0;
	int c = 0;
	struct part party;
	struct rusage mytime;
	party.to = "";
	party.from = "";
	party.now ="";
	party.doub = 0;
	party.ifto = 0;
	party.iffrom = 0;
	int to = 0;
	letswork.push_back(party);
	notworking.push_back(party);
	notworking[0].now = " ";
	string myst;
	for (int i = 0 ; i < word.length();i++)
	{
		if ((word[i] != '<') && (word[i] != '>') && (word[i] != '|'))
		{
			if ((letswork[j].from != "") || (word[i] != ' '))
			{
				letswork[j].from = letswork[j].from + word[i];
			}
		}
		else
		{
			if (word[i] == '|')
			{
				string myst = letswork[j].to;
				if( c > 2)
				{
					letswork[j].to = letswork[j].from;
					letswork[j].from = myst;
				}
				if (c == 0)
				{
					letswork[j].now = letswork[j].from;
					letswork[j].from = "";
				}
				c = 0;
				j++;
				letswork.push_back(party);
			}
			if (word[i] == '<')
			{
				letswork[j].iffrom++;
				if (c == 0)
				{
					letswork[j].now = letswork[j].from;
					letswork[j].from = "";
				}
				else
				{
					letswork[j].to = letswork[j].from;
					letswork[j].from = "";
					to = 0;
				}
				c++;
			}
			if (word[i] == '>')
			{
				if(word[i+1] == '>')
				{
					letswork[j].doub = 1;
				}
				else
				{
					letswork[j].ifto++;
					if (c == 0)
					{
						letswork[j].now = letswork[j].from;
						letswork[j].from = "";
						to = 1;
					}
					else
					{
						letswork[j].to = letswork[j].from;
						letswork[j].from = "";
						to = 1;
						c++;
					}
					c++;
				}
			}
		}
	}
	if (c == 0)
	{
		letswork[j].now = letswork[j].from;
		letswork[j].from = "";
	}
	if ( to == 1)
	{
		myst = letswork[j].to;
		letswork[j].to = letswork[j].from;
		letswork[j].from = myst;
	}
	if ((letswork[0].iffrom >1) || (letswork[letswork.size() - 1].ifto >1))
	{
		cout << "too many directions\n";
		return notworking;
	}
	cout << letswork[j].to << " " << letswork[j].from;
	return letswork;
}
string read(string b)
{

	if (!(std::getline(std::cin,b)))
	{
		b = "quit";
	}
	return b;
}
int getdirect()
{
	std::string cwd = getcwd(NULL,0);
	cout << cwd << "/Microsha";
	if (getuid() == 0)
	{
		cout << "!";
	}
	else
	{
		cout << ">";
	}
}
void execution()
{
	while (true)
	{
		int corr = getdirect();
		vector <string> mine;
		vector <char *> argv;
		string b;
		int needtime = 0;
		size_t newperem;
		b = read(b);
		vector<struct part> letswork;
		letswork = convsplit(b);
		if (b == "quit")
		{
			break;
		}
		string b1 = "";
		string b2 = "";
		vector <string> a;
	       	a = split(a,letswork[0].now);
		if (a[0] == "time")
		{
			for (int i = 0; i < a.size() - 1 ; i++)
			{
				a[i] = a[i+1];
			}
			a.pop_back();
			needtime = 1;
		}	
		if (a[0] == "cd")
		{
			int changeit;
			if (a.size() == 1)
			{
				string c;
				c = "/home";
				changeit = chdir(c.c_str());
			} 
			if (a.size() > 2)
			{
				cout <<"Wrong arguments number";
			}
			if (a.size() == 2)
			{
				changeit = chdir(a[1].c_str());
			}
		}
		else
		{	
			struct rusage mytime = timebegin();
			struct timeval utime;
			struct timeval realtime;
			struct timeval realtimeend;
			struct timezone zone1;
			gettimeofday(&realtime,&zone1);
			utime = mytime.ru_utime;
			cout << "\n" << "seichas vremya" << utime.tv_usec;
			struct timeval stime;
			stime = mytime.ru_stime;
			pid_t pid = fork();
			if (pid == 0)
			{
				vector <char*> arguments;
				if (letswork[0].from != "")
				{
					int from = open(letswork[0].from.c_str(),O_RDONLY,0666);
					int op1 = dup2(from,0);
				}
				if ((letswork[letswork.size() -1].to != "") && (letswork[letswork.size() -1].doub == 0))
				{
					int to = open(letswork[letswork.size() - 1].to.c_str(),O_WRONLY|O_CREAT|O_TRUNC,0666);
					int close1 = dup2(to,1);
				}
				if ((letswork[letswork.size() -1].to != "") && (letswork[letswork.size() -1].doub == 1))
				{
					int to = open(letswork[letswork.size() -1].to.c_str(),O_WRONLY|O_CREAT|O_APPEND,0666);
					int close = dup2(to,1);
				}
				for(int i = 0 ; i < a.size() ; i++)
				{
					mine.push_back(a[i]);
					arguments.push_back(&mine[i][0]);
				}
				arguments.push_back(NULL);
		     		execvp(mine[0].c_str(),&arguments[0]);
				exit(0);
			}
			else
			{
				int x;
				wait(&x);
			}
			gettimeofday(&realtimeend,&zone1);
			if (needtime == 1)
			{
				cout << "\n real	" <<(double)( realtimeend.tv_sec - realtime.tv_sec) + ((double)(realtimeend.tv_usec - realtime.tv_usec))/1000000;
				timeend(mytime,utime,stime);
			}
		}
	}
}
int main()
{
	execution();
}


