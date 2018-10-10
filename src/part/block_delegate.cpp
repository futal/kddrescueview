#include "block_delegate.h"
#include "square_color.h"
#include <QPainter>
#include <QDebug>

BlockDelegate::BlockDelegate(QObject *parent)
    : QItemDelegate(parent)
{
}

QSize BlockDelegate::sizeHint(const QStyleOptionViewItem & /* option */, const QModelIndex & /* index */) const
{
    return QSize(8, 8);
}
