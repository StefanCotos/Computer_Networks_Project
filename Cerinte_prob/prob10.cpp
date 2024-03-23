#include <fstream>
using namespace std;
ifstream is("input10.txt");
ofstream os("output.txt");

int main()
{
    int n,x,ap1[100]={0}, ap2[100]={0};
    is>>n;
    for(int i=1;i<=n;i)
    {
        is>>x;
        if(ap1[x]==0) ap1[x]=i;
        else if(ap2[x]==0) ap2[x]=i;
             else if(i-ap2[x]>ap2[x]-ap1[x])
             {
                 ap1[x]=ap2[x];
                 ap2[x]=i;
             }
    }
    int maxx=0;
    for(int i=0;i<100;i++)
        if(ap2[i]-ap1[i]>maxx) maxx=ap2[i]-ap1[i];
    os<<maxx+1;
    is.close();
    os.close();
    return 0;
}