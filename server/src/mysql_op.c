#include "../include/factory.h"

void generateStr(char *str)
{
    int i,flag;    
    srand(time(NULL));//通过时间函数设置随机数种子，使得每次运行结果随机。
    str[0]='$';
    str[1]='6';
    str[2]='$';
    str[11]='$';
    for(i = 3; i < 10; i ++)
    {
		flag = rand()%3;
		switch(flag)
		{
		case 0:
			str[i] = rand()%26 + 'a'; 
			break;
		case 1:
			str[i] = rand()%26 + 'A'; 
			break;
		case 2:
			str[i] = rand()%10 + '0'; 
			break;
		}
    }
}

//注册时调用insert
int insertUserInfo(char *username, char *userPassword, char *salt)
{
    MYSQL *conn;
	char* server="localhost";
	char* user="root";
	char* password="wjz456";
	char* database="netdisk";
	char query[1024]="insert into user_info(username, salt, password) values ";
    int t;
	conn=mysql_init(NULL);
	if(!mysql_real_connect(conn,server,user,password,database,0,NULL,0))
	{
		printf("Error connecting to database:%s\n",mysql_error(conn));
        mysql_close(conn);
        return -1;
	}else{
	}
    sprintf(query,"%s ('%s','%s','%s')", query, username, salt, userPassword);
    printf("%s\n", query);
    t=mysql_query(conn,query);
    if(t)
	{
		printf("Error making query:%s\n",mysql_error(conn));
        mysql_close(conn);
        return -1;
	}else{
		printf("insert success\n");
	}
	mysql_close(conn);
	return 0;
}
//登陆验证用select
int selectUserInfo(char *column,char *username, char *result)
{
    MYSQL *conn;
	MYSQL_RES *res;
	MYSQL_ROW row;
	char* server="localhost";
	char* user="root";
	char* password="wjz456";
	char* database="netdisk";//要访问的数据库名称
	char query[300];
    sprintf(query, "select %s from user_info where username='%s'", column, username);
	puts(query);
	int t;
	conn=mysql_init(NULL);
	if(!mysql_real_connect(conn,server,user,password,database,0,NULL,0))
	{
		printf("Error connecting to database:%s\n",mysql_error(conn));
        mysql_close(conn);
		return -1;
	}else{
		
	}
	t=mysql_query(conn,query);
	if(t)
	{
		printf("Error making query:%s\n",mysql_error(conn));
        mysql_close(conn);
        return -1;
	}else{
	//	printf("Query made...\n");
		res=mysql_use_result(conn);
		if(res!=NULL)
		{
			row=mysql_fetch_row(res);
            if(row!=NULL)
			    strcpy(result, row[0]);
            else
            {
                printf("Don't find data\n");
                mysql_close(conn);
                return -1;
            }
            
		}else{
			printf("Don't find data\n");
            return -1;
		}
		mysql_free_result(res);
	}
	mysql_close(conn);
	return 0;
}
//修改用户信息（登录时修改isLogin）
int updateUserInfo(char *username, char* isLogin)
{
	MYSQL *conn;
	char* server="localhost";
	char* user="root";
	char* password="wjz456";
	char* database="netdisk";
	char query[200]="update user_info set isLogin=";
	if(username!=NULL)
	{
		sprintf(query, "%s%s where username='%s'", query, isLogin, username);
	}
	else
	{
		sprintf(query, "%s%s",query, isLogin);
	}
	
	int t;
	conn=mysql_init(NULL);
	if(!mysql_real_connect(conn,server,user,password,database,0,NULL,0))
	{
		printf("Error connecting to database:%s\n",mysql_error(conn));
		mysql_close(conn);
		return -1;
	}else{
		
	}
	t=mysql_query(conn,query);
	if(t)
	{
		printf("Error making query:%s\n",mysql_error(conn));
		mysql_close(conn);
		return -1;
	}else{
		printf("update success\n");
	}
	mysql_close(conn);
	return 0;
}

