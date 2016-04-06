#include <stdio.h>
#include <sql.h>
//#include <windows.h>
#include <sqlext.h>
#include <sqltypes.h>
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
int main()
{
	DBTest();
	return 0;
}
void DBTest()
{
	int choice;
	error = SQLAllocEnv(&henv);//分配环境
	error = SQLAllocConnect(henv,&conn);//分配连接
	error = SQLConnect(conn,"MyClassmates",SQL_NTS,"SA",SQL_NTS,"whbY52015",SQL_NTS);
	printf("-----------------------------------\n");
	printf("\t选择操作：\n");
	while(1){
		printf("1.图书 2.借书 3.还书 4.图书入库 5.借阅证管理 0.退出系统\n");
		printf("请输入服务编号：\n");
		scanf("%d",choice);
		switch(choice)
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
	MessageBox(NULL,TEXT("执行成功"),TEXT("标题"),MB_OK);
}
void check_Book()
{
	char *query;
	int choice=0;
	char book_ID[10],book_category[10],book_name[20],press[20],writer[10];
	int year = 0;
	float price=0.0;
	int total_Amount=0,now_Amount=0;
	SQLINTEGER lot[9];
	while(1){
		printf("1.查询全部 2.按书名查询 3.按书号查询 0.退出\n");
		printf("请输入需要的服务编号：\n");
		scanf("％d",&choice);
		switch(choice)
		{
			case 0:
				return;
			case 1:
				error=SQLAllocStmt(conn,&hstmt);
				query="select * from book";
				error=SQLExctDirect(hstmt,query,SQL_NTS);
				if(error==SQL_SUCCESS){
					SQLBindCol(hstmt,1,SQL_C_CHAR,book_ID,10,&lot[0]);
					SQLBindCol(hstmt,2,SQL_C_CHAR,book_category,10,&lot[1]);				
					SQLBindCol(hstmt,3,SQL_C_CHAR,book_name,20,&lot[2]);
					SQLBindCol(hstmt,4,SQL_C_CHAR,press,20,&lot[3]);
					SQLBindCol(hstmt,5,SQL_C_SHORT,&year,0,&lot[4]);
					SQLBindCol(hstmt,6,SQL_C_CHAR,writer,10,&lot[5]);
					SQLBindCol(hstmt,7,SQL_C_FLOAT,&price,0,&lot[6]);
					SQLBindCol(hstmt,8,SQL_C_SHORT,&total_Amount,0,&lot[7]);
					SQLBindCol(hstmt,9,SQL_C_SHORT,&now_Amount,0,&lot[8]);
					printf("查询结果：\n");
					printf("----------------------------\n");
					printf("书号\t类别\t书名\t出版社\t年份\t作者\t价格\t总藏书量\t库存\n");
					printf("----------------------------\n");
					while(SQLFecth(hstmt)!=SQL_NO_DATA_FOUND)
					{
						printf("%s\t%s\t%s\t%s\t%d\t%s\t%.2f\t%d\t%d\n",\
						book_ID,book_category,book_name,press,year,writer,price,\
						total_Amount,now_Amount );
					}
				}
				SQFreeStmt(hstmt,SQL_CLOSE);
				break;
			default:
				printf("服务编号错误！\n");
		}
	}
}

void borrow_Book()
{

}
void return_Book()
{

}
void add_Book()
{

}
void proof_Manage()
{

}













