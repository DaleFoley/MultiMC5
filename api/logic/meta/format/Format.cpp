/* Copyright 2015-2017 MultiMC Contributors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "Format.h"

#include "minecraft/onesix/OneSixVersionFormat.h""

#include "meta/Index.h"
#include "meta/Version.h"
#include "meta/VersionList.h"

#include "Json.h"
using namespace Json;

namespace Meta
{

static const int currentFormatVersion = 0;

// Index
static BaseEntity::Ptr parseIndexInternal(const QJsonObject &obj)
{
	const QVector<QJsonObject> objects = requireIsArrayOf<QJsonObject>(obj, "packages");
	QVector<VersionListPtr> lists;
	lists.reserve(objects.size());
	std::transform(objects.begin(), objects.end(), std::back_inserter(lists), [](const QJsonObject &obj)
	{
		VersionListPtr list = std::make_shared<VersionList>(requireString(obj, "uid"));
		list->setName(ensureString(obj, "name", QString()));
		return list;
	});
	return std::make_shared<Index>(lists);
}

// Version
static VersionPtr parseCommonVersion(const QString &uid, const QJsonObject &obj)
{
	const QVector<QJsonObject> requiresRaw = obj.contains("requires") ? requireIsArrayOf<QJsonObject>(obj, "requires") : QVector<QJsonObject>();
	QVector<Reference> requires;
	requires.reserve(requiresRaw.size());
	std::transform(requiresRaw.begin(), requiresRaw.end(), std::back_inserter(requires), [](const QJsonObject &rObj)
	{
		Reference ref(requireString(rObj, "uid"));
		ref.setVersion(ensureString(rObj, "version", QString()));
		return ref;
	});

	VersionPtr version = std::make_shared<Version>(uid, requireString(obj, "version"));
	version->setTime(QDateTime::fromString(requireString(obj, "releaseTime"), Qt::ISODate).toMSecsSinceEpoch() / 1000);
	version->setType(ensureString(obj, "type", QString()));
	version->setRequires(requires);
	return version;
}

static BaseEntity::Ptr parseVersionInternal(const QJsonObject &obj)
{
	VersionPtr version = parseCommonVersion(requireString(obj, "uid"), obj);

	version->setData(OneSixVersionFormat::versionFileFromJson(QJsonDocument(obj),
										   QString("%1/%2.json").arg(version->uid(), version->version()),
										   obj.contains("order")));
	return version;
}

// Version list / package
static BaseEntity::Ptr parseVersionListInternal(const QJsonObject &obj)
{
	const QString uid = requireString(obj, "uid");

	const QVector<QJsonObject> versionsRaw = requireIsArrayOf<QJsonObject>(obj, "versions");
	QVector<VersionPtr> versions;
	versions.reserve(versionsRaw.size());
	std::transform(versionsRaw.begin(), versionsRaw.end(), std::back_inserter(versions), [uid](const QJsonObject &vObj)
	{
		return parseCommonVersion(uid, vObj);
	});

	VersionListPtr list = std::make_shared<VersionList>(uid);
	list->setName(ensureString(obj, "name", QString()));
	list->setVersions(versions);
	return list;
}


static int formatVersion(const QJsonObject &obj)
{
	if (!obj.contains("formatVersion")) {
		throw ParseException(QObject::tr("Missing required field: 'formatVersion'"));
	}
	if (!obj.value("formatVersion").isDouble()) {
		throw ParseException(QObject::tr("Required field has invalid type: 'formatVersion'"));
	}
	return obj.value("formatVersion").toInt();
}

void parseIndex(const QJsonObject &obj, Index *ptr)
{
	const int version = formatVersion(obj);
	switch (version) {
	case 0:
		ptr->merge(parseIndexInternal(obj));
		break;
	default:
		throw ParseException(QObject::tr("Unknown formatVersion: %1").arg(version));
	}
}

void parseVersionList(const QJsonObject &obj, VersionList *ptr)
{
	const int version = formatVersion(obj);
	switch (version) {
	case 0:
		ptr->merge(parseVersionListInternal(obj));
		break;
	default:
		throw ParseException(QObject::tr("Unknown formatVersion: %1").arg(version));
	}
}

void parseVersion(const QJsonObject &obj, Version *ptr)
{
	const int version = formatVersion(obj);
	switch (version) {
	case 0:
		ptr->merge(parseVersionInternal(obj));
		break;
	default:
		throw ParseException(QObject::tr("Unknown formatVersion: %1").arg(version));
	}
}
}
