/*
 * p4bisect.cc
 *
 * p4bisect provides the equivalent to git-bisect for Perforce(P4).
 *
 * Copyright (C) 2014 Chih-Chyuan Hwang (hwangcc@csie.nctu.edu.tw)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <string>
#include "p4bisect.h"

P4Bisect::P4Bisect()
{
	Error e;
	StrBuf msg;

	// Connect to server

	client.Init(&e);

	if (e.Test()) {
		e.Fmt(&msg);
		fprintf(stderr, "%s\n", msg.Text());
		return ;
	}
}

P4Bisect::~P4Bisect()
{
	Error e;
	StrBuf msg;

	if (client.Dropped()) {
		return ;
	}

	// Close connection

	client.Final(&e);

	if (e.Test()) {
		e.Fmt(&msg);
		fprintf(stderr, "%s\n", msg.Text());
	}
}

int P4Bisect::start(const char *file, 
		const char *good, const char *bad)
{
	// TODO:  Run the P4 command to get revisions
#if 0
	char *args[1];
	std::string s, s0(file), s1(good), s2(bad);

	if (client.Dropped()) {
		return -1;
	}

	s = s0 + "..." + s1 + "," + s2;
	fprintf(stdout, "s = %s\n", s.c_str());

	args[0] = (char *)s.c_str();
	client.SetArgv(1, args);
	client.Run("labels", &ui);
#endif
	return 0;
}

const char *P4Bisect::revision(unsigned long long rev)
{
	// TODO: return the n-th revision
	char temp[100];
	sprintf(temp, "revision%lld", rev);
	std::string s = temp;
	return s.c_str();
}

const unsigned int P4Bisect::nr_revisions()
{
	// TODO: count the number of revisions
	return 100;
}
