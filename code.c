#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include <windows.h>
#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>
#include <string.h>
#include <time.h>
#include <string.h>
#define LOGIN_TIMEOUT 30
#define MAXBUFLEN 255
RETCODE error;
HENV henv = NULL;
HDBC conn = NULL;
HSTMT hstmt = NULL;
void DBTest();
void check_Book();
void borrow_Book();
void return_Book();
void add_Book();
void proof_Manage();
void borrow_query(HSTMT hstmt, char *query);
int card_query(HSTMT hstmt, char *query);
void New_Card_query(HSTMT hstmt, char *query);
void return_query(HSTMT hstmt, char *query);
void Single_add_query(HSTMT hstmt, char *query);
void book_query(HSTMT hstmt, char *query);
void now_Amount_query(HSTMT hstmt, char *query, int *now_Amount);
int New_Card_test_query(HSTMT hstmt, char *query);
void Delete_Card_query(HSTMT hstmt, char *query);
int main()
{
	DBTest();
	system("pause");
	return 0;
}
void DBTest()
{
	int choice;
	error = SQLAllocEnv(&henv);//分配环境
	error = SQLAllocConnect(henv, &conn);//分配连接
	error = SQLConnect(conn, "HBdb", SQL_NTS, "SA", SQL_NTS, "whbY52015", SQL_NTS);
	printf("-----------------------------------\n");
	printf("\t选择操作：\n");
	while (1){
		printf("1.图书 2.借书 3.还书 4.图书入库 5.借阅证管理 0.退出系统\n");
		printf("请输入服务编号：\n");
		scanf("%d", &choice);
		getchar();
		switch (choice)
		{
		case 0:
			SQLDisconnect(conn);
			SQLFreeConnect(conn);
			SQLFreeEnv(henv);
			return;
		case 1:
			check_Book();
			break;
		case 2:
			borrow_Book();
			break;
		case 3:
			return_Book();
			break;
		case 4:
			add_Book();
			break;
		case 5:
			proof_Manage();
			break;
		default:
			printf("服务编号错误，请重新输入！\n");
		}
		printf("\n");
	}
	MessageBox(NULL, TEXT("执行成功"), TEXT("标题"), MB_OK);
}
void check_Book()
{
	char query[100], buff[20], *cquery;
	//cquery means const string query
	int choice = 0;
	while (1){
		printf("1.查询全部 2.按书名查询 3.按书号查询 0.退出\n");
		printf("请输入需要的服务编号：\n");
		scanf("%d", &choice);
		getchar();
		switch (choice)
		{
		case 0:
			return;
		case 1:
			error = SQLAllocStmt(conn, &hstmt);
			cquery = "select * from book";
			book_query(hstmt, cquery);
			break;
		case 2:
			error = SQLAllocStmt(conn, &hstmt);
			printf("输入书名：\n");
			gets(buff);
			sprintf(query, "select * from book where title = '%s'", buff);
			//query="select * from book where ";
			book_query(hstmt, query);
			break;
		case 3:
			error = SQLAllocStmt(conn, &hstmt);
			printf("输入书号：\n");
			gets(buff);
			sprintf(query, "select * from book where bno = '%s'", buff);
			//query="select * from book where ";
			book_query(hstmt, query);
			break;
		default:
			printf("服务编号错误！\n");
		}
		printf("\n");
	}
}
void borrow_Book()
{
	char query[200],cno[8],bno[11];
	int choice = 0;
	int now_Amount = 0;//必须初始化...
	printf("输入借书证号：\n");
	error = SQLAllocStmt(conn, &hstmt);
	gets(cno);
	sprintf(query, "select * from card where cno = %s", cno);
	if (card_query(hstmt, query) == 0)return;
	while (1){
		printf("1.查询已借书籍 2.输入书号借书 0.退出\n");
		printf("请输入需要的服务编号：\n");
		scanf("%d", &choice);
		getchar();
		switch (choice)
		{
		case 0:
			return;
		case 1:
			error = SQLAllocStmt(conn, &hstmt);
			sprintf(query, "select book.* from book INNER JOIN borrow on book.bno = borrow.bno where borrow.cno = %s ", cno);
			//注意返回格式和查询书籍相同，这里未包含已归还图书的记录
			printf("已借书籍：\n");
			book_query(hstmt, query);
			break;
		case 2:
			error = SQLAllocStmt(conn, &hstmt);
			printf("请输入书号借阅书籍：\n");
			gets(bno);
			sprintf(query, "select stock from book where bno = %s", bno);
			now_Amount_query(hstmt, query, &now_Amount);
			if (now_Amount){
				sprintf(query, "update book set stock = stock - 1 where bno = %s insert into borrow values('%s','%s',DEFAULT,DEFAULT) ",bno,cno,bno);
				borrow_query(hstmt, query);
			}
			else printf("书号输入错误或图书已无库存！\n");
			break;
		default:
			printf("服务编号错误！\n");
		}
		printf("\n");
	}
}
void return_Book()
{
	char query[200], cno[8], bno[11];
	int choice = 0;
	int now_Amount = 0;//必须初始化...
	printf("输入借书证号：\n");
	error = SQLAllocStmt(conn, &hstmt);
	gets(cno);
	sprintf(query, "select * from card where cno = %s", cno);
	if (card_query(hstmt, query) == 0)return;
	while (1){
		printf("1.查询已借书籍 2.输入书号还书 0.退出\n");
		printf("请输入需要的服务编号：\n");
		scanf("%d", &choice);
		getchar();
		switch (choice)
		{
		case 0:
			return;
		case 1:
			error = SQLAllocStmt(conn, &hstmt);
			sprintf(query, "select book.* from book INNER JOIN borrow on book.bno = borrow.bno where borrow.cno = %s ", cno);
			//注意返回格式和查询书籍相同，这里未包含已归还图书的记录
			printf("已借书籍：\n");
			book_query(hstmt, query);
			break;
		case 2:
			error = SQLAllocStmt(conn, &hstmt);
			printf("请输入书号归还书籍：\n");
			gets(bno);
			sprintf(query, "update book set stock = stock + 1 where bno = %s \
						    delete from borrow where cno=%s and bno=%s ", bno, cno, bno);
			return_query(hstmt, query);
			break;
		default:
			printf("服务编号错误！\n");
		}
		printf("\n");
	}
}
void add_Book()
{
	char query[300];//此处buff最好改成bno
	int choice = 0;
	char book_ID[11], book_category[11], book_name[21], press[21], writer[11],cno[8],Year[5],Price[10],Num[5];
	char path[50];
	int year = 0;
	float price = 0.0;
	int num = 0,i=0;
	FILE *fp;
	char ch=NULL;
	printf("输入管理员卡号：\n");
	error = SQLAllocStmt(conn, &hstmt);
	gets(cno);
	sprintf(query, "select * from card where cno = %s and type='M'", cno);
	if (card_query(hstmt, query) == 0)return;
	while (1){
		printf("1.单本入库 2.批量入库 0.退出\n");
		printf("请输入需要的服务编号：\n");
		scanf("%d", &choice);
		getchar();
		switch (choice)
		{
		case 0:
			return;
		case 1:
			error = SQLAllocStmt(conn, &hstmt);
			printf("输入：书号\t类别\t书名\t出版社\t年份\t作者\t价格：（请严格按照格式输入）\n");
			scanf("%s", book_ID);
			scanf("%s", book_category);
			scanf("%s", book_name);
			scanf("%s", press);
			scanf("%d",&year);
			scanf("%s", writer);
			scanf("%f",&price);
			sprintf(query, "if((select count(bno) from book where bno=%s)=0)insert into book values('%s','%s','%s','%s',%d,'%s',%f,1,1) else update book set total = total + 1, stock = stock + 1 where bno = %s ", book_ID, book_ID, book_category, book_name, press, year, writer, price, book_ID);
			Single_add_query(hstmt, query);
			break;
		case 2:
			printf("输入文件绝对路径：\n");
			gets(path);
			fp=fopen(path,"r");
			while(ch!=EOF){
				i=0;
				while((ch=fgetc(fp))!=EOF){
					if(ch==',')break;
					else book_ID[i++]=ch;
				}
				book_ID[i]=0;
				i=0;
				while((ch=fgetc(fp))!=EOF){
					if(ch==',')break;
					else book_category[i++]=ch;
				}
				book_category[i]=0;
				i=0;
				while((ch=fgetc(fp))!=EOF){
					if(ch==',')break;
					else book_name[i++]=ch;
				}
				book_name[i]=0;
				i=0;
				while((ch=fgetc(fp))!=EOF){
					if(ch==',')break;
					else press[i++]=ch;
				}
				press[i]=0;
				i=0;
				while((ch=fgetc(fp))!=EOF){
					if(ch==',')break;
					else Year[i++]=ch;
				}
				Year[i]=0;
				i=0;
				while((ch=fgetc(fp))!=EOF){
					if(ch==',')break;
					else writer[i++]=ch;
				}
				writer[i]=0;
				i=0;	
				while((ch=fgetc(fp))!=EOF){
					if(ch==',')break;
					else Price[i++]=ch;
				}
				Price[i]=0;	
				while((ch=fgetc(fp))!=EOF){
					if(ch==';')break;
					else Num[i++]=ch;
				}
				Num[i]=0;				
			//book_ID, book_category, book_name, press, year, writer, price,num
				error = SQLAllocStmt(conn, &hstmt);
				sprintf(query, "if((select count(bno) from book where bno=%s)=0)insert into book values('%s','%s','%s','%s',%s,'%s',%s,%s,%s) else update book set total = total + %s, stock = stock + %s where bno = %s ", book_ID, book_ID, book_category, book_name, press, Year, writer, Price,Num,Num,Num,Num, book_ID);
				Single_add_query(hstmt, query);
			}
			fclose(fp);
			break;
		default:
			printf("服务编号错误！\n");
		}
		printf("\n");
	}
}
void proof_Manage()
{
	char query[200];
	//cquery means const string query
	int choice = 0;
	char cno[8], name[11], department[41], type[2],password[16];
	printf("输入管理员卡号：\n");
	error = SQLAllocStmt(conn, &hstmt);
	gets(cno);
	sprintf(query, "select * from card where cno = %s and type='M'", cno);
	if (card_query(hstmt, query) == 0)return;
	while (1){
		printf("1.新建借书卡 2.删除借书卡 0.退出\n");
		printf("请输入需要的服务编号：\n");
		scanf("%d", &choice);
		getchar();
		switch (choice)
		{
		case 0:
			return;
		case 1:
			error = SQLAllocStmt(conn, &hstmt);
			printf("输入学号或工号\t名字\t专业\t类型\t密码：\n");
			//简化，不用动态分配卡号
			scanf("%s", cno);
			scanf("%s", name);
			scanf("%s", department);
			scanf("%s", type);
			scanf("%s", password);
			sprintf(query, "select count(cno) from card where cno=%s",cno);
			if (New_Card_test_query(hstmt, query) != 0){
				printf("借书卡已存在！\n");
				break;
			}
			//测试是否已有对应的借书卡
			sprintf(query, "INSERT INTO card values('%s','%s','%s','%s','%s')",cno, name, department, type, password);
			New_Card_query(hstmt, query);
			//没有已完成的query传入，需要重新写
			break;
		case 2:
			error = SQLAllocStmt(conn, &hstmt);
			printf("输入卡号认证：\n");
			gets(cno);
			sprintf(query, "select * from card where cno = %s", cno);
			if (card_query(hstmt, query) == 0)break;
			sprintf(query, "delete from card where cno = %s", cno);
			Delete_Card_query(hstmt, query);
			break;
		default:
			printf("服务编号错误！\n");
		}
		printf("\n");
	}
}