//文件操作的mysql,columns要查找的项目，condition查找条件, result查找结果,condition注意加引号
int selectFileInfo(char *columns, char *condition, char *result)
{
	MYSQL *conn;
	MYSQL_RES *res;
	MYSQL_ROW row;
	char* server="localhost";
	char* user="root";
	char* password="wjz456";
	char* database="netdisk";//要访问的数据库名称
	char query[300];
    sprintf(query, "select %s from file_info where %s", columns, condition);
	puts(query);
	int t;
	conn=mysql_init(NULL);
	if(!mysql_real_connect(conn,server,user,password,database,0,NULL,0))
	{
		printf("Error connecting to database:%s\n",mysql_error(conn));
        mysql_close(conn);
		return -1;
	}else{
		
	}
	t=mysql_query(conn,query);
	if(t)
	{
		printf("Error making query:%s\n",mysql_error(conn));
        mysql_close(conn);
        return -1;
	}else{
		res=mysql_use_result(conn);
		if(res!=NULL)
		{
			while((row=mysql_fetch_row(res))!=NULL)
			{
				for (t = 0; t < mysql_num_fields(res); t++)
				{
					sprintf(result, "%s %s", result, row[t]);
				}
			}
		}else{
			printf("Don't find data\n");
            return -1;
		}
		mysql_free_result(res);
	}
	mysql_close(conn);
	return 0;
}
//插入文件信息传入user 和fileInfo
int insertFileInfo(User_t userC, FileInfo_t fileInfo)
{
	MYSQL *conn;
	char* server="localhost";
	char* user="root";
	char* password="wjz456";
	char* database="netdisk";
	char query[1024]="insert into file_info(precode, filename, md5, owner, filesize, filetype, exist) values ";
	sprintf(query, "%s(%s,'%s','%s','%s',%ld,'%s',%d)", query, userC.usingDir, fileInfo.fileName, fileInfo.md5, userC.userName, fileInfo.fileSize, fileInfo.fileType, fileInfo.exist);
	int t;
	conn=mysql_init(NULL);
	if(!mysql_real_connect(conn,server,user,password,database,0,NULL,0))
	{
		printf("Error connecting to database:%s\n",mysql_error(conn));
        mysql_close(conn);
        return -1;
	}else{
		
	}
    printf("%s\n", query);
    t=mysql_query(conn,query);
    if(t)
	{
		printf("Error making query:%s\n",mysql_error(conn));
        mysql_close(conn);
        return -1;
	}else{
		printf("insert success\n");
	}
	mysql_close(conn);
	return 0;
}
//condition注意加引号
int deleteFileInfo(char *condition)
{
	MYSQL *conn;
	char* server="localhost";
	char* user="root";
	char* password="wjz456";
	char* database="netdisk";
	char query[200]="delete from file_info where ";
	sprintf(query, "%s%s", query, condition);
	int t;
	conn=mysql_init(NULL);
	if(!mysql_real_connect(conn,server,user,password,database,0,NULL,0))
	{
		printf("Error connecting to database:%s\n",mysql_error(conn));
		return -1;
	}else{
		
	}
	t=mysql_query(conn,query);
	if(t)
	{
		printf("Error making query:%s\n",mysql_error(conn));
		mysql_close(conn);
		return -1;
	}else{
		printf("delete success,delete row=%ld\n",(long)mysql_affected_rows(conn));
	}
	mysql_close(conn);
	return 0;
}
int updateFileInfo(char *column, char *condition, char *value)
{
	MYSQL *conn;
	char* server="localhost";
	char* user="root";
	char* password="wjz456";
	char* database="netdisk";
	char query[200]="update file_info set";
	sprintf(query, "%s %s=%s where %s", query, column, value, condition);
	int t;
	conn=mysql_init(NULL);
	if(!mysql_real_connect(conn,server,user,password,database,0,NULL,0))
	{
		printf("Error connecting to database:%s\n",mysql_error(conn));
		mysql_close(conn);
		return -1;
	}
	t=mysql_query(conn,query);
	if(t)
	{
		printf("Error making query:%s\n",mysql_error(conn));
		mysql_close(conn);
		return -1;
	}else{
		printf("update success\n");
	}
	mysql_close(conn);
	return 0;
}
//找token
int selectTokenInfo(char *username, char *token)
{
	MYSQL *conn;
	MYSQL_RES *res;
	MYSQL_ROW row;
	char* server="localhost";
	char* user="root";
	char* password="wjz456";
	char* database="netdisk";//要访问的数据库名称
	char query[300];
    sprintf(query, "select username from token_info where token='%s'", token);
	puts(query);
	int t;
	conn=mysql_init(NULL);
	if(!mysql_real_connect(conn,server,user,password,database,0,NULL,0))
	{
		printf("Error connecting to database:%s\n",mysql_error(conn));
        mysql_close(conn);
		return -1;
	}
	t=mysql_query(conn,query);
	if(t)
	{
		printf("Error making query:%s\n",mysql_error(conn));
        mysql_close(conn);
        return -1;
	}else{
		res=mysql_use_result(conn);
		if(res!=NULL)
		{
			row=mysql_fetch_row(res);
			puts(row[0]);
			if(NULL!=row)
				strcpy(username, row[0]);
		}else{
			printf("Don't find data\n");
            return -1;
		}
		mysql_free_result(res);
	}
	mysql_close(conn);
	return 0;
}
//插入token
int insertTokenInfo(char *username, char *token)
{
	MYSQL *conn;
	char* server="localhost";
	char* user="root";
	char* password="wjz456";
	char* database="netdisk";
	char query[1024]="insert into token_info(username, token) values ";
    int t;
	conn=mysql_init(NULL);
	if(!mysql_real_connect(conn,server,user,password,database,0,NULL,0))
	{
		printf("Error connecting to database:%s\n",mysql_error(conn));
        mysql_close(conn);
        return -1;
	}
    sprintf(query,"%s ('%s','%s')", query, username, token);
    printf("%s\n", query);
    t=mysql_query(conn,query);
    if(t)
	{
		printf("Error making query:%s\n",mysql_error(conn));
        mysql_close(conn);
        return -1;
	}else{
		printf("insert success\n");
	}
	mysql_close(conn);
	return 0;
}
//username是NULL时全部删除（程序退出时会用到）
int deleteTokenInfo(char *username)
{
	MYSQL *conn;
	char* server="localhost";
	char* user="root";
	char* password="wjz456";
	char* database="netdisk";
	char query[200]="delete from token_info";
	if(NULL!=username)
	{
		sprintf(query, "%s where username='%s'", query, username);
	}
	int t;
	conn=mysql_init(NULL);
	if(!mysql_real_connect(conn,server,user,password,database,0,NULL,0))
	{
		printf("Error connecting to database:%s\n",mysql_error(conn));
		return -1;
	}
	t=mysql_query(conn,query);
	if(t)
	{
		printf("Error making query:%s\n",mysql_error(conn));
		mysql_close(conn);
		return -1;
	}else{
		printf("delete success,delete row=%ld\n",(long)mysql_affected_rows(conn));
	}
	mysql_close(conn);
	return 0;
}