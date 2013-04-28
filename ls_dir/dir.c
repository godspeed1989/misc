#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>

void List(char *path, int indent)
{
	DIR *pDir;
	char dir[512];
	struct dirent* ent = NULL;
	struct stat statbuf;
	if((pDir=opendir(path)) == NULL)
	{
		fprintf( stderr, "Cannot open directory:%s\n", path );
		return;
	}
	while((ent=readdir(pDir)) != NULL)
	{
		//得到读取文件的绝对路径名
		snprintf(dir, 512,"%s/%s", path, ent->d_name);
		//得到文件信息
		lstat(dir, &statbuf);
		//判断是目录还是文件
		if(S_ISDIR(statbuf.st_mode))
		{
			//排除当前目录和上级目录
			if(strcmp(".", ent->d_name) == 0 ||
			   strcmp("..", ent->d_name) == 0)
				continue;
			//如果是子目录,递归调用函数本身,实现子目录中文件遍历
			printf( "%*s%s/\n", indent, "", ent->d_name);
			//递归调用,遍历子目录中文件
			List(dir, indent + 2);
		}
		else
		{
			printf("%*s%s\n", indent, "", ent->d_name);
		}
	}//while
	closedir(pDir);
}
int main(int argc, char* argv[])
{
	if(argc == 2)
		List(argv[1], 2);
	else
		List(".", 0);
	return 0;
}

