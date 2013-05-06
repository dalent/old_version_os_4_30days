#include <set>
#include<stdlib.h>
#include<time.h>
using namespace std;
//set结构的随机数
class IntSetImp
{
	public:
		InsetImp(){}
		int size();
		void insert(int n);
		void report(int *v);
};
class IntSetSTL:public IntSetImp
{
	private:
		set<int> S;
	public:
		IntSetSTL(int maxelements, int maxval)
		{
			
		}
		
		int size(){return S.size();}
		
		void insert(int t){S.insert(t);}
		
		void report(int *v)
		{
			int j = 0;
			set<int>::iterator i;
			for(i = S.begin(); i != S.end(); i++)
				v[j++] = *i;
		}
		
};
//用法
void getsets(int m, int maxval)
{
	srand((unsigned int)time(0));
	int *v = new int[m];
	IntSetSTL S(m,maxval);
	while(S.size() < m)
		S.insert(rand()%maxval);
	S.report(v);
	for(int i = 0; i < m; i++)
	{
		cout << v[i] << endl;
	}
}

//线性结构
class IntSetArray
{
	pirvate:
		int n, *x;
	InsetArray(maxelements, int maxval)
	{
		x = new int[maxelements + 1];
		n = 0;
		x[0] = maxval;
	}
	
	void insert(int t)
	{
		for(int i = 0; x[i] < t; i++);
		for(int j = n; j > i; j--)
			x[j + 1] = x[j];
		x[i] = t;
		n++;
	}
	
	int size(){return n;}
	
	void report(int *v)
	{
		for( int i = 0; i < n; i++)
		{
			v[i] = x[i];
		}
	}
};
//链表结构
class IntsetList
{
private:
	int n;
	struct node{
		int val;
		node *next;
		node(int v, node*p){val = v; next = p;}
	};
	node *head, *sentinel;
public:
	IntsetList(int maxelements, int val)
	{
		sentinel = head = new node(naxval, 0);
		n = 0;
	}
	void report(int *v)
	{
		int j = 0;
		for(p = head; p != sentinel; p = p->next)
		{
			v[j++] = p->val;
		}
	}
	void insert(int t)
	{
		head = rinsert(head, t);
	}
	node *rinsert(p, t)
	{
		if(p->val < t)
			p->next = rinsert(p->next ,t);
		else
		{
			p = new node(t, p);
			n++;
		
		}
		
		return p;
		
	}
};

//2分搜索树
class IntsetBST
{
	int n, *v, vn;
	struct node{
		int val;
		node *left, node*right;
		node(int i){val = i; left = right = 0;
	};
	node *root;
	IntsetBST(int maxelements, int maxval)
	{
		root = 0; n = 0;
	}
	void insert(int t){root = rinsert(root, t);}
	void report(int *x){ v = x; vn = 0; traverser(root);}
	node *rinsert(p, t)
	{
		if(p == 0)
		{
			p = new node(t);
			n++;
		}else if(t < p->val)
		{
			p->left = rinsert(p->left, t);
		}else if( t > p->val)
			p->right - rinsert(p->right, t);
		return p;
	}
	
	void traverse(p)
	{
		if(p == 0)
			return;
		traverse(p->left);
		v[vn++] = p->val;
		traverse(p->right);
	}
	
	
};

//位图，箱子