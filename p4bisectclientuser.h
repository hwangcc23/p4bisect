/*
 * p4bisectclientuser.h
 *
 * p4bisect provides the equivalent to git-bisect for Perforce(P4).
 *
 * Copyright (C) 2014 Chih-Chyuan Hwang (hwangcc@csie.nctu.edu.tw)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef _P4BISECTCLIENT_H
#define _P4BISECTCLIENT_H

#include <clientapi.h>

class P4Bisect;

class P4BisectClientUser: public ClientUser
{
	public:
	void Message(Error *err);
	void OutputError(const char *errBuf);
	void SetP4Bisect(P4Bisect *p) { p4bisect = p; };

	private:
	P4Bisect *p4bisect;
};

#endif
