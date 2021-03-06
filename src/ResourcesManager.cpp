#include "ResourcesManager.h"
#include "ResourcesManager_P.h"

#include <QStandardPaths>

#include "ObfInfo.h"

OsmAnd::ResourcesManager::ResourcesManager(
    const QString& localStoragePath_,
    const QList<QString>& extraStoragePaths_ /*= QList<QString>()*/,
    const QString& miniBasemapFilename_ /*= QString::null*/,
    const QString& localTemporaryPath_ /*= QString::null*/,
    const QString& repositoryBaseUrl_ /*= QLatin1String("http://download.osmand.net")*/)
    : _p(new ResourcesManager_P(this))
    , localStoragePath(localStoragePath_)
    , extraStoragePaths(extraStoragePaths_)
    , miniBasemapFilename(miniBasemapFilename_)
    , localTemporaryPath(!localTemporaryPath_.isNull() ? localTemporaryPath_ : QStandardPaths::writableLocation(QStandardPaths::TempLocation))
    , repositoryBaseUrl(repositoryBaseUrl_)
    , obfsCollection(_p->obfsCollection)
{
    _p->initialize();
    _p->inflateBuiltInResources();
    _p->rescanLocalStoragePaths();
    _p->loadRepositoryFromCache();
    _p->attachToFileSystem();
}

OsmAnd::ResourcesManager::~ResourcesManager()
{
    _p->detachFromFileSystem();
}

QList< std::shared_ptr<const OsmAnd::ResourcesManager::Resource> > OsmAnd::ResourcesManager::getBuiltInResources() const
{
    return _p->getBuiltInResources();
}

std::shared_ptr<const OsmAnd::ResourcesManager::Resource> OsmAnd::ResourcesManager::getBuiltInResource(const QString& id) const
{
    return _p->getBuiltInResource(id);
}

bool OsmAnd::ResourcesManager::isBuiltInResource(const QString& id) const
{
    return _p->isBuiltInResource(id);
}

bool OsmAnd::ResourcesManager::rescanLocalStoragePaths() const
{
    return _p->rescanLocalStoragePaths();
}

QList< std::shared_ptr<const OsmAnd::ResourcesManager::Resource> > OsmAnd::ResourcesManager::getLocalResources() const
{
    return _p->getLocalResources();
}

std::shared_ptr<const OsmAnd::ResourcesManager::Resource> OsmAnd::ResourcesManager::getLocalResource(const QString& id) const
{
    return _p->getLocalResource(id);
}

bool OsmAnd::ResourcesManager::isLocalResource(const QString& id) const
{
    return _p->isLocalResource(id);
}

bool OsmAnd::ResourcesManager::isRepositoryAvailable() const
{
    return _p->isRepositoryAvailable();
}

bool OsmAnd::ResourcesManager::updateRepository() const
{
    return _p->updateRepository();
}

QList< std::shared_ptr<const OsmAnd::ResourcesManager::Resource> > OsmAnd::ResourcesManager::getResourcesInRepository() const
{
    return _p->getResourcesInRepository();
}

std::shared_ptr<const OsmAnd::ResourcesManager::Resource> OsmAnd::ResourcesManager::getResourceInRepository(const QString& id) const
{
    return _p->getResourceInRepository(id);
}

bool OsmAnd::ResourcesManager::isResourceInRepository(const QString& id) const
{
    return _p->isResourceInRepository(id);
}

bool OsmAnd::ResourcesManager::isResourceInstalled(const QString& id) const
{
    return _p->isResourceInstalled(id);
}

bool OsmAnd::ResourcesManager::uninstallResource(const QString& id)
{
    return _p->uninstallResource(id);
}

bool OsmAnd::ResourcesManager::installFromFile(const QString& filePath, const ResourceType resourceType)
{
    return _p->installFromFile(filePath, resourceType);
}

bool OsmAnd::ResourcesManager::installFromFile(const QString& id, const QString& filePath, const ResourceType resourceType)
{
    return _p->installFromFile(id, filePath, resourceType);
}

bool OsmAnd::ResourcesManager::installFromRepository(const QString& id, const WebClient::RequestProgressCallbackSignature downloadProgressCallback /*= nullptr*/)
{
    return _p->installFromRepository(id, downloadProgressCallback);
}

bool OsmAnd::ResourcesManager::isInstalledResourceOutdated(const QString& id) const
{
    return _p->isInstalledResourceOutdated(id);
}

QList<QString> OsmAnd::ResourcesManager::getOutdatedInstalledResources() const
{
    return _p->getOutdatedInstalledResources();
}

bool OsmAnd::ResourcesManager::updateFromFile(const QString& filePath)
{
    return _p->updateFromFile(filePath);
}

