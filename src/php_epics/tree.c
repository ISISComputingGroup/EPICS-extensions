/****************************************************************************************

	Quad tree (tree with 4 childs) with chained elements (in order to walk through all elements)

	Alain Bertrand / Renata Krempaska
	2004 - SLS / PSI

****************************************************************************************/

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <cadef.h>

#include "tree.h"

node *root=NULL;
node *begin=NULL;
node *end=NULL;

int maxlevel=0;

unsigned int crc_32(unsigned char str[])
{
	unsigned int res;
	int i;

	res=0;
	for(i=0;i < strlen(str);i++)
	{
		//res^=(unsigned int)str[i]<<((i%4)*8);
		res = (res ^ str[i]) + ((res<<26)+(res>>6));
	}
	return res/256;
}

void add_node(node *newnode,int link)
{
        node *n,*c;
	int comp;
	unsigned int crc,cn;

	if(begin == NULL)
	{
		begin=newnode;
		end=newnode;
	}
        if(root == NULL)
        {
		root=newnode;
                return;
        }
	crc=newnode->crc;

        c=root;

        while(c != NULL)
        {
		comp=strcmp(c->pvname,newnode->pvname);
		cn=c->crc;
		if(comp < 0 && crc < cn && c->ll == NULL) // Add to the left left
		{
                       	c->ll=newnode;
			newnode->parent=c;
			newnode->clean='Y';

			if(link == 1)
			{
				end->next=newnode;
				newnode->prev=end;
				end=newnode;
			}
                       	break;
               	}
		else if(comp < 0 && crc >= cn && c->l == NULL) // Add to the left
		{
                       	c->l=newnode;
			newnode->parent=c;
			newnode->clean='Y';

			if(link == 1)
			{
				end->next=newnode;
				newnode->prev=end;
				end=newnode;
			}
                       	break;
               	}
		else if(comp > 0 && crc <= cn && c->r == NULL) // Add to the right
                {
                        c->r=newnode;
			newnode->parent=c;
			newnode->clean='Y';

			if(link == 1)
			{
				newnode->prev=end;
				end->next=newnode;
				end=newnode;
			}
                        break;
                }
		else if(comp > 0 && crc > cn && c->rr == NULL) // Add to the right right
                {
                        c->rr=newnode;
			newnode->parent=c;
			newnode->clean='Y';

			if(link == 1)
			{
				newnode->prev=end;
				end->next=newnode;
				end=newnode;
			}
                        break;
                }
		if(comp < 0 && crc < cn)
                	c=(node *)c->ll;
		else if(comp < 0 && crc >= cn)
			c=(node *)c->l;
		else if(comp == 0)
			break;
		else if(comp > 0 && crc <= cn)
			c=(node *)c->r;
		else
                	c=(node *)c->rr;
        }
}

node * find_node(char *str)
{
        node *c;
	int comp;
	int level;
	unsigned int crc,cn;

        c=root;
	crc=crc_32(str);
	level=0;
        while(c != NULL)
        {
		comp=strcmp(c->pvname,str);
                if(comp == 0)
		{
			if(level > maxlevel)
				maxlevel=level;
			c->lastaccess=time(NULL);
                        return c;
		}
		cn=c->crc;
		if(comp < 0 && crc < cn)
			c=(node *)c->ll;
		else if(comp < 0 && crc >= cn)
			c=(node *)c->l;
		else if(comp > 0 && crc <= cn)
			c=(node *)c->r;
		else
                	c=(node *)c->rr;
		level++;
        }
        return NULL;
}

node * full_search(char *str)
{
	node *n;

	n=begin;
	while(n != NULL)
	{
		if(strcmp(n->pvname,str) == 0)
			return n;
		n=(node *)n->next;
	}
	return NULL;
}

void retree(node *n)
{
	node *p;

	if(n == root)
		root=NULL;

	if(n->ll != NULL)
		retree((node *)n->ll);
	if(n->l != NULL)
		retree((node *)n->l);
	if(n->r != NULL)
		retree((node *)n->r);
	if(n->rr != NULL)
		retree((node *)n->rr);

	p=(node *)n->parent;
	if(n->parent != NULL)
	{
		if((node *)p->ll == n)
			p->ll=NULL;
		else if((node *)p->l == n)
			p->l=NULL;
		else if((node *)p->r == n)
			p->r=NULL;
		else if((node *)p->rr == n)
			p->rr=NULL;
		n->parent=NULL;
	}
	add_node(n,0);
}

void del_node(node *todel,int relink)
{
	node *t;

	/****** Remove from the tree */
	if(todel->parent != NULL)
	{
		t=(node *)todel->parent;
		if((node *)(t->ll) == todel)
			t->ll=NULL;
		else if((node *)(t->l) == todel)
			t->l=NULL;
		else if((node *)(t->r) == todel)
			t->r=NULL;
		else if((node *)(t->rr) == todel)
			t->rr=NULL;
	}
	else
		root=NULL;

	/***** Relink the chain *******/
	if(todel->prev != NULL)
	{
		t=(node *)todel->prev;
		t->next=todel->next;
	}
		
	if(todel->next != NULL)
	{
		t=(node *)todel->next;
		t->prev=todel->prev;
	}

	if(begin == todel)
		begin=todel->next;
	if(end == todel)
		end=todel->prev;
	if(root == todel)
		root=NULL;

	/****** Update the tree *******/
	if(relink == 1)
	{
		if(todel->ll != NULL)
			retree((node *)todel->ll);
		if(todel->l != NULL)
			retree((node *)todel->l);
		if(todel->r != NULL)
			retree((node *)todel->r);
		if(todel->rr != NULL)
			retree((node *)todel->rr);
	}
	else
	{
		if(todel->ll != NULL)
		{
			t=(node *)todel->ll;
			t->parent=NULL;
			t->clean='N';
			todel->ll=NULL;	
		}
		if(todel->l != NULL)
		{
			t=(node *)todel->l;
			t->parent=NULL;
			t->clean='N';
			todel->l=NULL;	
		}
		if(todel->r != NULL)
		{
			t=(node *)todel->r;
			t->parent=NULL;
			t->clean='N';
			todel->r=NULL;	
		}
		if(todel->rr != NULL)
		{
			t=(node *)todel->rr;
			t->parent=NULL;
			t->clean='N';
			todel->rr=NULL;	
		}
	}

	free(todel->pvname);
	ca_clear_channel(*(todel->pvchid));
	free(todel);
}

void remove_old(int oldtime)
{
	node *n,*d;

	n=begin;

	while(n != NULL)
	{
		d=(node *)n->next;
		if(n->lastaccess < oldtime)
			del_node(n,0);
		n=d;
	}

	n=begin;
	while(n != NULL)
	{
		if(n->clean == 'N')
			retree(n);
		n=(node *)n->next;
	}
}

int count_nodes()
{
	node *n;
	int tot=0;

	n=begin;
	while(n != NULL)
	{
		tot++;
		n=(node *)n->next;
	}

	return tot;
}

node * new_node(char *pvName)
{
	node *newnode;
	newnode=(node *)malloc(sizeof(node));
	newnode->pvname=(char *)malloc((strlen(pvName)+1)*sizeof(char));
	strcpy(newnode->pvname,pvName);
	newnode->parent=NULL;
	newnode->l=NULL;
	newnode->ll=NULL;
	newnode->r=NULL;
	newnode->rr=NULL;
	newnode->prev=NULL;
	newnode->next=NULL;
	newnode->lastaccess=time(NULL);
	newnode->crc=crc_32(pvName);
	newnode->clean='N';

	return newnode;
}

