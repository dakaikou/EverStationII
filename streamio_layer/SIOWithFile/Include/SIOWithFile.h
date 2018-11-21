#ifndef _FILE_TRANSRECV_H_
#define _FILE_TRANSRECV_H_

#if defined(_WIN32) || defined(_WIN64)
#   ifdef _SIO_FILE_EXPORT
#       define SIO_FILE_LIB __declspec(dllexport)
#   elif defined(_SIO_FILE_IMPORT)
#       define SIO_FILE_LIB __declspec(dllimport)
#   else
#       define SIO_FILE_LIB
#   endif
#elif __GNUC__ >= 4
#   define SIO_FILE_LIB __attribute__((visibility("default")))
#else
#   define SIO_FILE_LIB
#endif

#include <stdint.h>

#define TSFILE_NO_ERROR				0x00000000
#define TSFILE_OPEN_ERROR			0xE2010001
#define TSFILE_CLOSE_ERROR			0xE2010002
#define TSFILE_READ_ERROR			0xE2010003
#define TSFILE_WRITE_ERROR			0xE2010004
#define TSFILE_EOF_ERROR			0xE2010005

#define TSFILE_UNKNOWN_ERROR		0xFFFFFFFF

SIO_FILE_LIB	int file_receive_init(char* pszFile);
SIO_FILE_LIB	int file_start_receive(void);
SIO_FILE_LIB	int file_receive(unsigned char *buffer, int count);
SIO_FILE_LIB	int file_stop_receive(void);
SIO_FILE_LIB	int file_receive_fini(void);

SIO_FILE_LIB int64_t file_attr_get_total_length(void);
SIO_FILE_LIB int64_t file_attr_tell(void);
SIO_FILE_LIB	int file_seek(int64_t offset);

#endif
