/* -----------------------------------------------------------------------------
 *
 * mcl-flex
 *
 * -----------------------------------------------------------------------------
 *
 * Copyright (C) 2010-2022 Giovanni A. Zuliani | Monocasual Laboratories
 *
 * This file is part of mcl-flex.
 *
 * mcl-flex is free software: you can redistribute it and/or modify it under the 
 * terms of the GNU General Public License as published by the Free Software 
 * Foundation, either version 3 of the License, or (at your option) any later 
 * version.
 *
 * mcl-flex is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR 
 * A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with mcl-flex. If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * -------------------------------------------------------------------------- */

#include "fl_flex.hpp"
#include <cstddef>
#include <numeric>

namespace mcl
{
Fl_Flex::Elem::Elem(Fl_Widget& w, Fl_Flex& parent, Direction d, int size, geompp::Border<int> pad)
: size(size)
, m_w(w)
, m_parent(parent)
, m_dir(d)
, m_pad(pad)
{
}

/* -------------------------------------------------------------------------- */

int Fl_Flex::Elem::getSize() const
{
	if (isFixed())
		return size;
	return m_dir == Direction::VERTICAL ? m_w.h() : m_w.w();
}

/* -------------------------------------------------------------------------- */

bool Fl_Flex::Elem::isFixed() const
{
	return size != -1;
}

/* -------------------------------------------------------------------------- */

void Fl_Flex::Elem::resize(int pos, int newSize)
{
	geompp::Rect<int> bounds;

	if (m_dir == Direction::VERTICAL)
		bounds = geompp::Rect<int>(m_parent.x(), pos, m_parent.w(), newSize).reduced(m_pad);
	else
		bounds = geompp::Rect<int>(pos, m_parent.y(), newSize, m_parent.h()).reduced(m_pad);

	m_w.resize(bounds.x, bounds.y, bounds.w, bounds.h);
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

Fl_Flex::Fl_Flex(int x, int y, int w, int h, Direction d, int gutter)
: Fl_Group(x, y, w, h, 0)
, m_direction(d)
, m_gutter(gutter)
, m_numFixed(0)
{
	Fl_Group::end();
}

/* -------------------------------------------------------------------------- */

Fl_Flex::Fl_Flex(geompp::Rect<int> r, Direction d, int gutter)
: Fl_Flex(r.x, r.y, r.w, r.h, d, gutter)
{
}

/* -------------------------------------------------------------------------- */

Fl_Flex::Fl_Flex(Direction d, int gutter)
: Fl_Flex(0, 0, 0, 0, d, gutter)
{
}

/* -------------------------------------------------------------------------- */

void Fl_Flex::add(Fl_Widget& w, int size, geompp::Border<int> pad)
{
	Fl_Group::add(w);
	m_elems.push_back({w, *this, m_direction, size, pad});
	if (size != -1)
		m_numFixed++;
}

void Fl_Flex::add(Fl_Widget* w, int size, geompp::Border<int> pad)
{
	Fl_Flex::add(*w, size, pad);
}

/* -------------------------------------------------------------------------- */

void Fl_Flex::resize(int X, int Y, int W, int H)
{
	Fl_Group::resize(X, Y, W, H);

	const size_t numAllElems    = m_elems.size();
	const size_t numLiquidElems = numAllElems - m_numFixed;

	const int pos  = m_direction == Direction::VERTICAL ? y() : x();
	const int size = m_direction == Direction::VERTICAL ? h() : w();

	/* No fancy computations if there are no liquid elements. Just lay children
	according to their fixed size. */

	if (numLiquidElems == 0)
	{
		layWidgets(pos);
		return;
	}

	const int fixedElemsSize = std::accumulate(m_elems.begin(), m_elems.end(), 0, [](int acc, const Elem& e) {
		return e.isFixed() ? acc + e.getSize() : acc;
	});
	const int availableSize  = size - (m_gutter * (numAllElems - 1)); // Total size - gutters
	const int liquidElemSize = (availableSize - fixedElemsSize) / numLiquidElems;

	layWidgets(pos, liquidElemSize);
}

/* -------------------------------------------------------------------------- */

void Fl_Flex::layWidgets(int startPos, int sizeIfLiquid)
{
	int nextElemPos = startPos;
	for (Elem& e : m_elems)
	{
		e.resize(nextElemPos, e.isFixed() ? e.size : sizeIfLiquid);
		nextElemPos += e.getSize() + m_gutter;
	}
}

/* -------------------------------------------------------------------------- */

void Fl_Flex::end()
{
	Fl_Group::end();
	resize(x(), y(), w(), h());
}
} // namespace mcl
