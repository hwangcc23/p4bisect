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
#include <string>
#include <vector>

class rev_entry
{
public:
	int status;
	struct __date {
		int year, month, day;
	} date;
	std::string desc;

	bool operator<(const rev_entry &entry) const 
	{
		if (date.year != entry.date.year) {
			return date.year < entry.date.year;
		} else if (date.month != entry.date.month) {
			return date.month < entry.date.month;
		} else if (date.day != entry.date.day) {
			return date.day < entry.date.day;
		} else {
			return false;
		}
	}
};

typedef void (*sync_callback)(const char *msg);

class P4Bisect
{
public:
	P4Bisect();
	~P4Bisect();
	int start(const char *file, int use_changes,
			const char *good, const char *bad);
	const char *revision(unsigned long long rev);
	const unsigned int nr_revisions();
	void AddRevision(StrBuf s);
	int MarkRevision(unsigned long long rev, int good_rev);
	unsigned long long CurrentRevision(void);
	int SyncRevision(unsigned long long rev, sync_callback callback);
	int SyncingFile(StrBuf sb);

private:
	enum { REV_STAT_GOOD, REV_STAT_BAD, REV_STAT_UNKNOWN };
	ClientApi client;
	P4BisectClientUser ui;
	std::vector<rev_entry> rev_vec;
	std::string working_file;
	unsigned long long last_good, first_bad, sync_rev;
	sync_callback syncing_cb;
};

#endif
