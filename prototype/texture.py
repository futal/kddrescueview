#!/usr/bin/env python3

# Copyright 2020  Adrien Cordonnier <adrien.cordonnier@gmail.com>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 2 of
# the License or (at your option) version 3 or any later version
# accepted by the membership of KDE e.V. (or its successor approved
# by the membership of KDE e.V.), which shall act as a proxy
# defined in Section 14 of version 3 of the license.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.


# Prototype to create a texture of a ddrescue rescue map
# This first prototype only merges two lists of rescue blocks

class Block:
    def __init__(self, start, size, status):
        self.start = start
        self.size = size
        self.status = status

    @property
    def end(self):
        return self.start + self.size

    def __str__(self):
        return f"Block(start={self.start}, size={self.size}, status={self.status})"

    def __eq__(self, other):
        return str(self) == str(other)

def merge(list1, list2):
    # check that both lists have the same start and end
    if list1[0].start != list2[0].start:
        print('Error: the lists do not have the same start')
        return
    if list1[-1].end != list2[-1].end:
        print('Error: the lists do not have the same end')
        return

    iter1 = iter(list1)
    iter2 = iter(list2)
    result = list()
    item1 = next(iter1, None)
    item2 = next(iter2, None)
    while item1 and item2:
        if item1.end > item2.end:
            part = Block(item2.start, item2.size, (item1.status, item2.status))
            result.append(part)
            item1 = Block(item2.end, item1.size - item2.size, item1.status)
            item2 = next(iter2, None)
            continue
        if item1.end < item2.end:
            part = Block(item1.start, item1.size, (item1.status, item2.status))
            result.append(part)
            item2 = Block(item1.end, item2.size - item1.size, item2.status)
            item1 = next(iter1, None)
            continue
        # else item1.end == item2.end
        part = Block(item1.start, item1.size, (item1.status, item2.status))
        result.append(part)
        item1 = next(iter1, None)
        item2 = next(iter2, None)
    return result

test_list_1 = [
    Block(0, 100, '+'),
    Block(100, 20, '-'),
    Block(120, 35, '+'),
    Block(155, 40, '-'),
    Block(195, 55, '+'),
    Block(250, 50, '-'),
]

test_list_2 = [
    Block(0, 35, 'u'),
    Block(35, 45, 'v'),
    Block(80, 35, 'u'),
    Block(115, 40, 'v'),
    Block(155, 35, 'u'),
    Block(190, 45, 'v'),
    Block(235, 65, 'u'),
]

test_result = [
    Block(0, 35, ('+', 'u')),
    Block(35, 45, ('+', 'v')),
    Block(80, 20, ('+', 'u')),
    Block(100, 15, ('-', 'u')),
    Block(115, 5, ('-', 'v')),
    Block(120, 35, ('+', 'v')),
    Block(155, 35, ('-', 'u')),
    Block(190, 5, ('-', 'v')),
    Block(195, 40, ('+', 'v')),
    Block(235, 15, ('+', 'u')),
    Block(250, 50, ('-', 'u')),
]

if __name__ == '__main__':
    for block in merge(test_list_1, test_list_2):
        print(block)
    print()
    
    for block in test_result:
        print(block)
    print()

    if test_result == merge(test_list_1, test_list_2):
        print('Test succeeded')
    else:
        print('Test failed')
