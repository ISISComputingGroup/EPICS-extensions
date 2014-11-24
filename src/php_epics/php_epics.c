/*
$Author: bertrand $
$Date: 2005/03/17 09:04:29 $
$Source: /cvs/CVSROOT/G/PHP_EPICS/php_epics.c,v $
$Name:  $
$Revision: 1.10 $
$Log: php_epics.c,v $
Revision 1.10  2005/03/17 09:04:29  bertrand
Fixed a couple of bugs...

Revision 1.9  2005/03/07 14:16:58  bertrand
Modified monitor in order to return the correct datatype... not all in strings.
Removed a bit of fields of ca_info as it doesn't work via the Gateway

Revision 1.8  2004/11/08 14:00:20  bertrand
Added get array support, and returning ZEND type for caget instead of strings.

Revision 1.7  2004/10/13 12:18:24  krempaska
*** empty log message ***

Revision 1.6  2004/02/23 16:01:01  krempaska
*** empty log message ***

Revision 1.5  2004/02/23 12:36:35  bertrand
New PHP_EPICS module, which integrare the quad tree and a new function ca_state which return the current state of a channel.

Revision 1.4  2004/02/18 09:50:16  krempaska
added ca_type function

Revision 1.3  2004/02/16 17:00:33  krempaska
added switch for channel field types

Revision 1.2  2004/02/12 18:40:13  krempaska
*** empty log message ***

*/
/*
 * File:        php_epics.c
 * Description:	Implementatio of dynamic loadable php module. It allows a php 
 *		Web page to read, set or monitor EPICS channels.
 * Authors:	Alain Bertrand, Renata Krempaska
*/

/* include standard headers */
#include "main/php_config.h"
#include "main/php.h"
/* include EPICS header */
#include <cadef.h>
#include <tsDefs.h>
#include "tree.h"

/* declaration of functions to be exported, i.e. to be called from a php script */
ZEND_FUNCTION(ca_get);
ZEND_FUNCTION(ca_put);
ZEND_FUNCTION(ca_monitor);
ZEND_FUNCTION(ca_type);
ZEND_FUNCTION(ca_state);
ZEND_FUNCTION(ca_info);

int php_ca_err=0;
char php_ca_msg[255];

/* a utility function to parse a string.
 * We are using it for parsing a string containing channel names separated 
 * by comma (,).
*/
int get_part(char *str,char sep,int pos,char *res,int max)
{
        int i,j;
        int last;
        char fl;

        last=0;
        fl=0;
        res[last]='\0';

        for(i=0,j=0;i <= strlen(str);i++)
        {
                if(str[i] == '\"')
                {
                        fl=1-fl;
                        continue;
                }
                if(str[i] == sep && fl==0)
                {
                        j++;
                        if(j > pos)
                                return last;
                        continue;
                }
                if(j == pos)
                {
                        res[last]=str[i];
                        last++;
                        res[last]='\0';
                        if(last > max-2)
                                break;
                }
        }

        return last;
}


/*
 * Initialize an EPICS channel specified by its name and returns a pointer to the
 * EPICS chid.  
*/
chid * initChannel(char* pvName)
{
	int status;
	chid *newchid;

	static int isinit=-1;

	if(isinit == -1)
	{
		status = ca_task_initialize();
		if(status != ECA_NORMAL)
		{
			php_ca_err=1;
			strcpy(php_ca_msg,"Cannot inizialize Channel access");
    			return NULL;
		}
		isinit=1;
	}

	newchid=(chid *)malloc(sizeof(chid));
	if(newchid == NULL)
	{
		php_ca_err=1;
		strcpy(php_ca_msg,"Cannot allocate memory");
		return NULL;
	}

	// convert name to chan id
	status = ca_search(pvName, newchid);
	if(status != ECA_NORMAL)
	{
		php_ca_err=1;
		strcpy(php_ca_msg,"Cannot find the channel");
		free(newchid);
		return NULL;
	}
	status = ca_pend_io(.50);
	if(status != ECA_NORMAL)
	{
		php_ca_err=1;
		strcpy(php_ca_msg,"CA_PEND_IO error");
		ca_clear_channel(*newchid);
		free(newchid);
		return NULL;
	}
  //success
	return newchid;
}

