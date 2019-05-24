#if defined(WIN32) && defined(UNICODE)
#define W2X_FOPEN(a, b) _wfopen(a, b);
#define W2X_CHAR WCHAR
#define W2X_L(a) L ## a
#define W2X_STRLEN(a) wcslen(a)
#define W2X_IMWRITE(a,b,c) write_imageW(a,b,c)
#define W2X_STRING std::wstring
#define W2X_STRING_METHOD wstring
#define W2X_STRCMP(a,b) wcscmp(a,b)
#else
#define W2X_FOPEN(a, b) fopen(a, b);
#define W2X_CHAR char
#define W2X_L(a) a
#define W2X_STRLEN(a) strlen(a)
#define W2X_IMWRITE(a,b,c) cv::imwrite(a,b,c)
#define W2X_STRING std::string
#define W2X_STRING_METHOD string
#define W2X_STRCMP(a,b) strcmp(a,b)
#endif