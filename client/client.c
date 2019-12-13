#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> // bzero
#include <sys/prctl.h>
#include <sys/socket.h> // socket
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h> // socket
#include <time.h>
#include <unistd.h>

#define BUFFER_SIZE 2048
#define FILE_NAME_MAX_SIZE 128

int menu(void)
{
	int select = 0;
	printf("We now support the following options : \n");
ABCD:
	printf("  - 1. Send file to server.\n");
	printf("  - 2. Download files to the server.\n");
	printf("  - 3. Exit.\n");
	printf("Please enter Your Choice : ");
	scanf("%d", &select);
	if (select < 1 || select > 3)
	{
		printf("[ %ld ] Input error, please input again : \n", time(NULL));
		goto ABCD;
	}
	return select;
}

int upload(int fd, int sel)
{
	struct timeval tvBegin;
	struct timeval tvNow;

	char file_name[FILE_NAME_MAX_SIZE] = {0};

	printf("Please enter the file to upload : ");
	scanf("%s", file_name);
	char buffer[BUFFER_SIZE] = {0};

	// 打开文件并读取文件数据
	FILE *fp = fopen(file_name, "r");
	if (NULL == fp)
	{
		printf("\n[ %ld ] File : %s Not Found!\n\n", time(NULL), file_name);
		return -1;
	}
	char buf[BUFFER_SIZE] = {0};
	sprintf(buf, "%d", sel);
	// 向服务器发送用户的选择
	if (send(fd, buf, BUFFER_SIZE, 0) < 0)
	{
		perror("Send File Failed : ");
		exit(1);
	}

	printf("line = %d, file_name = %s\n", __LINE__, file_name);

	int bytes = 0;

	// 向服务器发送buffer中的数据
	if ((bytes = send(fd, file_name, strlen(file_name) + 1, 0)) < 0)
	{
		perror("Send File Name Failed : ");
		exit(1);
	}
	sleep(2);
	printf("\n[ %ld ] File : %s Transfer Starting!\n\n", time(NULL), file_name);
	bzero(buffer, BUFFER_SIZE);
	int length = 0;
	// start counter
	gettimeofday(&tvBegin, NULL);
	// 每读取一段数据，便将其发送给客户端，循环直到文件读完为止
	while ((length = fread(buffer, sizeof(char), BUFFER_SIZE, fp)) > 0)
	{
		if (send(fd, buffer, length, 0) < 0)
		{
			printf("\n[ %ld ] Send File : %s Failed.\n\n", time(NULL), file_name);
			break;
		}
		bzero(buffer, BUFFER_SIZE);
	}
	// usleep(1);
	// send(fd, "_F_I_N_", 7, 0);
	// 关闭文件
	fclose(fp);
	gettimeofday(&tvNow, NULL);
	long int timespan = (tvNow.tv_sec - tvBegin.tv_sec) * 1000000 + tvNow.tv_usec - tvBegin.tv_usec;

	// 接收成功后，关闭文件，关闭socket
	printf("\n[ %ld ] File : %s Transfer Successful!\n\n", time(NULL), file_name);
	printf("%ld\n", timespan);

	return 0;
}

