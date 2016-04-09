#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include <windows.h>
#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>
#include <string.h>
#include <time.h>
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
void borrow_query(HSTMT hstmt, char query);
void card_query(HSTMT hstmt, char query){}
void New_Card_query(HSTMT hstmt, char query){}
void record_query(HSTMT hstmt, char query){}
void return_query(HSTMT hstmt, char query){}
void Single_add_query(hstmt, query, book_ID, book_name, book_category, press, writer, year){}
void book_query(HSTMT hstmt, char query);
int main()
{
	DBTest();
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
		switch (choice)
		{
		case 0:
			SQLDisconnect(conn);
			SQLFreeConnect(conn);
			SQLFreeEnv(henv);
			exit(0);
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
		scanf("％d", &choice);
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
		default:
			printf("服务编号错误！\n");
		}
	}
}

void borrow_Book()
{
	char query[100], buff[20], *cquery;
	int choice = 0;
	int length, now_Amount;
	while (1){
		printf("1.查询借书证信息 2.输入书号借书 0.退出\n");
		printf("请输入需要的服务编号：\n");
		scanf("％d", &choice);
		switch (choice)
		{
		case 0:
			return;
		case 1:
			error = SQLAllocStmt(conn, &hstmt);
			printf("输入借书证号：\n");
			gets(buff);
			sprintf(query, "select * from card where cno = %s", buff);
			card_query(hstmt, query);
			sprintf(query, "select * from borrow natural join book \
						   					where cno = %s and return_date = null", buff);
			//注意返回格式和查询书籍相同，这里未包含已归还图书的记录
			record_query(hstmt, query);
			break;
		case 2:
			error = SQLAllocStmt(conn, &hstmt);
			printf("输入书号：\n");
			gets(buff);
			sprintf(query, "select now_Amount from book where bno = '%s'", buff);
			error = SQLExecDirect(hstmt, query, SQL_NTS);
			if (error == SQL_SUCCESS)
				SQLBindCol(hstmt, 1, SQL_C_SHORT, &now_Amount, 0, &length);
			else printf("书籍不存在！\n");
			borrow_query(hstmt, query, now_Amount);
			break;
		default:
			printf("服务编号错误！\n");
		}
	}
}
void return_Book()
{
	char query[100], buff[20], *cquery;//此处buff最好改成bno
	int choice = 0;
	int length1, length2, now_Amount = 0, total_Amount = 0;
	while (1){
		printf("1.查询借书证信息 2.输入书号借书 0.退出\n");
		printf("请输入需要的服务编号：\n");
		scanf("％d", &choice);
		switch (choice)
		{
		case 0:
			return;
		case 1:
			error = SQLAllocStmt(conn, &hstmt);
			printf("输入借书证号：\n");
			gets(buff);
			sprintf(query, "select * from card where cno = %s", buff);
			card_query(hstmt, query);
			sprintf(query, "select *	from borrow natural join book \
						   					where cno = %s and return_date = null", buff);
			record_query(hstmt, query);
			break;
		case 2:
			error = SQLAllocStmt(conn, &hstmt);
			printf("输入书号：\n");
			gets(buff);
			sprintf(query, "select total_Amount,now_Amount from book where bno = '%s'", buff);
			error = SQLExecDirect(hstmt, query, SQL_NTS);
			if (error == SQL_SUCCESS){
				SQLBindCol(hstmt, 1, SQL_C_SHORT, &now_Amount, 0, &length1);
				SQLBindCol(hstmt, 2, SQL_C_SHORT, &now_Amount, 0, &length2);
			}
			else {
				printf("书籍不存在！\n");
				break;
			}
			if (now_Amount<total_Amount)return_query(hstmt, query, now_Amount);
			else printf("图书已满存！\n");
			break;
		default:
			printf("服务编号错误！\n");
		}
	}
}
void add_Book()
{
	char query[100], buff[20], *cquery;//此处buff最好改成bno
	int choice = 0;
	char book_ID[10], book_category[10], book_name[20], press[20], writer[10];
	int year = 0;
	float price = 0.0;
	while (1){
		printf("1.单本入库 2.批量入库 0.退出\n");
		printf("请输入需要的服务编号：\n");
		scanf("％d", &choice);
		switch (choice)
		{
		case 0:
			return;
		case 1:
			error = SQLAllocStmt(conn, &hstmt);
			/*printf("输入书号：\n");
			gets(book_ID);
			printf("输入类别：\n");
			gets(book_category);
			printf("输入书名：\n");
			gets(book_name);
			printf("输入出版社：\n");
			gets(press);
			printf("输入作者：\n");
			gets(writer);
			printf("输入年份：\n");
			scanf("%d",&year);
			printf("输入价格：\n");
			scanf("%f",&price);*/
			Single_add_query(hstmt, query, book_ID, book_name, book_category, press, writer, year);
			//可能需要简化
			break;
		case 2:
			break;
		default:
			printf("服务编号错误！\n");
		}
	}
}
void proof_Manage()
{
	char query[100], buff[20];
	//cquery means const string query
	int choice = 0;
	char cno[7], name[10], department[40], type[1];
	while (1){
		printf("1.新建借书卡 2.删除借书卡 0.退出\n");
		printf("请输入需要的服务编号：\n");
		scanf("％d", &choice);
		switch (choice)
		{
		case 0:
			return;
		case 1:
			error = SQLAllocStmt(conn, &hstmt);
			printf("输入学号：\n");
			//简化，不用动态分配卡号
			gets(cno);
			printf("输入名字：\n");
			gets(name);
			printf("输入专业：\n");
			gets(department);
			printf("输入类型：（学生输入0，教师输入1）\n");
			gets(type);
			New_Card_query(hstmt, query, cno, name, department, type);
			//没有已完成的query传入，需要重新写
			break;
		case 2:
			error = SQLAllocStmt(conn, &hstmt);
			printf("输入卡号：\n");
			gets(buff);
			sprintf(query, "delete from card where cno = %s", buff);
			//query="select * from book where ";
			card_query(hstmt, query);
			printf("删除成功！\n");
			break;
		default:
			printf("服务编号错误！\n");
		}
	}

}


void book_query(HSTMT hstmt, char query)
{
	char book_ID[10], book_category[10], book_name[20], press[20], writer[10];
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

void borrow_query(HSTMT hstmt, char query)
{
	char book_ID[10], book_category[10], book_name[20], press[20], writer[10];
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