/*
 * Returns a pointer the EPICS chid.
*/
chid * findChannel(char *pvName)
{
        node *c;
        chid *pvchid;

        c=find_node(pvName);
        if(c == NULL)
        {
		c=new_node(pvName);
                pvchid=initChannel(pvName);
                c->pvchid=pvchid;
                add_node(c,1);
                return pvchid;
        }
	if(c->pvchid == NULL)
	{
		pvchid=initChannel(pvName);
		c->pvchid=pvchid;
	}
        return c->pvchid;
}

/*
 * EPICS channel access get implementation. It is called by php ca_get 
 * exported function.
 * The result is returned as string (char *res).
*/
/*double pv_get(char *pvName,char *res)*/
zval * pv_get(char *pvName)
{
	chid *pvchid;
	float fData;
	double dData;
	int iData;
	char sData[100];
	double *adData;
	zval *res,*newval;
	int nelem,i;

	MAKE_STD_ZVAL(res);

	pvchid=findChannel(pvName);

	if(pvchid == NULL) {
		//php_ca_err=1;
		//strcpy(php_ca_msg,"CA_GET Failure");
		strcpy(sData,"ERROR: CA_GET Failure");
		ZVAL_STRING(res,sData,1);
		return res;
	}

	if(ca_field_type(*pvchid) == TYPENOTCONN)
	{
		if(ca_get(DBR_STRING,*pvchid,(void *)res) != ECA_NORMAL)
		{
			php_ca_err=1;
			ca_pend_io(.5);
			strcpy(php_ca_msg,"CA_GET Failure...");

			strcpy(sData,"ERROR: CA_GET Failure");
			ZVAL_STRING(res,sData,1);
			return res;
		}
		if(ca_pend_io(.5) != ECA_NORMAL)
		{
			php_ca_err=1;
			strcpy(php_ca_msg,"CA_PEND_IO Failure");

			strcpy(sData,"ERROR: CA_PEND_IO Failure");
			ZVAL_STRING(res,sData,1);
			return res;
		}
	}

	nelem = ca_element_count(*pvchid);

	/*sprintf(sData,"%d %s",nelem, dbf_type_to_text(ca_field_type(*pvchid)));
	ZVAL_STRING(res,sData,1);
	return res;*/
	
	if(nelem > 1)
	{	
		//type = ca_field_type(*pvchid);
		adData=(double *)malloc(sizeof(double)*nelem);
		if(ca_array_get(DBF_DOUBLE,nelem,*pvchid,(void *)adData) != ECA_NORMAL)
		{
			free(adData);
			php_ca_err=1;
			strcpy(php_ca_msg,"CA_PEND_IO Failure");

			strcpy(sData,"ERROR: CA_ARRAY_GET Failure");
			ZVAL_STRING(res,sData,1);
			return res;
		}                
		if(ca_pend_io(5.) != ECA_NORMAL)
		{
			free(adData);
			php_ca_err=1;
			strcpy(php_ca_msg,"CA_PEND_IO Failure");

			strcpy(sData,"ERROR: CA_PEND_IO Failure");
			ZVAL_STRING(res,sData,1);
			return res;
		}                
		array_init(res);
		for(i=0;i < nelem;i++)
		{
			MAKE_STD_ZVAL(newval);
			ZVAL_DOUBLE(newval, adData[i]);
			//ZVAL_DOUBLE(newval,i);
			zend_hash_index_update(res->value.ht, i, (void *)&newval, sizeof(zval *), NULL);
		}
		free(adData);
		/*strcpy(sData,"TEST");
		ZVAL_STRING(res,sData,1);*/
	}
	else switch (ca_field_type(*pvchid))  //data depending on field type
	{
	case DBR_FLOAT:
		if(ca_get(DBR_FLOAT,*pvchid,(void *)&fData) != ECA_NORMAL) {
			php_ca_err=1;
			strcpy(php_ca_msg,"CA_GET Failure");

			strcpy(sData,"ERROR: CA_GET Failure");
			ZVAL_STRING(res,sData,1);
			return res;
		}
		if(ca_pend_io(.5) != ECA_NORMAL)
		{
			php_ca_err=1;
			strcpy(php_ca_msg,"CA_PEND_IO Failure");

			strcpy(sData,"ERROR: CA_PEND_IO Failure");
			ZVAL_STRING(res,sData,1);
			return res;
		}                
                //sprintf(res, "%f",fData);
		ZVAL_DOUBLE(res,dData);
        	break;
	case DBR_DOUBLE:
  		if(ca_get(DBR_DOUBLE,*pvchid,(void *)&dData) != ECA_NORMAL) {
			php_ca_err=1;
			strcpy(php_ca_msg,"CA_GET Failure");

			strcpy(sData,"ERROR: CA_GET Failure");
			ZVAL_STRING(res,sData,1);
			return res;
		}
		if(ca_pend_io(.5) != ECA_NORMAL) {
			php_ca_err=1;
			strcpy(php_ca_msg,"CA_PEND_IO Failure");

			strcpy(sData,"ERROR: CA_PEND_IO Failure");
			ZVAL_STRING(res,sData,1);
			return res;
		}
                //sprintf(res, "%f",dData);		
		ZVAL_DOUBLE(res,dData);
        	break;
	case DBR_CHAR:
	case DBR_STRING:
	case DBR_ENUM:
	   	if(ca_get(DBR_STRING,*pvchid,(void *)sData) != ECA_NORMAL) {
			php_ca_err=1;
			strcpy(php_ca_msg,"CA_GET Failure");

			strcpy(sData,"ERROR: CA_GET Failure");
			ZVAL_STRING(res,sData,1);
			return res;
		}
		if(ca_pend_io(.5) != ECA_NORMAL) {
			php_ca_err=1;
			strcpy(php_ca_msg,"CA_PEND_IO Failure");

			strcpy(sData,"ERROR: CA_PEND_IO Failure");
			ZVAL_STRING(res,sData,1);
			return res;
		}
		ZVAL_STRING(res,sData,1);
        	break;
	case DBR_SHORT:
	case DBR_LONG:
        	if(ca_get(DBR_LONG,*pvchid,(void *)&iData) != ECA_NORMAL) {
			php_ca_err=1;
			strcpy(php_ca_msg,"CA_GET Failure");

			strcpy(sData,"ERROR: CA_GET Failure");
			ZVAL_STRING(res,sData,1);
			return res;
		}
		if(ca_pend_io(.5) != ECA_NORMAL) {
			php_ca_err=1;
			strcpy(php_ca_msg,"CA_PEND_IO Failure");

			strcpy(sData,"ERROR: CA_PEND_IO Failure");
			ZVAL_STRING(res,sData,1);
			return res;
		}
                //sprintf(res, "%d",iData);
		ZVAL_LONG(res,iData);
        	break;
	case TYPENOTCONN:
		php_ca_err=1;
		strcpy(php_ca_msg,"Not connected");

		strcpy(sData,"ERROR: Not connected");
		ZVAL_STRING(res,sData,1);
		break;
      	default:
	      	php_ca_err=1;
		strcpy(php_ca_msg,"Unknown type");

		strcpy(sData,"ERROR: Unknown type");
		ZVAL_STRING(res,sData,1);
		return res;
  	} //switch
	return res;
}

