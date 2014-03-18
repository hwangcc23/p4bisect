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

#include <string.h>
#include "p4bisect.h"

P4Bisect::P4Bisect()
{
	Error e;
	StrBuf msg;

	fprintf(stdout, "p4bisect - Find by binary search the change that introduced a bug\n");

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

int P4Bisect::start(const char *revision1, const char *revision2)
{
	// TODO:  Run the P4 command to get revisions

	//client.SetArgv(argc - 2, argv + 2);
	//client.Run(argv[1], &ui);

	return 0;
}

const char *P4Bisect::revision(const int n)
{
	// TODO: return the n-th revision

	return NULL;
}

const int P4Bisect::nr_revisions()
{
	// TODO: count the number of revisions
	return 0;
}
