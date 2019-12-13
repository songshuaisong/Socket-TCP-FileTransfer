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

#define LENGTH_OF_LISTEN_QUEUE 200
#define BUFFER_SIZE 2048
#define FILE_NAME_MAX_SIZE 128

int upload(int fd);
int download(int fd);

int main(int argc, const char **argv)
{
	if (argc < 3)
	{
		printf("\n[ %ld ] Enter server Address and port\n", time(NULL));
		printf("[ %ld ] usage : ./server ipaddr port\n\n", time(NULL));
		exit(-1);
	}

	// 声明并初始化一个服务器端的socket地址结构
	struct sockaddr_in server_addr;
	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(argv[2]));
	server_addr.sin_addr.s_addr = inet_addr(argv[1]);

	// 创建socket，若成功，返回socket描述符
	int server_socket_fd = socket(PF_INET, SOCK_STREAM, 0);
	if (server_socket_fd < 0)
	{
		char buf[128] = {0};
		sprintf(buf, "[ %ld ] Create Socket Failed : ", time(NULL));
		perror(buf);
		exit(1);
	}
	int opt = 1;
	setsockopt(server_socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	// 绑定socket和socket地址结构
	if (-1 == (bind(server_socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr))))
	{
		char buf[128] = {0};
		sprintf(buf, "[ %ld ] Server Bind Failed : ", time(NULL));
		perror(buf);
		exit(1);
	}
	// socket监听
	if (-1 == (listen(server_socket_fd, LENGTH_OF_LISTEN_QUEUE)))
	{
		char buf[128] = {0};
		sprintf(buf, "[ %ld ] Server Listen Failed : ", time(NULL));
		perror(buf);
		exit(1);
	}
	printf("\n[ %ld ] Server start successful! \n\n", time(NULL));
	int new_server_socket_fd = 0;

	while (1)
	{
		// 定义客户端的socket地址结构
		struct sockaddr_in client_addr;
		socklen_t client_addr_length = sizeof(client_addr);

		new_server_socket_fd = accept(server_socket_fd, (struct sockaddr *)&client_addr, &client_addr_length);
		if (new_server_socket_fd < 0)
		{
			char buf[128] = {0};
			sprintf(buf, "[ %ld ] Failed to accept : ", time(NULL));
			perror(buf);
			return -1;
		}
		printf("\n[ %ld ] Client connent sucessfull : ip: %s, port: %d\n\n", time(NULL), inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
		int pid = 0;
		if ((pid = fork()) == -1)
		{
			printf("[ %ld ] line %d func fork error\n", time(NULL), __LINE__);
		}
		else if (pid > 0) //父进程进行客户端的链接
		{
			close(new_server_socket_fd); //父进程关闭acceptfd文件描述符
		}
		else //子进程进行数据的通信
		{
			char buffer[BUFFER_SIZE];
			bzero(buffer, BUFFER_SIZE);
			if (recv(new_server_socket_fd, buffer, BUFFER_SIZE, 0) < 0)
				perror("Server Recieve Data Failed : ");

			// printf("line = %d, %s\n", __LINE__, buffer);
			if (strncmp(buffer, "1", 1) == 0) // 接收文件
			{
				upload(new_server_socket_fd);
			}
			else if (strncmp(buffer, "2", 1) == 0) // 下载文件
			{
				download(new_server_socket_fd);
			}
			close(new_server_socket_fd);
			printf("[ %ld ] Client Exit!\n", time(NULL));
			exit(0);
		}
	}
	// 关闭监听用的socket
	close(server_socket_fd);
	prctl(PR_SET_PDEATHSIG, SIGKILL);

	return 0;
}

