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

#include "Version.h"

#include <QDateTime>

#include "JsonFormat.h"
#include "minecraft/MinecraftProfile.h"

void Meta::Version::applyTo(MinecraftProfile* profile)
{
	if(m_data)
	{
		m_data->applyTo(profile);
	}
}

Meta::Version::Version(const QString &uid, const QString &version)
	: BaseVersion(), m_uid(uid), m_version(version)
{
}

QString Meta::Version::descriptor()
{
	return m_version;
}
QString Meta::Version::name()
{
	return m_version;
}
QString Meta::Version::typeString() const
{
	return m_type;
}

QDateTime Meta::Version::time() const
{
	return QDateTime::fromMSecsSinceEpoch(m_time * 1000, Qt::UTC);
}

void Meta::Version::parse(const QJsonObject& obj)
{
	parseVersion(obj, this);
}

void Meta::Version::merge(const std::shared_ptr<BaseEntity> &other)
{
	VersionPtr version = std::dynamic_pointer_cast<Version>(other);
	if (m_type != version->m_type)
	{
		setType(version->m_type);
	}
	if (m_time != version->m_time)
	{
		setTime(version->m_time);
	}
	if (m_requires != version->m_requires)
	{
		setRequires(version->m_requires);
	}

	setData(version->m_data);
}

QString Meta::Version::localFilename() const
{
	return m_uid + '/' + m_version + ".json";
}

void Meta::Version::setType(const QString &type)
{
	m_type = type;
	emit typeChanged();
}
void Meta::Version::setTime(const qint64 time)
{
	m_time = time;
	emit timeChanged();
}
void Meta::Version::setRequires(const QVector<Reference> &requires)
{
	m_requires = requires;
	emit requiresChanged();
}
void Meta::Version::setData(const VersionFilePtr &data)
{
	m_data = data;
}

