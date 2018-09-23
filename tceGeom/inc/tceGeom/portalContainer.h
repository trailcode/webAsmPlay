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

#ifndef __PORTAL_CONTAINER_H__
#define __PORTAL_CONTAINER_H__

namespace tce 
{
	namespace geom
	{
		/**
		* Template portal container wrapper, provides a constant and normal iterator 
		* to the container.
		*/
		template <typename Container>
		class BSPPortalContainer
		{
			public:	
				BSPPortalContainer() : container() { ;}

				virtual ~BSPPortalContainer() { ;}

				/**
				*	Provides access to the iterator of the container
				*/
				typedef typename Container::iterator iterator;
				
				/**
				* Provides access to the const_iterator of the container
				*/
				typedef typename Container::const_iterator const_iterator;

				typedef typename Container::reverse_iterator reverse_iterator;	

				typedef typename Container::const_reverse_iterator const_reverse_iterator;

				/**
				* stl container member functions
				*/

				inline iterator begin() { return container.begin() ;}

				inline const_iterator begin() const { return container.begin() ;}

				inline iterator end() { return container.end() ;}

				inline const_iterator end() const { return container.end() ;}

				inline reverse_iterator rbegin() { return container.rbegin() ;}

				inline const_reverse_iterator rbegin() const { return container.rbegin() ;}

				inline reverse_iterator rend() { return container.rend() ;}

				inline const_reverse_iterator rend() const { return container.rend() ;}

			private:
			
				Container container;
		};
	}
}

#endif // __PORTAL_CONTAINER_H__

