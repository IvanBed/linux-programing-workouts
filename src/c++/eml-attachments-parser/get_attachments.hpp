#ifndef GET_ATTACHMENTS_HPP
#define GET_ATTACHMENTS_HPP

    #include <iostream>
    #include <fstream>
    #include <sstream> 
    #include <string>
    #include <regex>
    #include <cctype>
    #include <iomanip>
    #include <cerrno>

    #include "base64.hpp"
    #include "base16.hpp"

    #define BOUNDARY_PATTERN "boundary=\"*"

    // Header keys
    #define ATTACH_PATTERN   "Content-Disposition: attachment*"
	// Content-Disposition params
	#define FILENAME_PATTERN "filename*"
	#define FILESIZE_PATTERN "size=*"
	
	#define FILECD_PATTERN "creation-date=*"
	#define FILEMD_PATTERN "modification-date=*"
	#define FILERD_PATTERN "read-date=*"
	
	#define ENCODING_PATTERN "Content-Transfer-Encoding:*"
    #define ID_PATTERN "Content-ID:*"
    #define TYPE_PATTERN "Content-Type:*"
    #define DESCRIPTION_PATTERN "Content-Description:*"
    #define RFC2047HINT "=\?.*\?="
    #define RFC2231HINT ".*'.*'.*"

    #define RFC2047     "=\?([^?]+)\?([QBqb])\?([^?]+)\?="
    #define RFC2231     ""

    // TO DO 
    // Добавить:
    // Проверку charset
    // Сбор информации по кодировкам
    // Предкомпиляцию регулярных выражений

    enum encode
    {
        UNKOWN_ENCODE = 0,
        RFC2047_ENCODE,
        RFC2231_ENCODE
    };

    typedef enum encode encode_result; 

#endif GET_ATTACHMENTS_HPP