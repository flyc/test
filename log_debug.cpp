#include "log_debug.h"
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <time.h>
#include <dirent.h>
#include <string.h>
#include <stdio.h>

#include <algorithm>
#include <functional>

int  IsLogFile(char* path)
{
    if (NULL == path)
        return -1;

    int length = strlen(path);
    if (0 == length)
        return -1;

    char    ext[5] = {0};
    strncpy(ext, path+length-4, 4);

    return strcmp(ext, ".log");
}
bool	is_valid_diretory(const std::string& dir)
{
	if (dir.empty())
		return false;

	struct stat buf;
	int ret = stat(dir.c_str(), &buf);

	if (-1 == ret)
		return false;

	return (buf.st_mode&S_IFDIR) == S_IFDIR;
} 

// CLogDir
CLogDir::CLogDir( std::string strDir, int storageDay )
:_storageDay(storageDay)
{
	bool bValid = is_valid_diretory(strDir);

	if (!bValid)
	{
		std::cout<<_strLogDiretory<<std::endl;

		if (0 != mkdir(strDir.c_str(), S_IRWXU|S_IRWXG)) // try to create diretory
			std::cout<<std::endl<<"ERROR: Failed to Init Log Diretory! "<<strDir<<std::endl;
		else
			bValid = true;
	}
	if (bValid)		
	{
		_strLogDiretory = strDir;
		bValid = _InitDir();
		if (!bValid)
			std::cout<<std::endl<<"ERROR: Failed to init dir!"<<std::endl;

	std::cout<<std::endl<<"storeageday:"<<_storageDay<<std::endl;

		bValid = ClearOverdueFiles();
		if (!bValid)
			std::cout<<std::endl<<"ERROR: Failed to Clear Overdue files in "<<strDir<<std::endl;
	}
}

CLogDir::~CLogDir()
{
	_vec_files_unix_stamp.clear();
	_map_files_cache.clear();
}

bool CLogDir::_InitDir()
{
    return  _InitDirLinux();
}

//2013-3-18_17-17-19_1363598239.log -> 1363598239
time_t CLogDir::_SpiltUnixStamp(const std::string& filename)
{
	std::string path = filename;
	std::string::size_type posEnd = path.npos;
	posEnd = path.find_last_of(".");
	if (path.npos == posEnd)
		return 0;
	
	std::string::size_type posStart = path.npos;
	posStart = path.find_last_of("_");
	if (path.npos == posStart)
		return 0;

	std::string strUnixStamp = path.substr(posStart+1, posEnd-posStart-1);
	if (strUnixStamp.empty())
		return 0;

	return	time_t(strtol(strUnixStamp.c_str(), NULL, 10));
}

bool CLogDir::ClearOverdueFiles()
{
	if (_vec_files_unix_stamp.empty())
		return true;

	time_t t_term = _vec_files_unix_stamp[0]-_storageDay*g_sec_of_day;

	std::cout<<"max:"<<_vec_files_unix_stamp[0]<<"sub:"<<t_term<<std::endl;
	
	std::vector<time_t>::iterator	itor, itorRm;
	for (itor = _vec_files_unix_stamp.begin(); itor != _vec_files_unix_stamp.end(); ++itor )	
	{
		std::cout<<std::endl<<(*itor)<<"--"<<t_term<<std::endl;
		if ((*itor) <= t_term)
		{
			std::cout<<"outout:"<<(*itor)<<std::endl;
			break;
		}
	}

	if (itor == _vec_files_unix_stamp.end()) // no overdue files
		return true;

	std::cout<<"ClearOverduefiles"<<std::endl;

	itorRm = itor;
	//del overdue files
	while(itor != _vec_files_unix_stamp.end())
	{
		std::string filename ;
		std::map<time_t, std::string>::iterator ret = _map_files_cache.find((*itor));
		if (ret == _map_files_cache.end())
			return false;
		
		std::string rmFile = _strLogDiretory + "/" + ret->second;
		if (0 != remove(rmFile.c_str()) )
			return false;
		
		_map_files_cache.erase(ret);
		itor++;
	}

	_vec_files_unix_stamp.erase(itorRm, itor);

	return true;
}

bool CLogDir::_InitDirWin32()
{
#if _WIN32
    if (_strLogDiretory.empty())
        return false;

    std::string  strPattern = _strLogDiretory + "/*.log";

    _finddata_t		c_file;
    intptr_t hFile = _findfirst(strPattern.c_str(), &c_file);
    if (-1 == hFile)
    {
        _findclose(hFile);
        return true;
    }

    std::string filename(c_file.name);
    time_t	uinxStampBuf = _SpiltUnixStamp(filename);
    if (0 == uinxStampBuf)
    {
        _findclose(hFile);
        return false;
    }

    _vec_files_unix_stamp.push_back(uinxStampBuf);
    _map_files_cache.insert(std::map<time_t, std::string>::value_type(uinxStampBuf, filename));

    while(-1 != _findnext(hFile, &c_file))
    {
        std::string filename(c_file.name);
        time_t	uinxStampBuf = _SpiltUnixStamp(filename);
        if (0 == uinxStampBuf)
        {
            _findclose(hFile);
            return false;
        }

        _vec_files_unix_stamp.push_back(uinxStampBuf);
        _map_files_cache.insert(std::map<time_t, std::string>::value_type(uinxStampBuf, filename));
    }

    std::sort(_vec_files_unix_stamp.begin(), _vec_files_unix_stamp.end(), std::greater<time_t>());
    _findclose(hFile);

    return true;
#else
    return false;
#endif
	
}

bool CLogDir::_InitDirLinux()
{
    if (_strLogDiretory.empty())
        return false;

    DIR* dir = NULL;
    struct dirent* s_dir = NULL;
    struct stat file_stat;

    dir = opendir(_strLogDiretory.c_str());
    if (NULL == dir)
        return false;

    while(1)
    {
        s_dir = readdir(dir);
        if (NULL == s_dir)
        {
            break;
            return true;
        }
        std::string filename(s_dir->d_name);
        std::string strBuf = _strLogDiretory +"/"+ filename;
	std::cout<<strBuf<<std::endl;
        if (0 == stat(strBuf.c_str(), &file_stat))
        {
            if (S_ISREG(file_stat.st_mode) && 0 == IsLogFile(s_dir->d_name))
            {
                time_t	uinxStampBuf = _SpiltUnixStamp(filename);
                if (0 == uinxStampBuf)
                {
			std::cout<<"failed to call _spiltuninxstamp"<<std::endl;
			continue;
                }

                _vec_files_unix_stamp.push_back(uinxStampBuf);
                _map_files_cache.insert(std::map<time_t, std::string>::value_type(uinxStampBuf, filename));
            }
        }
    }

    std::sort(_vec_files_unix_stamp.begin(), _vec_files_unix_stamp.end(), std::greater<time_t>());

	std::cout<<"size:"<<_vec_files_unix_stamp.size();
    
    closedir(dir);
    return true;
}
