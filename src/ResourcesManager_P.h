#ifndef _OSMAND_CORE_RESOURCES_MANAGER_P_H_
#define _OSMAND_CORE_RESOURCES_MANAGER_P_H_

#include "stdlib_common.h"
#include <functional>

#include "QtExtensions.h"
#include <QList>
#include <QHash>
#include <QString>
#include <QReadWriteLock>
#include <QFileSystemWatcher>
#include <QXmlStreamReader>

#include "OsmAndCore.h"
#include "PrivateImplementation.h"
#include "WebClient.h"
#include "ResourcesManager.h"
#include "IObfsCollection.h"

namespace OsmAnd
{
    class ResourcesManager_P
    {
    public:
        typedef ResourcesManager::ResourceOriginType ResourceOriginType;
        typedef ResourcesManager::ResourceOrigin ResourceOrigin;
        typedef ResourcesManager::LocalResourceOrigin LocalResourceOrigin;
        typedef ResourcesManager::UserResourceOrigin UserResourceOrigin;
        typedef ResourcesManager::InstalledResourceOrigin InstalledResourceOrigin;
        typedef ResourcesManager::RepositoryResourceOrigin RepositoryResourceOrigin;
        typedef ResourcesManager::ResourceType ResourceType;
        typedef ResourcesManager::Resource Resource;
        typedef ResourcesManager::LocalObfResource LocalObfResource;
        typedef ResourcesManager::MapStylesPresetsResource MapStylesPresetsResource;
        typedef ResourcesManager::OnlineTileSourcesResource OnlineTileSourcesResource;

    private:
        QFileSystemWatcher* const _fileSystemWatcher;
        QMetaObject::Connection _onDirectoryChangedConnection;
        void onDirectoryChanged(const QString& path);
        QMetaObject::Connection _onFileChangedConnection;
        void onFileChanged(const QString& path);

        void attachToFileSystem();
        void detachFromFileSystem();

        QHash< QString, std::shared_ptr<const Resource> > _builtinResources;
        void inflateBuiltInResources();

        mutable QReadWriteLock _localResourcesLock;
        mutable QHash< QString, std::shared_ptr<const Resource> > _localResources;
        static bool rescanLocalStoragePath(const QString& storagePath, const bool isExtraStorage, QHash< QString, std::shared_ptr<const Resource> >& outResult);

        std::shared_ptr<const ObfFile> _miniBasemapObfFile;

        mutable QReadWriteLock _resourcesInRepositoryLock;
        mutable QHash< QString, std::shared_ptr<const Resource> > _resourcesInRepository;
        mutable bool _resourcesInRepositoryLoaded;
        bool parseRepository(QXmlStreamReader& xmlReader, QList< std::shared_ptr<const Resource> >& repository) const;

        mutable WebClient _webClient;

        bool uninstallMapRegion(const std::shared_ptr<const Resource>& resource);
        bool uninstallVoicePack(const std::shared_ptr<const Resource>& resource);

        bool installMapRegionFromFile(const QString& id, const QString& filePath);
        bool installVoicePackFromFile(const QString& id, const QString& filePath);

        class ObfsCollection : public IObfsCollection
        {
        private:
        protected:
            ObfsCollection(ResourcesManager_P* owner);
        public:
            virtual ~ObfsCollection();

            ResourcesManager_P* const owner;

            virtual QVector< std::shared_ptr<const ObfFile> > getObfFiles() const;
            virtual std::shared_ptr<ObfDataInterface> obtainDataInterface() const;

        friend class OsmAnd::ResourcesManager_P;
        };
    protected:
        ResourcesManager_P(ResourcesManager* owner);

        void initialize();
        void loadRepositoryFromCache();
    public:
        virtual ~ResourcesManager_P();

        ImplementationInterface<ResourcesManager> owner;

        // Built-in resources:
        QList< std::shared_ptr<const Resource> > getBuiltInResources() const;
        std::shared_ptr<const Resource> getBuiltInResource(const QString& id) const;
        bool isBuiltInResource(const QString& id) const;

        // Local resources:
        bool rescanLocalStoragePaths() const;
        QList< std::shared_ptr<const Resource> > getLocalResources() const;
        std::shared_ptr<const Resource> getLocalResource(const QString& id) const;
        bool isLocalResource(const QString& id) const;

        // Resources in repository:
        bool isRepositoryAvailable() const;
        bool updateRepository() const;
        QList< std::shared_ptr<const Resource> > getResourcesInRepository() const;
        std::shared_ptr<const Resource> getResourceInRepository(const QString& id) const;
        bool isResourceInRepository(const QString& id) const;

        // Install / Uninstall:
        bool isResourceInstalled(const QString& id) const;
        bool uninstallResource(const QString& id);
        bool installFromFile(const QString& filePath, const ResourceType resourceType);
        bool installFromFile(const QString& id, const QString& filePath, const ResourceType resourceType);
        bool installFromRepository(const QString& id, const WebClient::RequestProgressCallbackSignature downloadProgressCallback);

        // Updates:
        bool isInstalledResourceOutdated(const QString& id) const;
        QList<QString> getOutdatedInstalledResources() const;
        bool updateFromFile(const QString& filePath);
        bool updateFromFile(const QString& id, const QString& filePath);
        bool updateFromRepository(const QString& id, const WebClient::RequestProgressCallbackSignature downloadProgressCallback);

        // OBFs collection
        const std::shared_ptr<const IObfsCollection> obfsCollection;

    friend class OsmAnd::ResourcesManager;
    friend class OsmAnd::ResourcesManager_P::ObfsCollection;
    };
}

#endif // !defined(_OSMAND_CORE_RESOURCES_MANAGER_P_H_)
