#include <fstream>
using namespace std;
ifstream fin("input3.txt");
ofstream fout("output.txt");
struct nod
{
    int info;
    nod* next;
};

void addsf(nod* &prim, nod* &ultim, int v)
{//adauga la finalul listei
    if(prim==0)
    {
        prim=ultim=new nod;
        prim->info=v;
        prim->next=0;
    }
    else
    {
        nod *nou=new nod;
        nou->info=v;
        nou->next=0;
        ultim->next=nou;
        ultim=nou;
    }
}

void afis(nod *prim)
{//afiseaza lista
    for(nod* p=prim; p; p=p->next)
        fout<<p->info<<" ";
    fout<<endl;
}

void stergprim(nod* &prim, nod* &ultim)
{//sterge primul nod al listei
    nod* p=prim;
    prim=prim->next;
    if(prim==0) ultim=0;
    delete p;
}

void permutstanga(nod* &prim, nod* &ultim)
{//permuta lista cu o pozitie spre stanga
    if(prim)
    {
        addsf(prim, ultim, prim->info);//adauga la sfarsit un nod cu informatia din primul nod
        stergprim(prim, ultim);//sterge primul nod
    }
}

int main()
{
    int k,v;
    nod* prim, *ultim;
    prim=ultim=0;
    fin>>k;
    while(fin>>v) addsf(prim, ultim, v);
    for(int i=1;i<=k;i++) permutstanga(prim, ultim);//permut de k ori
    afis(prim);
    fin.close();
    fout.close();
    return 0;
}