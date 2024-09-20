#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <QObject>
#include "httpserver/httprequesthandler.h"
#include <QSettings>
#include "httpserver/httplistener.h"	//新增代码
#include "httpserver/httprequesthandler.h"	//新增代码
#include "httpserver/staticfilecontroller.h"
#include "httpserver.h"
#include <QFile>
#include <QFileInfo>

class HttpServer : public stefanfrings::HttpRequestHandler
{
    Q_OBJECT
public:
    explicit HttpServer(QObject *parent = nullptr);
    void service(stefanfrings::HttpRequest& request, stefanfrings::HttpResponse& response);

    /** Encoding of text files */
    QString encoding;

    /** Root directory of documents */
    QString docroot;

    /** Maximum age of files in the browser cache */
    int maxAge;

    struct CacheEntry {
        QByteArray document;
        qint64 created;
        QByteArray filename;
    };

    /** Timeout for each cached file */
    int cacheTimeout;

    /** Maximum size of files in cache, larger files are not cached */
    int maxCachedFileSize;

    /** Cache storage */
    QCache<QString,CacheEntry> cache;

    /** Used to synchronize cache access for threads */
    QMutex mutex;
};

#endif // HTTPSERVER_H
