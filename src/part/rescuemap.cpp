#include "rescuemap.h"

#include <QStandardItemModel>
#include <QDebug>

BlockData::BlockData(long long position, long long size, QString status) 
{
    if( position < 0 ) {
        m_position = 0;
        qDebug() << "Error: negative position in block data";
    }
    if( size < 0 ) {
        m_size = 0;
        qDebug() << "Error: negative size in block data";
    }
    m_position = position;
    m_size = size;
    if( status == StatusCharacter[Status::nontried] ) {
        m_status = Status::nontried;
    } else if( status == StatusCharacter[Status::nontrimmed] ) {
        m_status = Status::nontrimmed;
    } else if( status == StatusCharacter[Status::nonscraped] ) {
        m_status = Status::nonscraped;
    } else if( status == StatusCharacter[Status::badsector] ) {
        m_status = Status::badsector;
    } else if( status == StatusCharacter[Status::rescued] ) {
        m_status = Status::rescued;
    } else {
        m_status = Status::unknown;
        qDebug() << "Error: unknown status in block data";
    }
}
    
RescueMap::RescueMap(QObject *parent)
    :QStandardItemModel(parent)
{

}

void RescueMap::addRescueBlock(long long position, long long size, QString operation)
{
    m_rescue_map.append(BlockData(position, size, operation));
}

long long RescueMap::lastPosition() const
{
    if( m_rescue_map.isEmpty() ) {
        return 0;
    }
    return m_rescue_map.last().position();
}

long long RescueMap::lastSize() const
{ 
    if( m_rescue_map.isEmpty() ) {
        return 0;
    }
    return m_rescue_map.last().size(); 
}

void RescueMap::print() const
{
    for (int i = 0; i < m_rescue_map.size(); ++i) {
        qDebug() << m_rescue_map.at(i).position() << m_rescue_map.at(i).size() << StatusCharacter[m_rescue_map.at(i).status()];
    }
}
