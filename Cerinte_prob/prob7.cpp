#include <fstream>
using namespace std;
ifstream fin("input7.txt");
ofstream fout("output.txt");

int main()
{
    int x, p[100]={0}, u[100]={0}, n, lmax=0;
    fin>>n;
    for(int i=1;i<=n;i++)
    {
        fin>>x;
        if(p[x]==0) p[x]=i;//prima aparitie
        else u[x]=i;//utima aparitie
    }
    for(int i=0;i<=99;i++)
        if(u[i]-p[i]+1>lmax) lmax=u[i]-p[i]+1;//diferenta maxima dintre ultima si prima aparitie
    fout<<lmax;
    fin.close();
    fout.close();
    return 0;
}