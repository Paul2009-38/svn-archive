/*
    Copyright (C) 2005 Nick Whitelegg, Hogweed Software, nick@hogweed.org

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111 USA

 */
#include "Parser.h"
#include "Node.h"
#include "Img.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "time.h"

using std::cout;
using std::cout;
using std::endl;

int main(int argc,char* argv[])
{
	if(argc<3)
	{
		cout<<"Usage: osm2img InOsmFile OutImgFile" << endl;
		exit(1);
	}
	
	std::ifstream in(argv[1]);
	OSM::Components *comp1 = OSM::Parser::parse(in);
	in.close();

	std::ofstream out(argv[2]);
	Img img;

	//Pass interesting POIs, etc into IMG class.

	img.WriteFile(out);
	out.close();
	return 0;
}
