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
#ifndef METER_H
#define METER_H

#include <queue>
#include <map>
#include <vector>
#include <QVariant>
#include "container/circular.h"
#include "weighting.h"
class Meter
{
public:
    enum Time {
        Fast = 0,
        Slow = 1
    };

    Meter(unsigned long size = DEFAULT_SIZE);
    Meter(Weighting w, Time time);
    static const unsigned long DEFAULT_SIZE = 100;

    void  add(const float &data) noexcept;
    float value() const noexcept;   //! mean squared value
    float dB() const noexcept;
    float peakSquared() const noexcept;
    float peakdB() const noexcept;
    void  reset() noexcept;

    void setSampleRate(unsigned int sampleRate);
    static constexpr Time allTimes[] = {Fast, Slow};

    static QVariant availableTimes();
    static QString timeName(Time time);
    static Time timeByName(QString name);

private:
    container::circular<float> m_data;
    Weighting m_weighting;
    Time m_time;
    unsigned long m_size;
    float m_integrator, m_peak;

    static const std::map<Time, QString> m_timeMap;
};

#endif // METER_H