int download(int fd)
{
	struct timeval tvBegin;
	struct timeval tvNow;
	// 输入文件名 并放到缓冲区buffer中等待发送
	char file_name[FILE_NAME_MAX_SIZE + 1] = {0};
	char buffer[BUFFER_SIZE] = {0};
	printf("Please enter the file to download : ");
	scanf("%s", file_name);
	strncpy(buffer, file_name, strlen(file_name) > BUFFER_SIZE ? BUFFER_SIZE : strlen(file_name));
	// 向服务器发送buffer中的数据
	if (send(fd, buffer, BUFFER_SIZE, 0) < 0)
	{
		perror("Send File Name Failed : ");
		exit(1);
	}
	int length = 0;
	if ((length = recv(fd, buffer, BUFFER_SIZE, 0)) > 0)
	{
		if (strncmp(buffer, "_N_O_E_X_", strlen("_N_O_E_X_")) == 0) // 文件不存在
		{
			printf("\n[ %ld ] There is no file named %s on the server!\n\n", time(NULL), file_name);
			return -1;
		}
		// else if (strncmp(buffer, "_S_T_A_R_T_", strlen("_S_T_A_R_T_")) == 0) // 文件存在并且开始
		// {
		// }
	}
	else
	{
		return -1;
	}

	// 打开文件，准备写入
	FILE *fp = fopen(file_name, "w");
	if (NULL == fp)
	{
		printf("\n[ %ld ] File %s Can Not Open To Write\n\n", time(NULL), file_name);
		exit(1);
	}
	// 从服务器接收数据到buffer中
	// 每接收一段数据，便将其写入文件中，循环直到文件接收完并写完为止
	bzero(buffer, BUFFER_SIZE);
	printf("\n[ %ld ] Recv Files Starting!\n\n", time(NULL));
	// start counter
	gettimeofday(&tvBegin, NULL);
	while ((length = recv(fd, buffer, BUFFER_SIZE, 0)) > 0)
	{
		if (fwrite(buffer, sizeof(char), length, fp) < length)
		{
			printf("[ %ld ] File %s Write Failed!\n", time(NULL), file_name);
			break;
		}
		bzero(buffer, BUFFER_SIZE);
		fflush(fp);
	}
	fflush(fp);
	fclose(fp);
	gettimeofday(&tvNow, NULL);
	long int timespan = (tvNow.tv_sec - tvBegin.tv_sec) * 1000000 + tvNow.tv_usec - tvBegin.tv_usec;

	// 接收成功后，关闭文件，关闭socket
	printf("\n[ %ld ] Receive File %s From Server IP Successful!\n\n", time(NULL), file_name);
	printf("%ld\n", timespan);
	return 0;
}

int main(int argc, const char **argv)
{
	int sel = 0;
	if (argc < 3)
	{
		printf("\n[ %ld ] Enter server Address and port\n", time(NULL));
		printf("[ %ld ] usage : ./server ipaddr port\n\n", time(NULL));
		exit(-1);
	}

	// 声明并初始化一个客户端的socket地址结构
	struct sockaddr_in client_addr;
	bzero(&client_addr, sizeof(client_addr));
	client_addr.sin_family = AF_INET;
	client_addr.sin_port = htons(atoi(argv[2]));
	client_addr.sin_addr.s_addr = inet_addr(argv[1]);

	// 创建socket，若成功，返回socket描述符
	int client_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (client_socket_fd < 0)
	{
		char buf[128] = {0};
		sprintf(buf, "[ %ld ] Create Socket Failed : ", time(NULL));
		perror(buf);
		exit(1);
	}

	// 向服务器发起连接，连接成功后client_socket_fd代表了客户端和服务器的一个socket连接
	if (connect(client_socket_fd, (struct sockaddr *)&client_addr, sizeof(client_addr)) < 0)
	{
		char buf[128] = {0};
		sprintf(buf, "[ %ld ] Can Not Connect To Server IP : ", time(NULL));
		perror(buf);
		exit(0);
	}
	// QWER:
	sel = menu();

	if (sel == 1)
	{
		upload(client_socket_fd, sel);
		// goto QWER;
	}
	else if (sel == 2)
	{
		char buf[BUFFER_SIZE] = {0};
		sprintf(buf, "%d", sel);
		// 向服务器发送用户的选择
		if (send(client_socket_fd, buf, BUFFER_SIZE, 0) < 0)
		{
			char buf[128] = {0};
			sprintf(buf, "[ %ld ] Send File Failed : ", time(NULL));
			perror(buf);
			exit(1);
		}
		download(client_socket_fd);
		// goto QWER;
	}
	else
	{
		close(client_socket_fd);
	}

	return 0;
}