void book_query(HSTMT hstmt, char *query)
{
	char book_ID[11], book_category[11], book_name[21], press[21], writer[11];
	int year = 0;
	float price = 0.0;
	int total_Amount = 0, now_Amount = 0;
	SQLINTEGER lot[9];
	error = SQLExecDirect(hstmt, query, SQL_NTS);
	if (error == SQL_SUCCESS){
		SQLBindCol(hstmt, 1, SQL_C_CHAR, book_ID, 10, &lot[0]);
		SQLBindCol(hstmt, 2, SQL_C_CHAR, book_category, 10, &lot[1]);
		SQLBindCol(hstmt, 3, SQL_C_CHAR, book_name, 20, &lot[2]);
		SQLBindCol(hstmt, 4, SQL_C_CHAR, press, 20, &lot[3]);
		SQLBindCol(hstmt, 5, SQL_C_SHORT, &year, 0, &lot[4]);
		SQLBindCol(hstmt, 6, SQL_C_CHAR, writer, 10, &lot[5]);
		SQLBindCol(hstmt, 7, SQL_C_FLOAT, &price, 0, &lot[6]);
		SQLBindCol(hstmt, 8, SQL_C_SHORT, &total_Amount, 0, &lot[7]);
		SQLBindCol(hstmt, 9, SQL_C_SHORT, &now_Amount, 0, &lot[8]);
		printf("查询结果：\n");
		printf("----------------------------\n");
		printf("书号\t类别\t书名\t出版社\t年份\t作者\t价格\t总藏书量\t库存\n");
		printf("----------------------------\n");
		while (SQLFetch(hstmt) != SQL_NO_DATA_FOUND)
		{
			printf("%s\t%s\t%s\t%s\t%d\t%s\t%.2f\t%d\t%d\n", \
				book_ID, book_category, book_name, press, year, writer, price, \
				total_Amount, now_Amount);
		}
	}
	SQLFreeStmt(hstmt, SQL_CLOSE);
}