/*
 * EPICS channel access put implementation. It is called by php ca_put 
 * exported function.
 * The parameter is passed as string.
*/
void pv_put(char *pvName,char *data)
{
	chid *pvchid;

	pvchid=findChannel(pvName);

	if(pvchid == NULL)
		return;

	SEVCHK(ca_put(DBR_STRING,*pvchid,(void *)data),"ca_put failure");
	SEVCHK(ca_pend_io(.50),"ca_pend_io failure");
}

/*
 * It is called by php ca_info 
 * exported function.
 * The result is returned as string (char *res).
*/
zval * pv_info(char *pvName)
{
	chid *pvchid;
	float fData;
	double dData;
	int iData;
	char recTyp[100];
	//dbr_class_name_t recTyp;
	struct dbr_time_string timeVal;
	char res1[100];	
	char tmp[255];
	char *keys[]={"RTYP","TYPE","VALUE","STAMP","IOC"};
	zval *new_array,*new_element,*res;
	char tsTxt[32];
	
	MAKE_STD_ZVAL(new_array);
	array_init(new_array);

	pvchid=findChannel(pvName);

	if(pvchid == NULL) {
		php_ca_err=1;
		strcpy(php_ca_msg,"CA_GET Failure");
		MAKE_STD_ZVAL(res);
		ZVAL_STRING(res,php_ca_msg,1);
		return res;
	}

	//printf("pv_get: TYPE: %s\n", dbf_type_to_text(ca_field_type(*pvchid)));
	
	if(ca_field_type(*pvchid) == TYPENOTCONN)
	{
		if(ca_get(DBR_STRING,*pvchid,(void *)res1) != ECA_NORMAL)
		{
			php_ca_err=1;
			strcpy(php_ca_msg,"CA_GET Failure...");
			//ca_pend_io(.5);
			MAKE_STD_ZVAL(res);
			ZVAL_STRING(res,php_ca_msg,1);
			return res;
		}
		if(ca_pend_io(.5) != ECA_NORMAL)
		{
			php_ca_err=1;
			strcpy(php_ca_msg,"CA_PEND_IO Failure");
			MAKE_STD_ZVAL(res);
			ZVAL_STRING(res,php_ca_msg,1);
			return res;
		}
	}

	//get record type, like AI, AO, etc
	/*if (ca_get(DBR_CLASS_NAME,*pvchid,(void *)recTyp) != ECA_NORMAL) {
		php_ca_err=1;
		strcpy(php_ca_msg,"CA_GET Failure...");
		//ca_pend_io(.5);
		MAKE_STD_ZVAL(res);
		ZVAL_STRING(res,php_ca_msg,1);
		return res;
	}*/
	
	//get TIME STAMP
	/*if (ca_get(DBR_TIME_STRING,*pvchid,(void *)&timeVal) != ECA_NORMAL) {
		php_ca_err=1;
		strcpy(php_ca_msg,"CA_GET Failure...");
		//ca_pend_io(.5);
		MAKE_STD_ZVAL(res);
		ZVAL_STRING(res,php_ca_msg,1);
		return res;
	}*/
	/*switch (ca_field_type(*pvchid))  //data depending on field type
	{
	case DBR_FLOAT:
		if(ca_get(DBR_FLOAT,*pvchid,(void *)&fData) != ECA_NORMAL) {
			php_ca_err=1;
			strcpy(php_ca_msg,"CA_GET Failure");
			MAKE_STD_ZVAL(res);
			ZVAL_STRING(res,php_ca_msg,1);
			return res;
		}
		if(ca_pend_io(.5) != ECA_NORMAL)
		{
			php_ca_err=1;
			strcpy(php_ca_msg,"CA_PEND_IO Failure (float)");
			MAKE_STD_ZVAL(res);
			ZVAL_STRING(res,php_ca_msg,1);
			return res;
		}                
                sprintf(res1, "%f",fData);
        	break;
	case DBR_DOUBLE:
  		if(ca_get(DBR_DOUBLE,*pvchid,(void *)&dData) != ECA_NORMAL) {
			php_ca_err=1;
			strcpy(php_ca_msg,"CA_GET Failure");
			MAKE_STD_ZVAL(res);
			ZVAL_STRING(res,php_ca_msg,1);
			return res;
		}
		if(ca_pend_io(.5) != ECA_NORMAL) {
			php_ca_err=1;
			strcpy(php_ca_msg,"CA_PEND_IO Failure (double)");
			MAKE_STD_ZVAL(res);
			ZVAL_STRING(res,php_ca_msg,1);
			return res;
		}
                sprintf(res1, "%f",dData);		
        	break;
	case DBR_CHAR:
	case DBR_STRING:
	case DBR_ENUM:
	   	if(ca_get(DBR_STRING,*pvchid,(void *)res1) != ECA_NORMAL) {
			php_ca_err=1;
			strcpy(php_ca_msg,"CA_GET Failure");
			MAKE_STD_ZVAL(res);
			ZVAL_STRING(res,php_ca_msg,1);
			return res;
		}
		if(ca_pend_io(.5) != ECA_NORMAL) {
			php_ca_err=1;
			strcpy(php_ca_msg,"CA_PEND_IO Failure (string)");
			MAKE_STD_ZVAL(res);
			ZVAL_STRING(res,php_ca_msg,1);
			return res;
		}
        	break;
	case DBR_SHORT:
	case DBR_LONG:
        	if(ca_get(DBR_LONG,*pvchid,(void *)&iData) != ECA_NORMAL) {
			php_ca_err=1;
			strcpy(php_ca_msg,"CA_GET Failure");
			MAKE_STD_ZVAL(res);
			ZVAL_STRING(res,php_ca_msg,1);
			return res;
		}
		if(ca_pend_io(.5) != ECA_NORMAL) {
			php_ca_err=1;
			strcpy(php_ca_msg,"CA_PEND_IO Failure (long)");
			MAKE_STD_ZVAL(res);
			ZVAL_STRING(res,php_ca_msg,1);
			return res;
		}
                sprintf(res1, "%d",iData);
        	break;
	case TYPENOTCONN:
		php_ca_err=1;
		strcpy(php_ca_msg,"Not connected");
		MAKE_STD_ZVAL(res);
		ZVAL_STRING(res,php_ca_msg,1);
		return res;
		break;
      	default:
	      	php_ca_err=1;
		strcpy(php_ca_msg,"Unknown type");
		MAKE_STD_ZVAL(res);
		ZVAL_STRING(res,php_ca_msg,1);
		return res;
		return new_array;
  	} //switch*/
	
	//concatenate all results
	/*sprintf(res, "%s\n%s\n%s\n%s\n%s\n",
		recTyp, dbf_type_to_text(ca_field_type(*pvchid)),
		res1,timeVal,ca_host_name(*pvchid));*/
		
	/*MAKE_STD_ZVAL(new_element);
	ZVAL_STRING(new_element,recTyp , 1);
	zend_hash_update(new_array->value.ht, keys[0], strlen(keys[0]) + 1, (void *)&new_element, sizeof(zval *), NULL);*/
	
	strcpy(tmp,dbf_type_to_text(ca_field_type(*pvchid)));	
	MAKE_STD_ZVAL(new_element);
	ZVAL_STRING(new_element,tmp , 1);
	zend_hash_update(new_array->value.ht, keys[1], strlen(keys[1]) + 1, (void *)&new_element, sizeof(zval *), NULL);
 
	/*MAKE_STD_ZVAL(new_element);
	ZVAL_STRING(new_element,res1 , 1);
	zend_hash_update(new_array->value.ht, keys[2], strlen(keys[2]) + 1, (void *)&new_element, sizeof(zval *), NULL);*/
	
	/*strcpy(tmp,tsStampToText(&timeVal.stamp, TS_TEXT_MONDDYYYY, tsTxt));	
	MAKE_STD_ZVAL(new_element);
	ZVAL_STRING(new_element,tmp , 1);
	zend_hash_update(new_array->value.ht, keys[3], strlen(keys[3]) + 1, (void *)&new_element, sizeof(zval *), NULL);*/

	strcpy(tmp,ca_host_name(*pvchid));		
	MAKE_STD_ZVAL(new_element);
	ZVAL_STRING(new_element,tmp , 1);
	zend_hash_update(new_array->value.ht, keys[4], strlen(keys[4]) + 1, (void *)&new_element, sizeof(zval *), NULL);
		
	return new_array;	
}



