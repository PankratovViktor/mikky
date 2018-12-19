#include <string.h>
#include <fnmatch.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
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
struct pipeelem
{
	int a[2];
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
string setprob(string b)
{
	while (b[b.size()-1] == ' ')
	{
		b.erase(b.size() -1);
	}
	return b;
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
		if ((word[i] != '<') && (word[i] != '>') && (word[i] != '|') )
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
		return notworking;
	}
	letswork[0].from = setprob(letswork[0].from);
	letswork[letswork.size()-1].to = setprob(letswork[letswork.size() -1].to);
	for (int k =0 ; k < letswork.size() ;k++)
	{
		letswork[k].from = setprob(letswork[k].from);
		letswork[k].to = setprob(letswork[k].to);
		letswork[k].now = setprob(letswork[k].now);
	}
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
	char * buf = new char[10000];
	getcwd(buf,10000);
	cout << string(buf) << "/Microsha";
	if (getuid() == 0)
	{
		cout << "!";
	}
	else
	{
		cout << ">";
	}
	delete [] buf;
	return 0;
}
int execute (vector < vector < string > > direktlist, vector <string > mine,vector <char * > arguments)
{
	int i;
	int j;
    	vector < char* > argiments;
	for ( i =0 ; i < mine.size() ; i++)
	{
		if ((mine[i].find('*') != std::string::npos) || (mine[i].find('?') != std::string::npos))
		{
			if (direktlist[i].size() == 0)
			{
				exit(0);
			}
			for(j = 0 ; j < direktlist[i].size() ; j++)
			{
				argiments.push_back(&direktlist[i][j][0]);
			}
		}
        	else
        	{
//			if ((i != 0) || (mine[i] != "pwd"))
//			{
           			argiments.push_back(&mine[i][0]);
//			}
//			else
//			{
//				char * buf = new char[10000];

//				printf("%s%s\n",getenv("PATH"),buf);	
//			}	
        	}
	}
	argiments.push_back(NULL);
	if ( argiments[0] != NULL)
	{
        	execvp(mine[0].c_str(),&argiments[0]);
	}
        exit(0);
	return 0;
}
vector <string> parse(string arg)
{
	int i = 0;
	vector < string > ans;
	string news ;
	news = "";
	while (i < arg.length())
	{
		if (arg[i] != '/')
		{
			news = news + arg[i];
		}
		else
		{
			ans.push_back(news);
			news = "";
		}
		i++;
	}
	ans.push_back(news);
	return ans;
}
vector <vector <string> > scanner(string c, string where, int currentelem , vector <vector < string> > direktlist, DIR *d, vector <string> pathy , int i)
{
	if ((d = opendir(where.c_str())) == NULL)
    	{
        	perror("opendir: ") ;
        	return direktlist;
    	}
	string where1;
	string c1;
	for (dirent *entry = readdir(d) ; entry != NULL ; entry = readdir(d))
	{
		string b = entry->d_name;
		if ( currentelem != 0)
		{	
			c1 = c + "/" + entry->d_name ;
		}
		else
		c1 = c + entry->d_name;
        	if ((b == ".")||(b == "..")) continue;
		if ((currentelem == (pathy.size() -1)) && (fnmatch(pathy[pathy.size() -1].c_str() , b.c_str(),FNM_PATHNAME) == 0))
		{
			direktlist[i].push_back(c1);
		}
		where1 = where + "/" + entry->d_name ;
		struct stat isitway;
		stat (where1.c_str() , &isitway);
		if ((S_ISDIR(isitway.st_mode))&&(currentelem < pathy.size())  && (currentelem <= pathy.size()))
		{
			if (fnmatch(pathy[currentelem].c_str(), b.c_str(),FNM_PATHNAME) == 0)
			{
				direktlist = scanner(c1,where1,currentelem + 1,direktlist,d,pathy,i);
			}
		}
	}
	closedir(d);
	return direktlist;			
}
vector <vector <string> > updatelist(vector<vector<string> > direktlist,string argument,string where,int i)
{
	DIR *d ;
	vector <string> pathy;
	pathy = parse(argument);
	string c;
	int currentelem = 0;
	if (pathy[0] == "")
	{
		c = "/";
		where = "/";
		d = opendir("/");
		currentelem++;	
	}
	else
	{
		c = "";
		d = opendir(where.c_str());
	}
	direktlist = scanner(c,where,currentelem,direktlist,d,pathy,i);
	closedir(d);
	return direktlist;
}
static void ihatectrlc(int ctrlc)
{
}
void execution()
{
	signal(SIGINT,ihatectrlc);
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
		vector <string> aa;
	       	aa = split(aa,letswork[0].now);
		if (aa[0] == "time")
		{
			for (int i = 0; i < aa.size() - 1 ; i++)
			{
				aa[i] =aa[i+1];
			}
			aa.pop_back();
			needtime = 1;
			while (letswork[0].now[0] != 'e')
			{
				letswork[0].now.erase(letswork[0].now.begin());
			}
			letswork[0].now.erase(letswork[0].now.begin());
		}	
		if (aa[0] == "cd")
		{
			int changeit;
			if (aa.size() == 1)
			{
				string c;
				c = "/home";
				changeit = chdir(getenv("HOME"));
			} 
			if (aa.size() > 2)
			{
			}
			if (aa.size() == 2)
			{
				changeit = chdir(aa[1].c_str());
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
			struct timeval stime;
			stime = mytime.ru_stime;
			vector <struct pipeelem> pipevec;
			struct pipeelem pipi;           
	  		char * buf = new char[10000];
		        getcwd(buf,10000);  
		        std::string cwd(buf);
        		delete [] buf;          
			string currentdir;
			currentdir = cwd;
			vector <char*> arguments;
			for (int l = 0 ; l < letswork.size() - 1; l++)
			{
				pipe(pipi.a);
				pipevec.push_back(pipi);
			}
			for (int l = 0 ; l < letswork.size() ; l++)
			{
	                pid_t pidi = fork();
        	        if (pidi == 0)
        	        {
				for (int i = 0 ; i < pipevec.size();i++)
				{
					if (i != (l)) 
					{
						close(pipevec[i].a[1]);
					}
					if (i != (l-1))
					{
						close(pipevec[i].a[0]);
					}
				}
				if (l != 0)
				{
					int open = dup2(pipevec[l-1].a[0],0);
				}
				if (l != (letswork.size() - 1))
				{
					int close = dup2(pipevec[l].a[1],1);
		     		}
				if (( l == 0) && (letswork[l].from != ""))
				{
					int open1 = open(letswork[l].from.c_str(),O_RDONLY,0666);
					int open2 = dup2(open1,0);
				}
				if (( l == letswork.size() - 1) && (letswork[l].to != ""))
				{
					if (letswork[l].doub == 0)
					{
						int close1 = open(letswork[l].to.c_str(),O_WRONLY|O_CREAT|O_TRUNC,0666);
						int close3 = dup2(close1,1);
					}
					else
					{
						int close2 = open(letswork[l].to.c_str(),O_WRONLY|O_CREAT|O_APPEND,0666);
						int close4 = dup2(close2,1);
					}
				}
				vector <string> a;
				a = split(a,letswork[l].now);	
				for (int i = 0 ; i < a.size() ; i++)
				{
					mine.push_back(a[i]);
				}
				vector <vector <string> > directlist;
				vector <string> empty;
				const char * cbp;
				const char * cmp;
				string no;
				for (int i = 0 ; i < mine.size() ; i++)
				{   
					if (((mine[i].find('*')) != std::string::npos) || ((mine[i].find('?')) != std::string::npos))						
					{         
						directlist.push_back(empty);
						directlist = updatelist(directlist, mine[i] ,currentdir,i );	
					}
					else
					{
						directlist.push_back(empty);
						directlist[i].push_back(mine[i]);
					}
				}
				int ifwe = execute(directlist , mine,arguments);					
			}
		}
		gettimeofday(&realtimeend,&zone1);
	//	if (needtime == 1)
	//	{
	//		cout << "\n real	" <<(double)( realtimeend.tv_sec - realtime.tv_sec) + ((double)(realtimeend.tv_usec - realtime.tv_usec))/1000000;
	//		timeend(mytime,utime,stime);
	//	}
        	for (int i = 0 ; i < pipevec.size() ; i++)
        	{
        		close(pipevec[i].a[1]);
        		close(pipevec[i].a[0]);
        	}
        	int child;
        	while(wait(&child) > 0);
		gettimeofday(&realtimeend,&zone1);
		if (needtime == 1)
		{
			cout << "\n real " <<(double)( realtimeend.tv_sec - realtime.tv_sec) + ((double)(realtimeend.tv_usec - realtime.tv_usec))/1000000;
			timeend(mytime,utime,stime);
			needtime = 0;
		}	
	} 
    }
}
int main()
{
	execution();
}


