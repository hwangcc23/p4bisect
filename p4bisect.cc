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
#include <i18napi.h>

P4Bisect::P4Bisect()
{
	Error e;
	StrBuf msg;

	ui.SetP4Bisect(this);

	// Connect to server

	client.SetTrans(CharSetApi::UTF_8, 0, 0, 0);
	client.SetCharset("UTF8");
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
	// TODO: Currently only labels are supported.

	char *args[1];
	StrBuf s;

	if (client.Dropped()) {
		return -1;
	}

	s.Append(file);
	s.Append("...");
	s.Append(good);
	s.Append(",");
	s.Append(bad);

	args[0] = (char *)s.Text();
	client.SetArgv(1, args);
	client.Run("labels", &ui);

	return 0;
}

const char *P4Bisect::revision(unsigned long long rev)
{
	if (rev_vec.empty()) {
		return NULL;
	}
	if (rev >= rev_vec.size()) {
		return NULL;
	}
	return rev_vec[rev].Text();
}

const unsigned int P4Bisect::nr_revisions()
{
	return rev_vec.size();
}

void P4Bisect::AddRevision(StrBuf s)
{
	rev_vec.push_back(s);
}
