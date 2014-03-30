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
#include <algorithm>
#include <iterator>
#include <sstream>

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

int P4Bisect::start(const char *file, int use_changes,
		const char *good, const char *bad)
{
	char *args[1];
	StrBuf s;

	if (client.Dropped()) {
		return -1;
	}

	rev_vec.clear();
	working_file = file;

	s.Append(working_file.c_str());
	s.Append("...");
	s.Append(good);
	s.Append(",");
	s.Append(bad);

	args[0] = (char *)s.Text();
	client.SetArgv(1, args);
	client.Run(use_changes? "changes": "labels", &ui);

	if (!rev_vec.empty()) {
		if (use_changes) {
			std::reverse(rev_vec.begin(), rev_vec.end());
		}

		last_good = 0;
		first_bad = rev_vec.size() - 1;
		sync_rev = rev_vec.size();
		rev_vec[0].status = REV_STAT_GOOD;
		rev_vec[rev_vec.size() - 1].status = REV_STAT_BAD;

		return 0;
	} else {
		return -1;
	}
}

const char *P4Bisect::revision(unsigned long long rev)
{
	std::string s;

	if (rev_vec.empty()) {
		return NULL;
	}
	if (rev >= rev_vec.size()) {
		return NULL;
	}

	switch (rev_vec[rev].status) {
	case REV_STAT_GOOD:
		s = "[g] ";
		break;
	case REV_STAT_BAD:
		s = "[b] ";
		break;
	default:
		if (rev == sync_rev) {
			s = "[s] ";
		} else {
			s = "    ";
		}
		break;
	}

	s += rev_vec[rev].desc.c_str();

	return s.c_str();
}

const unsigned int P4Bisect::nr_revisions()
{
	return rev_vec.size();
}

void P4Bisect::AddRevision(StrBuf s)
{
	struct rev_entry entry;

	entry.desc = s.Text();
	entry.status = REV_STAT_UNKNOWN;

	rev_vec.push_back(entry);
}

int P4Bisect::MarkRevision(unsigned long long rev, int good_rev)
{
	unsigned long long i;

	if (rev_vec.empty()) {
		return -1;
	}
	if (rev >= rev_vec.size()) {
		return -1;
	}
	if (rev_vec[rev].status != REV_STAT_UNKNOWN) {
		return -1;
	}
	if (rev <= last_good || rev > first_bad) {
		return -1;
	}

	if (good_rev) {
		if (rev_vec[rev].status == REV_STAT_GOOD) {
			return -1;
		}
		for (i = last_good; i <= rev; i++) {
			rev_vec[i].status = REV_STAT_GOOD;
		}
		last_good = rev;
	} else {
		if (rev_vec[rev].status == REV_STAT_BAD) {
			return -1;
		}
		for (i = rev; i < first_bad; i++) {
			rev_vec[i].status = REV_STAT_BAD;
		}
		first_bad = rev;
	}

	return 0;
}

unsigned long long P4Bisect::CurrentRevision(void)
{
	if (!rev_vec.empty()) {
		return (last_good + first_bad) / 2;
	} else {
		return 0;
	}
}

int P4Bisect::SyncRevision(unsigned long long rev, sync_callback callback)
{
	char *args[1];
	StrBuf sb;

	if (client.Dropped()) {
		return -1;
	}
	if (rev_vec.empty()) {
		return -1;
	}
	if (rev >= rev_vec.size()) {
		return -1;
	}

	// Extract the changelist or label from rev_vec[rev]

	std::string s = rev_vec[rev].desc;
	std::stringstream ss(s);
	std::vector<std::string> v;

	v.assign(
		std::istream_iterator<std::string>(ss),
		std::istream_iterator<std::string>());

	sb.Append(working_file.c_str());
	sb.Append("...");
	sb.Append("@");
	sb.Append(v[1].c_str());

	syncing_cb = callback;

	args[0] = (char *)sb.Text();
	client.SetArgv(1, args);
	client.Run("sync", &ui);

	syncing_cb = NULL;
	sync_rev = rev;

	return 0;
}

int P4Bisect::SyncingFile(StrBuf sb)
{
	if (syncing_cb) {
		syncing_cb(sb.Text());
		return 1;
	} else {
		return 0;
	}
}
