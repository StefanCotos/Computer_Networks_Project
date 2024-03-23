#include <fstream>
using namespace std;
ifstream fin("input5.txt");
ofstream fout("output.txt");

int A[101][101],n,p;

int main()
{
    fin>>n>>p;
    for(int k=1;k<=p;k++)
    {
        int x,y,l,c;
        fin>>x>>y>>l>>c;
        for(int i=x;i<x+l;i++)
            for(int j=y;j<y+l;j++)
                A[i][j]=c;
    }
    //for(int i=1;i<=n;i++)
    {
        for(int j=1;j<=n;j++)
            fout<<A[i][j]<<" ";
        fout<<endl;
    }
    return 0;
}