bool OsmAnd::ResourcesManager::updateFromFile(const QString& id, const QString& filePath)
{
    return _p->updateFromFile(id, filePath);
}

bool OsmAnd::ResourcesManager::updateFromRepository(const QString& id, const WebClient::RequestProgressCallbackSignature downloadProgressCallback /*= nullptr*/)
{
    return _p->updateFromRepository(id, downloadProgressCallback);
}

OsmAnd::ResourcesManager::ResourceOrigin::ResourceOrigin(const ResourceOriginType type_)
    : type(type_)
{
}

OsmAnd::ResourcesManager::ResourceOrigin::~ResourceOrigin()
{
}

OsmAnd::ResourcesManager::LocalResourceOrigin::LocalResourceOrigin(
    const ResourceOriginType type_,
    const QString& localPath_,
    const uint64_t size_)
    : ResourceOrigin(type_)
    , localPath(localPath_)
    , size(size_)
{
}

OsmAnd::ResourcesManager::LocalResourceOrigin::~LocalResourceOrigin()
{
}

OsmAnd::ResourcesManager::UserResourceOrigin::UserResourceOrigin(
    const QString& localPath_,
    const uint64_t size_,
    const QString& name_)
    : LocalResourceOrigin(ResourceOriginType::User, localPath_, size_)
    , name(name_)
{
}

OsmAnd::ResourcesManager::UserResourceOrigin::~UserResourceOrigin()
{
}

OsmAnd::ResourcesManager::InstalledResourceOrigin::InstalledResourceOrigin(
    const QString& localPath_,
    const uint64_t size_,
    const uint64_t timestamp_)
    : LocalResourceOrigin(ResourceOriginType::Installed, localPath_, size_)
    , timestamp(timestamp_)
{
}

OsmAnd::ResourcesManager::InstalledResourceOrigin::~InstalledResourceOrigin()
{
}

OsmAnd::ResourcesManager::RepositoryResourceOrigin::RepositoryResourceOrigin(
    const QUrl& url_,
    const uint64_t size_,
    const uint64_t timestamp_,
    const uint64_t packageSize_)
    : ResourceOrigin(ResourceOriginType::Repository)
    , url(url_)
    , size(size_)
    , timestamp(timestamp_)
    , packageSize(packageSize_)
{
}

OsmAnd::ResourcesManager::RepositoryResourceOrigin::~RepositoryResourceOrigin()
{
}

OsmAnd::ResourcesManager::Resource::Resource(
    const QString& id_,
    const ResourceType type_,
    const std::shared_ptr<const ResourceOrigin>& origin_)
    : id(id_)
    , type(type_)
    , origin(origin_)
{
}

OsmAnd::ResourcesManager::Resource::~Resource()
{
}

OsmAnd::ResourcesManager::LocalObfResource::LocalObfResource(
    const QString& id_,
    const ResourceType type_,
    const std::shared_ptr<const ResourceOrigin>& origin_,
    const std::shared_ptr<const ObfFile>& obfFile_)
    : Resource(id_, type_, origin_)
    , obfFile(obfFile_)
{
}

OsmAnd::ResourcesManager::LocalObfResource::~LocalObfResource()
{
}

OsmAnd::ResourcesManager::MapStylesPresetsResource::MapStylesPresetsResource(
    const QString& id_,
    const std::shared_ptr<const ResourceOrigin>& origin_)
    : Resource(id_, ResourceType::MapStylePresets, origin_)
    , presets(nullptr)
{
}

OsmAnd::ResourcesManager::MapStylesPresetsResource::MapStylesPresetsResource(
    const QString& id_,
    const std::shared_ptr<const MapStylesPresets>& presets_)
    : Resource(id_, ResourceType::MapStylePresets, std::shared_ptr<ResourceOrigin>(new ResourceOrigin(ResourceOriginType::Builtin)))
    , presets(presets_)
{
}

OsmAnd::ResourcesManager::MapStylesPresetsResource::~MapStylesPresetsResource()
{
}

OsmAnd::ResourcesManager::OnlineTileSourcesResource::OnlineTileSourcesResource(
    const QString& id_,
    const std::shared_ptr<const ResourceOrigin>& origin_)
    : Resource(id_, ResourceType::OnlineTileSources, origin_)
    , sources(nullptr)
{
}

OsmAnd::ResourcesManager::OnlineTileSourcesResource::OnlineTileSourcesResource(
    const QString& id_,
    const std::shared_ptr<const OnlineTileSources>& sources_)
    : Resource(id_, ResourceType::OnlineTileSources, std::shared_ptr<ResourceOrigin>(new ResourceOrigin(ResourceOriginType::Builtin)))
    , sources(sources_)
{
}

OsmAnd::ResourcesManager::OnlineTileSourcesResource::~OnlineTileSourcesResource()
{
}
