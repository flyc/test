#ifndef		__LOG_DEBUG_H_
#define		__LOG_DEBUG_H_

#include <string>
#include <vector>
#include <map>
#include <iostream>


#define   sprintf_s  snprintf   

// config
#define		_LOG_DEBUG_				1
#define     _LOG_DEBUG_TRACE_       1
#define     _LOG_DEBUG_ERROR_       1

#define		_LOG_DEBUG_OUT_FILE		1	

#define		_LOG_DEBUG_DIR_			"/home/liwanyuan/source/ret"

#define		_LOG_DEBUG_OVERDUE		0			//10day
//config end
//
static const long	g_sec_of_day = 86400;	//1day == 86400s;

//CLogDir
class CLogDir
{
public:
	CLogDir(std::string strDir, int storageDay = 1);
	~CLogDir();

	bool			ClearOverdueFiles();		

protected:
	bool			_InitDir();	
    bool            _InitDirWin32();
    bool            _InitDirLinux();
	time_t			_SpiltUnixStamp(const std::string& filename);

private:
	std::string		_strLogDiretory;
	int				_storageDay;

	std::map<time_t, std::string>		_map_files_cache;
	std::vector<time_t>					_vec_files_unix_stamp;
};


#endif
