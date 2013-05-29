/*
  Q Light Controller Plus
  audiotriggerwidget.cpp

  Copyright (c) Massimo Callegari

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  Version 2 as published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details. The license is
  in the file "COPYING".

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include <QPainter>
#include <QDebug>
#include <qmath.h>

#include "audiotriggerwidget.h"

AudioTriggerWidget::AudioTriggerWidget(QWidget *parent) :
    QWidget(parent)
  , m_spectrumBands(NULL)
  , m_volumeBarHeight(0)
  , m_barsNumber(0)
  , m_maxFrequency(0)
{
}

void AudioTriggerWidget::setBarsNumber(int num)
{
    m_barsNumber = num;
    if (m_spectrumBands)
        delete m_spectrumBands;
    m_spectrumBands = new double[m_barsNumber];
    for (int i = 0; i < m_barsNumber; i++)
        m_spectrumBands[i] = 0;
    m_volumeBarHeight = 0;
    m_barWidth = (width() - 10) / (m_barsNumber + 1);
    update();
}

void AudioTriggerWidget::setMaxFrequency(int freq)
{
    m_maxFrequency = freq;
}

void AudioTriggerWidget::displaySpectrum(double *spectrumData, double maxMagnitude, quint32 power)
{
    m_spectrumHeight = height() - 20;
    m_volumeBarHeight = (power * m_spectrumHeight) / 0x7FFF;
    for (int i = 0; i < m_barsNumber; i++)
        m_spectrumBands[i] =  (m_volumeBarHeight * spectrumData[i]) / maxMagnitude;

    qDebug() << "[displaySpectrum] power: " << power << ", first bar: " << m_spectrumBands[0];
    update();
}

void AudioTriggerWidget::resizeEvent(QResizeEvent *e)
{
    QWidget::resizeEvent(e);
    m_barWidth = (width() - 10) / (m_barsNumber + 1);
}

void AudioTriggerWidget::paintEvent(QPaintEvent *e)
{
    QWidget::paintEvent(e);

    if (m_barsNumber == 0)
        return;

    QPainter painter(this);

    // fill spectrum background
    painter.setPen(QPen(Qt::darkGray, 2));
    if (this->isEnabled())
        painter.setBrush(QBrush(Qt::black));
    else
        painter.setBrush(QBrush(Qt::gray));
    painter.drawRect(0, 0, m_barWidth * m_barsNumber, m_spectrumHeight);

    // fill volume bar background
    painter.setBrush(QBrush(Qt::lightGray));
    painter.drawRect(width() - m_barWidth, 0, m_barWidth, m_spectrumHeight);

    // fill frequencies background
    painter.setBrush(QBrush(Qt::darkGray));
    painter.drawRect(0, m_spectrumHeight + 1, width(), 20);

    float xpos = 1;
    painter.setBrush(QBrush(Qt::yellow));

    for (int i = 0; i < m_barsNumber; i++)
    {
        painter.setPen(QPen(Qt::NoPen));
        painter.drawRect(xpos, m_spectrumHeight - m_spectrumBands[i], m_barWidth - 1, m_spectrumBands[i]);
        painter.setPen(QPen(Qt::lightGray, 1));
        painter.drawLine(xpos + m_barWidth, 0, xpos + m_barWidth, m_spectrumHeight);

        xpos += m_barWidth;
    }

    // draw frequencies scale
    float freqIncr = m_maxFrequency / 10;
    painter.setPen(QPen(Qt::black, 1));

    for (int i = 1; i < 11; i++)
    {
        float xpos = ((m_barWidth * m_barsNumber) / 10 * i);
        painter.drawText(xpos - 50, height() - 5, QString("%1Hz").arg(freqIncr * i));
        painter.drawLine(xpos - 2, m_spectrumHeight + 1, xpos - 2, height());
    }
    //painter.drawText(width() - 15, height() - 5, "V");

    painter.setPen(QPen(Qt::NoPen));
    painter.setBrush(QBrush(Qt::green));
    painter.drawRect(width() - m_barWidth + 1, m_spectrumHeight - m_volumeBarHeight, m_barWidth - 2, m_volumeBarHeight);
}
