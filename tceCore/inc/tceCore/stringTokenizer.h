/***************************************************************************
 *   Copyright (C) 2006 by Matthew Tang                                    *
 *   trailcode@gmail.com                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef __STRING_TOKENIZER__H_
#define __STRING_TOKENIZER__H_

#include <string>

namespace tce
{
	namespace core
	{
		template <typename container>
		struct StringTokenizer : container
		{
			StringTokenizer(const std::string & str, const std::string & delimiters = " \r\n") : container()
			{
				// Skip delimiters at beginning.
				std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);

				// Find first "non-delimiter".
				std::string::size_type pos = str.find_first_of(delimiters, lastPos);

				while (std::string::npos != pos || std::string::npos != lastPos)
				{
					// Found a token, add it to the container.
					container::push_back(str.substr(lastPos, pos - lastPos));

					// Skip delimiters.  Note the "not_of"
					lastPos = str.find_first_not_of(delimiters, pos);

					// Find next "non-delimiter"
					pos = str.find_first_of(delimiters, lastPos);
				}
			}

			typedef typename container::iterator iterator;
			typedef typename container::const_iterator const_iterator;

		};
	}
}

#endif



