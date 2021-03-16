#ifndef __REQUEST_H__

void request_serve_dynamic(int fd, char *filename, char *cgiargs);
void request_serve_static(int fd, char *filename, int filesize);
void request_handle(int fd);

#endif // __REQUEST_H__
