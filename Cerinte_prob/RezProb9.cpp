#include <fstream>
using namespace std;
ifstream fin("input9.txt");
ofstream fout("output.txt");

int main()
{
    int n,k,a[100001],s,smax=0,dr;
    fin>>n>>k;
    for(int i=1;i<=n;i++) fin>>a[i];
    s=0;
    for(int i=1;i<=k;i++) s=s+a[i];
    smax=s;dr=k;
    for(int i=k+1;i<=n;i++)
        {
            s=s+a[i]-a[i-k];
            if(s>smax)
            {
                smax=s;
                dr=i;
            }
        }
    for(int i=dr-k+1;i<=dr;i++) fout<<a[i]<<" ";
    fin.close();
    fout.close();
    return 0;
}