//------------------------------------------------------------------------------

/* compiled function list so Zend knows what's in this module */
zend_function_entry epicsmod_functions[] =
{
    ZEND_FE(ca_get, NULL)
    ZEND_FE(ca_put, NULL)
    ZEND_FE(ca_monitor, NULL)
    ZEND_FE(ca_type, NULL)
    ZEND_FE(ca_state, NULL)
    ZEND_FE(ca_info, NULL)
    {NULL, NULL, NULL}
};

/* compiled module information */
zend_module_entry epicsmod_module_entry =
{
    STANDARD_MODULE_HEADER,
    "Epics Module",
    epicsmod_functions,
    NULL, 
    NULL, 
    NULL, 
    NULL, 
    NULL,
    NO_VERSION_YET,
    STANDARD_MODULE_PROPERTIES
};

/* it is called at load time of a module when a "dl()" is called from a php script*/
ZEND_GET_MODULE(epicsmod)

/* implementation of exported functions, ie.e to be made available to PHP */

/*
 * Returns an EPICS channel value if the channel, is connected, otherwise error.
 * Parameters:
 *		 char *name - channel name 
 * It is called from a php script by using a statement:
 *		$ret = ca_get("channel_name");
*/
ZEND_FUNCTION(ca_get)
{
	char *name;
	int size;

	//char res[255];
	zval *res;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name,&size) == FAILURE)
        	return;

	php_ca_err=0;
	res=pv_get(name);

	//RETURN_STRING(res,1);
	*return_value=*res;
}

