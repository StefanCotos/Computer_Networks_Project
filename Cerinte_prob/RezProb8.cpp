#include <fstream>
using namespace std;
ifstream fin("input8.txt");
ofstream fout("output.txt");

int main()
{
    int n,m,a[100001],b[100001],c[200001];
    fin>>n>>m;
    for(int i=1;i<=n;i++) fin>>a[i];
    for(int i=1;i<=m;i++) fin>>b[i];
    int i=1,j=m,k=0;
    while(i<=n && j>=1)
        if(a[i]<b[j])
        {
            if(a[i]%2==0) c[++k]=a[i++];
            else i++;
        }
        else
            {
                if(b[j]%2==0) c[++k]=b[j--];
                else j--;
            }
    while(i<=n)
    {
        if(a[i]%2==0) c[++k]=a[i++];
        else i++;
    }
    while(j>=1)
    {
        if(b[j]%2==0) c[++k]=b[j--];
        else j--;
    }
    for(i=1;i<=k;i++)
    { fout<<c[i]<<" ";
      if(i%20==0) fout<<endl;
    }
    fin.close();
    fout.close();
    return 0;
}