#include <fstream>
using namespace std;

ifstream is("input2.txt");
ofstream os("output.txt");

void citire(int a[], int n) // citeste elementele vectorului
{
    if(n>0)
    {
        citire(a,n-1);
        is>>a[n];
    }
}

void afisare(int a[], int n) //afiseaza elementele vectorului
{
    if(n>0)
    {
        afisare(a,n-1);
        os<<a[n]<<" ";
    }
}

int minim(int a[],int n) //returneaza minimul din vector
{
    if(n==1) return a[1];
    else return min(a[n],minim(a,n-1));
}

void del(int a[], int &n, int i) //sterge pozitia i
{
	if(i==n) n--;
	else
	{ a[i]=a[i+1];
	  del(a,n,i+1);
	}
}

void delmin(int a[], int &n, int i, int min) //sterge toate aparitiile elementului minim
{
    if(i<=n)
        if(a[i]==min)
        {
            del(a,n,i);
            delmin(a,n,i,min);
        }
        else delmin(a,n,i+1,min);
}

int main()
{
    int a[100],n;
    is>>n;
    citire(a,n);
    delmin(a,n,1,minim(a,n));
    afisare(a,n);
    is.close();
    os.close();
    return 0;
}
