#include "OnlineTileSources_P.h"
#include "OnlineTileSources.h"

#include "OnlineMapRasterTileProvider.h"
#include "Logging.h"

OsmAnd::OnlineTileSources_P::OnlineTileSources_P(OnlineTileSources* owner_)
    : owner(owner_)
{
}

OsmAnd::OnlineTileSources_P::~OnlineTileSources_P()
{
}

bool OsmAnd::OnlineTileSources_P::deserializeFrom(QXmlStreamReader& xmlReader)
{
    QHash< QString, std::shared_ptr<Source> > collection;

    while(!xmlReader.atEnd() && !xmlReader.hasError())
    {
        xmlReader.readNext();
        if(!xmlReader.isStartElement())
            continue;
        const auto tagName = xmlReader.name();
        if(tagName == QLatin1String("tile_source"))
        {
            // Original format of the tile sources, used in the Android application
            if(xmlReader.attributes().hasAttribute("rule"))
                continue; // Rules are not supported

            const auto name = xmlReader.attributes().value(QLatin1String("name")).toString();
            if(collection.contains(name))
            {
                LogPrintf(LogSeverityLevel::Warning, "Ignored duplicate online tile source with name '%s'", qPrintable(name));
                continue;
            }
            const auto originalUrlTemplate = xmlReader.attributes().value(QLatin1String("url_template")).toString();
            auto urlPattern = originalUrlTemplate;
            urlPattern = urlPattern.replace(QLatin1String("{0}"), QLatin1String("${zoom}"));
            urlPattern = urlPattern.replace(QLatin1String("{1}"), QLatin1String("${osm_x}"));
            urlPattern = urlPattern.replace(QLatin1String("{2}"), QLatin1String("${osm_y}"));
            const auto minZoom = static_cast<ZoomLevel>(xmlReader.attributes().value(QLatin1String("min_zoom")).toUInt());
            const auto maxZoom = static_cast<ZoomLevel>(xmlReader.attributes().value(QLatin1String("max_zoom")).toUInt());
            const auto tileSize = xmlReader.attributes().value(QLatin1String("tile_size")).toUInt();
            
            std::shared_ptr<Source> newSource(new Source(name));
            newSource->urlPattern = urlPattern;
            newSource->minZoom = minZoom;
            newSource->maxZoom = maxZoom;
            newSource->maxConcurrentDownloads = 1;
            newSource->tileSize = tileSize;
            newSource->alphaChannelData = AlphaChannelData::Undefined;
            collection.insert(name, newSource);
        }
        else if(tagName == QLatin1String("tileSource"))
        {
            //TODO: parse new format, but create it first :)
        }
    }
    if(xmlReader.hasError())
    {
        LogPrintf(LogSeverityLevel::Warning, "XML error: %s (%d, %d)", qPrintable(xmlReader.errorString()), xmlReader.lineNumber(), xmlReader.columnNumber());
        return false;
    }

    _collection = collection;

    return true;
}

bool OsmAnd::OnlineTileSources_P::serializeTo(QXmlStreamWriter& xmlWriter) const
{
    assert(false);
    return false;
}

bool OsmAnd::OnlineTileSources_P::loadFrom(const QByteArray& content)
{
    QXmlStreamReader xmlReader(content);
    return deserializeFrom(xmlReader);
}

bool OsmAnd::OnlineTileSources_P::loadFrom(QIODevice& ioDevice)
{
    QXmlStreamReader xmlReader(&ioDevice);
    return deserializeFrom(xmlReader);
}

bool OsmAnd::OnlineTileSources_P::saveTo(QIODevice& ioDevice) const
{
    QXmlStreamWriter xmlWriter(&ioDevice);
    return serializeTo(xmlWriter);
}

QList< std::shared_ptr<OsmAnd::OnlineTileSources_P::Source> > OsmAnd::OnlineTileSources_P::getCollection() const
{
    return _collection.values();
}

std::shared_ptr<OsmAnd::OnlineTileSources_P::Source> OsmAnd::OnlineTileSources_P::getSourceByName(const QString& sourceName) const
{
    const auto citSource = _collection.constFind(sourceName);
    if(citSource == _collection.cend())
        return nullptr;
    return *citSource;
}

bool OsmAnd::OnlineTileSources_P::addSource(const std::shared_ptr<Source>& source)
{
    if(_collection.constFind(source->name) != _collection.cend())
        return false;

    _collection.insert(source->name, source);

    return true;
}

bool OsmAnd::OnlineTileSources_P::removeSource(const QString& sourceName)
{
    return (_collection.remove(sourceName) > 0);
}

std::shared_ptr<OsmAnd::OnlineMapRasterTileProvider> OsmAnd::OnlineTileSources_P::createProviderFor(const QString& sourceName) const
{
    const auto citSource = _collection.constFind(sourceName);
    if(citSource == _collection.cend())
        return nullptr;

    const auto& source = *citSource;
    return std::shared_ptr<OsmAnd::OnlineMapRasterTileProvider>(new OnlineMapRasterTileProvider(
        source->name,
        source->urlPattern,
        source->minZoom,
        source->maxZoom,
        source->maxConcurrentDownloads,
        source->tileSize,
        source->alphaChannelData));
}

std::shared_ptr<OsmAnd::OnlineTileSources> OsmAnd::OnlineTileSources_P::_builtIn;

std::shared_ptr<const OsmAnd::OnlineTileSources> OsmAnd::OnlineTileSources_P::getBuiltIn()
{
    static QMutex mutex;
    QMutexLocker scopedLocker(&mutex);

    if(!_builtIn)
    {
        _builtIn.reset(new OnlineTileSources());

        std::shared_ptr<Source> mapnikOsmAndSource(new Source(QLatin1String("Mapnik (OsmAnd)")));
        mapnikOsmAndSource->urlPattern = QLatin1String("http://mapnik.osmand.net/${zoom}/${x}/${y}.png");
        mapnikOsmAndSource->minZoom = ZoomLevel0;
        mapnikOsmAndSource->maxZoom = ZoomLevel19;
        mapnikOsmAndSource->maxConcurrentDownloads = 0;
        mapnikOsmAndSource->tileSize = 256;
        mapnikOsmAndSource->alphaChannelData = AlphaChannelData::NotPresent;
        _builtIn->addSource(mapnikOsmAndSource);
    }

    return _builtIn;
}
