/* -----------------------------------------------------------------------------
 *
 * mcl-fl_flex
 *
 * -----------------------------------------------------------------------------
 *
 * Copyright (C) 2010-2022 Giovanni A. Zuliani | Monocasual Laboratories
 *
 * This file is part of mcl-fl_flex.
 *
 * mcl-fl_flex is free software: you can redistribute it and/or modify it under 
 * the terms of the GNU General Public License as published by the Free Software 
 * Foundation, either version 3 of the License, or (at your option) any later 
 * version.
 *
 * mcl-fl_flex is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR 
 * A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with mcl-fl_flex. If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * -------------------------------------------------------------------------- */

#ifndef MCL_FL_FLEX_H
#define MCL_FL_FLEX_H

#include "geompp/src/border.hpp"
#include "geompp/src/rect.hpp"
#include <FL/Fl_Group.H>
#include <vector>

namespace mcl
{
/* Fl_Flex
Like a FlexBox item, it's a group that contains widgets that can be stretched 
to fill the area. Inspired by https://github.com/osen/FL_Flex. */

class Fl_Flex : public Fl_Group
{
public:
	enum class Direction
	{
		HORIZONTAL,
		VERTICAL
	};

	Fl_Flex(int x, int y, int w, int h, Direction d, int gutter = 0);
	Fl_Flex(geompp::Rect<int>, Direction d, int gutter = 0);
	Fl_Flex(Direction d, int gutter = 0);

	/* add
	Adds an existing widget to the Flex layout. If 'size' == -1, the widget
	will be stretched to take up the available space. WARNING: like Fl_Group,
	geFlex owns widgets! */

	void add(Fl_Widget&, int size = -1, geompp::Border<int> pad = {});
	void add(Fl_Widget*, int size = -1, geompp::Border<int> pad = {});

	/* end
	Finalize the Flex item. Call this when you're done add()ing widgets. */

	void end();

private:
	class Elem
	{
	public:
		Elem(Fl_Widget&, Fl_Flex& parent, Direction, int size, geompp::Border<int> pad);

		int  getSize() const;
		bool isFixed() const;

		void resize(int pos, int size);

		int size;

	private:
		Fl_Widget&          m_w;
		Fl_Flex&            m_parent;
		Direction           m_dir;
		geompp::Border<int> m_pad;
	};

	void resize(int x, int y, int w, int h) override;

	void layWidgets(int startPos, int sizeIfLiquid = 0);

	Direction         m_direction;
	int               m_gutter;
	std::vector<Elem> m_elems;
	int               m_numFixed;
};
} // namespace mcl

#endif