int card_query(HSTMT hstmt, char *query)
{
	int suc = 0;
	//用来判断card是否存在
	char Card_ID[8], name[11], department[41], type[2],password[16];
	char input_password[16];
	SQLINTEGER lot[5];
	error = SQLExecDirect(hstmt, query, SQL_NTS);
	if (error == SQL_SUCCESS){
		SQLBindCol(hstmt, 1, SQL_C_CHAR, Card_ID, 8, &lot[0]);
		SQLBindCol(hstmt, 2, SQL_C_CHAR, name, 11, &lot[1]);
		SQLBindCol(hstmt, 3, SQL_C_CHAR, department, 41, &lot[2]);
		SQLBindCol(hstmt, 4, SQL_C_CHAR, type, 2, &lot[3]);
		SQLBindCol(hstmt, 5, SQL_C_CHAR, password, 16, &lot[4]);
		if (SQLFetch(hstmt) == SQL_NO_DATA_FOUND){
			printf("卡号输入错误或类型错误！\n");
			return 0;
		}
		printf("输入密码：\n");
		gets(input_password);
		if (strcmp(input_password, password)){
			SQLFreeStmt(hstmt, SQL_CLOSE);
			printf("密码错误！\n\n");
			return 0;
		}
		printf("认证成功！\n");
		printf("借书卡信息：\n");
		printf("----------------------------\n");
		printf("卡号\t名字\t专业\t类型\n");
		printf("----------------------------\n");
		printf("%s\t%s\t%s\t%s\t\n", Card_ID, name, department, type);
		suc = 1;
	}
	else printf("系统故障\n");
	SQLFreeStmt(hstmt, SQL_CLOSE);
	printf("\n");
	return suc;
}