/*
 * If the channel is connected, it sets the channel value.
 * Parameters:
 *		char *name - channel name
 		char *value - a value to be set
 * It is called from a php script by using a statement:
 *		ca_put("channel_name","value");
*/
ZEND_FUNCTION(ca_put)
{
	char *name;
	int s1,s2;
	char *value;

	char res[255];

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &name,&s1,&value,&s2) == FAILURE)
        	return;

	php_ca_err=0;
	pv_put(name,value);
	if(php_ca_err == 0)
		strcpy(php_ca_msg,"Ok");
	RETURN_STRING(php_ca_msg,1);
}
/*
 * Returns an EPICS info (record type, data type, value, time stamp, BootPC) 
 * about channel if the channel, is connected, otherwise error.
 * Parameters:
 *		 char *name - channel name 
 * It is called from a php script by using a statement:
 *		$ret = ca_info("channel_name");
*/
ZEND_FUNCTION(ca_info)
{
	char *name;
	int size;
	double data;

	/*char res[255];*/

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name,&size) == FAILURE)
        	return;

	php_ca_err=0;
	*return_value=*pv_info(name);
	/*if(php_ca_err != 0)
		strcpy(res,php_ca_msg);*/
	/*RETURN_STRING(res,1);*/
}

//------------------ Monitor Implementation ---------------------------------
/*
 * This function is called by php script every time when a monitored EPICS channel 
 * value is changed. See usage from php script bellow.
 * Parameters:
 *		args - a structure containing a user defined arguments
 		name - channel name
		timeout - time in seconds
 */
 void monitor_callback(struct event_handler_args args)
{
	zval *function,*a,*b;
 	zval **params[2];
	zval *return_value;
	char sVal[100];
	float *fData;
	double *dData,dval;
	
	int *iData;

	function=(zval *)args.usr;

	MAKE_STD_ZVAL(a);
	MAKE_STD_ZVAL(b);

	ZVAL_STRING(a,(char *)ca_name(args.chid),1);
//	ZVAL_STRING(b,(char *)args.dbr,1);


 	switch (args.type)         //convert the Data depending on type
  	{
      	case DBR_FLOAT:
                fData = (float *) args.dbr;
		dval=*fData;
		ZVAL_DOUBLE(b,dval);
                //sprintf(sVal, "%f",*fData);
        	break;
      	case DBR_DOUBLE:
                dData = (double *) args.dbr;
		ZVAL_DOUBLE(b,*dData);
                //sprintf(sVal, "%f",*dData);
		break;
 	case DBR_STRING:
      	case DBR_ENUM:
	case DBR_CHAR:
                sprintf(sVal, "%s", (char *) args.dbr);
		ZVAL_STRING(b,(char *)sVal,1);
	        break;
      	case DBR_SHORT:
      	case DBR_LONG:
                iData = (int *) args.dbr;
                //sprintf(sVal, "%d",*iData);
		ZVAL_LONG(b,*iData);
        	break;
      	default:
        	sprintf(sVal, "%s","Unknown Type\n");	
	} //switch
	
	//ZVAL_STRING(b,(char *)sVal,1);

	params[0]=&a;
	params[1]=&b;

	/* a Zend function used to implement callbacks */
	if(call_user_function_ex(CG(function_table), NULL, function, &return_value, 2, params, 0, NULL TSRMLS_CC) != FAILURE)
		return;
}

