#include <iostream>
#include <fstream>
using namespace std;
ifstream fin("input6.txt");
ofstream fout("output.txt");
int main ()
{
    int n, v[25005],z=0;
    fin>>n;
    for(int i=1; i<=n; i++)
    {
        fin >>v[i];
        if(i>1)
        if(v[i]==v[i-1]) //valori alaturate egale
        {
            z++;
            n=n-2;//fac stergerile direct de la citire
            i=i-2;
        }
    }
    fout<<z<<endl;
    for(int i=1;i<=n;i++) fout<<v[i]<<" ";
    return 0;
}