void now_Amount_query(HSTMT hstmt, char *query, int *now_Amount)
{
	SQLINTEGER length;
	error = SQLExecDirect(hstmt, query, SQL_NTS);
	if (error == SQL_SUCCESS){
		SQLBindCol(hstmt, 1, SQL_C_SHORT, now_Amount, 0, &length);
		SQLFetch(hstmt);
	}
	else printf("书籍不存在！\n");
	SQLFreeStmt(hstmt, SQL_CLOSE);
}

void borrow_query(HSTMT hstmt, char *query)
{
	error = SQLExecDirect(hstmt, query, SQL_NTS);
	if (error == SQL_SUCCESS){
		printf("借书成功！\n");
	}
	else printf("系统故障，请联系管理员！\n");
	SQLFreeStmt(hstmt, SQL_CLOSE);
}

void return_query(HSTMT hstmt, char *query)
{
	error = SQLExecDirect(hstmt, query, SQL_NTS);
	if (error == SQL_SUCCESS){
		printf("归还成功！\n");
	}
	else printf("未借阅书籍！请确认书籍为已借阅书籍！\n");
	SQLFreeStmt(hstmt, SQL_CLOSE);
}

void Single_add_query(HSTMT hstmt, char *query)
{
	error = SQLExecDirect(hstmt, query, SQL_NTS);
	if (error == SQL_SUCCESS){
		printf("图书入库成功！\n");
	}
	else printf("输入格式错误\n");
	SQLFreeStmt(hstmt, SQL_CLOSE);
}

void New_Card_query(HSTMT hstmt, char *query)
{
	error = SQLExecDirect(hstmt, query, SQL_NTS);
	if (error == SQL_SUCCESS){
		printf("借书卡新建成功！\n");
	}
	else printf("系统故障！\n");
	SQLFreeStmt(hstmt, SQL_CLOSE);
}

int New_Card_test_query(HSTMT hstmt, char *query)
{
	SQLINTEGER length;
	int test = 0;
	error = SQLExecDirect(hstmt, query, SQL_NTS);
	if (error == SQL_SUCCESS){
		SQLBindCol(hstmt, 1, SQL_C_SHORT, &test, 0, &length);
	}
	else printf("输入格式错误！\n");
	SQLFreeStmt(hstmt, SQL_CLOSE);
	return test;
}

void Delete_Card_query(HSTMT hstmt, char *query)
{
	error = SQLExecDirect(hstmt, query, SQL_NTS);
	if (error == SQL_SUCCESS){
		printf("借书卡删除成功！\n");
	}
	else printf("系统故障！\n");
	SQLFreeStmt(hstmt, SQL_CLOSE);
}