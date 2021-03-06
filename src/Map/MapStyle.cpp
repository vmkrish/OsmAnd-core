#include "MapStyle.h"
#include "MapStyle_P.h"

#include <OsmAndCore/QtExtensions.h>
#include <QFileInfo>
#include <QAtomicInt>
#include <QMutex>

#include "MapStyleRule.h"
#include "QKeyValueIterator.h"
#include "Logging.h"

OsmAnd::MapStyle::MapStyle( MapStyles* styles_, const QString& resourcePath_, const bool isEmbedded_ )
    : _p(new MapStyle_P(this))
    , styles(styles_)
    , resourcePath(resourcePath_)
    , isEmbedded(isEmbedded_)
    , title(_p->_title)
    , name(_p->_name)
    , parentName(_p->_parentName)
{
    _p->_name = QFileInfo(resourcePath).fileName().replace(QLatin1String(".render.xml"), QLatin1String(""));
}

OsmAnd::MapStyle::~MapStyle()
{
}

bool OsmAnd::MapStyle::isStandalone() const
{
    return parentName.isEmpty();
}

bool OsmAnd::MapStyle::resolveValueDefinition( const QString& name, std::shared_ptr<const MapStyleValueDefinition>& outDefinition ) const
{
    auto itValueDefinition = _p->_valuesDefinitions.constFind(name);
    if(itValueDefinition != _p->_valuesDefinitions.cend())
    {
        outDefinition = *itValueDefinition;
        return true;
    }

    if(!_p->_parent)
        return false;

    return _p->_parent->resolveValueDefinition(name, outDefinition);
}

bool OsmAnd::MapStyle::resolveAttribute( const QString& name, std::shared_ptr<const MapStyleRule>& outAttribute ) const
{
    auto itAttribute = _p->_attributes.constFind(name);
    if(itAttribute != _p->_attributes.cend())
    {
        outAttribute = *itAttribute;
        return true;
    }

    return false;
}

void OsmAnd::MapStyle::dump( const QString& prefix /*= QString()*/ ) const
{
    LogPrintf(LogSeverityLevel::Debug, "%sPoint rules:", qPrintable(prefix));
    dump(MapStyleRulesetType::Point, prefix);

    LogPrintf(LogSeverityLevel::Debug, "%sLine rules:", qPrintable(prefix));
    dump(MapStyleRulesetType::Polyline, prefix);

    LogPrintf(LogSeverityLevel::Debug, "%sPolygon rules:", qPrintable(prefix));
    dump(MapStyleRulesetType::Polygon, prefix);

    LogPrintf(LogSeverityLevel::Debug, "%sText rules:", qPrintable(prefix));
    dump(MapStyleRulesetType::Text, prefix);

    LogPrintf(LogSeverityLevel::Debug, "%sOrder rules:", qPrintable(prefix));
    dump(MapStyleRulesetType::Order, prefix);
}

void OsmAnd::MapStyle::dump( MapStyleRulesetType type, const QString& prefix /*= QString()*/ ) const
{
    const auto& rules = _p->obtainRulesRef(type);

    for(const auto& ruleEntry : rangeOf(constOf(rules)))
    {
        auto tag = _p->getTagString(ruleEntry.key());
        auto value = _p->getValueString(ruleEntry.key());
        auto rule = ruleEntry.value();

        LogPrintf(LogSeverityLevel::Debug, "%sRule 0x%p [%s (%d):%s (%d)]",
            qPrintable(prefix),
            rule.get(),
            qPrintable(tag),
            _p->getTagStringId(ruleEntry.key()),
            qPrintable(value),
            _p->getValueStringId(ruleEntry.key()));
        rule->dump(prefix);
    }
}

static QMutex g_OsmAnd_MapStyle_builtinValueDefinitionsMutex;
static std::shared_ptr<const OsmAnd::MapStyleBuiltinValueDefinitions> g_OsmAnd_MapStyle_builtinValueDefinitions;

std::shared_ptr<const OsmAnd::MapStyleBuiltinValueDefinitions> OsmAnd::MapStyle::getBuiltinValueDefinitions()
{
    QMutexLocker scopedLocker(&g_OsmAnd_MapStyle_builtinValueDefinitionsMutex);

    if(!static_cast<bool>(g_OsmAnd_MapStyle_builtinValueDefinitions))
        g_OsmAnd_MapStyle_builtinValueDefinitions.reset(new OsmAnd::MapStyleBuiltinValueDefinitions());

    return g_OsmAnd_MapStyle_builtinValueDefinitions;
}