int upload(int fd)
{
	struct timeval tvBegin;
	struct timeval tvNow;
	// recv函数接收数据到缓冲区buffer中
	char buffer[BUFFER_SIZE] = {0}, filename[FILE_NAME_MAX_SIZE] = {0};
	int bytes = 0;
	if ((bytes = recv(fd, filename, FILE_NAME_MAX_SIZE, 0)) < 0)
	{
		char buf[128] = {0};
		sprintf(buf, "[ %ld ] Server Recieve Data Failed : ", time(NULL));
		perror(buf);
		return -1;
	}
	printf("[ %ld ] line = %d, bytes = %d, filename = %s\n", time(NULL), __LINE__, bytes, filename);

	// 打开文件，准备写入
	FILE *fp = fopen(filename, "w");
	if (NULL == fp)
	{
		printf("\n[ %ld ] File %s Can Not Open To Write\n\n", time(NULL), filename);
		exit(1);
	}
	// 从服务器接收数据到buffer中
	// 每接收一段数据，便将其写入文件中，循环直到文件接收完并写完为止
	bzero(buffer, BUFFER_SIZE);
	int length = 0;
	// start counter
	gettimeofday(&tvBegin, NULL);
	while ((length = recv(fd, buffer, BUFFER_SIZE, 0)) > 0)
	{
		if (fwrite(buffer, sizeof(char), length, fp) < length)
		{
			printf("\n[ %ld ] File %s Write Failed!\n", time(NULL), buffer);
			break;
		}
		bzero(buffer, BUFFER_SIZE);
		fflush(fp);
	}

	fflush(fp);
	fclose(fp);
	gettimeofday(&tvNow, NULL);
	long int timespan = (tvNow.tv_sec - tvBegin.tv_sec) * 1000000 + tvNow.tv_usec - tvBegin.tv_usec;

	printf("\n[ %ld ] Receive File %s From Client Successful!\n\n", time(NULL), filename);
	printf("%ld\n", timespan);

	return 0;
}

int download(int fd)
{
	struct timeval tvBegin;
	struct timeval tvNow;

	// recv函数接收数据到缓冲区buffer中
	char buffer[BUFFER_SIZE];
	bzero(buffer, BUFFER_SIZE);
	if (recv(fd, buffer, BUFFER_SIZE, 0) < 0)
	{
		char buf[128] = {0};
		sprintf(buf, "[ %ld ] Server Recieve Data Failed : ", time(NULL));
		perror(buf);
		// break;
	}
	// 然后从buffer(缓冲区)拷贝到file_name中
	char file_name[FILE_NAME_MAX_SIZE + 1];
	bzero(file_name, FILE_NAME_MAX_SIZE + 1);
	strncpy(file_name, buffer, strlen(buffer) > FILE_NAME_MAX_SIZE ? FILE_NAME_MAX_SIZE : strlen(buffer));
	// printf("%s\n", file_name);

	// 打开文件并读取文件数据
	FILE *fp = fopen(file_name, "r");
	if (NULL == fp)
	{
		printf("\n[ %ld ] File %s Not Found!\n", time(NULL), file_name);
		strcpy(buffer, "_N_O_E_X_");
		if (send(fd, buffer, strlen(buffer), 0) < 0)
			printf("\n[ %ld ] Send Message Failed.\n\n", time(NULL));
	}
	else
	{
		strcpy(buffer, "_S_T_A_R_T_");
		if (send(fd, buffer, strlen(buffer), 0) < 0)
			printf("\n[ %ld ] Send Message Failed.\n\n", time(NULL));

		bzero(buffer, BUFFER_SIZE);
		int length = 0;

		printf("\n[ %ld ] Send Files Starting!\n\n", time(NULL));
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
		// 关闭文件
		fclose(fp);

		gettimeofday(&tvNow, NULL);
		long int timespan = (tvNow.tv_sec - tvBegin.tv_sec) * 1000000 + tvNow.tv_usec - tvBegin.tv_usec;

		printf("\n[ %ld ] File : %s Transfer Successful!\n\n", time(NULL), file_name);
		printf("%ld\n", timespan);
	}

	return 0;
}

