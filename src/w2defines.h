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


/* variadic arguments, for reference.
#if defined(WIN32) && defined(UNICODE)
#define W2X_FOPEN(...) _wfopen(##__VA_ARGS__)
#define W2X_CHAR WCHAR
#define W2X_L(a) L ## a
#define W2X_STRLEN(...) wcslen(##__VA_ARGS__)
#define W2X_IMWRITE(...) write_imageW(##__VA_ARGS__)
#define W2X_STRING std::wstring
#define W2X_STRING_METHOD wstring
#define W2X_STRCMP(...) wcscmp(##__VA_ARGS__)
#else
#define W2X_FOPEN(...) fopen(##__VA_ARGS__)
#define W2X_CHAR char
#define W2X_L(a) a
#define W2X_STRLEN(...) strlen(##__VA_ARGS__)
#define W2X_IMWRITE(...) cv::imwrite(##__VA_ARGS__)
#define W2X_STRING std::string
#define W2X_STRING_METHOD string
#define W2X_STRCMP(...) strcmp(##__VA_ARGS__)
#endif
*/
