#include <fstream>
using namespace std;

ifstream is("input1.txt");
ofstream os("output.txt");

void citire(int a[], int n) 
{
    if(n>0)
    {
        citire(a,n-1);
        is>>a[n];
    }
}

void afisare(int a[], int n) 
{
    if(n>0)
    {
        afisare(a,n-1);
        os<<" "<<a[n];
    }
}

void split(int a[],int n,int b[],int &m,int c[],int &k)
{
    if(n>0)
    {
        split(a,n-1,b,m,c,k);
        if(a[n]%2==0)
            b[++m]=a[n];
        else
            c[++k]=a[n];
    }
}

int main()
{
    int a[200],b[100],c[100],n,m=0,k=0;
    is>>n;
    citire(a,n);
    split(a,n,b,m,c,k);
    afisare(b,m);
    os<<endl;
    afisare(c,k);
    is.close();
    os.close();
    return 0;
}