/*
 * It monitors the channel(s).
 * Parameters:
 *		char *channels - channel names string separated by comma (,)
 *		double timeout - time in seconds, which specifies how long 
 				a user wants to monitor the channels 
 		char *function - a function name
 * It is called from a php script by using a statement:
 *		$res=ca_monitor("channel1,channel2",40,"test");
*/
ZEND_FUNCTION(ca_monitor)
{
	zval *function;
	char *channels;
	int s1;
 	zval **params[2];
	chid *pvchid;
	double timeout;
	int status;
	char channel[255];
	char res[255];
	int i;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sdz",&channels,&s1,&timeout,&function) == FAILURE)
        	return;

	for(i=0;i < 1000;i++)
	{
		get_part(channels,',',i,channel,254);
		if(channel[0] == '\0')
			break;
		pvchid=findChannel(channel);
		if(pvchid == NULL)
			continue;
			//RETURN_STRING("No channel",1);

		//get the chid DBR type


		if(ca_field_type(*pvchid) == TYPENOTCONN)
		{
			if(ca_get(DBR_STRING,*pvchid,(void *)res) != ECA_NORMAL)
			{
				php_ca_err=1;
				ca_pend_io(.5);
				//RETURN_STRING("CA_GET Failure...",1);
				//return;
				continue;
			}
			if(ca_pend_io(.5) != ECA_NORMAL)
			{
				php_ca_err=1;
				//RETURN_STRING("CA_PEND_IO Failure",1);
				//return;
				continue;
			}
		}



		switch (ca_field_type(*pvchid))  //data depend on the field type
		{
		case DBR_FLOAT:
		   status=ca_add_event(DBR_FLOAT,*pvchid,(void (*)())monitor_callback,(void *)function,NULL);
		   if(status != ECA_NORMAL)
			   RETURN_STRING("CA_ADD_EVENT Failure",1);
		   break;
		case DBR_DOUBLE:
		   status=ca_add_event(DBR_DOUBLE,*pvchid,(void (*)())monitor_callback,(void *)function,NULL);
		   //printf("DOUBLE");
		   if(status != ECA_NORMAL)
			   RETURN_STRING("CA_ADD_EVENT Failure",1);	
		   break;
		case DBR_STRING:
		case DBR_ENUM:
		case DBR_CHAR:
		   status=ca_add_event(DBR_STRING,*pvchid,(void (*)())monitor_callback,(void *)function,NULL);
		   if(status != ECA_NORMAL)
			   RETURN_STRING("CA_ADD_EVENT Failure",1);
		   break;
		case DBR_SHORT:
		case DBR_LONG:
		   status=ca_add_event(DBR_LONG,*pvchid,(void (*)())monitor_callback,(void *)function,NULL);
		   if(status != ECA_NORMAL)
			   RETURN_STRING("CA_ADD_EVENT Failure",1);
		   break;
		default:
		   RETURN_STRING("Unknown type",1);
		} //switch

		/*status=ca_add_event(DBR_STRING,*pvchid,(void (*)())monitor_callback,(void *)function,NULL);
		if(status != ECA_NORMAL)
			RETURN_STRING("Error",1);*/
	} //for
	//flushes the buffer and always waits "timeout" seconds 0 means forever
	ca_pend_event(timeout);
	RETURN_STRING("OK",1);
}

