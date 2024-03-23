#include<fstream>
using namespace std;
ifstream fin("input4.txt");
ofstream fout("output.txt");

int main()
{
	int a[100][100],n,i,j,k,x=0,g;
	fin>>n;
	for(i=1;i<=n;i++)
		for(j=1;j<=n;j++)
		{
			fin>>a[i][j];
			if(a[i][j]==0) x++;
		}
	fout<<x/2<<endl;	
	g=0;
	for(k=1;k<=(n+1)/2;k++)
	{
		for(j=k;j<=n+1-k;j++) 
			if(a[k][j]==0)
			{
				g++;
				if(g%2==0) fout<<k<<" "<<j<<endl; 
			}
		for(i=k+1;i<=n+1-k;i++)
			if(a[i][n+1-k]==0)
			{
				g++;
				if(g%2==0) fout<<i<<" "<<n+1-k<<endl; 
			}	
		for(j=n-k;j>=k;j--)
			if(a[n+1-k][j]==0)
			{
				g++;
				if(g%2==0) fout<<n+1-k<<" "<<j<<endl; 
			}
		for(i=n-k;i>=k+1;i--)
			if(a[i][k]==0)
			{
				g++;
				if(g%2==0) fout<<i<<" "<<k<<endl; 
			}	
	}				
	fin.close();
	fout.close();
	return 0;
}