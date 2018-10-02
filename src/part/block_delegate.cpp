#include "block_delegate.h"
#include "square_color.h"
#include <QPainter>
#include <QDebug>

BlockDelegate::BlockDelegate(QObject *parent)
    : QItemDelegate(parent)
    , m_square_size(8)
{
}
/*
void BlockDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (option.state & QStyle::State_Selected)
        painter->fillRect(option.rect, option.palette.highlight());
    int size = qMin(option.rect.width(), option.rect.height());
    int brightness = index.model()->data(index, Qt::DisplayRole).value<SquareColor>().blue();
    double radius = (size / 2.0) - (brightness / 255.0 * size / 2.0);
    if (radius == 0.0)
        return;
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setPen(Qt::NoPen);
    if (option.state & QStyle::State_Selected)
        painter->setBrush(option.palette.highlightedText());
    else
        painter->setBrush(option.palette.text());
    painter->drawEllipse(QRectF(option.rect.x() + option.rect.width() / 2 - radius,
                                option.rect.y() + option.rect.height() / 2 - radius,
                                2 * radius, 2 * radius));
    painter->restore();
}
*/


QSize BlockDelegate::sizeHint(const QStyleOptionViewItem & /* option */, const QModelIndex & /* index */) const
{
    return QSize(m_square_size, m_square_size);
}

void BlockDelegate::setSquareSize(int size)
{
    m_square_size = size;
}