ZEND_FUNCTION(ca_type)
{
	char *channel;
	int s1;
	chid *pvchid;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",&channel,&s1) == FAILURE)
		return;
	pvchid=findChannel(channel);
	if(pvchid == NULL)
		RETURN_STRING("No channel",1);	
	switch(ca_field_type(*pvchid))
	{
	case DBR_FLOAT:
		RETURN_STRING("Float",1);
		break;
	case DBR_DOUBLE:
		RETURN_STRING("Double",1);
		break;
	case DBR_STRING:
		RETURN_STRING("String",1);
		break;
	case DBR_ENUM:
		RETURN_STRING("Enum",1);
		break;
	case DBR_CHAR:
		RETURN_STRING("Char",1);
		break;
	case DBR_SHORT:
		RETURN_STRING("Short",1);
		break;
	case DBR_LONG:
		RETURN_STRING("Long",1);
		break;
	default:
		RETURN_STRING("Unknown type",1);
	} //switch
}

ZEND_FUNCTION(ca_state)
{
	char *channel;
	int s1;
	chid *pvchid;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",&channel,&s1) == FAILURE)
		return;
	pvchid=findChannel(channel);
	if(pvchid == NULL)
		RETURN_STRING("No channel",1);	
	switch(ca_state(*pvchid))
	{
	case cs_never_conn: 
		RETURN_STRING("valid chid, server not found or unavailable",1);
        case cs_prev_conn: 
		RETURN_STRING("valid chid, previously connected to server",1);
        case cs_conn:     
		RETURN_STRING("valid chid, connected to server",1);
        case cs_closed:  
		RETURN_STRING("channel deleted by user",1);
	default:
		RETURN_STRING("Unkown state",1);
	}
}
