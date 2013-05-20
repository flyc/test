#include <stdio.h>
#include "log_debug.h"

/*
void scan(char* path)
{
	DIR* dir = NULL;
	struct dirent* s_dir = NULL;
	struct stat file_stat;
	int i = 0;	
	char buf[1024] = {0};
		
	printf("param %s\n", path);	

	dir = opendir(path);

	if (NULL == dir)
		printf("failed to open %s \n", path);

			
	while(1)
	{
		s_dir = readdir(dir);
		i++;
		if (NULL == s_dir)
		{
			printf("readdir over\n");
			break;
		}
		sprintf(buf, "%s/%s", path, s_dir->d_name);
		if (-1 == stat(buf, &file_stat)) 
		{
			printf("failed to call stat(%s),error:%d\n", s_dir->d_name, errno);
		}
		else
		{
			if (S_ISREG(file_stat.st_mode) && 0 == IsLogFile(s_dir->d_name))
			{
				printf("%s\n", s_dir->d_name);
			}
		}
	}
		closedir(dir);

}
*/




int main(int argc, char** argv)
{
	std::cout<<_LOG_DEBUG_DIR_<<_LOG_DEBUG_OVERDUE<<std::endl;
	CLogDir dir(_LOG_DEBUG_DIR_, _LOG_DEBUG_OVERDUE);
	
	printf("xixi");

	return 0;
}
