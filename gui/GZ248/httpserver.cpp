#include "httpserver.h"
#include <QDir>

HttpServer::HttpServer(QObject *parent)
    : HttpRequestHandler{parent}
{

    QString configFileName=":/new/prefix1/webapp.ini";
    QSettings* listenerSettings=new QSettings(configFileName, QSettings::IniFormat, this);
    listenerSettings->beginGroup("listener");	//新增代码

    new stefanfrings::HttpListener(listenerSettings, this, this);	//新增代码

    QString currentPath = QDir::currentPath();
    docroot = currentPath;
}

void HttpServer::service(stefanfrings::HttpRequest &request, stefanfrings::HttpResponse &response)
{
    QByteArray path=request.getPath();
    // Check if we have the file in cache
    qint64 now=QDateTime::currentMSecsSinceEpoch();
    mutex.lock();
    CacheEntry* entry=cache.object(path);
    if (entry && (cacheTimeout==0 || entry->created>now-cacheTimeout))
    {
        QByteArray document=entry->document; //copy the cached document, because other threads may destroy the cached entry immediately after mutex unlock.
        QByteArray filename=entry->filename;
        mutex.unlock();
        qDebug("StaticFileController: Cache hit for %s",path.data());
        response.setHeader("Content-Type", "application/x-zip-compressed");
        response.setHeader("Cache-Control","max-age="+QByteArray::number(maxAge/1000));
        response.write(document,true);
    }
    else
    {
        mutex.unlock();
        // The file is not in cache.
        qDebug("StaticFileController: Cache miss for %s",path.data());
        // Forbid access to files outside the docroot directory
        if (path.contains("/.."))
        {
            qWarning("StaticFileController: detected forbidden characters in path %s",path.data());
            response.setStatus(403,"forbidden");
            response.write("403 forbidden",true);
            return;
        }
        // If the filename is a directory, append index.html.
        if (QFileInfo(docroot+path).isDir())
        {
            response.setStatus(404,"not found");
            response.write("404 not found",true);
            return;
        }
        // Try to open the file
        QFile file(docroot+path);
        qDebug("StaticFileController: Open file %s",qPrintable(file.fileName()));
        if (file.open(QIODevice::ReadOnly))
        {
            response.setHeader("Content-Type", "application/x-zip-compressed");
            response.setHeader("Cache-Control","max-age="+QByteArray::number(maxAge/1000));
            response.setHeader("Content-Length",QByteArray::number(file.size()));
            if (file.size()<=maxCachedFileSize)
            {
                // Return the file content and store it also in the cache
                entry=new CacheEntry();
                while (!file.atEnd() && !file.error())
                {
                    QByteArray buffer=file.read(65536);
                    response.write(buffer);
                    entry->document.append(buffer);
                }
                entry->created=now;
                entry->filename=path;
                mutex.lock();
                cache.insert(request.getPath(),entry,entry->document.size());
                mutex.unlock();
            }
            else
            {
                // Return the file content, do not store in cache
                while (!file.atEnd() && !file.error())
                {
                    response.write(file.read(65536));
                }
            }
            file.close();
        }
        else {
            if (file.exists())
            {
                qWarning("StaticFileController: Cannot open existing file %s for reading",qPrintable(file.fileName()));
                response.setStatus(403,"forbidden");
                response.write("403 forbidden",true);
            }
            else
            {
                response.setStatus(404,"not found");
                response.write("404 not found",true);
            }
        }
    }

}

