/**

 *  OSM
 *  Copyright (C) 2018  Pavel Smokotnin

 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.

 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.

 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <cmath>
#include "meter.h"
#include <QtGlobal>

Meter::Meter(unsigned long size) :
    m_data(size), m_weighting(Weighting::Z), m_time(Fast),
    m_size(0),
    m_integrator(0.f),
    m_peak(0.f)
{
}

Meter::Meter(Weighting w, Time time) :
    m_data(DEFAULT_SIZE), m_weighting(w), m_time(time),
    m_size(0),
    m_integrator(0.f),
    m_peak(0.f)
{

}
void Meter::add(const float &data) noexcept
{
    float d = std::pow(m_weighting(data), 2);
    if (std::isnan(d)) {
        d = 0;
    }
    auto p = m_data.replace(d);
    m_integrator -= p;
    m_integrator += d;

    if (qFuzzyCompare(m_peak, p)) {
        m_peak = d;
    }
    m_peak = std::max(m_peak, d);

    if (m_integrator < 0.f)
        m_integrator = 0.f;

    if (m_size < m_data.size()) {
        ++m_size;
    }
}
float Meter::value() const noexcept
{
    if (m_size == 0)
        return std::numeric_limits<float>::min();

    return m_integrator / m_size;
}
float Meter::dB() const noexcept
{
    // 20log(sqrt(v)) = 10log(v)
    return 10.f * std::log10(value());
}

float Meter::peakSquared() const noexcept
{
    return m_peak;
}

float Meter::peakdB() const noexcept
{
    return 10.f * std::log10(m_peak);;
}

void Meter::reset() noexcept
{
    m_size = 0;
    m_integrator = 0.f;
    m_peak = 0.f;
    m_data.clear();
}

void Meter::setSampleRate(unsigned int sampleRate)
{
    switch (m_time) {
    case Fast:
        m_data.resize(0.125 * sampleRate);
        break;
    case Slow:
        m_data.resize(1 * sampleRate);
        break;
    }

    m_weighting.setSampleRate(sampleRate);
    reset();
}

const std::map<Meter::Time, QString>Meter::m_timeMap = {
    {Meter::Fast,   "Fast"},
    {Meter::Slow,   "Slow"}
};

QVariant Meter::availableTimes()
{
    QStringList typeList;
    for (const auto &type : m_timeMap) {
        typeList << type.second;
    }
    return typeList;
}

QString Meter::timeName(Meter::Time time)
{
    return m_timeMap.at(time);
}

Meter::Time Meter::timeByName(QString name)
{
    auto it = std::find_if(m_timeMap.begin(), m_timeMap.end(), [name](const auto & el) {
        return el.second == name;
    });
    if (it != m_timeMap.end()) {
        return it->first;
    }
    Q_ASSERT(false);
    return Time::Fast;
}
