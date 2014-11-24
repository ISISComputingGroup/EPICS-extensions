
/* Tree structure to store and manage channel access ID */
typedef struct s_node
{       
	char *pvname;
	chid *pvchid; 
	long lastaccess;
	unsigned int crc;
	/****** Tree structure *********/
	void *parent;
	void *ll,*l,*r,*rr;
	/****** 2-way linked chain *****/
	void *prev;
	void *next;
	/***** char is clean ? ********/
	char clean;
} node; 


void remove_old(int oldtime);
int count_nodes();
node * new_node(char *pvName);
node * find_node(char *str);
void add_node(node *newnode,int link);
