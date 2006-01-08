/*
    Copyright (C) 2005 Nick Whitelegg, Hogweed Software, nick@hogweed.org 

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111 USA

 */
#include "HTTPHandler.h"
#include "qmdcodec.h"
#include "MainWindow2.h"

#include <iostream>
using namespace std;

namespace OpenStreetMap
{

void HTTPHandler::sendRequest(const QString& requestType, 
							const QString& url,
							const QByteArray& b)
{
	int curReqId;
	if(curReqIDs.empty() || !locked)
	{
		cerr<<"Making request:" 
				<< " host: " << host
				<< " requestType: " <<requestType <<
				"URL :" << url << endl;
		method = requestType;
		QHttpRequestHeader header(requestType,url);
		header.setValue("Host",host);

		httpError = false;


		if(username!="" && password!="")
		{
			QString userpwd=QCodecs::base64Encode
					(QCString(username+":"+password));
			header.setValue("Authorization","Basic " + userpwd);
		}

		http->setHost(host);
		if(b.size())
			curReqId = http->request(header,b);
		else
			curReqId = http->request(header);

		cerr<<"curReqId is  " << curReqId << endl;
		QObject::connect(http,
					SIGNAL(responseHeaderReceived (const QHttpResponseHeader&)),
					this,
					SLOT(responseHeaderReceived(const QHttpResponseHeader&))
					);

		QObject::connect(http,SIGNAL(requestFinished(int,bool)),
					this,SLOT(responseReceived(int,bool)));

		makingRequest=true;
		curReqIDs.push_back(curReqId);
	}
	else
	{
		cerr<<"already making a request!" << endl;
		emit errorOccurred
			("Already making a request to the server. Please try again later");
	}
}

void HTTPHandler::responseHeaderReceived(const QHttpResponseHeader& header)
{
	cerr<<"Status code:" << header.statusCode() << endl;
	cerr<<"Reason phrase:" << header.reasonPhrase() << endl;
	httpError = header.statusCode()!=200;	
	if(httpError)
	{
		emit httpErrorOccurred(header.statusCode(), header.reasonPhrase());
	}
}

void HTTPHandler::responseReceived(int id, bool error)
{
	bool found=false;
	cerr<<"responseReceived(): id=" << id << endl;
	for(vector<int>::iterator i=curReqIDs.begin(); i!=curReqIDs.end(); i++)
	{
		if(id==*i)
		{
			cerr<<"doing erase" << endl;
			curReqIDs.erase(i);	
			cerr<<"done" << endl;
			found=true;
			break;
		}
	}

	if(found)
	{
		makingRequest = false;
		if(!httpError && !error)
		{
			cerr<<"response: id=" << id << " error=" << error << endl;
			cerr<<"RESPONSE RECEIVED!" << endl;
			emit responseReceived(http->readAll());
		}
		else
		{
			cerr<<"Error encountered" << endl;
		}
		//curReqId = 0;
	}
}

}
