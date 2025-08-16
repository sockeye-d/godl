#ifndef DOWNLOADMANAGERMODEL_H
#define DOWNLOADMANAGERMODEL_H

#include "abstractsimplelistmodel.h"
#include "downloadinfo.h"
#include <QStandardItemModel>
#include <QUrl>
#include <QtQml/qqmlregistration.h>

class DownloadManagerModel : public AbstractSimpleListModel<DownloadInfo *>
{
    Q_OBJECT

    friend class DownloadManager;

public:
    explicit DownloadManagerModel(QObject *parent = nullptr)
        : AbstractSimpleListModel<DownloadInfo *>(parent)
    {}
};

#endif // DOWNLOADMANAGERMODEL_H
