/*
 * p4bisect.h
 *
 * p4bisect provides the equivalent to git-bisect for Perforce(P4).
 *
 * Copyright (C) 2014 Chih-Chyuan Hwang (hwangcc@csie.nctu.edu.tw)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef _P4BISECT_H
#define _P4BISECT_H

#include "p4bisectclientuser.h"

class P4Bisect
{
	public:
	P4Bisect();
	~P4Bisect();
	int start(const char *file, const char *good, 
			const char *bad);
	const char *revision(unsigned long long rev);
	const unsigned int nr_revisions();

	private:
	ClientApi client;
	P4BisectClientUser ui;
};

#endif
