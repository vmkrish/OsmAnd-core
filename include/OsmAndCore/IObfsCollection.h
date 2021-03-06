#ifndef _OSMAND_CORE_I_OBFS_COLLECTION_H_
#define _OSMAND_CORE_I_OBFS_COLLECTION_H_

#include <OsmAndCore/stdlib_common.h>

#include <OsmAndCore/QtExtensions.h>
#include <QVector>

#include <OsmAndCore.h>
#include <OsmAndCore/Data/ObfFile.h>

namespace OsmAnd
{
    class ObfDataInterface;

    class OSMAND_CORE_API IObfsCollection
    {
        Q_DISABLE_COPY(IObfsCollection);
    public:

    private:
    protected:
        IObfsCollection();
    public:
        virtual ~IObfsCollection();

        virtual QVector< std::shared_ptr<const ObfFile> > getObfFiles() const = 0;
        virtual std::shared_ptr<ObfDataInterface> obtainDataInterface() const = 0;
    };
}

#endif // !defined(_OSMAND_CORE_MANUAL_OBFS_COLLECTION_H_)
